// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.IMAP
{
   /// <summary>
   ///    RFC 4315, the IMAP UIDPLUS extension: APPENDUID, COPYUID and UID EXPUNGE.
   /// </summary>
   [TestFixture]
   public class UidPlus : TestFixtureBase
   {
      private const string Password = "test";

      #region APPENDUID

      [Test]
      public void AppendReturnsAppendUid()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);

         var select = simulator.SendSingleCommand("A01 SELECT INBOX");
         var uidValidity = GetResponseCodeValue(select, "UIDVALIDITY");
         var uidNext = GetResponseCodeValue(select, "UIDNEXT");

         var first = simulator.Append("INBOX", CreateMessage("first@example.test"));
         var firstUid = AssertAppendUid(first, uidValidity);
         Assert.AreEqual(uidNext, firstUid, first);

         var second = simulator.Append("INBOX", CreateMessage("second@example.test"));
         Assert.AreEqual(firstUid + 1, AssertAppendUid(second, uidValidity), second);

         // The reported UID must be the UID of the message that was appended.
         Assert.AreEqual(firstUid.ToString(), UidSearch(simulator, "HEADER Message-ID first@example.test"));

         simulator.Disconnect();
      }

      [Test]
      public void AppendToPublicFolderReturnsItsAppendUid()
      {
         var account = AddAccount();
         CreatePublicFolder("Share", account);

         var simulator = ConnectAndLogon(account);

         var select = simulator.SendSingleCommand("A01 SELECT \"#Public.Share\"");
         Assert.IsTrue(select.Contains("A01 OK"), select);
         var uidValidity = GetResponseCodeValue(select, "UIDVALIDITY");
         var uidNext = GetResponseCodeValue(select, "UIDNEXT");

         var append = simulator.Append("#Public.Share", CreateMessage("public@example.test"));
         Assert.AreEqual(uidNext, AssertAppendUid(append, uidValidity), append);

         simulator.Disconnect();
      }

      [Test]
      [Description("A client that can't SELECT the folder must not be told its UIDVALIDITY or UIDs (RFC 4315 6).")]
      public void AppendWithoutReadPermissionHasNoAppendUid()
      {
         var account = AddAccount();
         CreatePublicFolder("Dropbox", account, false);

         var simulator = ConnectAndLogon(account);

         var append = simulator.Append("#Public.Dropbox", CreateMessage("dropbox@example.test"));
         Assert.IsTrue(append.Contains("A40 OK"), append);
         Assert.IsFalse(append.Contains("APPENDUID"), append);

         simulator.Disconnect();

         CustomAsserts.AssertFolderMessageCount(_settings.PublicFolders.get_ItemByName("Dropbox"), 1);
      }

      [Test]
      public void AppendToUnselectedFolderReturnsThatFoldersValidity()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);
         Assert.IsTrue(simulator.CreateFolder("Other"));

         var status = simulator.Status("Other", "UIDVALIDITY UIDNEXT");
         var uidValidity = GetStatusValue(status, "UIDVALIDITY");
         var uidNext = GetStatusValue(status, "UIDNEXT");

         Assert.IsTrue(simulator.SelectFolder("INBOX"));

         var append = simulator.Append("Other", CreateMessage("other@example.test"));
         Assert.AreEqual(uidNext, AssertAppendUid(append, uidValidity), append);

         simulator.Disconnect();
      }

      [Test]
      [Description("Several clients appending into one folder at the same time must each be told a " +
                   "distinct UID, and it must be the UID the message really got.")]
      public void ConcurrentAppendsReportDistinctUids()
      {
         const int threadCount = 4;
         const int appendsPerThread = 25;

         var account = AddAccount();

         var reported = new List<long>();
         var failures = new List<string>();
         var threads = new List<Thread>();

         for (var t = 0; t < threadCount; t++)
         {
            var threadIndex = t;

            threads.Add(new Thread(() =>
            {
               try
               {
                  var simulator = ConnectAndLogon(account);

                  for (var i = 0; i < appendsPerThread; i++)
                  {
                     var response = simulator.Append("INBOX", CreateMessage("t" + threadIndex + "-" + i + "@example.test"));
                     var match = Regex.Match(response, @"\[APPENDUID \d+ (\d+)\]");

                     lock (reported)
                     {
                        if (match.Success)
                           reported.Add(long.Parse(match.Groups[1].Value));
                        else
                           failures.Add(response);
                     }
                  }

                  simulator.Disconnect();
               }
               catch (Exception ex)
               {
                  lock (reported)
                     failures.Add(ex.ToString());
               }
            }));
         }

         foreach (var thread in threads)
            thread.Start();

         foreach (var thread in threads)
            thread.Join();

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures));
         Assert.AreEqual(threadCount * appendsPerThread, reported.Distinct().Count(), "Duplicate APPENDUID reported.");

         var reader = ConnectAndLogon(account);
         Assert.IsTrue(reader.SelectFolder("INBOX"));
         var actual = UidSearch(reader, "ALL").Split(' ').Select(long.Parse).OrderBy(uid => uid);
         reader.Disconnect();

         CollectionAssert.AreEqual(reported.OrderBy(uid => uid), actual);
      }

      #endregion

      #region COPYUID

      [Test]
      public void CopyReturnsCopyUid()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);
         Assert.IsTrue(simulator.CreateFolder("Target"));

         AppendMessages(simulator, "INBOX", 3);
         AppendMessages(simulator, "Target", 3);

         var uidValidity = GetStatusValue(simulator.Status("Target", "UIDVALIDITY"), "UIDVALIDITY");

         Assert.IsTrue(simulator.SelectFolder("INBOX"));

         var copy = simulator.SendSingleCommand("A01 COPY 1:3 \"Target\"");
         Assert.IsTrue(copy.Contains("A01 OK [COPYUID " + uidValidity + " 1:3 4:6]"), copy);

         var uidCopy = simulator.UidCopy("1,3", "Target");
         Assert.IsTrue(uidCopy.Contains("A41 OK [COPYUID " + uidValidity + " 1,3 7:8]"), uidCopy);

         simulator.Disconnect();
      }

      [Test]
      [Description("The source set is listed in the order the messages were copied, and the " +
                   "destination set corresponds to it.")]
      public void CopyUidSetsCorrespond()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);
         Assert.IsTrue(simulator.CreateFolder("Target"));

         AppendMessages(simulator, "INBOX", 5);

         var uidValidity = GetStatusValue(simulator.Status("Target", "UIDVALIDITY"), "UIDVALIDITY");

         Assert.IsTrue(simulator.SelectFolder("INBOX"));

         var uidCopy = simulator.UidCopy("1:2,4:5", "Target");
         Assert.IsTrue(uidCopy.Contains("A41 OK [COPYUID " + uidValidity + " 1:2,4:5 1:4]"), uidCopy);

         simulator.Disconnect();
      }

      [Test]
      public void CopyOfNoMessagesHasNoCopyUid()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);
         Assert.IsTrue(simulator.CreateFolder("Target"));

         AppendMessages(simulator, "INBOX", 1);
         Assert.IsTrue(simulator.SelectFolder("INBOX"));

         var uidCopy = simulator.UidCopy("100", "Target");
         Assert.IsTrue(uidCopy.Contains("A41 OK"), uidCopy);
         Assert.IsFalse(uidCopy.Contains("COPYUID"), uidCopy);

         simulator.Disconnect();
      }

      [Test]
      public void CopyIntoSameFolderReturnsCopyUid()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);

         AppendMessages(simulator, "INBOX", 2);

         var select = simulator.SendSingleCommand("A01 SELECT INBOX");
         var uidValidity = GetResponseCodeValue(select, "UIDVALIDITY");

         var copy = simulator.SendSingleCommand("A02 COPY 1:2 INBOX");
         Assert.IsTrue(copy.Contains("A02 OK [COPYUID " + uidValidity + " 1:2 3:4]"), copy);

         simulator.Disconnect();
      }

      [Test]
      public void CopyToPublicFolderReturnsItsValidity()
      {
         var account = AddAccount();
         CreatePublicFolder("Share", account);

         var simulator = ConnectAndLogon(account);
         AppendMessages(simulator, "INBOX", 1);

         var uidValidity = GetResponseCodeValue(simulator.SendSingleCommand("A01 SELECT \"#Public.Share\""), "UIDVALIDITY");

         Assert.IsTrue(simulator.SelectFolder("INBOX"));

         var copy = simulator.SendSingleCommand("A02 COPY 1 \"#Public.Share\"");
         Assert.IsTrue(copy.Contains("A02 OK [COPYUID " + uidValidity + " 1 1]"), copy);

         simulator.Disconnect();
      }

      [Test]
      [Description("A client that can't SELECT the folder must not be told its UIDVALIDITY or UIDs (RFC 4315 6).")]
      public void CopyWithoutReadPermissionHasNoCopyUid()
      {
         var account = AddAccount();
         CreatePublicFolder("Dropbox", account, false);

         var simulator = ConnectAndLogon(account);
         AppendMessages(simulator, "INBOX", 2);
         Assert.IsTrue(simulator.SelectFolder("INBOX"));

         var copy = simulator.SendSingleCommand("A01 COPY 1 \"#Public.Dropbox\"");
         Assert.IsTrue(copy.Contains("A01 OK"), copy);
         Assert.IsFalse(copy.Contains("COPYUID"), copy);

         var uidCopy = simulator.UidCopy("2", "#Public.Dropbox");
         Assert.IsTrue(uidCopy.Contains("A41 OK"), uidCopy);
         Assert.IsFalse(uidCopy.Contains("COPYUID"), uidCopy);

         simulator.Disconnect();

         CustomAsserts.AssertFolderMessageCount(_settings.PublicFolders.get_ItemByName("Dropbox"), 2);
      }

      [Test]
      [Description("COPY is atomic (RFC 3501 6.4.7). If one message can't be copied, the copies " +
                   "already made are removed again.")]
      public void CopyFailingPartWayLeavesDestinationUnchanged()
      {
         // 1 MB quota. Two 300 KB messages and one copy fit, a second copy does not.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "uidplus@example.test", Password, 1);
         var simulator = ConnectAndLogon(account);
         Assert.IsTrue(simulator.CreateFolder("Target"));

         var body = string.Concat(Enumerable.Repeat(new string('x', 98) + "\r\n", 3072));

         for (var i = 0; i < 2; i++)
            AssertAppendUid(simulator.Append("INBOX", CreateMessage(Guid.NewGuid() + "@example.test", body)));

         Assert.IsTrue(simulator.SelectFolder("INBOX"));

         var copy = simulator.SendSingleCommand("A01 COPY 1:2 \"Target\"");
         Assert.IsTrue(copy.Contains("A01 NO"), copy);
         Assert.IsFalse(copy.Contains("COPYUID"), copy);

         Assert.AreEqual(0, GetStatusValue(simulator.Status("Target", "MESSAGES"), "MESSAGES"));

         // The removed copy no longer counts against the quota.
         copy = simulator.SendSingleCommand("A02 COPY 1 \"Target\"");
         Assert.IsTrue(copy.Contains("A02 OK"), copy);

         simulator.Disconnect();
      }

      #endregion

      #region UID EXPUNGE

      [Test]
      [Description("UID EXPUNGE removes only the \\Deleted messages in the given set.")]
      public void UidExpungeRemovesOnlyDeletedMessagesInSet()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);

         AppendMessages(simulator, "INBOX", 4);
         Assert.IsTrue(simulator.SelectFolder("INBOX"));

         simulator.SendSingleCommand("A01 UID STORE 1,3 +FLAGS (\\Deleted)");

         var expunge = simulator.UidExpunge("3");
         Assert.IsTrue(expunge.Contains("A42 OK"), expunge);
         Assert.AreEqual(new[] {"3"}, GetExpungedSequences(expunge), expunge);

         // Message 1 is still there and still flagged.
         var flags = simulator.SendSingleCommand("A02 UID FETCH 1 (FLAGS)");
         Assert.IsTrue(flags.Contains("\\Deleted"), flags);

         // An unflagged message in the set is left alone.
         expunge = simulator.UidExpunge("2");
         Assert.IsTrue(expunge.Contains("A42 OK"), expunge);
         Assert.IsEmpty(GetExpungedSequences(expunge), expunge);

         expunge = simulator.UidExpunge("1:*");
         Assert.IsTrue(expunge.Contains("A42 OK"), expunge);
         Assert.AreEqual(new[] {"1"}, GetExpungedSequences(expunge), expunge);

         Assert.AreEqual("2 4", UidSearch(simulator, "ALL"));

         simulator.Disconnect();
      }

      [Test]
      public void UidExpungeIsReportedToOtherSessions()
      {
         var account = AddAccount();

         var setup = ConnectAndLogon(account);
         AppendMessages(setup, "INBOX", 3);
         setup.Disconnect();

         var observer = ConnectAndLogon(account);
         Assert.IsTrue(observer.SelectFolder("INBOX"));

         var expunger = ConnectAndLogon(account);
         Assert.IsTrue(expunger.SelectFolder("INBOX"));
         expunger.SendSingleCommand("A01 UID STORE 1,2 +FLAGS (\\Deleted)");
         var expunge = expunger.UidExpunge("2");
         Assert.AreEqual(new[] {"2"}, GetExpungedSequences(expunge), expunge);
         expunger.Disconnect();

         var noop = observer.NOOP() + observer.NOOP();
         Assert.AreEqual(new[] {"2"}, GetExpungedSequences(noop), noop);
         Assert.AreEqual("1 3", UidSearch(observer, "ALL"));

         observer.Disconnect();
      }

      [Test]
      public void UidExpungeOnExaminedFolderIsRefused()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);

         AppendMessages(simulator, "INBOX", 1);
         Assert.IsTrue(simulator.SelectFolder("INBOX"));
         simulator.SendSingleCommand("A01 UID STORE 1 +FLAGS (\\Deleted)");

         simulator.ExamineFolder("INBOX");

         var expunge = simulator.UidExpunge("1");
         Assert.IsTrue(expunge.Contains("A42 NO"), expunge);

         Assert.IsTrue(simulator.SelectFolder("INBOX"));
         Assert.AreEqual("1", UidSearch(simulator, "ALL"));

         simulator.Disconnect();
      }

      [Test]
      public void UidExpungeRequiresExpungePermission()
      {
         var account = AddAccount();
         var permission = CreatePublicFolder("Share", account);

         var simulator = ConnectAndLogon(account);
         AppendMessages(simulator, "#Public.Share", 1);
         Assert.IsTrue(simulator.SelectFolder("#Public.Share"));
         simulator.SendSingleCommand("A01 UID STORE 1 +FLAGS (\\Deleted)");

         var expunge = simulator.UidExpunge("1");
         Assert.IsFalse(expunge.Contains("A42 OK"), expunge);

         permission.set_Permission(eACLPermission.ePermissionExpunge, true);
         permission.Save();

         expunge = simulator.UidExpunge("1");
         Assert.IsTrue(expunge.Contains("A42 OK"), expunge);
         Assert.AreEqual(new[] {"1"}, GetExpungedSequences(expunge), expunge);

         simulator.Disconnect();
      }

      [Test]
      public void UidExpungeWithoutSetIsRejected()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);
         Assert.IsTrue(simulator.SelectFolder("INBOX"));

         var result = simulator.SendSingleCommand("A01 UID EXPUNGE");
         Assert.IsTrue(result.Contains("A01 BAD"), result);

         simulator.Disconnect();
      }

      // The message set rules are tested in IMAPCommandRangeActionTester. This checks that UID EXPUNGE uses them.
      [Test]
      public void UidExpungeWithMalformedSetIsRejected()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);

         AppendMessages(simulator, "INBOX", 2);
         Assert.IsTrue(simulator.SelectFolder("INBOX"));
         simulator.SendSingleCommand("A01 UID STORE 1:2 +FLAGS (\\Deleted)");

         var expunge = simulator.UidExpunge("1,,2");
         Assert.IsTrue(expunge.Contains("A42 BAD"), expunge);
         Assert.IsEmpty(GetExpungedSequences(expunge), expunge);

         Assert.AreEqual("1 2", UidSearch(simulator, "ALL"));

         simulator.Disconnect();
      }

      [Test]
      [Description("* is the highest UID in the folder (RFC 3501 9).")]
      public void UidExpungeOfStarExpungesLastMessage()
      {
         var account = AddAccount();
         var simulator = ConnectAndLogon(account);

         AppendMessages(simulator, "INBOX", 3);
         Assert.IsTrue(simulator.SelectFolder("INBOX"));
         simulator.SendSingleCommand("A01 UID STORE 1:3 +FLAGS (\\Deleted)");

         var expunge = simulator.UidExpunge("*");
         Assert.IsTrue(expunge.Contains("A42 OK"), expunge);
         Assert.AreEqual(new[] {"3"}, GetExpungedSequences(expunge), expunge);

         simulator.Disconnect();
      }

      [Test]
      [Description("A client saving a draft replaces the old one with APPEND, UID STORE and UID EXPUNGE. " +
                   "A message another client flagged \\Deleted must survive this.")]
      public void ReplacingDraftDoesNotExpungeOtherDeletedMessages()
      {
         var account = AddAccount();

         var setup = ConnectAndLogon(account);
         Assert.IsTrue(setup.CreateFolder("MyDrafts"));
         setup.Append("MyDrafts", CreateMessage("unrelated@example.test"));
         setup.Disconnect();

         var other = ConnectAndLogon(account);
         Assert.IsTrue(other.SelectFolder("MyDrafts"));
         other.SendSingleCommand("A01 STORE 1 +FLAGS (\\Deleted)");

         var client = ConnectAndLogon(account);
         Assert.IsTrue(client.SelectFolder("MyDrafts"));

         var oldUid = AssertAppendUid(client.Append("MyDrafts", CreateMessage("draft-1@example.test")));
         var newUid = AssertAppendUid(client.Append("MyDrafts", CreateMessage("draft-2@example.test")));

         client.SendSingleCommand("A02 UID STORE " + oldUid + " +FLAGS (\\Deleted)");
         var expunge = client.UidExpunge(oldUid.ToString());
         Assert.IsTrue(expunge.Contains("A42 OK"), expunge);
         Assert.AreEqual(new[] {"2"}, GetExpungedSequences(expunge), expunge);

         Assert.AreEqual("1 " + newUid, UidSearch(client, "ALL"));

         other.NOOP();
         Assert.AreEqual("1 " + newUid, UidSearch(other, "ALL"));

         client.Disconnect();
         other.Disconnect();
      }

      #endregion

      #region Helpers

      private Account AddAccount()
      {
         return SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "uidplus@example.test", Password);
      }

      private static ImapClientSimulator ConnectAndLogon(Account account)
      {
         var simulator = new ImapClientSimulator();
         Assert.IsTrue(simulator.ConnectAndLogon(account.Address, Password));
         return simulator;
      }

      private IMAPFolderPermission CreatePublicFolder(string name, Account account, bool read = true)
      {
         var folder = _settings.PublicFolders.Add(name);
         folder.Save();

         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = account.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionLookup, true);
         permission.set_Permission(eACLPermission.ePermissionRead, read);
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.set_Permission(eACLPermission.ePermissionWriteDeleted, true);
         permission.Save();

         return permission;
      }

      private static string CreateMessage(string messageId, string body = "Body\r\n")
      {
         return "Message-ID: <" + messageId + ">\r\n" +
                "Subject: " + messageId + "\r\n" +
                "\r\n" +
                body;
      }

      private static void AppendMessages(ImapClientSimulator simulator, string folderName, int count)
      {
         for (var i = 0; i < count; i++)
         {
            var response = simulator.Append(folderName, CreateMessage(Guid.NewGuid() + "@example.test"));
            Assert.IsTrue(response.Contains("A40 OK"), response);
         }
      }

      private static long AssertAppendUid(string response, long expectedUidValidity = -1)
      {
         var match = Regex.Match(response, @"A40 OK \[APPENDUID (\d+) (\d+)\]");
         Assert.IsTrue(match.Success, "No APPENDUID in response. " + response);

         if (expectedUidValidity >= 0)
            Assert.AreEqual(expectedUidValidity, long.Parse(match.Groups[1].Value), response);

         return long.Parse(match.Groups[2].Value);
      }

      private static long GetResponseCodeValue(string response, string name)
      {
         var match = Regex.Match(response, @"\[" + name + @" (\d+)\]");
         Assert.IsTrue(match.Success, name + " not found. " + response);
         return long.Parse(match.Groups[1].Value);
      }

      private static long GetStatusValue(string response, string name)
      {
         var match = Regex.Match(response, name + @" (\d+)");
         Assert.IsTrue(match.Success, name + " not found. " + response);
         return long.Parse(match.Groups[1].Value);
      }

      private static string[] GetExpungedSequences(string response)
      {
         return Regex.Matches(response, @"^\* (\d+) EXPUNGE", RegexOptions.Multiline)
            .Cast<Match>()
            .Select(match => match.Groups[1].Value)
            .ToArray();
      }

      private static string UidSearch(ImapClientSimulator simulator, string criteria)
      {
         var response = simulator.SendSingleCommand("A43 UID SEARCH " + criteria);
         Assert.IsTrue(response.Contains("A43 OK"), response);

         var match = Regex.Match(response, @"^\* SEARCH ?(.*)\r$", RegexOptions.Multiline);
         Assert.IsTrue(match.Success, response);

         return match.Groups[1].Value.Trim();
      }

      #endregion
   }
}
