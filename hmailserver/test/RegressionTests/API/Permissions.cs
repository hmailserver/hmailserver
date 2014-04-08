using System.Runtime.InteropServices;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.API
{
   public class Permissions : TestFixtureBase
   {
      [Test]
      [Description("Issue 303, Domain administrators should not have permission to add new domains.")]
      [ExpectedException(typeof (COMException), MatchType = MessageMatch.Contains,
         ExpectedMessage = "You do not have access to this property / method.")]
      public void DomainAdminShouldNotBeAbleToAddDomain()
      {
         Domain domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         // Create an account with normal privileges.
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelDomainAdmin;
         account.Save();

         var newApp = new Application();
         Account authenticated = newApp.Authenticate(account.Address, "test");
         Assert.IsNotNull(authenticated);

         // This should throw an exception.
         Domain newDomain = newApp.Domains.Add();
      }

      [Test]
      [Description("Issue 303, Normal users should not have permission to add new domains.")]
      [ExpectedException(typeof (COMException), MatchType = MessageMatch.Contains,
         ExpectedMessage = "You do not have access to this property / method.")]
      public void NormalUserShouldNotBeAbleToAddDomain()
      {
         Domain domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         // Create an account with normal privileges.
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@test.com", "test");

         var newApp = new Application();
         Account authenticated = newApp.Authenticate(account.Address, "test");
         Assert.IsNotNull(authenticated);

         // This should throw an exception.
         Domain newDomain = newApp.Domains.Add();
      }

      [Test]
      [Description("Issue 303, Server admin should be allowed to add domain.")]
      public void ServerAdminShouldBeAbleToAddDomain()
      {
         Domain domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         // Create an account with normal privileges.
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelServerAdmin;
         account.Save();

         var newApp = new Application();
         Account authenticated = newApp.Authenticate(account.Address, "test");
         Assert.IsNotNull(authenticated);

         // This should throw an exception.
         Domain newDomain = newApp.Domains.Add();
      }

      [Test]
      public void DomainAdminShouldBeAbleToSaveDomain()
      {
         Domain domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         // Create an account with normal privileges.
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelDomainAdmin;
         account.Save();

         var newApp = new Application();
         Account authenticated = newApp.Authenticate(account.Address, "test");
         Assert.IsNotNull(authenticated);

         Assert.AreEqual(1, newApp.Domains.Count);

         // Retrieve our domain.
         Domain newDomain = newApp.Domains[0];
         newDomain.Save();
      }

      [Test]
      [ExpectedException(typeof (COMException), MatchType = MessageMatch.Contains,
         ExpectedMessage = "You do not have access to this property / method.")]
      public void UserShouldNotBeAbleToSaveDomain()
      {
         Domain domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         // Create an account with normal privileges.
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@test.com", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelNormal;
         account.Save();

         var newApp = new Application();
         Account authenticated = newApp.Authenticate(account.Address, "test");
         Assert.IsNotNull(authenticated);

         Assert.AreEqual(1, newApp.Domains.Count);

         // Retrieve our domain.
         Domain newDomain = newApp.Domains[0];
         newDomain.Save();
      }
   }
}