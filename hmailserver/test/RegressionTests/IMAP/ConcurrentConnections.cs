using System;
using System.Collections.Generic;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class ConcurrentConnections : TestFixtureBase
   {
      [Test]
      [Description("Bug: SendCachedNotifications passed lastExists to SendRECENT_ instead of lastRecent")]
      public void NoopRecentCountReflectsRecentMessagesNotExistsCount()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // SELECT an empty inbox; the connection's recent-messages set starts empty (size=0).
         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         // Deliver two messages after SELECT. They are not added to this connection's
         // recent-messages set, so lastRecent remains 0 while lastExists becomes 2.
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test 1", "Body 1");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test 2", "Body 2");

         // Wait for delivery without opening a second IMAP SELECT (which would claim \Recent).
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 2);

         // NOOP flushes cached notifications. EXISTS must be 2, RECENT must be 0.
         var response = sim.NOOP();
         Assert.IsTrue(response.Contains("* 2 EXISTS"), response);
         Assert.IsTrue(response.Contains("* 0 RECENT"), response);

         sim.Disconnect();
      }


      [Test]
      public void ChangingFlagShouldAffectAllConnections()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var sim1 = new ImapClientSimulator();
         Assert.IsTrue(sim1.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(sim1.SelectFolder("Inbox"));

         var sim2 = new ImapClientSimulator();
         Assert.IsTrue(sim2.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(sim2.SelectFolder("Inbox"));

         sim1.SetFlagOnMessage(1, true, "\\Deleted");

         var flags1 = sim1.GetFlags(1);
         var flags2 = sim2.GetFlags(1);

         Assert.IsTrue(flags2.Contains(@"* 1 FETCH (FLAGS (\Deleted))"), flags2);
         Assert.IsTrue(flags2.Contains(@"* 1 FETCH (FLAGS (\Deleted) UID 1)"), flags2);
      }

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
      [Explicit("Stress test - long running, run manually.")]
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
      [Explicit("Stress test - long running, run manually.")]
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
   }
}