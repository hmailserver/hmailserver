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
using static RegressionTests.Shared.UidPlusHelpers;

namespace RegressionTests.Details.IMAP
{
   /// <summary>
   ///    Requirements from RFC 4315 and the RFCs it touches (3501, 4314, 5530, 9208), one test per
   ///    requirement. Explicit, see README.md. The fixed gaps are covered by IMAP.UidPlusCompliance.
   /// </summary>
   [TestFixture]
   [Explicit("Detail test - run manually.")]
   [Category("Details")]
   public class UidPlusRfcTests : TestFixtureBase
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

      private static void Append(TrackedImapSession session, string folder, int count, string flags = null)
      {
         for (var i = 0; i < count; i++)
            AssertAppendUid(session.Append(folder, CreateMessage(Guid.NewGuid() + "@example.test"), flags));
      }

      private IMAPFolderPermission CreatePublicFolder(string name, bool writeDeleted, bool expunge = false)
      {
         var folder = _settings.PublicFolders.Add(name);
         folder.Save();

         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = _account.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionLookup, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.set_Permission(eACLPermission.ePermissionWriteDeleted, writeDeleted);
         permission.set_Permission(eACLPermission.ePermissionExpunge, expunge);
         permission.Save();

         return permission;
      }

      #region RFC 3501 2.3.1.1 / 7.3.1: UIDNEXT and UIDVALIDITY agree with APPENDUID and COPYUID

      [Test]
      [Description("After an APPEND in any session, STATUS and SELECT report UIDNEXT above the reported UID.")]
      public void UidNextFollowsAppendUidAcrossSessions()
      {
         var watcher = Connect("watcher");
         var appender = Connect("appender");

         var uid = AssertAppendUid(appender.Append("INBOX", CreateMessage("a@example.test")));

         Assert.AreEqual(uid + 1, GetStatusValue(watcher.Command("STATUS INBOX (UIDNEXT)"), "UIDNEXT"));
         Assert.AreEqual(uid + 1, GetResponseCodeValue(watcher.Select("INBOX"), "UIDNEXT"));

         watcher.Disconnect();
         appender.Disconnect();
      }

      [Test]
      public void UidNextFollowsAppendUidInPublicFolder()
      {
         CreatePublicFolder("Share", true);

         var watcher = Connect("watcher");
         var appender = Connect("appender");

         var uid = AssertAppendUid(appender.Append("#Public.Share", CreateMessage("a@example.test")));
         Assert.AreEqual(uid + 1, GetStatusValue(watcher.Command("STATUS \"#Public.Share\" (UIDNEXT)"), "UIDNEXT"));

         watcher.Disconnect();
         appender.Disconnect();
      }

