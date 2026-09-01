// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.API
{
   [TestFixture]
   public class UtilitiesTests : TestFixtureBase
   {
      [Test]
      [Description("Import the same message twice.")]
      public void TestImportDuplicateMessage()
      {
         var messageText =
            "From: test@example.test\r\n" +
            "\r\n" +
            "Test\r\n";

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var domainPath = Path.Combine(_application.Settings.Directories.DataDirectory, "example.test");
         var accountPath = Path.Combine(domainPath, "test");

         Directory.CreateDirectory(accountPath);

         var fileName = Path.Combine(accountPath, "something.eml");

         File.WriteAllText(fileName, messageText);

         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(fileName, account.ID));
         Assert.IsFalse(_application.Utilities.ImportMessageFromFile(fileName, account.ID));

         Pop3ClientSimulator.AssertMessageCount("test@example.test", "test", 1);
      }

      [Test]
      [Description("Import a mail located properly in a sub directory.")]
      public void TestImportOfMessageInInvalidSubName()
      {
         var messageText =
            "From: test@example.test\r\n" +
            "\r\n" +
            "Test\r\n";

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var domainPath = Path.Combine(_application.Settings.Directories.DataDirectory, "example.test");
         var accountPath = Path.Combine(domainPath, "test");
         Directory.CreateDirectory(accountPath);

         var guid = Guid.NewGuid().ToString();
         var guidPath = Path.Combine(accountPath, guid.Substring(1, 2));
         Directory.CreateDirectory(guidPath);

         var fileName = Path.Combine(guidPath, "§§§§.eml");

         File.WriteAllText(fileName, messageText);

         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(fileName, account.ID));

         var message = _domain.Accounts[0].IMAPFolders.get_ItemByName("Inbox").Messages[0];
         Assert.IsFalse(fileName.Contains("$$$$.eml"));
      }

      [Test]
      [Description(
         "Import all messages in public folders. This must fail, since we don't know what public folder to put it into."
      )]
      public void TestImportOfMessageInPublicFolder()
      {
         var messageText =
            "From: test@example.test\r\n" +
            "\r\n" +
            "Test\r\n";

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         var publicFolder = Path.Combine(_application.Settings.Directories.DataDirectory, "#Public");

         if (!Directory.Exists(publicFolder))
            Directory.CreateDirectory(publicFolder);

         var fileName = Path.Combine(publicFolder, "§§§§.eml");

         File.WriteAllText(fileName, messageText);

         Assert.IsFalse(_application.Utilities.ImportMessageFromFile(fileName, account.ID));
      }

      [Test]
      [Description(
         "Import a message stored in a sub directory of the public folder. This must fail as well, since " +
         "the public IMAP folder isn't part of the path on disk."
      )]
      public void TestImportOfMessageInPublicFolderSubdirectory()
      {
         var messageText =
            "From: test@example.test\r\n" +
            "\r\n" +
            "Test\r\n";

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var guid = Guid.NewGuid().ToString();
         var publicFolder = Path.Combine(_application.Settings.Directories.DataDirectory,
            _application.Settings.PublicFolderDiskName);
         var guidPath = Path.Combine(publicFolder, guid.Substring(1, 2));

         Directory.CreateDirectory(guidPath);

         var fileName = Path.Combine(guidPath, guid + ".eml");

         File.WriteAllText(fileName, messageText);

         Assert.IsFalse(_application.Utilities.ImportMessageFromFile(fileName, account.ID));
         Assert.IsFalse(_application.Utilities.ImportMessageFromFile(fileName, 0));

         // The file should have been left alone.
         Assert.IsTrue(File.Exists(fileName));

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 0);

         Directory.Delete(guidPath, true);
      }

      [Test]
      [Description("Import a mail located properly in a sub directory.")]
      public void TestImportOfMessageInSubdirectory()
      {
         var messageText =
            "From: test@example.test\r\n" +
            "\r\n" +
            "Test\r\n";

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var domainPath = Path.Combine(_application.Settings.Directories.DataDirectory, "example.test");
         var accountPath = Path.Combine(domainPath, "test");
         Directory.CreateDirectory(accountPath);

         var guid = Guid.NewGuid().ToString();
         var guidPath = Path.Combine(accountPath, guid.Substring(1, 2));
         Directory.CreateDirectory(guidPath);

         var fileName = Path.Combine(guidPath, guid + ".eml");

         File.WriteAllText(fileName, messageText);

         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(fileName, account.ID));

         var message = _domain.Accounts[0].IMAPFolders.get_ItemByName("Inbox").Messages[0];
         Assert.AreEqual(fileName, message.Filename);
      }

      [Test]
      [Description("Import a message using the mail importer")]
      public void TestImportOfMessageIntoInbox()
      {
         var messageText =
            "From: test@example.test\r\n" +
            "\r\n" +
            "Test\r\n";

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var domainPath = Path.Combine(_application.Settings.Directories.DataDirectory, "example.test");
         var accountPath = Path.Combine(domainPath, "test");

         Directory.CreateDirectory(accountPath);

         var fileName = Path.Combine(accountPath, "something.eml");

         File.WriteAllText(fileName, messageText);

         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(fileName, account.ID));

         var text = Pop3ClientSimulator.AssertGetFirstMessageText("test@example.test", "test");
         Assert.IsTrue(text.Contains(messageText));
      }

      [Test]
      [Description("Import a message using the mail importer")]
      public void TestImportOfMessageIntoInbox2()
      {
         var messageText =
            "From: test@example.test\r\n" +
            "\r\n" +
            "Test\r\n";

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var domainPath = Path.Combine(_application.Settings.Directories.DataDirectory, "example.test");
         var accountPath = Path.Combine(domainPath, "test");

         Directory.CreateDirectory(accountPath);

         var fileName = Path.Combine(accountPath, "something.eml");

         File.WriteAllText(fileName, messageText);

         Assert.IsTrue(_application.Utilities.ImportMessageFromFileToIMAPFolder(fileName, account.ID, "Inbox"));

         var text = Pop3ClientSimulator.AssertGetFirstMessageText("test@example.test", "test");
         Assert.IsTrue(text.Contains(messageText));
      }

      [Test]
      [Description("Import a message using the mail importer")]
      public void TestImportOfMessageIntoOtherFolder()
      {
         var messageText =
            "From: test@example.test\r\n" +
            "\r\n" +
            "Test\r\n";

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         account.IMAPFolders.Add("Woho");

         var domainPath = Path.Combine(_application.Settings.Directories.DataDirectory, "example.test");
         var accountPath = Path.Combine(domainPath, "test");

         Directory.CreateDirectory(accountPath);
         var fileName = Path.Combine(accountPath, "something.eml");

         File.WriteAllText(fileName, messageText);

         Assert.IsTrue(_application.Utilities.ImportMessageFromFileToIMAPFolder(fileName, account.ID, "Woho"));

         Pop3ClientSimulator.AssertMessageCount("test@example.test", "test", 0);
         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon("test@example.test", "test");
         Assert.AreEqual(1, sim.GetMessageCount("Woho"));
         sim.Disconnect();
      }

      [Test]
      [Description(
         "A message stored in a public folder must be found by its file name, so that the data directory " +
         "synchronizer doesn't consider it an orphan and deletes it."
      )]
      public void TestRetrieveMessageIdForMessageInPublicFolder()
      {
         var publicFolders = _settings.PublicFolders;
         var folder = publicFolders.Add("Share1");
         folder.Save();

         var message = folder.Messages.Add();
         message.Subject = "Test";
         message.Save();

         var fileName = message.Filename;

         Assert.IsTrue(fileName.Contains(_settings.PublicFolderDiskName));
         Assert.AreEqual(message.ID, _application.Utilities.RetrieveMessageID(fileName));
      }

      [Test]
      [Description(
         "Importing a message which is already stored in a public folder should be a no-op. It should " +
         "neither be duplicated nor moved to another file."
      )]
      public void TestImportOfExistingMessageInPublicFolder()
      {
         var publicFolders = _settings.PublicFolders;
         var folder = publicFolders.Add("Share1");
         folder.Save();

         var message = folder.Messages.Add();
         message.Subject = "Test";
         message.Save();

         var fileName = message.Filename;

         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(fileName, 0));

         _application.Reinitialize();

         Assert.AreEqual(1, _settings.PublicFolders[0].Messages.Count);
         Assert.AreEqual(fileName, _settings.PublicFolders[0].Messages[0].Filename);
         Assert.IsTrue(File.Exists(fileName));
      }

      [Test]
      [Description("Let the importer replace the full path in the database with a partial path")]
      public void TestReplaceFullPathInPublicFolderWithPartialPath()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account8@example.test", "test");

         var publicFolders = _settings.PublicFolders;
         var folder = publicFolders.Add("Share1");
         folder.Save();

         var message = folder.Messages.Add();
         message.Subject = "Test";
         message.Save();

         // Move the message file to another folder.
         var publicFolderPath = Path.Combine(_application.Settings.Directories.DataDirectory, "#Public");
         var fileName = Path.Combine(publicFolderPath, "randomMail.eml");
         File.Move(message.Filename, fileName);

         // Update the database with the 'invalid' path.
         var sql = string.Format("update hm_messages set messagefilename = '{0}' where messageid = {1}",
            TestSetup.Escape(fileName), message.ID);
         SingletonProvider<TestSetup>.Instance.GetApp().Database.ExecuteSQL(sql);

         // Now try to insert the message.
         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(fileName, 0));

         _application.Reinitialize();

         var newMessgaeFilename = _settings.PublicFolders[0].Messages[0].Filename;
         Assert.AreNotEqual(fileName, newMessgaeFilename);
         Assert.IsTrue(File.Exists(newMessgaeFilename));
      }

      [Test]
      [Description("Let the importer replace the full path in the database with a partial path")]
      public void TestReplaceFullPathWithPartialPath()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test message", "Test body");

         var folder = account.IMAPFolders.get_ItemByName("Inbox");
         CustomAsserts.AssertFolderMessageCount(folder, 1);

         var message = account.IMAPFolders.get_ItemByName("Inbox").Messages[0];

         // Now nothing should happen.
         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(message.Filename, account.ID));

         // Move the message file to another folder.
         var domainPath = Path.Combine(_application.Settings.Directories.DataDirectory, _domain.Name);
         var accountPath = Path.Combine(domainPath, "test");
         var fileName = Path.Combine(accountPath, "randomMail.eml");
         File.Move(message.Filename, fileName);

         // Update the database with the 'invalid' path.
         var sql = string.Format("update hm_messages set messagefilename = '{0}' where messageid = {1}",
            TestSetup.Escape(fileName), message.ID);
         SingletonProvider<TestSetup>.Instance.GetApp().Database.ExecuteSQL(sql);

         Assert.IsTrue(File.Exists(fileName));
         // Now the file should be moved to the correct path.
         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(fileName, account.ID));

         Assert.IsFalse(File.Exists(fileName));

         // Now nothing should happen because the file is no longer there.
         Assert.IsFalse(_application.Utilities.ImportMessageFromFile(fileName, account.ID));

         var content = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(content.Contains("Test message"));
      }

      [Test]
      [Description("Let the importer replace the full path in the database with a partial path")]
      public void TestReplaceInvalidPathWithCorrectPath()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test message", "Test body");

         var folder = account.IMAPFolders.get_ItemByName("Inbox");
         CustomAsserts.AssertFolderMessageCount(folder, 1);

         var message = account.IMAPFolders.get_ItemByName("Inbox").Messages[0];

         var filename = message.Filename;
         // Now nothing should happen here.
         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(filename, account.ID));
         Assert.IsTrue(File.Exists(filename));

         var sql = string.Format("update hm_messages set messagefilename = '{0}' where messageid = {1}",
            TestSetup.Escape(message.Filename), message.ID);

         SingletonProvider<TestSetup>.Instance.GetApp().Database.ExecuteSQL(sql);

         // Now the path should be replaced.
         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(message.Filename, account.ID));

         // Now nothing should happen.
         Assert.IsTrue(_application.Utilities.ImportMessageFromFile(message.Filename, account.ID));
         Assert.IsTrue(File.Exists(message.Filename));

         var content = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(content.Contains("Test message"));
      }
   }
}