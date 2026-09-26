// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using static RegressionTests.IMAP.UidPlusTesting.UidPlusHelpers;

namespace RegressionTests.IMAP.UidPlusTesting
{
   /// <summary>
   ///    Temporary (temp/uidplustesting): edge cases and gap probes for UIDPLUS, RFC 4315.
   ///    Tests in the "UidPlusGap" category probe suspected gaps and are expected to fail today.
   /// </summary>
   [TestFixture]
   [Category("UidPlusTesting")]
   public class UidPlusEdgeCaseTests : TestFixtureBase
   {
      private Account _account;

      [SetUp]
      public void CreateAccount()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "uidplus@example.test", Password);
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
            var response = session.Append(folder, CreateMessage(id), flags);
            AssertAppendUid(response);
            ids.Add(id);
         }

         return ids;
      }

      #region APPENDUID

      [Test]
      [Description("APPEND to the selected folder reports EXISTS, and the APPENDUID is usable at once.")]
      public void AppendToSelectedFolderReportsExistsAndUidIsUsable()
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 2);
         session.Select("INBOX");

         var id = Guid.NewGuid() + "@example.test";
         var append = session.Append("INBOX", CreateMessage(id));
         var uid = AssertAppendUid(append);

         StringAssert.Contains("* 3 EXISTS", append);
         Assert.AreEqual(3, session.KnownCount);

         Assert.AreEqual(id, FetchMessageIds(session, uid.ToString())[uid]);

         session.AssertNoViolations();
         session.Disconnect();
      }

      [TestCase("INBOX")]
      [TestCase("inbox")]
      [TestCase("Parent.Child")]
      [TestCase("Space Folder")]
      [Description("APPENDUID, SELECT, EXAMINE and STATUS must all report the same UIDVALIDITY.")]
      public void UidValidityIsConsistentAcrossCommands(string folder)
      {
         var session = Connect();

         if (!folder.Equals("INBOX", StringComparison.OrdinalIgnoreCase))
            Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"" + folder + "\"")));

         var appendValidity = long.Parse(ParseAppendUid(session.Append(folder, CreateMessage("a@example.test"))).Groups[1].Value);
         var statusValidity = GetStatusValue(session.Command("STATUS \"" + folder + "\" (UIDVALIDITY)"), "UIDVALIDITY");
         var selectValidity = GetResponseCodeValue(session.Select(folder), "UIDVALIDITY");
         var examineValidity = GetResponseCodeValue(session.Select(folder, true), "UIDVALIDITY");

         Assert.AreEqual(appendValidity, statusValidity);
         Assert.AreEqual(appendValidity, selectValidity);
         Assert.AreEqual(appendValidity, examineValidity);
         Assert.Greater(appendValidity, 0);

         session.Disconnect();
      }

      [Test]
      [Description("UIDs are strictly ascending. Expunging the highest message must not free its UID.")]
      public void AppendAfterExpungingHighestDoesNotReuseUid()
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 2);
         session.Select("INBOX");

         var uid = AssertAppendUid(session.Append("INBOX", CreateMessage("x@example.test"), "\\Deleted"));
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("UID EXPUNGE " + uid)));

         var next = AssertAppendUid(session.Append("INBOX", CreateMessage("y@example.test")));
         Assert.Greater(next, uid);

         session.AssertNoViolations();
         session.Disconnect();
      }

      [Test]
      [Category("UidPlusGap")]
      [Description("GAP probe. UIDVALIDITY is the creation time in seconds. A folder deleted and re-created " +
                   "within the same second restarts its UIDs at 1 but keeps the UIDVALIDITY, so a client " +
                   "caching APPENDUID results maps the new UID 1 to the old message (RFC 3501 2.3.1.1).")]
      public void RecreatedFolderGetsNewUidValidityOrHigherUids()
      {
         var session = Connect();

         // Retry so the delete and re-create land in the same second at least once.
         for (var attempt = 0; attempt < 10; attempt++)
         {
            Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Recreated\"")));
            var first = ParseAppendUid(session.Append("Recreated", CreateMessage("old@example.test")));
            Assert.IsTrue(TrackedImapSession.IsOk(session.Command("DELETE \"Recreated\"")));

            Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Recreated\"")));
            var second = ParseAppendUid(session.Append("Recreated", CreateMessage("new@example.test")));
            Assert.IsTrue(TrackedImapSession.IsOk(session.Command("DELETE \"Recreated\"")));

            var firstValidity = long.Parse(first.Groups[1].Value);
            var secondValidity = long.Parse(second.Groups[1].Value);
            var firstUid = long.Parse(first.Groups[2].Value);
            var secondUid = long.Parse(second.Groups[2].Value);

            Assert.IsTrue(secondValidity > firstValidity || secondUid > firstUid,
               string.Format("Attempt {0}: UIDVALIDITY {1} reused and UID {2} re-issued.", attempt, firstValidity, secondUid));
         }

         session.Disconnect();
      }

      [Test]
      [Category("UidPlusGap")]
      [Description("GAP probe, low. Renaming an older folder over a deleted one gives the name a lower UIDVALIDITY " +
                   "than before. RFC 3501 2.3.1.1 wants it greater. Dovecot 2.3 behaves the same.")]
      public void RenamingOlderFolderOverDeletedOneRaisesUidValidity()
      {
         var session = Connect();

         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Older\"")));
         session.Append("Older", CreateMessage("older@example.test"));

         Thread.Sleep(1500);

         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         var oldValidity = long.Parse(ParseAppendUid(session.Append("Target", CreateMessage("target@example.test"))).Groups[1].Value);

         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("DELETE \"Target\"")));
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("RENAME \"Older\" \"Target\"")));

         var newValidity = GetStatusValue(session.Command("STATUS \"Target\" (UIDVALIDITY)"), "UIDVALIDITY");
         Assert.Greater(newValidity, oldValidity);

         session.Disconnect();
      }

      [Test]
      public void AppendToMissingFolderHasTryCreateAndNoAppendUid()
      {
         var session = Connect();
         var append = session.Append("DoesNotExist", CreateMessage("a@example.test"));

         Assert.IsTrue(TrackedImapSession.IsNo(append), append);
         StringAssert.Contains("[TRYCREATE]", append);
         StringAssert.DoesNotContain("APPENDUID", append);

         session.Disconnect();
      }

      [Test]
      public void AppendOverQuotaHasNoAppendUid()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quota@example.test", Password, 1);
         var session = new TrackedImapSession(account.Address, Password);

         var body = string.Concat(Enumerable.Repeat(new string('x', 98) + "\r\n", 12000));
         var append = session.Append("INBOX", CreateMessage("big@example.test", body));

         Assert.IsTrue(TrackedImapSession.IsNo(append), append);
         StringAssert.DoesNotContain("APPENDUID", append);

         session.Disconnect();
      }

      [Test]
      [Description("A draft replaced from a single session: APPEND with \\Deleted, then UID EXPUNGE of the old one.")]
      public void AppendWithDeletedFlagCanBeUidExpungedFromAnotherSession()
      {
         var observer = Connect("observer");
         AppendMessages(observer, "INBOX", 1);
         observer.Select("INBOX");

         var appender = Connect("appender");
         var uid = AssertAppendUid(appender.Append("INBOX", CreateMessage("draft@example.test"), "\\Deleted"));
         appender.Disconnect();

         // The observer learns about the message and then expunges it by UID, without a NOOP first.
         var expunge = observer.Command("UID EXPUNGE " + uid);
         Assert.IsTrue(TrackedImapSession.IsOk(expunge), expunge);

         observer.Command("NOOP");
         CollectionAssert.AreEqual(new[] {1L}, UidSearchAll(observer));

         observer.AssertNoViolations();
         observer.Disconnect();
      }

      #endregion

      #region COPYUID

      [Test]
      [Description("COPYUID source and destination correspond element by element, in the order the set gives.")]
      public void CopyUidCorrespondsForUnorderedSet()
      {
         var session = Connect();
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         var ids = AppendMessages(session, "INBOX", 5);
         session.Select("INBOX");

         var copy = session.Command("UID COPY 5,1,3 \"Target\"");
         var copyUid = ParseCopyUid(copy);
         Assert.IsNotNull(copyUid, copy);
         Assert.AreEqual(3, copyUid.Source.Count, copy);
         Assert.AreEqual(3, copyUid.Destination.Count, copy);

         session.Select("Target");
         var destinationIds = FetchMessageIds(session, "1:*");

         for (var i = 0; i < copyUid.Source.Count; i++)
            Assert.AreEqual(ids[(int) copyUid.Source[i] - 1], destinationIds[copyUid.Destination[i]], copy);

         session.Disconnect();
      }

      [TestCase("3:1")]
      [TestCase("*:1")]
      [TestCase("1:*")]
      public void CopyUidWithRangeInEitherOrder(string set)
      {
         var session = Connect();
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         AppendMessages(session, "INBOX", 3);
         session.Select("INBOX");

         var copy = session.Command("UID COPY " + set + " \"Target\"");
         StringAssert.IsMatch(@"OK \[COPYUID \d+ 1:3 1:3\]", copy);

         session.Disconnect();
      }

      [TestCase("1,1")]
      [TestCase("1:2,2:3")]
      [TestCase("1,1:3,3")]
      [Category("UidPlusGap")]
      [Description("GAP probe. A set naming a message twice copies it twice. A message set is a set, so each " +
                   "message should be copied once, and COPYUID should not list a source UID twice.")]
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

      [Test]
      [Description("UID COPY skips a message another session expunged (RFC 3501 6.4.8). COPYUID lists only the copies.")]
      public void UidCopySkipsMessageExpungedElsewhere()
      {
         var session = Connect("copier");
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         AppendMessages(session, "INBOX", 3);
         session.Select("INBOX");

         var other = Connect("other");
         other.Select("INBOX");
         other.Command("UID STORE 2 +FLAGS (\\Deleted)");
         Assert.IsTrue(TrackedImapSession.IsOk(other.Command("UID EXPUNGE 2")));
         other.Disconnect();

         var copy = session.Command("UID COPY 1:3 \"Target\"");
         Assert.IsTrue(TrackedImapSession.IsOk(copy), copy);

         var copyUid = ParseCopyUid(copy);
         Assert.IsNotNull(copyUid, copy);
         CollectionAssert.AreEqual(new[] {1L, 3L}, copyUid.Source, copy);
         Assert.AreEqual(2, copyUid.Destination.Count, copy);

         session.AssertNoViolations();
         session.Disconnect();
      }

      [Test]
      [Description("COPY by sequence number of a message expunged elsewhere fails, and copies nothing.")]
      public void CopyBySequenceOfMessageExpungedElsewhereCopiesNothing()
      {
         var session = Connect("copier");
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         AppendMessages(session, "INBOX", 3);
         session.Select("INBOX");

         var other = Connect("other");
         other.Select("INBOX");
         other.Command("UID STORE 2 +FLAGS (\\Deleted)");
         Assert.IsTrue(TrackedImapSession.IsOk(other.Command("UID EXPUNGE 2")));
         other.Disconnect();

         var copy = session.Command("COPY 1:3 \"Target\"");
         Assert.IsTrue(TrackedImapSession.IsNo(copy), copy);
         StringAssert.DoesNotContain("COPYUID", copy);

         Assert.AreEqual(0, GetStatusValue(session.Command("STATUS \"Target\" (MESSAGES)"), "MESSAGES"));

         session.AssertNoViolations();
         session.Disconnect();
      }

      [TestCase("1")]
      [TestCase("999")]
      [Category("UidPlusGap")]
      [Description("GAP probe. A COPY to a missing folder should be NO [TRYCREATE] (RFC 3501 6.4.7). Today it's " +
                   "BAD, and with a set matching no message it is even OK.")]
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
      [Category("UidPlusGap")]
      [Description("GAP probe. Missing Insert permission should give NO, not BAD. BAD means a syntax error.")]
      public void CopyWithoutInsertPermissionIsNo()
      {
         var folder = _settings.PublicFolders.Add("ReadOnlyShare");
         folder.Save();
         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = _account.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionLookup, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.Save();

         var session = Connect();
         AppendMessages(session, "INBOX", 1);
         session.Select("INBOX");

         var copy = session.Command("UID COPY 1 \"#Public.ReadOnlyShare\"");
         Assert.IsTrue(TrackedImapSession.IsNo(copy), copy);
         StringAssert.DoesNotContain("COPYUID", copy);

         session.Disconnect();
      }

      [Test]
      [Category("UidPlusGap")]
      [Description("GAP probe. A session copying into its own selected folder is never told about the copies: " +
                   "notifications skip the session that made them, and COPY does not update its view. So the " +
                   "UIDs COPYUID just reported can't be fetched, and NOOP never reports EXISTS.")]
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
      [Category("UidPlusGap")]
      [Description("GAP probe, follows from the one above. EXPUNGE takes the unreported copies into the view and " +
                   "then reports EXPUNGE for sequence numbers the client never saw.")]
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
      [Category("UidPlusGap")]
      [Description("GAP probe, as above but with UID EXPUNGE of the UID that COPYUID reported.")]
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

      [Test]
      [Description("A folder with many gaps between the UIDs gives a long, non-contiguous COPYUID source set.")]
      public void CopyUidWithManyGaps()
      {
         const int count = 200;

         var session = Connect();
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         var ids = AppendMessages(session, "INBOX", count);
         session.Select("INBOX");

         // Expunge every even UID.
         var even = string.Join(",", Enumerable.Range(1, count / 2).Select(i => (i * 2).ToString()));
         session.Command("UID STORE " + even + " +FLAGS (\\Deleted)");
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("UID EXPUNGE 1:*")));
         Assert.AreEqual(count / 2, session.KnownCount);

         var copy = session.Command("UID COPY 1:* \"Target\"");
         var copyUid = ParseCopyUid(copy);
         Assert.IsNotNull(copyUid, copy);

         CollectionAssert.AreEqual(Enumerable.Range(0, count / 2).Select(i => (long) (i * 2 + 1)), copyUid.Source);
         StringAssert.Contains(" 1:" + count / 2 + "]", copy);

         session.Select("Target");
         var destinationIds = FetchMessageIds(session, "1:*");
         for (var i = 0; i < copyUid.Source.Count; i++)
            Assert.AreEqual(ids[(int) copyUid.Source[i] - 1], destinationIds[copyUid.Destination[i]]);

         session.AssertNoViolations();
         session.Disconnect();
      }

      [Test]
      [Description("Destination UIDs continue above the highest UID ever used, even after the folder was emptied.")]
      public void CopyUidDestinationContinuesAfterEmptiedFolder()
      {
         var session = Connect();
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         AppendMessages(session, "Target", 3, "\\Deleted");
         session.Select("Target");
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("UID EXPUNGE 1:*")));
         Assert.AreEqual(0, session.KnownCount);

         AppendMessages(session, "INBOX", 1);
         session.Select("INBOX");
         var copy = session.Command("UID COPY 1 \"Target\"");
         StringAssert.IsMatch(@"OK \[COPYUID \d+ 1 4\]", copy);

         session.Disconnect();
      }

      [Test]
      [Description("COPY from an EXAMINEd folder is allowed and reports COPYUID.")]
      public void CopyFromExaminedFolderReturnsCopyUid()
      {
         var session = Connect();
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         AppendMessages(session, "INBOX", 2);
         session.Select("INBOX", true);

         var copy = session.Command("UID COPY 1:2 \"Target\"");
         StringAssert.IsMatch(@"OK \[COPYUID \d+ 1:2 1:2\]", copy);

         session.Disconnect();
      }

      [Test]
      [Description("A destination folder name given as a literal.")]
      public void CopyToFolderNamedByLiteralReturnsCopyUid()
      {
         var session = Connect();
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));
         AppendMessages(session, "INBOX", 1);
         session.Select("INBOX");

         var copy = session.Simulator.SendSingleCommandWithLiteral("L01 UID COPY 1 {6}", "Target");
         StringAssert.IsMatch(@"L01 OK \[COPYUID \d+ 1 1\]", copy);

         session.Disconnect();
      }

      [Test]
      [Description("A failed COPY removes its copies again. A session watching the destination must see a " +
                   "consistent stream and end up with nothing.")]
      public void FailedCopyIsConsistentForObserverOfDestination()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quota@example.test", Password, 1);

         var copier = new TrackedImapSession(account.Address, Password, "copier");
         Assert.IsTrue(TrackedImapSession.IsOk(copier.Command("CREATE \"Target\"")));

         var body = string.Concat(Enumerable.Repeat(new string('x', 98) + "\r\n", 3072));
         for (var i = 0; i < 2; i++)
            AssertAppendUid(copier.Append("INBOX", CreateMessage(Guid.NewGuid() + "@example.test", body)));

         var observer = new TrackedImapSession(account.Address, Password, "observer");
         observer.Select("Target");

         copier.Select("INBOX");
         var copy = copier.Command("COPY 1:2 \"Target\"");
         Assert.IsTrue(TrackedImapSession.IsNo(copy), copy);

         observer.Command("NOOP");
         observer.Command("NOOP");
         Assert.AreEqual(0, observer.KnownCount);
         Assert.IsEmpty(UidSearchAll(observer));

         observer.AssertNoViolations();
         copier.AssertNoViolations();
         observer.Disconnect();
         copier.Disconnect();
      }

      #endregion

      #region UID EXPUNGE

      [Test]
      [Description("Several messages expunged by one UID EXPUNGE. Each EXPUNGE accounts for the ones before it.")]
      public void UidExpungeOfSeveralMessagesAdjustsSequenceNumbers()
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 5);
         session.Select("INBOX");

         session.Command("UID STORE 2,3,5 +FLAGS (\\Deleted)");
         var expunge = session.Command("UID EXPUNGE 1:*");

         CollectionAssert.AreEqual(new[] {2, 2, 3}, GetExpungedSequences(expunge), expunge);
         CollectionAssert.AreEqual(new[] {1L, 4L}, UidSearchAll(session));

         session.AssertNoViolations();
         session.Disconnect();
      }

      [Test]
      [Description("Messages appended elsewhere with \\Deleted and not yet reported must get EXISTS before EXPUNGE.")]
      public void UidExpungeOfUnreportedMessageSendsExistsFirst()
      {
         var session = Connect("expunger");
         AppendMessages(session, "INBOX", 2);
         session.Select("INBOX");

         var other = Connect("other");
         var uid = AssertAppendUid(other.Append("INBOX", CreateMessage("new@example.test"), "\\Deleted"));
         other.Disconnect();

         var expunge = session.Command("UID EXPUNGE " + uid);
         Assert.IsTrue(TrackedImapSession.IsOk(expunge), expunge);

         session.AssertNoViolations();
         CollectionAssert.AreEqual(new[] {1L, 2L}, UidSearchAll(session));
         session.Disconnect();
      }

      [Test]
      [Description("A set naming messages already expunged elsewhere. The session is told about those too.")]
      public void UidExpungeWithMessagesExpungedElsewhere()
      {
         var session = Connect("expunger");
         AppendMessages(session, "INBOX", 4);
         session.Select("INBOX");
         session.Command("UID STORE 1:4 +FLAGS (\\Deleted)");

         var other = Connect("other");
         other.Select("INBOX");
         Assert.IsTrue(TrackedImapSession.IsOk(other.Command("UID EXPUNGE 2")));
         other.Disconnect();

         var expunge = session.Command("UID EXPUNGE 1:3");
         Assert.IsTrue(TrackedImapSession.IsOk(expunge), expunge);

         session.Command("NOOP");
         Assert.AreEqual(1, session.KnownCount);
         CollectionAssert.AreEqual(new[] {4L}, UidSearchAll(session));

         session.AssertNoViolations();
         session.Disconnect();
      }

      [Test]
      public void UidExpungeIsCaseInsensitive()
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 1, "\\Deleted");
         session.Select("INBOX");

         var expunge = session.Command("uid expunge 1");
         CollectionAssert.AreEqual(new[] {1}, GetExpungedSequences(expunge), expunge);

         session.Disconnect();
      }

      [Test]
      public void UidExpungeWithoutSelectedFolderIsRefused()
      {
         var session = Connect();
         var expunge = session.Command("UID EXPUNGE 1");
         Assert.IsTrue(TrackedImapSession.IsNo(expunge) || TrackedImapSession.IsBad(expunge), expunge);

         AppendMessages(session, "INBOX", 1, "\\Deleted");
         session.Select("INBOX");
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CLOSE")));

         expunge = session.Command("UID EXPUNGE 1");
         Assert.IsFalse(TrackedImapSession.IsOk(expunge), expunge);

         session.Disconnect();
      }

      [TestCase("1, 2")]
      [TestCase("1 2")]
      [TestCase("1:2:3")]
      [TestCase("4294967296")]
      [TestCase("-1")]
      [TestCase("1,")]
      [TestCase("*:")]
      [Description("Malformed sets are rejected, and nothing is expunged.")]
      public void UidExpungeWithMalformedSetExpungesNothing(string set)
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 2, "\\Deleted");
         session.Select("INBOX");

         var expunge = session.Command("UID EXPUNGE " + set);
         Assert.IsTrue(TrackedImapSession.IsBad(expunge), expunge);
         Assert.IsEmpty(GetExpungedSequences(expunge), expunge);
         CollectionAssert.AreEqual(new[] {1L, 2L}, UidSearchAll(session));

         session.Disconnect();
      }

      [TestCase("0")]
      [TestCase("0:1")]
      [TestCase("(1)")]
      [Category("UidPlusGap")]
      [Description("GAP probe. 0 is not a valid UID (nz-number, RFC 3501 9) and a sequence-set is an atom, " +
                   "not a quoted string. These should be BAD. Dovecot rejects 0 and (1), but accepts \"1\".")]
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

      [TestCase("1,1", new[] {1})]
      [TestCase("2:1", new[] {1, 1})]
      [TestCase("*:1", new[] {1, 1})]
      [TestCase("4294967295", new int[0])]
      [TestCase("3:4294967295", new int[0])]
      public void UidExpungeWithUnusualValidSets(string set, int[] expected)
      {
         var session = Connect();
         AppendMessages(session, "INBOX", 2, "\\Deleted");
         session.Select("INBOX");

         var expunge = session.Command("UID EXPUNGE " + set);
         Assert.IsTrue(TrackedImapSession.IsOk(expunge), expunge);
         CollectionAssert.AreEqual(expected, GetExpungedSequences(expunge), expunge);

         session.AssertNoViolations();
         session.Disconnect();
      }

      [Test]
      [Category("UidPlusGap")]
      [Description("GAP probe. Missing Expunge permission should give NO, not BAD.")]
      public void UidExpungeWithoutPermissionIsNo()
      {
         var folder = _settings.PublicFolders.Add("Share");
         folder.Save();
         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = _account.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionLookup, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.set_Permission(eACLPermission.ePermissionWriteDeleted, true);
         permission.Save();

         var session = Connect();
         AppendMessages(session, "#Public.Share", 1, "\\Deleted");
         session.Select("#Public.Share");

         var expunge = session.Command("UID EXPUNGE 1");
         Assert.IsTrue(TrackedImapSession.IsNo(expunge), expunge);

         session.Disconnect();
      }

      [Test]
      [Description("An IDLE session sees UID EXPUNGE from another session straight away, with valid numbers.")]
      public void UidExpungeIsReportedToIdleSession()
      {
         var setup = Connect("setup");
         AppendMessages(setup, "INBOX", 3);
         setup.Disconnect();

         var idler = new ImapClientSimulator();
         Assert.IsTrue(idler.ConnectAndLogon(_account.Address, Password));
         Assert.IsTrue(idler.SelectFolder("INBOX"));
         Assert.IsTrue(idler.StartIdle());

         var expunger = Connect("expunger");
         expunger.Select("INBOX");
         expunger.Command("UID STORE 1,3 +FLAGS (\\Deleted)");
         Assert.IsTrue(TrackedImapSession.IsOk(expunger.Command("UID EXPUNGE 3")));

         Assert.IsTrue(idler.AssertPendingDataExists(), "IDLE session was not notified.");

         string output;
         Assert.IsTrue(idler.EndIdle(false, out output), output);
         CollectionAssert.AreEqual(new[] {3}, GetExpungedSequences(output), output);
         StringAssert.Contains("* 2 EXISTS", output);

         expunger.Disconnect();
         idler.Disconnect();
      }

      [Test]
      [Description("A long UID set with many parts on a large folder.")]
      public void UidExpungeWithLongSet()
      {
         const int count = 500;

         var session = Connect();
         AppendMessages(session, "INBOX", count, "\\Deleted");
         session.Select("INBOX");

         // Every third UID, with some UIDs that don't exist, written out one by one.
         var uids = Enumerable.Range(1, count + 100).Where(i => i % 3 == 0).ToList();
         var expunge = session.Command("UID EXPUNGE " + string.Join(",", uids));
         Assert.IsTrue(TrackedImapSession.IsOk(expunge), expunge);

         var expected = uids.Count(uid => uid <= count);
         Assert.AreEqual(expected, GetExpungedSequences(expunge).Length);
         Assert.AreEqual(count - expected, session.KnownCount);

         session.AssertNoViolations();
         session.Disconnect();
      }

      [Test]
      [Description("UID EXPUNGE must not touch \\Deleted messages outside the set, even ones other sessions flagged.")]
      public void UidExpungeLeavesOtherDeletedMessages()
      {
         var session = Connect("expunger");
         AppendMessages(session, "INBOX", 4);
         session.Select("INBOX");

         var other = Connect("other");
         other.Select("INBOX");
         other.Command("UID STORE 1:4 +FLAGS (\\Deleted)");

         var expunge = session.Command("UID EXPUNGE 2");
         CollectionAssert.AreEqual(new[] {2}, GetExpungedSequences(expunge), expunge);

         other.Command("NOOP");
         CollectionAssert.AreEqual(new[] {1L, 3L, 4L}, UidSearchAll(other));

         session.AssertNoViolations();
         other.AssertNoViolations();
         session.Disconnect();
         other.Disconnect();
      }

      #endregion
   }
}