      [Test]
      [Description("UIDNEXT after COPY, and after a COPY that failed and was rolled back. A rolled-back UID must " +
                   "never be handed out again.")]
      public void UidNextFollowsCopyUidAndRollback()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quota@example.test", Password, 1);
         var session = new TrackedImapSession(account.Address, Password);
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Target\"")));

         var body = string.Concat(Enumerable.Repeat(new string('x', 98) + "\r\n", 3072));
         for (var i = 0; i < 2; i++)
            AssertAppendUid(session.Append("INBOX", CreateMessage(Guid.NewGuid() + "@example.test", body)));

         session.Select("INBOX");

         // The first message is copied, then the second fails and the copy is removed again.
         Assert.IsTrue(TrackedImapSession.IsNo(session.Command("COPY 1:2 \"Target\"")));
         var uidNext = GetStatusValue(session.Command("STATUS \"Target\" (UIDNEXT)"), "UIDNEXT");
         Assert.GreaterOrEqual(uidNext, 2, "The rolled-back UID 1 may be handed out again.");

         var copyUid = ParseCopyUid(session.Command("COPY 1 \"Target\""));
         Assert.IsNotNull(copyUid);
         Assert.AreEqual(uidNext, copyUid.Destination[0]);
         Assert.AreEqual(uidNext + 1, GetStatusValue(session.Command("STATUS \"Target\" (UIDNEXT)"), "UIDNEXT"));

         session.Disconnect();
      }

      [Test]
      [Description("UIDVALIDITY is kept across a restart, so cached APPENDUID/COPYUID results stay valid.")]
      public void UidValiditySurvivesServerRestart()
      {
         var session = Connect();
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Restart\"")));
         var before = long.Parse(ParseAppendUid(session.Append("Restart", CreateMessage("a@example.test"))).Groups[1].Value);
         session.Disconnect();

         _application.Stop();
         _application.Start();

         session = Connect();
         Assert.AreEqual(before, GetStatusValue(session.Command("STATUS \"Restart\" (UIDVALIDITY)"), "UIDVALIDITY"));
         var after = ParseAppendUid(session.Append("Restart", CreateMessage("b@example.test")));
         Assert.AreEqual(before, long.Parse(after.Groups[1].Value));
         Assert.AreEqual(2, long.Parse(after.Groups[2].Value));
         session.Disconnect();
      }

      [Test]
      [Description("RENAME keeps the messages. Either UIDVALIDITY is kept and the UIDs still name the same " +
                   "messages, or UIDVALIDITY changes.")]
      public void RenameKeepsUidsOrChangesUidValidity()
      {
         var session = Connect();
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("CREATE \"Before\"")));
         var append = ParseAppendUid(session.Append("Before", CreateMessage("renamed@example.test")));
         Assert.IsTrue(TrackedImapSession.IsOk(session.Command("RENAME \"Before\" \"After\"")));

         var validity = GetResponseCodeValue(session.Select("After"), "UIDVALIDITY");
         if (validity == long.Parse(append.Groups[1].Value))
         {
            var uid = long.Parse(append.Groups[2].Value);
            Assert.AreEqual("renamed@example.test", FetchMessageIds(session, uid.ToString())[uid]);
         }

         session.Disconnect();
      }

      #endregion

      #region RFC 3501 5.2 / 7.3.1: EXISTS for APPEND to the selected folder

      [Test]
      [Description("APPEND to a folder selected with EXAMINE still reports EXISTS, and the new UID can be fetched.")]
      public void AppendToExaminedFolderReportsExists()
      {
         var session = Connect();
         Append(session, "INBOX", 1);
         session.Select("INBOX", true);

         var append = session.Append("INBOX", CreateMessage("examined@example.test"));
         var uid = AssertAppendUid(append);
         Assert.AreEqual(2, session.KnownCount, append);
         Assert.AreEqual("examined@example.test", FetchMessageIds(session, uid.ToString())[uid]);

         session.AssertNoViolations();
         session.Disconnect();
      }

      #endregion

      #region RFC 4314 4: flags set by APPEND and COPY need the matching right

      [Test]
      [Description("Without the 't' right (WriteDeleted), APPEND must not set \\Deleted. Otherwise a " +
                   "draft-replacing client could plant messages that an Expunge-capable user removes with the next EXPUNGE.")]
      public void AppendWithoutWriteDeletedRightDoesNotSetDeleted()
      {
         CreatePublicFolder("NoDelete", false);

         var session = Connect();
         var uid = AssertAppendUid(session.Append("#Public.NoDelete", CreateMessage("a@example.test"), "\\Deleted"));
         session.Select("#Public.NoDelete");

         var flags = session.Command("UID FETCH " + uid + " (FLAGS)");
         StringAssert.DoesNotContain("\\Deleted", flags);

         session.Disconnect();
      }

      [Test]
      [Description("The same for COPY of a \\Deleted message.")]
      public void CopyWithoutWriteDeletedRightDoesNotSetDeleted()
      {
         CreatePublicFolder("NoDelete", false);

         var session = Connect();
         Append(session, "INBOX", 1, "\\Deleted");
         session.Select("INBOX");

         var copyUid = ParseCopyUid(session.Command("UID COPY 1 \"#Public.NoDelete\""));
         Assert.IsNotNull(copyUid);

         session.Select("#Public.NoDelete");
         var flags = session.Command("UID FETCH " + copyUid.Destination[0] + " (FLAGS)");
         StringAssert.DoesNotContain("\\Deleted", flags);

         session.Disconnect();
      }

      #endregion

      #region RFC 5530 / RFC 9208: response codes on failure

      [TestCase("APPEND")]
      [TestCase("COPY")]
      [TestCase("UID EXPUNGE")]
      [Description("An ACL denial is NO [NOPERM] (RFC 5530 3, RFC 4314 4), not BAD, which would mean a syntax error.")]
      public void AclDenialIsNoPerm(string command)
      {
         var folder = _settings.PublicFolders.Add("Locked");
         folder.Save();
         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = _account.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionLookup, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.Save();

         var session = Connect();
         Append(session, "INBOX", 1, "\\Deleted");

         string response;
         switch (command)
         {
            case "APPEND":
               response = session.Append("#Public.Locked", CreateMessage("a@example.test"));
               break;
            case "COPY":
               session.Select("INBOX");
               response = session.Command("UID COPY 1 \"#Public.Locked\"");
               break;
            default:
               session.Select("#Public.Locked");
               response = session.Command("UID EXPUNGE 1:*");
               break;
         }

         StringAssert.IsMatch(@" NO \[NOPERM\]", response);
         session.Disconnect();
      }

      #endregion

      #region RFC 3501 6.4.7 / 6.4.8: COPY errors are NO, never BAD

      [Test]
      [Description("A source message expunged by another session while UID COPY is running must be skipped or " +
                   "give NO, never BAD.")]
      public void UidCopyRacingWithExpungeIsNeverBad()
      {
         var setup = Connect("setup");
         Assert.IsTrue(TrackedImapSession.IsOk(setup.Command("CREATE \"Target\"")));
         setup.Disconnect();

         var failures = new List<string>();
         var deadline = DateTime.UtcNow + TimeSpan.FromSeconds(20);

         var copier = new Thread(() =>
         {
            try
            {
               var session = Connect("copier");
               session.Select("INBOX");
               while (DateTime.UtcNow < deadline)
               {
                  session.Command("NOOP");
                  var copy = session.Command("UID COPY 1:* \"Target\"");
                  if (TrackedImapSession.IsBad(copy))
                     lock (failures) failures.Add(copy);
               }
               lock (failures) failures.AddRange(session.Violations);
               session.Disconnect();
            }
            catch (Exception ex)
            {
               lock (failures) failures.Add(ex.ToString());
            }
         });

         var churner = new Thread(() =>
         {
            try
            {
               var session = Connect("churner");
               session.Select("INBOX");
               while (DateTime.UtcNow < deadline)
               {
                  Append(session, "INBOX", 10, "\\Deleted");
                  session.Command("UID EXPUNGE 1:*");
               }
               session.Disconnect();
            }
            catch (Exception ex)
            {
               lock (failures) failures.Add(ex.ToString());
            }
         });

         copier.Start();
         churner.Start();
         copier.Join();
         churner.Join();

         Assert.IsEmpty(failures, string.Join(Environment.NewLine, failures.Take(10)));
      }

      #endregion
   }
}
