using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.IMAP.Commands
{
   [TestFixture]
   public class Copy : TestFixtureBase
   {
      [Test]
      public void TestCopySingleMessage()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");


         for (int i = 0; i < 2; i++)
            SmtpClientSimulator.StaticSend("test@test.com", account.Address, "Test", "test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 2);

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon("test@test.com", "test");
         simulator.CreateFolder("SomeFolder");

         simulator.SelectFolder("Inbox");

         var response = simulator.SendSingleCommand("A01 COPY 1 SomeFolder");
         StringAssert.IsMatch("^A01 OK \\[COPYUID \\d{0,50} 1 1\\] COPY completed\r\n$", response);

         var messagesCopied = simulator.GetMessageCount("SomeFolder");
         Assert.AreEqual(1, messagesCopied);
      }

      [Test]
      public void TestCopyRange()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");


         for (int i = 0; i < 5; i++)
            SmtpClientSimulator.StaticSend("test@test.com", account.Address, "Test", "test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 5);

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon("test@test.com", "test");
         simulator.CreateFolder("SomeFolder");

         simulator.SelectFolder("Inbox");

         var response = simulator.SendSingleCommand("A01 COPY 2:3 SomeFolder");
         StringAssert.IsMatch("^A01 OK \\[COPYUID \\d{0,50} 2,3 1,2\\] COPY completed\r\n$", response);

         var messagesCopied = simulator.GetMessageCount("SomeFolder");
         Assert.AreEqual(2, messagesCopied);
      }

      [Test]
      public void TestCopyIndividualAndRange()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");


         for (int i = 0; i < 7; i++)
            SmtpClientSimulator.StaticSend("test@test.com", account.Address, "Test", "test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 7);

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon("test@test.com", "test");
         simulator.CreateFolder("SomeFolder");

         simulator.SelectFolder("Inbox");

         var response = simulator.SendSingleCommand("A01 COPY 1,4:5 SomeFolder");

         StringAssert.IsMatch("^A01 OK \\[COPYUID \\d{0,50} 1,4,5 1,2,3\\] COPY completed\r\n$", response);

         var messagesCopied = simulator.GetMessageCount("SomeFolder");
         Assert.AreEqual(3, messagesCopied);
      }

      [Test]
      public void TestCopyMessageToPublicFolder_COPYUIDNotIncludedUnlessReadAccess()
      {
         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account7@test.com", "test");

         IMAPFolders publicFolders = _settings.PublicFolders;
         IMAPFolder folder = publicFolders.Add("Share1");
         folder.Save();

         IMAPFolderPermission permission = folder.Permissions.Add();
         permission.PermissionAccountID = account1.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionCreate, true);
         permission.Save();

         string folderName = "#Public.Share1";

         SmtpClientSimulator.StaticSend("test@test.com", account1.Address, "TestMessage", "Body");

         CustomAsserts.AssertFolderMessageCount(account1.IMAPFolders.get_ItemByName("INBOX"), 1);
         CustomAsserts.AssertFolderMessageCount(folder, 0);

         var imapClientSimulator = new ImapClientSimulator();
         imapClientSimulator.Connect();
         imapClientSimulator.Logon(account1.Address, "test");
         imapClientSimulator.SelectFolder("INBOX");

         // Give write-permission...
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.Save();

         Assert.IsTrue(imapClientSimulator.Copy(1, folderName, out string response1));
         StringAssert.IsMatch("^A18 OK COPY completed\r\n$", response1);

         // Give read-permission...
         permission.set_Permission(eACLPermission.ePermissionRead | eACLPermission.ePermissionInsert, true);
         permission.Save();

         Assert.IsTrue(imapClientSimulator.Copy(1, folderName, out string response2));
         StringAssert.IsMatch("^A18 OK \\[COPYUID \\d{0,50} 1 2\\] COPY completed\r\n$", response2);

         imapClientSimulator.Disconnect();
      }
   }
}
