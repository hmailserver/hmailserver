// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class Folders : TestFixtureBase
   {
      [Test]
      public void TestCreateDeepFolder()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder4@test.com", "test");

         var simulator = new ImapClientSimulator();

         simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");
         string result =
            simulator.Send("A01 CREATE 1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20.21.22.23.24.25.26");
         Assert.IsTrue(result.Contains("A01 NO"));

         simulator.Disconnect();
      }


      [Test]
      public void TestCreateFolderWithHash()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder6@test.com", "test");

         var simulator = new ImapClientSimulator();

         simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsFalse(simulator.CreateFolder("#Test"));
         Assert.IsTrue(simulator.CreateFolder("Test.#Testar"));
         simulator.Disconnect();
      }


      [Test]
      public void TestCreateFolderWithQuote()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");


         const string folderName = "ABC\"123";

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         Assert.IsTrue(simulator.List().Contains(folderName));
         Assert.IsTrue(simulator.SelectFolder(folderName));
         simulator.Disconnect();
      }

      [Test]
      public void TestCreateFolderWithSlash()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         const string folderName = "ABC\\123";

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         Assert.IsTrue(simulator.List().Contains(folderName));
         Assert.IsTrue(simulator.SelectFolder(folderName));
         simulator.Disconnect();
      }

      [Test]
      public void TestCreateLongFolder()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder9@test.com", "test");

         var simulator = new ImapClientSimulator();

         simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");

         string folderName = "";
         for (int i = 0; i < 255; i++)
            folderName = folderName + "A";

         string result = simulator.Send("A01 CREATE " + folderName);
         Assert.IsTrue(result.Contains("A01 OK"));
         Assert.IsTrue(simulator.SelectFolder(folderName));

         folderName = "";
         for (int i = 0; i < 256; i++)
            folderName = folderName + "A";

         result = simulator.Send("A01 CREATE " + folderName);
         Assert.IsTrue(result.Contains("A01 NO"));
         Assert.IsFalse(simulator.SelectFolder(folderName));

         simulator.Disconnect();
      }

      [Test]
      public void TestCreateParallelFolder()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder5@test.com", "test");

         var simulator = new ImapClientSimulator();

         simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");
         string result = simulator.Send("A01 CREATE 1.2.3");
         Assert.IsTrue(result.Contains("A01 OK"));
         result = simulator.Send("A01 CREATE 1.2.4");
         Assert.IsTrue(result.Contains("A01 OK"));

         simulator.Disconnect();
      }

      [Test]
      public void TestCreateUnnamedSubFolder()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder7@test.com", "test");

         var simulator = new ImapClientSimulator();

         simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");
         string result = simulator.Send("A01 CREATE 1..3");
         Assert.IsTrue(result.Contains("A01 NO"));
         result = simulator.Send("A01 CREATE 1....3");
         Assert.IsTrue(result.Contains("A01 NO"));
         result = simulator.Send("A01 CREATE 1.......3");
         Assert.IsTrue(result.Contains("A01 NO"));


         simulator.Disconnect();
      }

      [Test]
      public void TestFolderCaseInLIST()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "ABC.def.GHI";

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         Assert.IsFalse(simulator.List("ABC.DEF.*").Contains("ABC.def.GHI"));
         Assert.IsTrue(simulator.List("ABC.DEF.*").Contains("ABC.DEF.GHI"));
         Assert.IsFalse(simulator.List("ABC.def.*").Contains("ABC.DEF"));
         Assert.IsTrue(simulator.List("ABC.def.*").Contains("ABC.def.GHI"));
         Assert.IsTrue(simulator.SelectFolder(folderName));
         simulator.Disconnect();
      }

      [Test]
      public void TestFolderCaseInLSUB()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "ABC.def.GHI";

         var simulator = new ImapClientSimulator();
         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         Assert.IsTrue(simulator.Subscribe(folderName));
         Assert.IsFalse(simulator.LSUB("ABC.DEF.*").Contains("ABC.def.GHI"));
         Assert.IsTrue(simulator.LSUB("ABC.DEF.*").Contains("ABC.DEF.GHI"));
         Assert.IsFalse(simulator.LSUB("ABC.def.*").Contains("ABC.DEF"));
         Assert.IsTrue(simulator.LSUB("ABC.def.*").Contains("ABC.def.GHI"));
         Assert.IsTrue(simulator.SelectFolder(folderName));
         simulator.Disconnect();
      }

      /// <summary>
      ///  Tests that the LSUB resposne doesn't contain an empty line if a folder is not subscribed to.
      /// </summary>
      [Test]
      public void TestFolderLSUBUnsubscribedFolder()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         const string folderName = "ABC.def.GHI";

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         Assert.IsFalse(simulator.LSUB().Contains("\r\n\r\n"));
         simulator.Disconnect();
      }


      /// <summary>
      /// Test that folder names in the LSUB response are put in quotes.
      /// This is how gmail acts, and Thunderbird crashes if a folder name
      /// is * without quotes.
      /// </summary>
      [Test]
      public void TestFolderQuote()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         const string folderName = "Test*";

         var simulator = new ImapClientSimulator();
         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         Assert.IsTrue(simulator.Subscribe(folderName));
         Assert.IsTrue(simulator.LSUB().Contains("\"" + folderName + "\""));
         simulator.Disconnect();
      }

      [Test]
      public void TestListCommandChangedHierarchyDelimiter()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");
         _settings.IMAPHierarchyDelimiter = "\\";

         var simulator = new ImapClientSimulator();

         simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");
         simulator.CreateFolder("Test\\HelloWorld\\Test2");

         string response = simulator.List("%");
         Assert.IsTrue(response.Contains("\"Test\""), response);

         response = simulator.List("%\\%");
         Assert.IsTrue(response.Contains("\"Test\\HelloWorld\""), response);

         response = simulator.List("%\\%\\%%");
         Assert.IsTrue(response.Contains("\"Test\\HelloWorld\\Test2\""), response);
      }


      [Test]
      [Description("Test that the namespace is included in an empty list response.")]
      public void TestListSpecial()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         var simulator = new ImapClientSimulator();
         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         string response = simulator.List("");
         Assert.IsTrue(response.StartsWith("* LIST (\\Noselect) \".\" \"\""));
         simulator.Disconnect();

         _settings.IMAPHierarchyDelimiter = "/";

         simulator = new ImapClientSimulator();
         sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         response = simulator.List("");
         Assert.IsTrue(response.StartsWith("* LIST (\\Noselect) \"/\" \"\""));
         simulator.Disconnect();

         _settings.IMAPHierarchyDelimiter = "\\";

         simulator = new ImapClientSimulator();
         sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         response = simulator.List("", false);
         string expectedResponse = "* LIST (\\Noselect) \"\\\\\" \"\"";
         Assert.IsTrue(response.StartsWith(expectedResponse));
         simulator.Disconnect();
      }

      [Test]
      [Description("Test to include reference in LIST command (issue 163).")]
      public void TestListWithReference()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         simulator.CreateFolder("Main.Sub1.Sub2.Sub3");
         simulator.CreateFolder("SomeOtherFolder");

         simulator.Subscribe("Main");
         simulator.Subscribe("Main.Sub1");
         simulator.Subscribe("Main.Sub1.Sub2");
         simulator.Subscribe("Main.Sub1.Sub2.Sub3");
         simulator.Subscribe("SomeOtherFolder");

         string response = simulator.List("Main", "*", true);
         Assert.IsFalse(response.Contains("INBOX"));
         Assert.IsFalse(response.Contains("SomeOtherFolder"));
         Assert.IsTrue(response.Contains("* LIST (\\HasChildren) \".\" \"Main.Sub1\""));
         Assert.IsTrue(response.Contains("* LIST (\\HasChildren) \".\" \"Main.Sub1.Sub2\""));
         Assert.IsTrue(response.Contains("* LIST (\\HasNoChildren) \".\" \"Main.Sub1.Sub2.Sub3\""));

         response = simulator.List("Main.Sub1", "*", true);
         Assert.IsFalse(response.Contains("INBOX"));
         Assert.IsFalse(response.Contains("SomeOtherFolder"));
         Assert.IsTrue(response.Contains("* LIST (\\HasChildren) \".\" \"Main.Sub1.Sub2\""));
         Assert.IsTrue(response.Contains("* LIST (\\HasNoChildren) \".\" \"Main.Sub1.Sub2.Sub3\""));

         response = simulator.LSUB("Main", "*");
         Assert.IsFalse(response.Contains("INBOX"));
         Assert.IsFalse(response.Contains("SomeOtherFolder"));
         Assert.IsTrue(response.Contains("* LSUB (\\HasChildren) \".\" \"Main.Sub1\""));
         Assert.IsTrue(response.Contains("* LSUB (\\HasChildren) \".\" \"Main.Sub1.Sub2\""));
         Assert.IsTrue(response.Contains("* LSUB (\\HasNoChildren) \".\" \"Main.Sub1.Sub2.Sub3\""));

         response = simulator.LSUB("Main.Sub1", "*");
         Assert.IsFalse(response.Contains("INBOX"));
         Assert.IsFalse(response.Contains("SomeOtherFolder"));
         Assert.IsTrue(response.Contains("* LSUB (\\HasChildren) \".\" \"Main.Sub1.Sub2\""));
         Assert.IsTrue(response.Contains("* LSUB (\\HasNoChildren) \".\" \"Main.Sub1.Sub2.Sub3\""));

         simulator.Disconnect();
      }

      [Test]
      [Description("Test to include reference in LIST command (test case from issue 165).")]
      public void TestListWithReferenceTestCase2()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         simulator.CreateFolder("INBOX.MyApp.SubFolder1");
         simulator.CreateFolder("INBOX.MyApp.SubFolder2");
         simulator.CreateFolder("INBOX.SomeOtherFolder");


         string response = simulator.List("INBOX.MyApp", "%.%", true);
         Assert.IsFalse(response.Contains("\"INBOX.MyApp\""));
         Assert.IsFalse(response.Contains("\"INBOX.SomeOtherFolder\""));
         Assert.IsTrue(response.Contains("* LIST (\\HasNoChildren) \".\" \"INBOX.MyApp.SubFolder1\""));
         Assert.IsTrue(response.Contains("* LIST (\\HasNoChildren) \".\" \"INBOX.MyApp.SubFolder2\""));

         simulator.Disconnect();
      }

      /// <summary>
      ///  Tests that a folder which is not subscribed to isn't included and vice versa.
      /// </summary>
      [Test]
      public void TestLsubInclusion()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "Folder1";

         var simulator = new ImapClientSimulator();
         string sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder(folderName));
         Assert.IsFalse(simulator.LSUB().Contains(folderName));
         Assert.IsTrue(simulator.Subscribe(folderName));
         Assert.IsTrue(simulator.LSUB().Contains(folderName));

         simulator.Disconnect();
      }

      [Test]
      public void TestRenameIncorrectParameters()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder8@test.com", "test");

         var simulator = new ImapClientSimulator();

         string sWelcomeMessage = simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");
         string result = simulator.Send("A01 CREATE A");

         result = simulator.Send("A02 RENAME A B C");
         Assert.IsTrue(result.Contains("A02 BAD"));

         simulator.Disconnect();
      }

      [Test]
      public void TestRenameLongFolder()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder2@test.com", "test");

         var simulator = new ImapClientSimulator();

         string sWelcomeMessage = simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder("1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20.21.22.23.24.25"));
         Assert.IsTrue(simulator.CreateFolder("A"));

         Assert.IsFalse(simulator.RenameFolder("1", "A.1"));
         Assert.IsTrue(simulator.RenameFolder("1.2.3", "A.1"));
         Assert.IsTrue(simulator.SelectFolder("A.1.4"));


         simulator.Disconnect();
      }

      [Test]
      public void TestRenameRootPublicFolder()
      {
         IMAPFolders folders = _application.Settings.PublicFolders;
         IMAPFolder folder = folders.Add("ShareA");
         folder.Save();

         IMAPFolderPermission permission = folder.Permissions.Add();
         permission.PermissionType = eACLPermissionType.ePermissionTypeAnyone;
         permission.set_Permission(eACLPermission.ePermissionLookup, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionDeleteMailbox, true);
         permission.Save();

         IMAPFolder folder2 = folders.Add("ShareB");
         folder2.Save();

         IMAPFolderPermission permission2 = folder2.Permissions.Add();
         permission2.PermissionType = eACLPermissionType.ePermissionTypeAnyone;
         permission2.set_Permission(eACLPermission.ePermissionLookup, true);
         permission2.set_Permission(eACLPermission.ePermissionRead, true);
         permission2.set_Permission(eACLPermission.ePermissionDeleteMailbox, true);
         permission2.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "shared@test.com", "test");

         var simulator1 = new ImapClientSimulator();
         simulator1.ConnectAndLogon(account.Address, "test");
         Assert.IsTrue(simulator1.SelectFolder("#Public.ShareA"));
         Assert.IsTrue(simulator1.SelectFolder("#Public.ShareB"));
         Assert.IsTrue(simulator1.RenameFolder("#Public.ShareA", "#Public.ShareB.ShareA"));
         Assert.IsFalse(simulator1.SelectFolder("#Public.ShareA"));
         Assert.IsTrue(simulator1.SelectFolder("#Public.ShareB"));
         Assert.IsTrue(simulator1.SelectFolder("#Public.ShareB.ShareA"));
      }

      [Test]
      [Description("Issue 274, IMAP: Moving folder to sub-folder")]
      public void TestRenameSubFolderToChildSub()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");

         var simulator = new ImapClientSimulator();
         Assert.IsTrue(simulator.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(simulator.CreateFolder("A.B"));

         string result;
         Assert.IsFalse(simulator.RenameFolder("A.B", "A.B.C", out result));
         Assert.IsTrue(result.Contains("A folder cannot be moved into one of its subfolders."));

         Assert.IsTrue(simulator.SelectFolder("A.B"));

         simulator.Disconnect();
      }

      [Test]
      [Description("Issue 274, Test side effect of fix for 274")]
      public void TestRenameSubFolderToMatchingName()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");

         var simulator = new ImapClientSimulator();
         Assert.IsTrue(simulator.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(simulator.CreateFolder("Folder1"));
         Assert.IsTrue(simulator.SelectFolder("Folder1"));

         string result = string.Empty;
         Assert.IsFalse(simulator.RenameFolder("Folder1", "Folder1.Sub1", out result));
         Assert.IsTrue(result.Contains("A folder cannot be moved into one of its subfolders."));
         Assert.IsTrue(simulator.SelectFolder("Folder1"));

         result = string.Empty;
         Assert.IsTrue(simulator.RenameFolder("Folder1", "Folder1Test", out result));
         Assert.IsTrue(simulator.SelectFolder("Folder1Test"));

         simulator.Disconnect();
      }

      [Test]
      public void TestRenameToAndFromINBOX()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder3@test.com", "test");

         var simulator = new ImapClientSimulator();

         string sWelcomeMessage = simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");
         simulator.Send("A01 CREATE A\r\n");
         Assert.IsFalse(simulator.RenameFolder("A", "INBOX"));
         Assert.IsFalse(simulator.RenameFolder("INBOX", "B"));

         simulator.Disconnect();
      }

      [Test]
      [Description("Test renaming sub folder to a new sub folder where the new parent does not exist.")]
      public void TestRenameToParallelFolder()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");

         var simulator = new ImapClientSimulator();

         string sWelcomeMessage = simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder("Main.Sub"));
         Assert.IsTrue(simulator.RenameFolder("Main.Sub", "Second.Sub"));

         string listResponse = simulator.List();
         Assert.IsFalse(listResponse.Contains("Main.Sub"));
         Assert.IsTrue(listResponse.Contains("Second.Sub"));


         simulator.Disconnect();
      }

      [Test]
      public void TestRenameToSubFolder()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");

         var simulator = new ImapClientSimulator();

         simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder("1.2.3.4.5"));
         Assert.IsTrue(simulator.CreateFolder("A"));
         Assert.IsTrue(simulator.RenameFolder("1", "A.1"));
         Assert.IsTrue(simulator.SelectFolder("A.1"));
         Assert.IsTrue(simulator.SelectFolder("A.1.2.3.4.5"));
         Assert.IsTrue(simulator.RenameFolder("A.1", "1"));
         Assert.IsFalse(simulator.SelectFolder("A.1"));
         Assert.IsFalse(simulator.SelectFolder("A.1.2.3.4.5"));
         Assert.IsTrue(simulator.SelectFolder("1"));
         Assert.IsTrue(simulator.SelectFolder("1.2.3.4.5"));

         simulator.Disconnect();
      }

      [Test]
      public void TestTryCreateInvalidStructure()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");

         var simulator = new ImapClientSimulator();

         simulator.Connect();
         simulator.LogonWithLiteral(account.Address, "test");
         Assert.IsTrue(simulator.CreateFolder("1.2.3"));
         Assert.IsTrue(simulator.CreateFolder("1.2.3.4"));

         // Should fail because name taken.
         Assert.IsFalse(simulator.RenameFolder("1.2.3.4", "1.2.3"));
         Assert.IsFalse(simulator.RenameFolder("1.2.3.4", "1.2"));
         Assert.IsFalse(simulator.RenameFolder("1.2.3.4", "1"));

         // Should fail because invalid destination name.
         Assert.IsFalse(simulator.RenameFolder("1.2.3.4", ""));

         // Should fail because destination name taken.
         Assert.IsFalse(simulator.RenameFolder("1.2.3", "1.2.3.4"));
         Assert.IsFalse(simulator.RenameFolder("1.2", "1.2.3.4"));
         Assert.IsFalse(simulator.RenameFolder("1", "1.2.3.4"));

         Assert.IsFalse(simulator.RenameFolder("1.2.3", "1.2"));
         Assert.IsFalse(simulator.RenameFolder("1.2.3", "1"));

         Assert.IsTrue(simulator.RenameFolder("1.2.3", "A"));

         simulator.Disconnect();
      }
   }
}