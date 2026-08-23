using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.Stress.IMAP
{
   [TestFixture]
   [Explicit("Stress test - long running, run manually.")]
   [Category("Stress")]
   internal class ConcurrentConnectionsStressTests : TestFixtureBase
   {

      private const int PreloadedMessageCount = 5000;
      private const int ReaderThreadCount = 4;
      private const int WriterThreadCount = 2;
      private static readonly TimeSpan RaceDuration = TimeSpan.FromSeconds(60);

      private class RaceCounters
      {
         public int Examines;
         public int Expunges;
         public int Appends;
      }

      [Test]
      [Description(
         "Issue #551: a concurrent refresh calls vector::reserve, which nulls the old slots while a " +
         "reader is walking them. Being a race, a single green run proves little.")]
      public void ConcurrentSelectAndAppendOnSameFolderDoesNotCrashServer()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // Both the move loop and the unguarded read scale with folder size.
         PreloadMessages(account.Address, PreloadedMessageCount);

         var failures = new List<string>();
         var counters = new RaceCounters();
         var deadline = DateTime.UtcNow + RaceDuration;
         var threads = new List<Thread>();

         // Readers share one cached Messages instance, keyed on folder id, so they walk the same vector.
         for (var i = 0; i < ReaderThreadCount; i++)
            threads.Add(new Thread(() => RepeatedlyExamineInbox(account.Address, deadline, failures, counters)));

         // Each APPEND marks the folder as needing refresh, so the next SELECT reallocates the
         // vector the readers are walking.
         for (var i = 0; i < WriterThreadCount; i++)
            threads.Add(new Thread(() => RepeatedlyAppendToInbox(account.Address, deadline, failures, counters)));

         foreach (var thread in threads)
            thread.Start();

         foreach (var thread in threads)
            thread.Join();

         // A green run is only meaningful if these numbers are large.
         TestContext.WriteLine("Issue #551 reserve race: {0} EXAMINEs, {1} APPENDs over {2}, {3} preloaded messages.",
            counters.Examines, counters.Appends, RaceDuration, PreloadedMessageCount);

         // The service survives the access violation, so the evidence is in the error log.
         CustomAsserts.AssertNoReportedError();

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures));
      }

      [Test]
      [Description(
         "Issue #551, backend-agnostic variant. vector::erase moves every following element down a slot, " +
         "nulling each source. Unlike reserve(), this does not depend on RecordCount.")]
      public void ConcurrentExamineAndExpungeOnSameFolderDoesNotCrashServer()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         PreloadMessages(account.Address, PreloadedMessageCount);

         var failures = new List<string>();
         var counters = new RaceCounters();
         var deadline = DateTime.UtcNow + RaceDuration;
         var threads = new List<Thread>();

         for (var i = 0; i < ReaderThreadCount; i++)
            threads.Add(new Thread(() => RepeatedlyExamineInbox(account.Address, deadline, failures, counters)));

         threads.Add(new Thread(() => RepeatedlyExpungeFirstMessage(account.Address, deadline, failures, counters)));

         foreach (var thread in threads)
            thread.Start();

         foreach (var thread in threads)
            thread.Join();

         TestContext.WriteLine("Issue #551 expunge race: {0} EXAMINEs, {1} EXPUNGEs over {2}, {3} preloaded messages.",
            counters.Examines, counters.Expunges, RaceDuration, PreloadedMessageCount);

         CustomAsserts.AssertNoReportedError();

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures));
      }

      private const int RefreshRaceReaderCount = 4;
      private const int RefreshRacePreloadedMessageCount = 3000;
      private static readonly TimeSpan RefreshRaceDuration = TimeSpan.FromSeconds(60);

      private class RefreshRaceState
      {
         // Only counts messages the server has already acknowledged as stored.
         public int CommittedCount;
         public int Examines;
         public int Appends;
         public int Selects;
      }

      [Test]
      [Description(
         "EXAMINE must never report fewer messages than the server has already acknowledged storing, " +
         "no matter what other connections are doing to the same folder.")]
      public void ConcurrentExamineReportsEveryAcknowledgedMessage()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         PreloadMessages(account.Address, RefreshRacePreloadedMessageCount);

         var failures = new List<string>();
         var state = new RefreshRaceState { CommittedCount = RefreshRacePreloadedMessageCount };
         var deadline = DateTime.UtcNow + RefreshRaceDuration;
         var threads = new List<Thread>();

         for (var i = 0; i < RefreshRaceReaderCount; i++)
            threads.Add(new Thread(() => RepeatedlyExamineAndCheckCount(account.Address, deadline, failures, state)));

         threads.Add(new Thread(() => RepeatedlyAppendWithoutSelecting(account.Address, deadline, failures, state)));

         // A concurrent SELECT loop, which is slow on a large folder, is what makes the timing
         // window wide enough to hit.
         threads.Add(new Thread(() => RepeatedlySelectInbox(account.Address, deadline, failures, state)));

         foreach (var thread in threads)
            thread.Start();

         foreach (var thread in threads)
            thread.Join();

         TestContext.WriteLine("Refresh flag race: {0} EXAMINEs, {1} APPENDs, {2} SELECTs over {3}, {4} preloaded messages.",
            state.Examines, state.Appends, state.Selects, RefreshRaceDuration, RefreshRacePreloadedMessageCount);

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures));
      }

      private static void RepeatedlyExamineAndCheckCount(string address, DateTime deadline, List<string> failures,
         RefreshRaceState state)
      {
         try
         {
            var simulator = new ImapClientSimulator();

            if (!simulator.ConnectAndLogon(address, "test"))
            {
               Record(failures, "Reader could not log on.");
               return;
            }

            while (DateTime.UtcNow < deadline)
            {
               // Read the committed count first, so every message it counts was already stored
               // when this EXAMINE was sent.
               var expectedAtLeast = Interlocked.CompareExchange(ref state.CommittedCount, 0, 0);

               var actual = ParseExists(simulator.ExamineFolder("INBOX"));

               Interlocked.Increment(ref state.Examines);

               if (actual < expectedAtLeast)
               {
                  Record(failures, string.Format("EXAMINE reported {0} EXISTS, but {1} messages were already stored.",
                     actual, expectedAtLeast));
                  return;
               }
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            Record(failures, "Reader aborted: " + ex.Message);
         }
      }

      private static void RepeatedlyAppendWithoutSelecting(string address, DateTime deadline, List<string> failures,
         RefreshRaceState state)
      {
         try
         {
            // This connection never selects a folder, so its APPENDs mark the folder as needing a
            // refresh without performing one.
            var simulator = new ImapClientSimulator();
            simulator.Connect();
            simulator.LogonWithLiteral(address, "test");

            while (DateTime.UtcNow < deadline)
            {
               simulator.SendSingleCommandWithLiteral("A01 APPEND INBOX {4}", "ABCD");

               Interlocked.Increment(ref state.CommittedCount);
               Interlocked.Increment(ref state.Appends);
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            Record(failures, "Writer aborted: " + ex.Message);
         }
      }

      private static void RepeatedlySelectInbox(string address, DateTime deadline, List<string> failures,
         RefreshRaceState state)
      {
         try
         {
            var simulator = new ImapClientSimulator();

            if (!simulator.ConnectAndLogon(address, "test"))
            {
               Record(failures, "Lock holder could not log on.");
               return;
            }

            while (DateTime.UtcNow < deadline)
            {
               if (!simulator.SelectFolderWithoutLiteral("INBOX"))
               {
                  Record(failures, "Lock holder could not select INBOX.");
                  return;
               }

               Interlocked.Increment(ref state.Selects);
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            Record(failures, "Lock holder aborted: " + ex.Message);
         }
      }

      private static int ParseExists(string response)
      {
         var match = Regex.Match(response, @"^\* (\d+) EXISTS", RegexOptions.Multiline);

         if (!match.Success)
            throw new InvalidOperationException("No EXISTS in EXAMINE response: " + response);

         return int.Parse(match.Groups[1].Value);
      }

      private static void RepeatedlyExpungeFirstMessage(string address, DateTime deadline, List<string> failures,
         RaceCounters counters)
      {
         try
         {
            var simulator = new ImapClientSimulator();

            if (!simulator.ConnectAndLogon(address, "test"))
            {
               Record(failures, "Mutator could not log on.");
               return;
            }

            // SELECT once only. Re-selecting each pass would run RemoveRecentFlags, which holds the
            // collection mutex across a folder-wide UPDATE.
            if (!simulator.SelectFolderWithoutLiteral("INBOX"))
            {
               Record(failures, "Mutator could not select INBOX.");
               return;
            }

            while (DateTime.UtcNow < deadline)
            {
               // Erasing the first message shifts every remaining element down a slot, sweeping a null
               // through the whole vector the readers are walking.
               simulator.SetDeletedFlag(1);
               simulator.Expunge();
               Interlocked.Increment(ref counters.Expunges);

               // Replenish so the folder keeps its size and the sweep stays long.
               simulator.SendSingleCommandWithLiteral("A01 APPEND INBOX {4}", "ABCD");
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            Record(failures, "Mutator aborted: " + ex.Message);
         }
      }

      private static void PreloadMessages(string address, int count)
      {
         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.LogonWithLiteral(address, "test");

         for (var i = 0; i < count; i++)
            simulator.SendSingleCommandWithLiteral("A01 APPEND INBOX {4}", "ABCD");

         Assert.AreEqual(count, simulator.GetMessageCount("INBOX"));

         simulator.Disconnect();
      }

      private static void RepeatedlyExamineInbox(string address, DateTime deadline, List<string> failures,
         RaceCounters counters)
      {
         try
         {
            var simulator = new ImapClientSimulator();

            if (!simulator.ConnectAndLogon(address, "test"))
            {
               Record(failures, "Reader could not log on.");
               return;
            }

            while (DateTime.UtcNow < deadline)
            {
               // EXAMINE runs the same loop as SELECT, but skips RemoveRecentFlags - which holds the
               // collection mutex across a folder-wide UPDATE and throttles the writers.
               if (!simulator.ExamineFolder("INBOX").StartsWith("*"))
               {
                  Record(failures, "EXAMINE did not return an untagged response.");
                  return;
               }

               Interlocked.Increment(ref counters.Examines);
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            // A crashing worker drops the connection mid-command.
            Record(failures, "Reader aborted: " + ex.Message);
         }
      }

      private static void RepeatedlyAppendToInbox(string address, DateTime deadline, List<string> failures,
         RaceCounters counters)
      {
         try
         {
            var simulator = new ImapClientSimulator();
            simulator.Connect();
            simulator.LogonWithLiteral(address, "test");

            while (DateTime.UtcNow < deadline)
            {
               simulator.SendSingleCommandWithLiteral("A01 APPEND INBOX {4}", "ABCD");
               Interlocked.Increment(ref counters.Appends);
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            Record(failures, "Writer aborted: " + ex.Message);
         }
      }

      private static void Record(List<string> failures, string message)
      {
         lock (failures)
            failures.Add(message);
      }

      private const int StableFolderCount = 150;
      private const int FolderRaceReaderCount = 4;
      private const int FolderRaceWriterCount = 2;
      private static readonly TimeSpan FolderRaceDuration = TimeSpan.FromSeconds(60);

      private class FolderRaceState
      {
         public int Lists;
         public int Creates;
         public int Deletes;
      }

      [Test]
      [Description(
         "FolderListCreator iterates the live vector returned by IMAPFolders::GetVector(), " +
         "while CREATE and DELETE on other connections push_back into and erase from the same vector.")]
      public void ConcurrentListAndFolderChurnListsEveryExistingFolder()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // These folders are never touched again, so every LIST must report all of them.
         var stableFolders = CreateStableFolders(account.Address, StableFolderCount);

         var failures = new List<string>();
         var state = new FolderRaceState();
         var deadline = DateTime.UtcNow + FolderRaceDuration;
         var threads = new List<Thread>();

         // All connections for one account share a single cached IMAPFolders instance.
         for (var i = 0; i < FolderRaceReaderCount; i++)
            threads.Add(new Thread(() => RepeatedlyListFolders(account.Address, stableFolders, deadline, failures, state)));

         // CREATE reallocates the vector; DELETE shifts every following element down a slot.
         for (var i = 0; i < FolderRaceWriterCount; i++)
         {
            var writerIndex = i;
            threads.Add(new Thread(() => RepeatedlyChurnFolders(account.Address, writerIndex, deadline, failures, state)));
         }

         foreach (var thread in threads)
            thread.Start();

         foreach (var thread in threads)
            thread.Join();

         TestContext.WriteLine("Folder list race: {0} LISTs, {1} CREATEs, {2} DELETEs over {3}, {4} stable folders.",
            state.Lists, state.Creates, state.Deletes, FolderRaceDuration, StableFolderCount);

         CustomAsserts.AssertNoReportedError();

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures));
      }

      private static List<string> CreateStableFolders(string address, int count)
      {
         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.LogonWithLiteral(address, "test");

         var folders = new List<string>();

         for (var i = 0; i < count; i++)
         {
            var name = string.Format("Stable{0:D3}", i);

            Assert.IsTrue(simulator.CreateFolder(name), "Could not create " + name);

            folders.Add(name);
         }

         var listing = simulator.List("*");

         foreach (var folder in folders)
            Assert.IsTrue(ListingContainsFolder(listing, folder), "Setup did not list " + folder);

         simulator.Disconnect();

         return folders;
      }

      private static void RepeatedlyListFolders(string address, List<string> stableFolders, DateTime deadline,
         List<string> failures, FolderRaceState state)
      {
         try
         {
            var simulator = new ImapClientSimulator();

            if (!simulator.ConnectAndLogon(address, "test"))
            {
               Record(failures, "Reader could not log on.");
               return;
            }

            while (DateTime.UtcNow < deadline)
            {
               var listing = simulator.List("*");

               Interlocked.Increment(ref state.Lists);

               foreach (var folder in stableFolders)
               {
                  if (!ListingContainsFolder(listing, folder))
                  {
                     Record(failures, string.Format("LIST omitted {0}, which exists and is never modified.", folder));
                     return;
                  }
               }
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            // A crashing worker drops the connection mid-command.
            Record(failures, "Reader aborted: " + ex.Message);
         }
      }

      private static void RepeatedlyChurnFolders(string address, int writerIndex, DateTime deadline,
         List<string> failures, FolderRaceState state)
      {
         try
         {
            var simulator = new ImapClientSimulator();

            if (!simulator.ConnectAndLogon(address, "test"))
            {
               Record(failures, "Writer could not log on.");
               return;
            }

            var counter = 0;

            while (DateTime.UtcNow < deadline)
            {
               var name = string.Format("Churn{0}_{1}", writerIndex, counter++);

               if (!simulator.CreateFolder(name))
               {
                  Record(failures, "Writer could not create " + name);
                  return;
               }

               Interlocked.Increment(ref state.Creates);

               if (!simulator.DeleteFolder(name))
               {
                  Record(failures, "Writer could not delete " + name);
                  return;
               }

               Interlocked.Increment(ref state.Deletes);
            }

            simulator.Disconnect();
         }
         catch (Exception ex)
         {
            Record(failures, "Writer aborted: " + ex.Message);
         }
      }

      private static bool ListingContainsFolder(string listing, string folderName)
      {
         // A LIST line ends with the folder name in quotes, so the quoted name is unambiguous.
         var quote = ((char)34).ToString();

         return listing.Contains(quote + folderName + quote);
      }
   }
}
