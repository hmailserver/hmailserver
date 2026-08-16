// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.Runtime.InteropServices;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class SpecialUse : TestFixtureBase
   {
      [Test]
      public void TestNewAccountDoesNotGetDefaultSpecialUseFoldersByDefault()
      {
         // CreateDefaultSpecialUseFoldersEnabled is off by default (PerformBasicSetup resets it),
         // so a brand-new account should only get INBOX.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse11@example.test", "test");

         Assert.AreEqual(1, account.IMAPFolders.Count);
         Assert.AreEqual("INBOX", account.IMAPFolders[0].Name);
      }

      [Test]
      public void TestNewAccountGetsDefaultSpecialUseFoldersWhenEnabled()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         settings.CreateDefaultSpecialUseFoldersEnabled = true;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse12@example.test", "test");

         Assert.AreEqual(eSpecialUse.eSUSent, account.IMAPFolders.get_ItemByName("Sent").SpecialUse);
         Assert.AreEqual(eSpecialUse.eSUDrafts, account.IMAPFolders.get_ItemByName("Drafts").SpecialUse);
         Assert.AreEqual(eSpecialUse.eSUTrash, account.IMAPFolders.get_ItemByName("Trash").SpecialUse);
         Assert.AreEqual(eSpecialUse.eSUJunk, account.IMAPFolders.get_ItemByName("Junk").SpecialUse);
      }

      [Test]
      public void TestCapabilityIncludesSpecialUse()
      {
         var simulator = new ImapClientSimulator();
         simulator.Connect();

         var capabilities = simulator.GetCapabilities();

         Assert.IsTrue(capabilities.Contains("SPECIAL-USE"));
         Assert.IsTrue(capabilities.Contains("CREATE-SPECIAL-USE"));

         simulator.Disconnect();
      }

      [Test]
      public void TestListShowsStoredSpecialUseAttributeForLocalizedFolderName()
      {
         // A folder with a non-English name should still be reported with the
         // correct special-use attribute, since it's stored per folder rather
         // than derived from the folder name.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse1@example.test", "test");

         var folder = account.IMAPFolders.Add("Verzonden");
         folder.SpecialUse = eSpecialUse.eSUSent;
         folder.Save();

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");

         var result = simulator.List();

         Assert.IsTrue(result.Contains("\\Sent"));
         Assert.IsTrue(result.Contains("Verzonden"));

         simulator.Disconnect();
      }

      [Test]
      public void TestSpecialUseSurvivesReloadFromDatabase()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse2@example.test", "test");

         var folder = account.IMAPFolders.Add("Papierkorb");
         folder.SpecialUse = eSpecialUse.eSUTrash;
         folder.Save();

         // Fetch a fresh Account object so the folder is reloaded from the database
         // rather than served from any in-memory cache.
         var reloadedAccount = _domain.Accounts.get_ItemByDBID(account.ID);
         var reloadedFolder = reloadedAccount.IMAPFolders.get_ItemByName("Papierkorb");

         Assert.AreEqual(eSpecialUse.eSUTrash, reloadedFolder.SpecialUse);
      }

      [Test]
      public void TestSpecialUseSupportsCombiningFlagsWithBitwiseOr()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse5@example.test", "test");

         var folder = account.IMAPFolders.Add("DraftsAndTrash");
         folder.SpecialUse = eSpecialUse.eSUDrafts | eSpecialUse.eSUTrash;
         folder.Save();

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");

         var result = simulator.List();

         Assert.IsTrue(result.Contains("\\Drafts"));
         Assert.IsTrue(result.Contains("\\Trash"));

         simulator.Disconnect();
      }

      [Test]
      public void TestCreateWithUseAssignsSpecialUse()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse3@example.test", "test");

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");

         var createResult = simulator.Send("A01 CREATE \"Archive\" (USE (\\Archive))");
         Assert.IsTrue(createResult.Contains("A01 OK"));

         var listResult = simulator.List();
         Assert.IsTrue(listResult.Contains("\\Archive"));
         Assert.IsTrue(listResult.Contains("Archive"));

         simulator.Disconnect();
      }

      [Test]
      public void TestCreateWithUseConflictReturnsUseAttrError()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse4@example.test", "test");

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");

         var firstCreateResult = simulator.Send("A01 CREATE \"Archive1\" (USE (\\Archive))");
         Assert.IsTrue(firstCreateResult.Contains("A01 OK"));

         var secondCreateResult = simulator.Send("A02 CREATE \"Archive2\" (USE (\\Archive))");
         Assert.IsTrue(secondCreateResult.Contains("A02 NO"));
         Assert.IsTrue(secondCreateResult.Contains("USEATTR"));

         simulator.Disconnect();
      }

      [Test]
      public void TestCreateWithUnsupportedUseAttributeReturnsUseAttrError()
      {
         // RFC 6154 defines a single response code, USEATTR, for CREATE-with-USE
         // failures - whether the attribute is unrecognized or already in use
         // elsewhere. The server must not invent a non-standard code here.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse6@example.test", "test");

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");

         var createResult = simulator.Send("A01 CREATE \"Bogus\" (USE (\\NotARealAttribute))");
         Assert.IsTrue(createResult.Contains("A01 NO"));
         Assert.IsTrue(createResult.Contains("USEATTR"));

         simulator.Disconnect();
      }

      [Test]
      public void TestCreateWithUseAllAttributeReturnsUseAttrError()
      {
         // \All is defined by RFC 6154 but intentionally not supported: it is
         // "almost certain to represent a virtual mailbox" aggregating messages
         // across all folders, which hMailServer has no concept of.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse9@example.test", "test");

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");

         var createResult = simulator.Send("A01 CREATE \"AllMail\" (USE (\\All))");
         Assert.IsTrue(createResult.Contains("A01 NO"));
         Assert.IsTrue(createResult.Contains("USEATTR"));

         simulator.Disconnect();
      }

      [Test]
      public void TestCreateWithUseFlaggedAttributeReturnsUseAttrError()
      {
         // \Flagged is defined by RFC 6154 but intentionally not supported, for the
         // same reason as \All - it is a virtual, cross-folder aggregate view.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse10@example.test", "test");

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");

         var createResult = simulator.Send("A01 CREATE \"Starred\" (USE (\\Flagged))");
         Assert.IsTrue(createResult.Contains("A01 NO"));
         Assert.IsTrue(createResult.Contains("USEATTR"));

         simulator.Disconnect();
      }

      [Test]
      public void TestListReturnSpecialUseOptionAnnotatesFolders()
      {
         // RFC 6154 LIST-EXTENDED: "LIST "" "*" RETURN (SPECIAL-USE)" must be
         // accepted (not BAD) by a server advertising the SPECIAL-USE capability,
         // and must annotate special-use folders in the response.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse7@example.test", "test");

         var folder = account.IMAPFolders.Add("Sent Items");
         folder.SpecialUse = eSpecialUse.eSUSent;
         folder.Save();

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");

         var result = simulator.Send("A01 LIST \"\" \"*\" RETURN (SPECIAL-USE)");

         Assert.IsTrue(result.Contains("A01 OK"), "Expected extended LIST with RETURN (SPECIAL-USE) to succeed. Result: " + result);
         Assert.IsTrue(result.Contains("\\Sent"));

         simulator.Disconnect();
      }

      [Test]
      public void TestListSelectSpecialUseOptionFiltersFolders()
      {
         // RFC 6154 LIST-EXTENDED: "LIST (SPECIAL-USE) "" "*"" must return only
         // mailboxes that have a special-use attribute set.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse8@example.test", "test");

         var specialFolder = account.IMAPFolders.Add("Trash");
         specialFolder.SpecialUse = eSpecialUse.eSUTrash;
         specialFolder.Save();

         account.IMAPFolders.Add("NotSpecial");

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         simulator.Logon(account.Address, "test");

         var result = simulator.Send("A01 LIST (SPECIAL-USE) \"\" \"*\"");

         Assert.IsTrue(result.Contains("A01 OK"), "Expected extended LIST with SPECIAL-USE selection option to succeed. Result: " + result);
         Assert.IsTrue(result.Contains("Trash"));
         Assert.IsFalse(result.Contains("NotSpecial"));

         simulator.Disconnect();
      }

      [Test]
      public void TestEmptyAccountRetainsSpecialUseFolders()
      {
         // Emptying an account already retains INBOX. Root folders tagged with a
         // special-use attribute (e.g. Sent, Trash) should be retained the same way -
         // only their messages should be cleared, not the folders themselves.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse15@example.test", "test");

         var sent = account.IMAPFolders.Add("Sent");
         sent.SpecialUse = eSpecialUse.eSUSent;
         sent.Save();

         var trash = account.IMAPFolders.Add("Trash");
         trash.SpecialUse = eSpecialUse.eSUTrash;
         trash.Save();

         account.IMAPFolders.Add("NotSpecial");

         account.DeleteMessages();

         var reloadedAccount = _domain.Accounts.get_ItemByDBID(account.ID);

         Assert.IsNotNull(reloadedAccount.IMAPFolders.get_ItemByName("INBOX"));

         var reloadedSent = reloadedAccount.IMAPFolders.get_ItemByName("Sent");
         Assert.IsNotNull(reloadedSent, "Expected the Sent folder to survive emptying the account.");
         Assert.AreEqual(eSpecialUse.eSUSent, reloadedSent.SpecialUse);

         var reloadedTrash = reloadedAccount.IMAPFolders.get_ItemByName("Trash");
         Assert.IsNotNull(reloadedTrash, "Expected the Trash folder to survive emptying the account.");
         Assert.AreEqual(eSpecialUse.eSUTrash, reloadedTrash.SpecialUse);

         Assert.Throws<COMException>(() => reloadedAccount.IMAPFolders.get_ItemByName("NotSpecial"), "Expected the non-special-use folder to be deleted.");
      }

      [Test]
      public void TestEmptyAccountRetainsNestedSpecialUseFolder()
      {
         // Special-use tagging isn't restricted to root folders (IMAPCommandCreate
         // validates uniqueness recursively, not just at the root), so a nested
         // folder like INBOX/Archive tagged \Archive should also survive emptying
         // the account, the same way a root-level special-use folder does.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse17@example.test", "test");

         var inbox = account.IMAPFolders.get_ItemByName("INBOX");
         var archive = inbox.SubFolders.Add("Archive");
         archive.SpecialUse = eSpecialUse.eSUArchive;
         archive.Save();

         account.DeleteMessages();

         var reloadedAccount = _domain.Accounts.get_ItemByDBID(account.ID);
         var reloadedInbox = reloadedAccount.IMAPFolders.get_ItemByName("INBOX");
         var reloadedArchive = reloadedInbox.SubFolders.get_ItemByName("Archive");

         Assert.IsNotNull(reloadedArchive, "Expected the nested special-use folder to survive emptying the account.");
         Assert.AreEqual(eSpecialUse.eSUArchive, reloadedArchive.SpecialUse);
      }

      [Test]
      public void TestDeletingAccountRemovesSpecialUseFolders()
      {
         // Full account deletion must still remove special-use folders - both at the
         // root and nested (unlike emptying the account, which now retains them) -
         // so no orphaned rows are left behind in hm_imapfolders.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "specialuse16@example.test", "test");

         var sent = account.IMAPFolders.Add("Sent");
         sent.SpecialUse = eSpecialUse.eSUSent;
         sent.Save();

         var inbox = account.IMAPFolders.get_ItemByName("INBOX");
         var archive = inbox.SubFolders.Add("Archive");
         archive.SpecialUse = eSpecialUse.eSUArchive;
         archive.Save();

         Assert.DoesNotThrow(() => _domain.Accounts.DeleteByDBID(account.ID));
      }
   }
}
