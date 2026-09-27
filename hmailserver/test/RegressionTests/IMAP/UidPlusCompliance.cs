// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.RegularExpressions;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using static RegressionTests.Shared.UidPlusHelpers;

namespace RegressionTests.IMAP
{
   /// <summary>
   ///    RFC requirements around COPY, message sets and UIDPLUS that hMailServer once got wrong.
   ///    TrackedImapSession checks that every response only uses message numbers the client knows.
   /// </summary>
   [TestFixture]
   public class UidPlusCompliance : TestFixtureBase
   {
      private Account _account;

      [SetUp]
      public void CreateAccount()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "uidplus@example.test", Password);
      }

      #region COPY

      [TestCase("1,1")]
      [TestCase("1:2,2:3")]
      [TestCase("1,1:3,3")]
      [Description("A message set is a set. A message named twice is copied once, and COPYUID lists its UID once.")]
      public void CopyWithDuplicatesInSetCopiesEachMessageOnce(string set)
      {
         var session = Connect();
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         AppendMessages(session, "INBOX", 3);
         session.Select("INBOX");

         var copy = session.Command("UID COPY " + set + " \"Target\"");
         Assert.IsTrue(TrackedImapSession.IsOk(copy), copy);

         var copyUid = ParseCopyUid(copy);
         Assert.IsNotNull(copyUid, copy);
         CollectionAssert.AllItemsAreUnique(copyUid.Source, copy);

         var expected = copyUid.Source.Count;
         Assert.AreEqual(expected, GetStatusValue(session.Command("STATUS \"Target\" (MESSAGES)"), "MESSAGES"));

         session.Disconnect();
      }

      [TestCase("1")]
      [TestCase("999")]
      [Description("COPY to a missing folder is NO [TRYCREATE] (RFC 3501 6.4.7), also when the set matches nothing.")]
      public void CopyToMissingFolderIsNoTryCreate(string set)
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 1);
         session.Select("INBOX");

         var copy = session.Command("UID COPY " + set + " \"DoesNotExist\"");
         Assert.IsTrue(TrackedImapSession.IsNo(copy), copy);
         StringAssert.Contains("[TRYCREATE]", copy);

         session.Disconnect();
      }

      [Test]
      [Description("A session copying into its own selected folder is told about the copies (RFC 3501 5.2), so " +
                   "the UIDs COPYUID reports can be fetched.")]
      public void CopyIntoSelectedFolderIsReportedToSameSession()
      {
         var session = Connect();
         var ids = AppendMessages(session, "INBOX", 2);
         session.Select("INBOX");

         var copy = session.Command("UID COPY 1 INBOX");
         var copyUid = ParseCopyUid(copy);
         Assert.IsNotNull(copyUid, copy);

         var noop = session.Command("NOOP");
         Assert.AreEqual(3, session.KnownCount, copy + noop);

         var fetched = FetchMessageIds(session, copyUid.Destination[0].ToString());
         Assert.IsTrue(fetched.ContainsKey(copyUid.Destination[0]), "COPYUID destination UID can't be fetched.");
         Assert.AreEqual(ids[0], fetched[copyUid.Destination[0]]);

         session.AssertNoViolations();
         session.Disconnect();
      }

      [Test]
      [Description("After copying into the selected folder, EXPUNGE only uses sequence numbers the client knows.")]
      public void ExpungeAfterCopyIntoSelectedFolderUsesKnownSequenceNumbers()
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 2);
         session.Select("INBOX");

         session.Command("UID STORE 2 +FLAGS (\\Deleted)");

         // The copy keeps \Deleted.
         var copy = session.Command("UID COPY 2 INBOX");
         var copyUid = ParseCopyUid(copy);
         Assert.IsNotNull(copyUid, copy);

         var expunge = session.Command("EXPUNGE");
         Assert.IsTrue(TrackedImapSession.IsOk(expunge), expunge);

         session.AssertNoViolations();
         CollectionAssert.AreEqual(new[] {1L}, UidSearchAll(session));

         session.Disconnect();
      }

      [Test]
      [Description("As above, but with UID EXPUNGE of the UID that COPYUID reported.")]
      public void UidExpungeOfCopyUidInSelectedFolderUsesKnownSequenceNumbers()
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 2);
         session.Select("INBOX");

         session.Command("UID STORE 2 +FLAGS (\\Deleted)");
         var copyUid = ParseCopyUid(session.Command("UID COPY 2 INBOX"));
         Assert.IsNotNull(copyUid);
         session.Command("UID STORE 2 -FLAGS (\\Deleted)");

         var expunge = session.Command("UID EXPUNGE " + copyUid.Destination[0]);
         Assert.IsTrue(TrackedImapSession.IsOk(expunge), expunge);

         session.AssertNoViolations();
         session.Disconnect();
      }

      #endregion

      #region Message sets

      [TestCase("0")]
      [TestCase("0:1")]
      [TestCase("(1)")]
      [Description("0 is not a valid UID (nz-number, RFC 3501 9), and a sequence set is never parenthesized.")]
      public void UidExpungeWithInvalidTokenIsBad(string set)
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 2, "\\Deleted");
         session.Select("INBOX");

         var expunge = session.Command("UID EXPUNGE " + set);
         Assert.IsTrue(TrackedImapSession.IsBad(expunge), expunge);
         CollectionAssert.AreEqual(new[] {1L, 2L}, UidSearchAll(session));

         session.Disconnect();
      }

      [TestCase("1:*")]
      [TestCase("2,5")]
      [TestCase("*")]
      [TestCase("2,*")]
      [TestCase("*:4")]
      [TestCase("4:2")]
      [TestCase("100:*")]
      [Description("UID SEARCH UID resolves sets like UID FETCH: * is the highest UID, ranges may be reversed, " +
                   "and N:* always includes the highest (RFC 3501 9).")]
      public void UidSearchUidAgreesWithUidFetch(string set)
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 6);
         session.Select("INBOX");

         // Gaps between the UIDs.
         session.Command("UID STORE 3 +FLAGS.SILENT (\\Deleted)");
         session.Command("UID EXPUNGE 3");

         CollectionAssert.AreEqual(UidFetch(session, set), UidSearch(session, "UID " + set), "UID SEARCH UID " + set);

         session.Disconnect();
      }

      [TestCase("2:*")]
      [TestCase("*")]
      [TestCase("3:1")]
      [Description("The same for a sequence set given as a SEARCH key.")]
      public void SearchSequenceSetAgreesWithFetch(string set)
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 4);
         session.Select("INBOX");

         var fetch = session.Command("FETCH " + set + " (UID)");
         var fetched = Regex.Matches(fetch, @"UID (\d+)").Cast<Match>().Select(m => long.Parse(m.Groups[1].Value)).OrderBy(u => u).ToList();

         CollectionAssert.AreEqual(fetched, UidSearch(session, set), "UID SEARCH " + set);

         session.Disconnect();
      }

      #endregion

      [Test]
      [Description("QUOTA is advertised, so a quota failure carries [OVERQUOTA] (RFC 9208 5.3, RFC 5530).")]
      public void QuotaFailuresCarryOverQuota()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quota@example.test", Password, 1);
         var session = new TrackedImapSession(account.Address, Password);
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));

         var body = string.Concat(Enumerable.Repeat(new string('x', 98) + "\r\n", 3072));
         for (var i = 0; i < 2; i++)
            AssertAppendUid(session.Append("INBOX", CreateMessage(Guid.NewGuid() + "@example.test", body)));

         var append = session.Append("INBOX", CreateMessage("over@example.test", body + body));
         StringAssert.Contains("NO [OVERQUOTA]", append);

         session.Select("INBOX");
         var copy = session.Command("COPY 1:2 \"Target\"");
         StringAssert.Contains("NO [OVERQUOTA]", copy);

         session.Disconnect();
      }

      private TrackedImapSession Connect(string name = "session")
      {
         return new TrackedImapSession(_account.Address, Password, name);
      }

      private static List<string> AppendMessages(TrackedImapSession session, string folder, int count, string flags = null)
      {
         var ids = new List<string>();

         for (var i = 0; i < count; i++)
         {
            var id = Guid.NewGuid() + "@example.test";
            AssertAppendUid(session.Append(folder, CreateMessage(id), flags));
            ids.Add(id);
         }

         return ids;
      }

      private static List<long> UidSearch(TrackedImapSession session, string criteria)
      {
         var response = session.Command("UID SEARCH " + criteria);
         Assert.IsTrue(TrackedImapSession.IsOk(response), response);
         var match = Regex.Match(response, @"^\* SEARCH ?(.*)\r$", RegexOptions.Multiline);
         Assert.IsTrue(match.Success, response);
         return match.Groups[1].Value.Split(new[] {' '}, StringSplitOptions.RemoveEmptyEntries).Select(long.Parse).OrderBy(u => u).ToList();
      }

      private static List<long> UidFetch(TrackedImapSession session, string set)
      {
         var response = session.Command("UID FETCH " + set + " (UID)");
         Assert.IsTrue(TrackedImapSession.IsOk(response), response);
         return Regex.Matches(response, @"UID (\d+)\)").Cast<Match>().Select(m => long.Parse(m.Groups[1].Value)).Distinct().OrderBy(u => u).ToList();
      }
   }
}
