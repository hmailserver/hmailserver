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
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder4@test.com", "test");

         var oSimulator = new IMAPClientSimulator();

         oSimulator.Connect();
         oSimulator.LogonWithLiteral(oAccount.Address, "test");
         string result =
            oSimulator.Send("A01 CREATE 1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20.21.22.23.24.25.26");
         CustomAssert.IsTrue(result.Contains("A01 NO"));

         oSimulator.Disconnect();
      }


      [Test]
      public void TestCreateFolderWithHash()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder6@test.com", "test");

         var oSimulator = new IMAPClientSimulator();

         oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         CustomAssert.IsFalse(oSimulator.CreateFolder("#Test"));
         CustomAssert.IsTrue(oSimulator.CreateFolder("Test.#Testar"));
         oSimulator.Disconnect();
      }


      [Test]
      public void TestCreateFolderWithQuote()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");


         const string folderName = "ABC\"123";

         var oSimulator = new IMAPClientSimulator();
         oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         CustomAssert.IsTrue(oSimulator.List().Contains(folderName));
         CustomAssert.IsTrue(oSimulator.SelectFolder(folderName));
         oSimulator.Disconnect();
      }

      [Test]
      public void TestCreateFolderWithSlash()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         const string folderName = "ABC\\123";

         var oSimulator = new IMAPClientSimulator();
         oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         CustomAssert.IsTrue(oSimulator.List().Contains(folderName));
         CustomAssert.IsTrue(oSimulator.SelectFolder(folderName));
         oSimulator.Disconnect();
      }

      [Test]
      public void TestCreateLongFolder()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder9@test.com", "test");

         var oSimulator = new IMAPClientSimulator();

         oSimulator.Connect();
         oSimulator.LogonWithLiteral(oAccount.Address, "test");

         string folderName = "";
         for (int i = 0; i < 255; i++)
            folderName = folderName + "A";

         string result = oSimulator.Send("A01 CREATE " + folderName);
         CustomAssert.IsTrue(result.Contains("A01 OK"));
         CustomAssert.IsTrue(oSimulator.SelectFolder(folderName));

         folderName = "";
         for (int i = 0; i < 256; i++)
            folderName = folderName + "A";

         result = oSimulator.Send("A01 CREATE " + folderName);
         CustomAssert.IsTrue(result.Contains("A01 NO"));
         CustomAssert.IsFalse(oSimulator.SelectFolder(folderName));

         oSimulator.Disconnect();
      }

      [Test]
      public void TestCreateParallelFolder()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder5@test.com", "test");

         var oSimulator = new IMAPClientSimulator();

         oSimulator.Connect();
         oSimulator.LogonWithLiteral(oAccount.Address, "test");
         string result = oSimulator.Send("A01 CREATE 1.2.3");
         CustomAssert.IsTrue(result.Contains("A01 OK"));
         result = oSimulator.Send("A01 CREATE 1.2.4");
         CustomAssert.IsTrue(result.Contains("A01 OK"));

         oSimulator.Disconnect();
      }

      [Test]
      public void TestCreateUnnamedSubFolder()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder7@test.com", "test");

         var simulator = new IMAPClientSimulator();

         simulator.Connect();
         simulator.LogonWithLiteral(oAccount.Address, "test");
         string result = simulator.Send("A01 CREATE 1..3");
         CustomAssert.IsTrue(result.Contains("A01 NO"));
         result = simulator.Send("A01 CREATE 1....3");
         CustomAssert.IsTrue(result.Contains("A01 NO"));
         result = simulator.Send("A01 CREATE 1.......3");
         CustomAssert.IsTrue(result.Contains("A01 NO"));


         simulator.Disconnect();
      }

      [Test]
      public void TestFolderCaseInLIST()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "ABC.def.GHI";

         var oSimulator = new IMAPClientSimulator();
         oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         CustomAssert.IsFalse(oSimulator.List("ABC.DEF.*").Contains("ABC.def.GHI"));
         CustomAssert.IsTrue(oSimulator.List("ABC.DEF.*").Contains("ABC.DEF.GHI"));
         CustomAssert.IsFalse(oSimulator.List("ABC.def.*").Contains("ABC.DEF"));
         CustomAssert.IsTrue(oSimulator.List("ABC.def.*").Contains("ABC.def.GHI"));
         CustomAssert.IsTrue(oSimulator.SelectFolder(folderName));
         oSimulator.Disconnect();
      }

      [Test]
      public void TestFolderCaseInLSUB()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "ABC.def.GHI";

         var oSimulator = new IMAPClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         CustomAssert.IsTrue(oSimulator.Subscribe(folderName));
         CustomAssert.IsFalse(oSimulator.LSUB("ABC.DEF.*").Contains("ABC.def.GHI"));
         CustomAssert.IsTrue(oSimulator.LSUB("ABC.DEF.*").Contains("ABC.DEF.GHI"));
         CustomAssert.IsFalse(oSimulator.LSUB("ABC.def.*").Contains("ABC.DEF"));
         CustomAssert.IsTrue(oSimulator.LSUB("ABC.def.*").Contains("ABC.def.GHI"));
         CustomAssert.IsTrue(oSimulator.SelectFolder(folderName));
         oSimulator.Disconnect();
      }

      /// <summary>
      ///  Tests that the LSUB resposne doesn't contain an empty line if a folder is not subscribed to.
      /// </summary>
      [Test]
      public void TestFolderLSUBUnsubscribedFolder()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         const string folderName = "ABC.def.GHI";

         var oSimulator = new IMAPClientSimulator();
         oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         CustomAssert.IsFalse(oSimulator.LSUB().Contains("\r\n\r\n"));
         oSimulator.Disconnect();
      }


      /// <summary>
      /// Test that folder names in the LSUB response are put in quotes.
      /// This is how gmail acts, and Thunderbird crashes if a folder name
      /// is * without quotes.
      /// </summary>
      [Test]
      public void TestFolderQuote()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         const string folderName = "Test*";

         var oSimulator = new IMAPClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         CustomAssert.IsTrue(oSimulator.Subscribe(folderName));
         CustomAssert.IsTrue(oSimulator.LSUB().Contains("\"" + folderName + "\""));
         oSimulator.Disconnect();
      }

      [Test]
      public void TestListCommandChangedHierarchyDelimiter()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");
         _settings.IMAPHierarchyDelimiter = "\\";

         var oSimulator = new IMAPClientSimulator();

         oSimulator.Connect();
         oSimulator.LogonWithLiteral(account.Address, "test");
         oSimulator.CreateFolder("Test\\HelloWorld\\Test2");

         string response = oSimulator.List("%");
         CustomAssert.IsTrue(response.Contains("\"Test\""), response);

         response = oSimulator.List("%\\%");
         CustomAssert.IsTrue(response.Contains("\"Test\\HelloWorld\""), response);

         response = oSimulator.List("%\\%\\%%");
         CustomAssert.IsTrue(response.Contains("\"Test\\HelloWorld\\Test2\""), response);
      }


      [Test]
      [Description("Test that the namespace is included in an empty list response.")]
      public void TestListSpecial()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         var oSimulator = new IMAPClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         string response = oSimulator.List("");
         CustomAssert.IsTrue(response.StartsWith("* LIST (\\Noselect) \".\" \"\""));
         oSimulator.Disconnect();

         _settings.IMAPHierarchyDelimiter = "/";

         oSimulator = new IMAPClientSimulator();
         sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         response = oSimulator.List("");
         CustomAssert.IsTrue(response.StartsWith("* LIST (\\Noselect) \"/\" \"\""));
         oSimulator.Disconnect();

         _settings.IMAPHierarchyDelimiter = "\\";

         oSimulator = new IMAPClientSimulator();
         sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         response = oSimulator.List("", false);
         string expectedResponse = "* LIST (\\Noselect) \"\\\\\" \"\"";
         CustomAssert.IsTrue(response.StartsWith(expectedResponse));
         oSimulator.Disconnect();
      }

      [Test]
      [Description("Test to include reference in LIST command (issue 163).")]
      public void TestListWithReference()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         var oSimulator = new IMAPClientSimulator();
         oSimulator.ConnectAndLogon(oAccount.Address, "test");
         oSimulator.CreateFolder("Main.Sub1.Sub2.Sub3");
         oSimulator.CreateFolder("SomeOtherFolder");

         oSimulator.Subscribe("Main");
         oSimulator.Subscribe("Main.Sub1");
         oSimulator.Subscribe("Main.Sub1.Sub2");
         oSimulator.Subscribe("Main.Sub1.Sub2.Sub3");
         oSimulator.Subscribe("SomeOtherFolder");

         string response = oSimulator.List("Main", "*", true);
         CustomAssert.IsFalse(response.Contains("INBOX"));
         CustomAssert.IsFalse(response.Contains("SomeOtherFolder"));
         CustomAssert.IsTrue(response.Contains("* LIST (\\HasChildren) \".\" \"Main.Sub1\""));
         CustomAssert.IsTrue(response.Contains("* LIST (\\HasChildren) \".\" \"Main.Sub1.Sub2\""));
         CustomAssert.IsTrue(response.Contains("* LIST (\\HasNoChildren) \".\" \"Main.Sub1.Sub2.Sub3\""));

         response = oSimulator.List("Main.Sub1", "*", true);
         CustomAssert.IsFalse(response.Contains("INBOX"));
         CustomAssert.IsFalse(response.Contains("SomeOtherFolder"));
         CustomAssert.IsTrue(response.Contains("* LIST (\\HasChildren) \".\" \"Main.Sub1.Sub2\""));
         CustomAssert.IsTrue(response.Contains("* LIST (\\HasNoChildren) \".\" \"Main.Sub1.Sub2.Sub3\""));

         response = oSimulator.LSUB("Main", "*");
         CustomAssert.IsFalse(response.Contains("INBOX"));
         CustomAssert.IsFalse(response.Contains("SomeOtherFolder"));
         CustomAssert.IsTrue(response.Contains("* LSUB (\\HasChildren) \".\" \"Main.Sub1\""));
         CustomAssert.IsTrue(response.Contains("* LSUB (\\HasChildren) \".\" \"Main.Sub1.Sub2\""));
         CustomAssert.IsTrue(response.Contains("* LSUB (\\HasNoChildren) \".\" \"Main.Sub1.Sub2.Sub3\""));

         response = oSimulator.LSUB("Main.Sub1", "*");
         CustomAssert.IsFalse(response.Contains("INBOX"));
         CustomAssert.IsFalse(response.Contains("SomeOtherFolder"));
         CustomAssert.IsTrue(response.Contains("* LSUB (\\HasChildren) \".\" \"Main.Sub1.Sub2\""));
         CustomAssert.IsTrue(response.Contains("* LSUB (\\HasNoChildren) \".\" \"Main.Sub1.Sub2.Sub3\""));

         oSimulator.Disconnect();
      }

      [Test]
      [Description("Test to include reference in LIST command (test case from issue 165).")]
      public void TestListWithReferenceTestCase2()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         var oSimulator = new IMAPClientSimulator();
         oSimulator.ConnectAndLogon(oAccount.Address, "test");
         oSimulator.CreateFolder("INBOX.MyApp.SubFolder1");
         oSimulator.CreateFolder("INBOX.MyApp.SubFolder2");
         oSimulator.CreateFolder("INBOX.SomeOtherFolder");


         string response = oSimulator.List("INBOX.MyApp", "%.%", true);
         CustomAssert.IsFalse(response.Contains("\"INBOX.MyApp\""));
         CustomAssert.IsFalse(response.Contains("\"INBOX.SomeOtherFolder\""));
         CustomAssert.IsTrue(response.Contains("* LIST (\\HasNoChildren) \".\" \"INBOX.MyApp.SubFolder1\""));
         CustomAssert.IsTrue(response.Contains("* LIST (\\HasNoChildren) \".\" \"INBOX.MyApp.SubFolder2\""));

         oSimulator.Disconnect();
      }

      /// <summary>
      ///  Tests that a folder which is not subscribed to isn't included and vice versa.
      /// </summary>
      [Test]
      public void TestLsubInclusion()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "quote@test.com", "test");

         string folderName = "Folder1";

         var oSimulator = new IMAPClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder(folderName));
         CustomAssert.IsFalse(oSimulator.LSUB().Contains(folderName));
         CustomAssert.IsTrue(oSimulator.Subscribe(folderName));
         CustomAssert.IsTrue(oSimulator.LSUB().Contains(folderName));

         oSimulator.Disconnect();
      }

      [Test]
      public void TestRenameIncorrectParameters()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder8@test.com", "test");

         var oSimulator = new IMAPClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.LogonWithLiteral(oAccount.Address, "test");
         string result = oSimulator.Send("A01 CREATE A");

         result = oSimulator.Send("A02 RENAME A B C");
         CustomAssert.IsTrue(result.Contains("A02 BAD"));

         oSimulator.Disconnect();
      }

      [Test]
      public void TestRenameLongFolder()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder2@test.com", "test");

         var oSimulator = new IMAPClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.LogonWithLiteral(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder("1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20.21.22.23.24.25"));
         CustomAssert.IsTrue(oSimulator.CreateFolder("A"));

         CustomAssert.IsFalse(oSimulator.RenameFolder("1", "A.1"));
         CustomAssert.IsTrue(oSimulator.RenameFolder("1.2.3", "A.1"));
         CustomAssert.IsTrue(oSimulator.SelectFolder("A.1.4"));


         oSimulator.Disconnect();
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

         var simulator1 = new IMAPClientSimulator();
         simulator1.ConnectAndLogon(account.Address, "test");
         CustomAssert.IsTrue(simulator1.SelectFolder("#Public.ShareA"));
         CustomAssert.IsTrue(simulator1.SelectFolder("#Public.ShareB"));
         CustomAssert.IsTrue(simulator1.RenameFolder("#Public.ShareA", "#Public.ShareB.ShareA"));
         CustomAssert.IsFalse(simulator1.SelectFolder("#Public.ShareA"));
         CustomAssert.IsTrue(simulator1.SelectFolder("#Public.ShareB"));
         CustomAssert.IsTrue(simulator1.SelectFolder("#Public.ShareB.ShareA"));
      }

      [Test]
      [Description("Issue 274, IMAP: Moving folder to sub-folder")]
      public void TestRenameSubFolderToChildSub()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");

         var oSimulator = new IMAPClientSimulator();
         CustomAssert.IsTrue(oSimulator.ConnectAndLogon(account.Address, "test"));
         CustomAssert.IsTrue(oSimulator.CreateFolder("A.B"));

         string result;
         CustomAssert.IsFalse(oSimulator.RenameFolder("A.B", "A.B.C", out result));
         CustomAssert.IsTrue(result.Contains("A folder cannot be moved into one of its subfolders."));

         CustomAssert.IsTrue(oSimulator.SelectFolder("A.B"));

         oSimulator.Disconnect();
      }

      [Test]
      [Description("Issue 274, Test side effect of fix for 274")]
      public void TestRenameSubFolderToMatchingName()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");

         var oSimulator = new IMAPClientSimulator();
         CustomAssert.IsTrue(oSimulator.ConnectAndLogon(account.Address, "test"));
         CustomAssert.IsTrue(oSimulator.CreateFolder("Folder1"));
         CustomAssert.IsTrue(oSimulator.SelectFolder("Folder1"));

         string result = string.Empty;
         CustomAssert.IsFalse(oSimulator.RenameFolder("Folder1", "Folder1.Sub1", out result));
         CustomAssert.IsTrue(result.Contains("A folder cannot be moved into one of its subfolders."));
         CustomAssert.IsTrue(oSimulator.SelectFolder("Folder1"));

         result = string.Empty;
         CustomAssert.IsTrue(oSimulator.RenameFolder("Folder1", "Folder1Test", out result));
         CustomAssert.IsTrue(oSimulator.SelectFolder("Folder1Test"));

         oSimulator.Disconnect();
      }

      [Test]
      public void TestRenameToAndFromINBOX()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder3@test.com", "test");

         var oSimulator = new IMAPClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.LogonWithLiteral(oAccount.Address, "test");
         oSimulator.Send("A01 CREATE A\r\n");
         CustomAssert.IsFalse(oSimulator.RenameFolder("A", "INBOX"));
         CustomAssert.IsFalse(oSimulator.RenameFolder("INBOX", "B"));

         oSimulator.Disconnect();
      }

      [Test]
      [Description("Test renaming sub folder to a new sub folder where the new parent does not exist.")]
      public void TestRenameToParallelFolder()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");

         var oSimulator = new IMAPClientSimulator();

         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.LogonWithLiteral(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder("Main.Sub"));
         CustomAssert.IsTrue(oSimulator.RenameFolder("Main.Sub", "Second.Sub"));

         string listResponse = oSimulator.List();
         CustomAssert.IsFalse(listResponse.Contains("Main.Sub"));
         CustomAssert.IsTrue(listResponse.Contains("Second.Sub"));


         oSimulator.Disconnect();
      }

      [Test]
      public void TestRenameToSubFolder()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");

         var oSimulator = new IMAPClientSimulator();

         oSimulator.Connect();
         oSimulator.LogonWithLiteral(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder("1.2.3.4.5"));
         CustomAssert.IsTrue(oSimulator.CreateFolder("A"));
         CustomAssert.IsTrue(oSimulator.RenameFolder("1", "A.1"));
         CustomAssert.IsTrue(oSimulator.SelectFolder("A.1"));
         CustomAssert.IsTrue(oSimulator.SelectFolder("A.1.2.3.4.5"));
         CustomAssert.IsTrue(oSimulator.RenameFolder("A.1", "1"));
         CustomAssert.IsFalse(oSimulator.SelectFolder("A.1"));
         CustomAssert.IsFalse(oSimulator.SelectFolder("A.1.2.3.4.5"));
         CustomAssert.IsTrue(oSimulator.SelectFolder("1"));
         CustomAssert.IsTrue(oSimulator.SelectFolder("1.2.3.4.5"));

         oSimulator.Disconnect();
      }

      [Test]
      public void TestTryCreateInvalidStructure()
      {
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "folder1@test.com", "test");

         var oSimulator = new IMAPClientSimulator();

         oSimulator.Connect();
         oSimulator.LogonWithLiteral(oAccount.Address, "test");
         CustomAssert.IsTrue(oSimulator.CreateFolder("1.2.3"));
         CustomAssert.IsTrue(oSimulator.CreateFolder("1.2.3.4"));

         // Should fail because name taken.
         CustomAssert.IsFalse(oSimulator.RenameFolder("1.2.3.4", "1.2.3"));
         CustomAssert.IsFalse(oSimulator.RenameFolder("1.2.3.4", "1.2"));
         CustomAssert.IsFalse(oSimulator.RenameFolder("1.2.3.4", "1"));

         // Should fail because invalid destination name.
         CustomAssert.IsFalse(oSimulator.RenameFolder("1.2.3.4", ""));

         // Should fail because destination name taken.
         CustomAssert.IsFalse(oSimulator.RenameFolder("1.2.3", "1.2.3.4"));
         CustomAssert.IsFalse(oSimulator.RenameFolder("1.2", "1.2.3.4"));
         CustomAssert.IsFalse(oSimulator.RenameFolder("1", "1.2.3.4"));

         CustomAssert.IsFalse(oSimulator.RenameFolder("1.2.3", "1.2"));
         CustomAssert.IsFalse(oSimulator.RenameFolder("1.2.3", "1"));

         CustomAssert.IsTrue(oSimulator.RenameFolder("1.2.3", "A"));

         oSimulator.Disconnect();
      }
   }
}