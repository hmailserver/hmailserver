using System.IO;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class Append : TestFixtureBase
   {
      [Test]
      [Description("Append a message to an account inbox and make sure it's placed in the right location on disk.")]
      public void ConfirmFileAddedToCorrectAccountFolder()
      {
         var testSetup = SingletonProvider<TestSetup>.Instance;
         var account = testSetup.AddAccount(_domain, "check@example.test", "test");
         var simulator = new ImapClientSimulator();

         // Confirm that the public folder is empty before we start our test.
         var publicDir = GetPublicDirectory();
         CustomAsserts.AssertFilesInDirectory(publicDir, 0);

         // Add a message to the inbox.
         simulator.Connect();
         simulator.LogonWithLiteral("check@example.test", "test");
         simulator.SendSingleCommandWithLiteral("A01 APPEND INBOX {4}", "ABCD");

         // Confirm it exists in the IMAP folder.
         Assert.AreEqual(1, simulator.GetMessageCount("INBOX"));
         simulator.Disconnect();

         // The public directory should still be empty - the message was added to the user account.
         CustomAsserts.AssertFilesInDirectory(publicDir, 0);

         // There should be a single file in the users directory.
         CustomAsserts.AssertFilesInUserDirectory(account, 1);
      }

      [Test]
      [Description("Append a message to an public folder and make sure it's placed in the right location on disk.")]
      public void ConfirmFileAddedToCorrectPublicFolder()
      {
         var testSetup = SingletonProvider<TestSetup>.Instance;
         var account = testSetup.AddAccount(_domain, "check@example.test", "test");
         var simulator = new ImapClientSimulator();

         // Confirm that the public folder is empty before we start our test.
         var publicDir = GetPublicDirectory();
         CustomAsserts.AssertFilesInDirectory(publicDir, 0);

         var folders = _application.Settings.PublicFolders;
         var folder = folders.Add("Share");
         folder.Save();

         // Give everyone access to the folder.
         var permission = folder.Permissions.Add();
         permission.PermissionType = eACLPermissionType.ePermissionTypeAnyone;
         permission.set_Permission(eACLPermission.ePermissionLookup, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.Save();

         // Add the message to the public folder.
         simulator.Connect();
         simulator.LogonWithLiteral("check@example.test", "test");
         simulator.SendSingleCommandWithLiteral("A01 APPEND #Public.Share {4}", "ABCD");

         // Confirm that the message exists in the public folder and not in the inbox.
         Assert.AreEqual(1, simulator.GetMessageCount("#Public.Share"));
         Assert.AreEqual(0, simulator.GetMessageCount("INBOX"));
         simulator.Disconnect();


         // The public directory should now contain the message.
         CustomAsserts.AssertFilesInDirectory(publicDir, 1);

         // There users directory should still be empty.
         CustomAsserts.AssertFilesInUserDirectory(account, 0);
      }

      [Test]
      public void TestAppend()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "check@example.test", "test");
         var simulator = new ImapClientSimulator();

         var sWelcomeMessage = simulator.Connect();
         simulator.LogonWithLiteral("check@example.test", "test");
         simulator.SendSingleCommandWithLiteral("A01 APPEND INBOX {4}", "ABCD");
         Assert.AreEqual(1, simulator.GetMessageCount("INBOX"));
         simulator.Disconnect();
      }

      [Test]
      public void TestAppendWithIncompleteTime()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "check@example.test", "test");
         var simulator = new ImapClientSimulator();

         simulator.Connect();
         simulator.LogonWithLiteral("check@example.test", "test");
         var result = simulator.SendSingleCommandWithLiteral("A01 APPEND INBOX \"01-Jan-2020 12:30 +0000\" {4}", "ABCD");
         StringAssert.Contains("A01 OK", result);
         Assert.AreEqual(1, simulator.GetMessageCount("INBOX"));
         simulator.Disconnect();
      }

      [Test]
      public void TestDomainMaxMessageSizeLimitDisabled()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test", 0);
         var message = new StringBuilder();

         // ~2 kb string
         for (var i = 0; i < 25; i++)
            message.AppendLine(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

         _domain.MaxMessageSize = 0; // 1 kb
         _domain.Save();

         var imapSim = new ImapClientSimulator("test@example.test", "test", "INBOX");
         var result = imapSim.SendSingleCommandWithLiteral("A01 APPEND INBOX {" + message.Length + "}",
            message.ToString());
         imapSim.Logout();

         Assert.IsFalse(result.StartsWith("A01 NO Message size exceeds fixed maximum message size."));
      }

      [Test]
      public void TestDomainMaxMessageSizeLimitEnabled()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test", 0);
         var message = new StringBuilder();

         // ~2 kb string
         for (var i = 0; i < 25; i++)
            message.AppendLine(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

         _domain.MaxMessageSize = 1; // 1 kb
         _domain.Save();

         var imapSim = new ImapClientSimulator("test@example.test", "test", "INBOX");
         var result = imapSim.SendSingleCommandWithLiteral("A01 APPEND INBOX {" + message.Length + "}",
            message.ToString());
         imapSim.Logout();

         Assert.IsTrue(result.StartsWith("A01 NO Message size exceeds fixed maximum message size."));
      }

      [Test]
      public void TestGlobalMaxMessageSizeLimitDisabled()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test", 0);
         var message = new StringBuilder();

         // ~2 kb string
         for (var i = 0; i < 25; i++)
            message.AppendLine(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

         _settings.MaxMessageSize = 0;

         var imapSim = new ImapClientSimulator("test@example.test", "test", "INBOX");
         var result = imapSim.SendSingleCommandWithLiteral("A01 APPEND INBOX {" + message.Length + "}",
            message.ToString());
         imapSim.Logout();

         Assert.IsFalse(result.StartsWith("A01 NO Message size exceeds fixed maximum message size."));
      }

      [Test]
      public void TestGlobalMaxMessageSizeLimitEnabled()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test", 0);
         var message = new StringBuilder();

         // ~2 kb string
         for (var i = 0; i < 25; i++)
            message.AppendLine(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

         _settings.MaxMessageSize = 1;

         var imapSim = new ImapClientSimulator("test@example.test", "test", "INBOX");
         var result = imapSim.SendSingleCommandWithLiteral("A01 APPEND INBOX {" + message.Length + "}",
            message.ToString());
         imapSim.Logout();

         Assert.IsTrue(result.StartsWith("A01 NO Message size exceeds fixed maximum message size."));
      }


      private string GetPublicDirectory()
      {
         var dataDir = _settings.Directories.DataDirectory;
         var publicDir = Path.Combine(dataDir, _settings.PublicFolderDiskName);
         return publicDir;
      }
   }
}