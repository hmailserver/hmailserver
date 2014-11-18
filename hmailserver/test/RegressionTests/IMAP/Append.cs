using System.IO;
using System.Text;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class Append : TestFixtureBase
   {
      [Test]
      [Description("Append a message to an account inbox and make sure it's placed in the right location on disk.")]
      public void ConfirmFileAddedToCorrectAccountFolder()
      {
         TestSetup testSetup = SingletonProvider<TestSetup>.Instance;
         Account oAccount = testSetup.AddAccount(_domain, "check@test.com", "test");
         var oSimulator = new ImapClientSimulator();

         // Confirm that the public folder is empty before we start our test.
         string publicDir = GetPublicDirectory();
         CustomAsserts.AssertFilesInDirectory(publicDir, 0);

         // Add a message to the inbox.
         oSimulator.Connect();
         oSimulator.LogonWithLiteral("check@test.com", "test");
         oSimulator.SendSingleCommandWithLiteral("A01 APPEND INBOX {4}", "ABCD");

         // Confirm it exists in the IMAP folder.
         Assert.AreEqual(1, oSimulator.GetMessageCount("INBOX"));
         oSimulator.Disconnect();

         // The public directory should still be empty - the message was added to the user account.
         CustomAsserts.AssertFilesInDirectory(publicDir, 0);

         // There should be a single file in the users directory.
         CustomAsserts.AssertFilesInUserDirectory(oAccount, 1);
      }

      [Test]
      [Description("Append a message to an public folder and make sure it's placed in the right location on disk.")]
      public void ConfirmFileAddedToCorrectPublicFolder()
      {
         TestSetup testSetup = SingletonProvider<TestSetup>.Instance;
         Account oAccount = testSetup.AddAccount(_domain, "check@test.com", "test");
         var oSimulator = new ImapClientSimulator();

         // Confirm that the public folder is empty before we start our test.
         string publicDir = GetPublicDirectory();
         CustomAsserts.AssertFilesInDirectory(publicDir, 0);

         IMAPFolders folders = _application.Settings.PublicFolders;
         IMAPFolder folder = folders.Add("Share");
         folder.Save();

         // Give everyone access to the folder.
         IMAPFolderPermission permission = folder.Permissions.Add();
         permission.PermissionType = eACLPermissionType.ePermissionTypeAnyone;
         permission.set_Permission(eACLPermission.ePermissionLookup, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.Save();

         // Add the message to the public folder.
         oSimulator.Connect();
         oSimulator.LogonWithLiteral("check@test.com", "test");
         oSimulator.SendSingleCommandWithLiteral("A01 APPEND #Public.Share {4}", "ABCD");

         // Confirm that the message exists in the public folder and not in the inbox.
         Assert.AreEqual(1, oSimulator.GetMessageCount("#Public.Share"));
         Assert.AreEqual(0, oSimulator.GetMessageCount("INBOX"));
         oSimulator.Disconnect();


         // The public directory should now contain the message.
         CustomAsserts.AssertFilesInDirectory(publicDir, 1);

         // There users directory should still be empty.
         CustomAsserts.AssertFilesInUserDirectory(oAccount, 0);
      }

      [Test]
      public void TestAppend()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "check@test.com", "test");
         var oSimulator = new ImapClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.LogonWithLiteral("check@test.com", "test");
         oSimulator.SendSingleCommandWithLiteral("A01 APPEND INBOX {4}", "ABCD");
         Assert.AreEqual(1, oSimulator.GetMessageCount("INBOX"));
         oSimulator.Disconnect();
      }

      [Test]
      public void TestDomainMaxMessageSizeLimitDisabled()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test", 0);
         var message = new StringBuilder();

         // ~2 kb string
         for (int i = 0; i < 25; i++)
            message.AppendLine(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

         _domain.MaxMessageSize = 0; // 1 kb
         _domain.Save();

         var imapSim = new ImapClientSimulator("test@test.com", "test", "INBOX");
         string result = imapSim.SendSingleCommandWithLiteral("A01 APPEND INBOX {" + message.Length + "}",
                                                              message.ToString());
         imapSim.Logout();

         Assert.IsFalse(result.StartsWith("A01 NO Message size exceeds fixed maximum message size."));
      }

      [Test]
      public void TestDomainMaxMessageSizeLimitEnabled()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test", 0);
         var message = new StringBuilder();

         // ~2 kb string
         for (int i = 0; i < 25; i++)
            message.AppendLine(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

         _domain.MaxMessageSize = 1; // 1 kb
         _domain.Save();

         var imapSim = new ImapClientSimulator("test@test.com", "test", "INBOX");
         string result = imapSim.SendSingleCommandWithLiteral("A01 APPEND INBOX {" + message.Length + "}",
                                                              message.ToString());
         imapSim.Logout();

         Assert.IsTrue(result.StartsWith("A01 NO Message size exceeds fixed maximum message size."));
      }

      [Test]
      public void TestGlobalMaxMessageSizeLimitDisabled()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test", 0);
         var message = new StringBuilder();

         // ~2 kb string
         for (int i = 0; i < 25; i++)
            message.AppendLine(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

         _settings.MaxMessageSize = 0;

         var imapSim = new ImapClientSimulator("test@test.com", "test", "INBOX");
         string result = imapSim.SendSingleCommandWithLiteral("A01 APPEND INBOX {" + message.Length + "}",
                                                              message.ToString());
         imapSim.Logout();

         Assert.IsFalse(result.StartsWith("A01 NO Message size exceeds fixed maximum message size."));
      }

      [Test]
      public void TestGlobalMaxMessageSizeLimitEnabled()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test", 0);
         var message = new StringBuilder();

         // ~2 kb string
         for (int i = 0; i < 25; i++)
            message.AppendLine(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890");

         _settings.MaxMessageSize = 1;

         var imapSim = new ImapClientSimulator("test@test.com", "test", "INBOX");
         string result = imapSim.SendSingleCommandWithLiteral("A01 APPEND INBOX {" + message.Length + "}",
                                                              message.ToString());
         imapSim.Logout();

         Assert.IsTrue(result.StartsWith("A01 NO Message size exceeds fixed maximum message size."));
      }


      private string GetPublicDirectory()
      {
         string dataDir = _settings.Directories.DataDirectory;
         string publicDir = Path.Combine(dataDir, _settings.PublicFolderDiskName);
         return publicDir;
      }
   }
}