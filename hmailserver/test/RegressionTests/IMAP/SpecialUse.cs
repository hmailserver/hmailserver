// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class SpecialUse : TestFixtureBase
   {
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
   }
}
