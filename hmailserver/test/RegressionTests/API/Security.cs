// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Runtime.InteropServices;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.API
{
   [TestFixture]
   public class Security : TestFixtureBase
   {
      [Test]
      [ExpectedException(ExpectedMessage = "You do not have access to this property / method.",
         MatchType = MessageMatch.Contains)]
      public void TestDomainAdminAccessBackupManager()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelDomainAdmin;
         account.Save();

         var newApplication = new Application();
         newApplication.Authenticate("user@test.com", "test");
         BackupManager backupManager = newApplication.BackupManager;
      }

      [Test]
      [ExpectedException(ExpectedMessage = "You do not have access to this property / method.",
         MatchType = MessageMatch.Contains)]
      public void TestDomainAdminAccessDatabase()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelDomainAdmin;
         account.Save();

         var newApplication = new Application();
         newApplication.Authenticate("user@test.com", "test");
         Database database = newApplication.Database;
         database.ExecuteSQL("select");
      }

      [Test]
      public void TestDomainAdminAccessOtherDomain()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelDomainAdmin;
         account.Save();

         SingletonProvider<TestSetup>.Instance.AddDomain("example.com");

         var newApplication = new Application();
         newApplication.Authenticate("user@test.com", "test");
         Assert.AreEqual(1, newApplication.Domains.Count);

         Domains domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;
         Assert.AreEqual(2, domains.Count);

         try
         {
            Domain secondDomain = newApplication.Domains.get_ItemByName("example.com");
            Assert.Fail("Was able to access other domain.");
         }
         catch (COMException ex)
         {
            Assert.IsTrue(ex.Message.Contains("Invalid index."));
         }
      }

      [Test]
      [ExpectedException(ExpectedMessage = "You do not have access to this property / method.",
         MatchType = MessageMatch.Contains)]
      public void TestDomainAdminAccessSettings()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelDomainAdmin;
         account.Save();

         var newApplication = new Application();
         newApplication.Authenticate("user@test.com", "test");
         Settings settings = newApplication.Settings;
      }

      [Test]
      [ExpectedException(ExpectedMessage = "You do not have access to this property / method.",
         MatchType = MessageMatch.Contains)]
      public void TestNormalUserAccessBackupManager()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelNormal;
         account.Save();

         var newApplication = new Application();
         newApplication.Authenticate("user@test.com", "test");
         BackupManager backupManager = newApplication.BackupManager;
      }

      [Test]
      [ExpectedException(ExpectedMessage = "You do not have access to this property / method.",
         MatchType = MessageMatch.Contains)]
      public void TestNormalUserAccessDatabase()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelNormal;
         account.Save();

         var newApplication = new Application();
         newApplication.Authenticate("user@test.com", "test");
         Database database = newApplication.Database;
         database.ExecuteSQL("select");
      }

      [Test]
      public void TestNormalUserAccessOtherAccount()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelNormal;
         account.Save();

         Account secondAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "second@test.com", "test");
         secondAccount.AdminLevel = eAdminLevel.hAdminLevelNormal;
         secondAccount.Save();

         var newApplication = new Application();
         newApplication.Authenticate("user@test.com", "test");
         Assert.AreEqual(1, newApplication.Domains.Count);
         Assert.AreEqual(1, newApplication.Domains[0].Accounts.Count);

         Account myAccount = newApplication.Domains[0].Accounts.get_ItemByAddress("user@test.com");

         try
         {
            Account otherAccount = newApplication.Domains[0].Accounts.get_ItemByAddress("second@test.com");

            Assert.Fail();
         }
         catch (COMException ex)
         {
            Assert.IsTrue(ex.Message.Contains("Invalid index."));
         }

         Domains domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;
         Assert.AreEqual(2, domains[0].Accounts.Count);
      }

      [Test]
      public void TestNormalUserAccessOtherDomain()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelNormal;
         account.Save();

         SingletonProvider<TestSetup>.Instance.AddDomain("example.com");

         var newApplication = new Application();
         newApplication.Authenticate("user@test.com", "test");
         Assert.AreEqual(1, newApplication.Domains.Count);

         Domains domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;
         Assert.AreEqual(2, domains.Count);

         try
         {
            Domain secondDomain = newApplication.Domains.get_ItemByName("example.com");
            Assert.Fail();
         }
         catch (COMException ex)
         {
            Assert.IsTrue(ex.Message.Contains("Invalid index."));
         }
      }

      [Test]
      [ExpectedException(ExpectedMessage = "You do not have access to this property / method.",
         MatchType = MessageMatch.Contains)]
      public void TestNormalUserAccessSettings()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelNormal;
         account.Save();

         var newApplication = new Application();
         newApplication.Authenticate("user@test.com", "test");
         Settings settings = newApplication.Settings;
      }
   }
}