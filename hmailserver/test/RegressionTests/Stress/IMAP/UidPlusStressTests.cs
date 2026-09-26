// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.IMAP.UidPlusTesting;
using RegressionTests.Shared;
using static RegressionTests.IMAP.UidPlusTesting.UidPlusHelpers;

namespace RegressionTests.Stress.IMAP
{
   /// <summary>
   ///    Temporary (temp/uidplustesting): concurrency and fuzz tests for UIDPLUS.
   ///    Every session's response stream is checked for EXISTS/EXPUNGE numbering it was never told about.
   /// </summary>
   [TestFixture]
   [Explicit("Stress test - long running, run manually.")]
   [Category("Stress")]
   [Category("UidPlusTesting")]
   internal class UidPlusStressTests : TestFixtureBase
   {
      private static readonly TimeSpan RaceDuration = TimeSpan.FromSeconds(45);

      private Account _account;

      [SetUp]
      public void CreateAccount()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "uidplus@example.test", Password);
      }

      private TrackedImapSession Connect(string name)
      {
         return new TrackedImapSession(_account.Address, Password, name);
      }

      /// <summary>Runs each worker on its own thread and collects exceptions and tracker violations.</summary>
      private static void RunWorkers(List<string> failures, params Action[] workers)
      {
         var threads = workers.Select(worker => new Thread(() =>
         {
            try
            {
               worker();
            }
            catch (Exception ex)
            {
               lock (failures)
                  failures.Add(ex.ToString());
            }
         })).ToList();

         threads.ForEach(thread => thread.Start());
         threads.ForEach(thread => thread.Join());
      }

      private static void AddViolations(List<string> failures, TrackedImapSession session)
      {
         lock (failures)
            failures.AddRange(session.Violations);
      }

      private Dictionary<long, string> Preload(string folder, int count, string flags = null)
      {
         var session = Connect("preload");
         var result = new Dictionary<long, string>();

         for (var i = 0; i < count; i++)
         {
            var id = "m" + i + "-" + Guid.NewGuid().ToString("N") + "@example.test";
            result[AssertAppendUid(session.Append(folder, CreateMessage(id), flags))] = id;
         }

         session.Disconnect();
         return result;
      }

      [Test]
      [Description("Several sessions copy random sets into one folder at once. Every destination UID must be " +
                   "distinct and hold the message its source UID names.")]
      public void ConcurrentCopiesIntoOneFolderReportCorrespondingUids()
      {
         const int threadCount = 6;
         const int copiesPerThread = 40;

         var setup = Connect("setup");
         Assert.IsTrue(TrackedImapSession.IsOk(setup.Command("CREATE \"Target\"")));
         setup.Disconnect();

         var sources = Preload("INBOX", 40);
         var failures = new List<string>();
         var copies = new List<CopyUid>();

         var workers = Enumerable.Range(0, threadCount).Select(t => (Action) (() =>
         {
            var random = new Random(t * 7919);
            var session = Connect("copier" + t);
            session.Select("INBOX");

            for (var i = 0; i < copiesPerThread; i++)
            {
               var first = random.Next(1, 41);
               var last = random.Next(1, 41);
               var set = random.Next(4) == 0 ? first + ":*" : first + ":" + last + "," + random.Next(1, 41);

               var response = session.Command("UID COPY " + set + " \"Target\"");
               var copyUid = ParseCopyUid(response);

               lock (copies)
               {
                  if (copyUid == null)
                     failures.Add("No COPYUID: " + response);
                  else if (copyUid.Source.Count != copyUid.Destination.Count)
                     failures.Add("COPYUID sets differ in length: " + response);
                  else
                     copies.Add(copyUid);
               }
            }

            AddViolations(failures, session);
            session.Disconnect();
         })).ToArray();

         RunWorkers(failures, workers);
         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures.Take(20)));

         var destinations = copies.SelectMany(copy => copy.Destination).ToList();
         CollectionAssert.AllItemsAreUnique(destinations, "Two COPY commands reported the same destination UID.");

         var reader = Connect("reader");
         reader.Select("Target");
         var actual = FetchMessageIds(reader, "1:*");
         reader.Disconnect();

         Assert.AreEqual(destinations.Count, actual.Count, "Target holds a different number of messages than reported.");

         foreach (var copy in copies)
         {
            for (var i = 0; i < copy.Source.Count; i++)
            {
               Assert.IsTrue(actual.ContainsKey(copy.Destination[i]), "Destination UID " + copy.Destination[i] + " missing.");
               Assert.AreEqual(sources[copy.Source[i]], actual[copy.Destination[i]],
                  "Destination UID " + copy.Destination[i] + " holds the wrong message.");
            }
         }

         CustomAsserts.AssertNoReportedError();
      }

      [Test]
      [Description("Several clients each keep replacing their own draft in a shared folder with APPEND, UID STORE " +
                   "and UID EXPUNGE, while an observer polls. Only the latest draft of each must survive.")]
      public void ConcurrentDraftReplacementKeepsOnlyLatestDrafts()
      {
         const int clientCount = 6;

         var setup = Connect("setup");
         Assert.IsTrue(TrackedImapSession.IsOk(setup.Command("CREATE \"SharedDrafts\"")));
         setup.Disconnect();

         var failures = new List<string>();
         var latest = new Dictionary<int, long>();
         var deadline = DateTime.UtcNow + RaceDuration;
         var replacements = 0;

         var workers = new List<Action>();

         for (var c = 0; c < clientCount; c++)
         {
            var clientIndex = c;

            workers.Add(() =>
            {
               var session = Connect("drafter" + clientIndex);
               session.Select("SharedDrafts");

               long previous = 0;

               for (var i = 0; DateTime.UtcNow < deadline; i++)
               {
                  var uid = AssertAppendUid(session.Append("SharedDrafts", CreateMessage("draft-" + clientIndex + "-" + i + "@example.test"), "\\Draft"));

                  if (previous != 0)
                  {
                     session.Command("UID STORE " + previous + " +FLAGS.SILENT (\\Deleted)");
                     var expunge = session.Command("UID EXPUNGE " + previous);

                     if (!TrackedImapSession.IsOk(expunge))
                        lock (failures) failures.Add("UID EXPUNGE failed: " + expunge);
                     else if (GetExpungedSequences(expunge).Length < 1)
                        lock (failures) failures.Add("UID EXPUNGE " + previous + " expunged nothing: " + expunge);

                     Interlocked.Increment(ref replacements);
                  }

                  previous = uid;
               }

               lock (latest)
                  latest[clientIndex] = previous;

               AddViolations(failures, session);
               session.Disconnect();
            });
         }

         TrackedImapSession observer = null;

         workers.Add(() =>
         {
            observer = Connect("observer");
            observer.Select("SharedDrafts");

            while (DateTime.UtcNow < deadline)
            {
               observer.Command("NOOP");
               observer.Command("FETCH 1:* (UID FLAGS)");
            }
         });

         RunWorkers(failures, workers.ToArray());

         TestContext.WriteLine("{0} draft replacements.", replacements);

         var expected = latest.Values.OrderBy(uid => uid).ToList();

         observer.Command("NOOP");
         AddViolations(failures, observer);
         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures.Take(20)));

         CollectionAssert.AreEqual(expected, UidSearchAll(observer), "Observer's view did not converge.");
         Assert.AreEqual(expected.Count, observer.KnownCount);
         observer.Disconnect();

         var reader = Connect("reader");
         reader.Select("SharedDrafts");
         CollectionAssert.AreEqual(expected, UidSearchAll(reader));
         reader.Disconnect();

         CustomAsserts.AssertNoReportedError();
      }

      [Test]
      [Description("Several sessions UID EXPUNGE overlapping sets of the same \\Deleted messages at once.")]
      public void ConcurrentOverlappingUidExpunges()
      {
         const int messageCount = 300;
         const int sessionCount = 6;

         Preload("INBOX", messageCount, "\\Deleted");

         var failures = new List<string>();
         var sessions = Enumerable.Range(0, sessionCount).Select(i => Connect("expunger" + i)).ToList();
         sessions.ForEach(session => session.Select("INBOX"));

         var workers = sessions.Select((session, index) => (Action) (() =>
         {
            var random = new Random(index * 104729);

            for (var i = 0; i < 60; i++)
            {
               var first = random.Next(1, messageCount + 1);
               var last = Math.Min(messageCount, first + random.Next(0, 20));
               var response = session.Command("UID EXPUNGE " + first + ":" + last + "," + random.Next(1, messageCount + 1));

               if (!TrackedImapSession.IsOk(response))
                  lock (failures) failures.Add(response);
            }

            session.Command("UID EXPUNGE 1:*");
         })).ToArray();

         RunWorkers(failures, workers);

         foreach (var session in sessions)
         {
            session.Command("NOOP");
            AddViolations(failures, session);

            if (session.KnownCount != 0)
               failures.Add("A session still believes there are " + session.KnownCount + " messages.");

            session.Disconnect();
         }

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures.Take(20)));

         var reader = Connect("reader");
         reader.Select("INBOX");
         Assert.AreEqual(0, reader.KnownCount);
         reader.Disconnect();

         CustomAsserts.AssertNoReportedError();
      }

      [Test]
      [Description("Appenders, copiers and expungers work on one folder while observers poll. Every session's " +
                   "stream must stay consistent, and each observer's view must match a fresh SELECT.")]
      public void MixedWorkloadKeepsEverySessionConsistent()
      {
         var setup = Connect("setup");
         Assert.IsTrue(TrackedImapSession.IsOk(setup.Command("CREATE \"Mixed\"")));
         setup.Disconnect();

         Preload("INBOX", 20);
         Preload("Mixed", 20);

         var failures = new List<string>();
         var deadline = DateTime.UtcNow + RaceDuration;
         var observers = new List<TrackedImapSession>();
         var workers = new List<Action>();

         // Appenders, some with \Deleted, so expungers have work.
         for (var a = 0; a < 2; a++)
         {
            var index = a;
            workers.Add(() =>
            {
               var session = Connect("appender" + index);
               session.Select("Mixed");
               for (var i = 0; DateTime.UtcNow < deadline; i++)
               {
                  var response = session.Append("Mixed", CreateMessage("a" + index + "-" + i + "@example.test"), i % 2 == 0 ? "\\Deleted" : null);
                  if (!ParseAppendUid(response).Success)
                     lock (failures) failures.Add("No APPENDUID: " + response);
               }
               AddViolations(failures, session);
               session.Disconnect();
            });
         }

         // Copiers from INBOX into the folder, with the folder not selected.
         workers.Add(() =>
         {
            var session = Connect("copier");
            session.Select("INBOX");
            var random = new Random(17);
            while (DateTime.UtcNow < deadline)
            {
               var response = session.Command("UID COPY " + random.Next(1, 21) + ":" + random.Next(1, 21) + " \"Mixed\"");
               if (ParseCopyUid(response) == null)
                  lock (failures) failures.Add("No COPYUID: " + response);
            }
            AddViolations(failures, session);
            session.Disconnect();
         });

         // Copier within the folder itself.
         workers.Add(() =>
         {
            var session = Connect("selfcopier");
            session.Select("Mixed");
            while (DateTime.UtcNow < deadline)
            {
               var response = session.Command("COPY 1:3 \"Mixed\"");
               if (!TrackedImapSession.IsOk(response) && !response.Contains("EXPUNGEISSUED"))
                  lock (failures) failures.Add("COPY failed: " + response);
               session.Command("NOOP");
            }
            AddViolations(failures, session);
            session.Disconnect();
         });

         // Expungers: one by UID, one plain, one flagging.
         workers.Add(() =>
         {
            var session = Connect("uidexpunger");
            session.Select("Mixed");
            var random = new Random(23);
            while (DateTime.UtcNow < deadline)
            {
               var uids = UidSearchAll(session);
               if (uids.Count == 0)
                  continue;
               var uid = uids[random.Next(uids.Count)];
               session.Command("UID STORE " + uid + " +FLAGS.SILENT (\\Deleted)");
               session.Command("UID EXPUNGE " + uid + ":*");
            }
            AddViolations(failures, session);
            session.Disconnect();
         });

         workers.Add(() =>
         {
            var session = Connect("expunger");
            session.Select("Mixed");
            while (DateTime.UtcNow < deadline)
            {
               session.Command("EXPUNGE");
               Thread.Sleep(50);
            }
            AddViolations(failures, session);
            session.Disconnect();
         });

         for (var o = 0; o < 3; o++)
         {
            var index = o;
            workers.Add(() =>
            {
               var session = Connect("observer" + index);
               lock (observers) observers.Add(session);
               session.Select("Mixed");
               while (DateTime.UtcNow < deadline)
               {
                  session.Command(index == 0 ? "NOOP" : "UID FETCH 1:* (FLAGS)");
                  session.Command("CHECK");
               }
            });
         }

         RunWorkers(failures, workers.ToArray());

         var reader = Connect("reader");
         reader.Select("Mixed");
         var expected = UidSearchAll(reader);
         reader.Disconnect();

         foreach (var observer in observers)
         {
            observer.Command("NOOP");
            AddViolations(failures, observer);

            var actual = UidSearchAll(observer);
            if (!actual.SequenceEqual(expected))
               failures.Add(string.Format("Observer view has {0} messages, fresh SELECT has {1}. Only in observer: {2}. Only in fresh: {3}",
                  actual.Count, expected.Count,
                  string.Join(",", actual.Except(expected).Take(10)), string.Join(",", expected.Except(actual).Take(10))));

            observer.Disconnect();
         }

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures.Take(30)));
         CustomAsserts.AssertNoReportedError();
      }

      [Test]
      [Description("Copies that fail on quota are rolled back while another session frees space. A failed COPY " +
                   "must leave nothing behind, and every reported destination UID must be real.")]
      public void ConcurrentCopyRollbackAgainstQuota()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quota@example.test", Password, 3);

         var setup = new TrackedImapSession(account.Address, Password, "setup");
         Assert.IsTrue(TrackedImapSession.IsOk(setup.Command("CREATE \"Target\"")));
         var body = string.Concat(Enumerable.Repeat(new string('x', 98) + "\r\n", 4000));
         for (var i = 0; i < 2; i++)
            AssertAppendUid(setup.Append("INBOX", CreateMessage("big" + i + "@example.test", body)));
         setup.Disconnect();

         var failures = new List<string>();
         var reported = new List<long>();
         var expungeAttempted = new HashSet<long>();
         var deadline = DateTime.UtcNow + RaceDuration;
         int ok = 0, no = 0;

         var workers = new List<Action>();

         for (var c = 0; c < 4; c++)
         {
            var index = c;
            workers.Add(() =>
            {
               var session = new TrackedImapSession(account.Address, Password, "copier" + index);
               session.Select("INBOX");
               while (DateTime.UtcNow < deadline)
               {
                  var response = session.Command("COPY 1:2 \"Target\"");
                  var copyUid = ParseCopyUid(response);

                  if (TrackedImapSession.IsOk(response))
                  {
                     Interlocked.Increment(ref ok);
                     if (copyUid == null || copyUid.Destination.Count != 2)
                        lock (failures) failures.Add("OK without full COPYUID: " + response);
                     else
                        lock (reported) reported.AddRange(copyUid.Destination);
                  }
                  else
                  {
                     Interlocked.Increment(ref no);
                     if (copyUid != null)
                        lock (failures) failures.Add("Failed COPY reported COPYUID: " + response);
                  }
               }
               AddViolations(failures, session);
               session.Disconnect();
            });
         }

         workers.Add(() =>
         {
            var session = new TrackedImapSession(account.Address, Password, "cleaner");
            session.Select("Target");
            while (DateTime.UtcNow < deadline)
            {
               var uids = UidSearchAll(session);
               if (uids.Count == 0)
               {
                  Thread.Sleep(20);
                  continue;
               }

               var set = string.Join(",", uids);
               lock (expungeAttempted) expungeAttempted.UnionWith(uids);
               session.Command("UID STORE " + set + " +FLAGS.SILENT (\\Deleted)");
               session.Command("UID EXPUNGE " + set);
            }
            AddViolations(failures, session);
            session.Disconnect();
         });

         RunWorkers(failures, workers.ToArray());
         TestContext.WriteLine("{0} copies succeeded, {1} failed.", ok, no);

         var reader = new TrackedImapSession(account.Address, Password, "reader");
         reader.Select("Target");
         var present = UidSearchAll(reader);
         reader.Disconnect();

         var unreported = present.Except(reported).ToList();
         if (unreported.Any())
            failures.Add("Messages in Target that no successful COPY reported (rollback leftovers?): " + string.Join(",", unreported));

         var lost = reported.Except(present).Except(expungeAttempted).ToList();
         if (lost.Any())
            failures.Add("Reported destination UIDs that vanished without being expunged: " + string.Join(",", lost));

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures.Take(20)));
         Assert.Greater(no, 0, "Quota never stopped a COPY, so rollback was not exercised.");
         CustomAsserts.AssertNoReportedError();
      }

      [Test]
      [Description("Random sets, valid and malformed, for FETCH, UID FETCH and UID COPY, checked against a model.")]
      public void MessageSetFuzz()
      {
         const int iterations = 3000;

         var setup = Connect("setup");
         Assert.IsTrue(TrackedImapSession.IsOk(setup.Command("CREATE \"FuzzTarget\"")));
         setup.Disconnect();

         Preload("INBOX", 30);

         var session = Connect("fuzzer");
         session.Select("INBOX");

         // Gaps between the UIDs.
         session.Command("UID STORE 3,7:9,20,30 +FLAGS.SILENT (\\Deleted)");
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("UID EXPUNGE 1:*")));
         var uids = UidSearchAll(session);

         var random = new Random(4242);
         var failures = new List<string>();

         for (var i = 0; i < iterations && failures.Count < 20; i++)
         {
            var set = RandomSet(random);
            var isUid = random.Next(2) == 0;
            var expected = ModelResolve(set, isUid, uids);

            var response = session.Command((isUid ? "UID FETCH " : "FETCH ") + set + " (UID)");

            if (expected == null)
            {
               if (!TrackedImapSession.IsBad(response))
                  failures.Add(string.Format("Set \"{0}\" ({1}) should be BAD: {2}", set, isUid ? "UID" : "seq", Trim(response)));
               continue;
            }

            if (!TrackedImapSession.IsOk(response))
            {
               failures.Add(string.Format("Set \"{0}\" ({1}) should be OK: {2}", set, isUid ? "UID" : "seq", Trim(response)));
               continue;
            }

            var actual = Regex.Matches(response, @"UID (\d+)").Cast<Match>().Select(m => long.Parse(m.Groups[1].Value)).Distinct().OrderBy(u => u).ToList();
            var expectedDistinct = expected.Distinct().OrderBy(u => u).ToList();

            if (!actual.SequenceEqual(expectedDistinct))
               failures.Add(string.Format("Set \"{0}\" ({1}) gave [{2}], expected [{3}]", set, isUid ? "UID" : "seq",
                  string.Join(",", actual), string.Join(",", expectedDistinct)));

            // Now and then, check that UID COPY copies in set order.
            if (isUid && expected.Count > 0 && random.Next(20) == 0)
            {
               var copy = session.Command("UID COPY " + set + " \"FuzzTarget\"");
               var copyUid = ParseCopyUid(copy);

               if (copyUid == null || !copyUid.Source.SequenceEqual(expected))
                  failures.Add(string.Format("UID COPY \"{0}\" source set [{1}], expected [{2}]", set,
                     copyUid == null ? Trim(copy) : string.Join(",", copyUid.Source), string.Join(",", expected)));
            }
         }

         session.AssertNoViolations();
         session.Disconnect();

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures));
         CustomAsserts.AssertNoReportedError();
      }

      [TestCase(5000, 2500)]
      [Description("Timing for UID EXPUNGE, UID COPY and UID FETCH with a long set on a large folder. " +
                   "Resolving each range walks the whole view, so time grows with ranges × messages.")]
      public void LongSetsOnLargeFolderStayFast(int messageCount, int parts)
      {
         var session = Connect("large");

         for (var i = 0; i < messageCount; i++)
            session.Simulator.SendSingleCommandWithLiteral("P01 APPEND INBOX (\\Deleted) {4}", "ABCD");

         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         session.Select("INBOX");
         Assert.AreEqual(messageCount, session.KnownCount);

         var set = string.Join(",", Enumerable.Range(0, parts).Select(i => (i * 2 + 1).ToString()));
         TestContext.WriteLine("Set is {0} characters.", set.Length);

         foreach (var command in new[] {"UID FETCH " + set + " (UID)", "UID COPY " + set + " \"Target\"", "UID EXPUNGE " + set})
         {
            var watch = Stopwatch.StartNew();
            var response = session.Command(command);
            watch.Stop();

            TestContext.WriteLine("{0}: {1} ms", command.Substring(0, 10), watch.ElapsedMilliseconds);
            Assert.IsTrue(TrackedImapSession.IsOk(response), Trim(response));
            Assert.Less(watch.Elapsed, TimeSpan.FromSeconds(20), command.Substring(0, 10));
         }

         Assert.AreEqual(messageCount - parts, session.KnownCount);
         session.AssertNoViolations();
         session.Disconnect();
      }

      [Test]
      [Description("Pathological but valid sets must not hang or crash the server.")]
      public void PathologicalSetsDoNotHangServer()
      {
         Preload("INBOX", 5, "\\Deleted");

         var session = Connect("pathological");
         session.Select("INBOX");

         var sets = new[]
         {
            "1:4294967295",
            "4294967295:1",
            "*:4294967295",
            string.Join(",", Enumerable.Repeat("1:*", 20000)),
            string.Join(",", Enumerable.Repeat("4294967295", 20000)),
            new string('9', 5000),
            string.Join(":", Enumerable.Repeat("1", 5000)),
            new string(',', 5000),
         };

         foreach (var set in sets)
         {
            foreach (var prefix in new[] {"UID FETCH ", "FETCH "})
            {
               var watch = Stopwatch.StartNew();
               var response = session.Command(prefix + set + " (UID)");
               watch.Stop();

               Assert.IsTrue(Regex.IsMatch(response, @"^T\d{5} (OK|NO|BAD)", RegexOptions.Multiline), Trim(response));
               Assert.Less(watch.Elapsed, TimeSpan.FromSeconds(20), prefix + Trim(set));
            }
         }

         // The server is still alive and the messages are intact.
         CollectionAssert.AreEqual(new[] {1L, 2L, 3L, 4L, 5L}, UidSearchAll(session));
         session.AssertNoViolations();
         session.Disconnect();

         CustomAsserts.AssertNoReportedError();
      }

      #region Fuzz model

      private static readonly string[] Atoms = {"1", "2", "5", "7", "8", "10", "25", "29", "30", "31", "100", "0", "*", "4294967295", "4294967296", "01", "99999999999"};
      private static readonly string[] Junk = {"", ":", ",", "-1", "a", "1.2", "**", "*1", "1*", "::"};

      private static string RandomSet(Random random)
      {
         var builder = new StringBuilder();
         var parts = random.Next(1, 5);

         for (var p = 0; p < parts; p++)
         {
            if (p > 0)
               builder.Append(',');

            if (random.Next(15) == 0)
            {
               builder.Append(Junk[random.Next(Junk.Length)]);
               continue;
            }

            builder.Append(Atoms[random.Next(Atoms.Length)]);

            if (random.Next(2) == 0)
               builder.Append(':').Append(Atoms[random.Next(Atoms.Length)]);
         }

         var result = builder.ToString();
         return result.Length == 0 ? "," : result;
      }

      /// <summary>
      ///    The server's rules as implemented: 0 is accepted, ranges may be reversed, * is the highest
      ///    UID or the message count. Returns null for a malformed set. Keeps set order and duplicates.
      /// </summary>
      private static List<long> ModelResolve(string set, bool isUid, List<long> uids)
      {
         var highest = isUid ? (uids.Count == 0 ? 0 : uids.Last()) : uids.Count;
         var result = new List<long>();

         foreach (var part in set.Split(','))
         {
            var bounds = part.Split(':');
            if (bounds.Length > 2)
               return null;

            long first, last;
            if (!ModelNumber(bounds[0], highest, out first) || !ModelNumber(bounds[bounds.Length - 1], highest, out last))
               return null;

            if (first > last)
            {
               var swap = first;
               first = last;
               last = swap;
            }

            if (isUid)
               result.AddRange(uids.Where(uid => uid >= first && uid <= last));
            else
               for (var sequence = Math.Max(1, first); sequence <= Math.Min(last, uids.Count); sequence++)
                  result.Add(uids[(int) sequence - 1]);
         }

         return result;
      }

      private static bool ModelNumber(string value, long highest, out long number)
      {
         number = 0;

         if (value == "*")
         {
            number = highest;
            return true;
         }

         if (value.Length == 0 || value.Length > 10 || !value.All(char.IsDigit))
            return false;

         number = long.Parse(value);
         return number <= uint.MaxValue;
      }

      private static string Trim(string text)
      {
         return text.Length > 300 ? text.Substring(0, 300) + "..." : text;
      }

      #endregion
   }
}
