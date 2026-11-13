using System.Runtime.InteropServices;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.API
{
   public class Permissions : TestFixtureBase
   {
      [Test]
      [Description("Issue 303, Domain administrators should not have permission to add new domains.")]
      public void DomainAdminShouldNotBeAbleToAddDomain()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         // Create an account with normal privileges.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelDomainAdmin;
         account.Save();

         var newApp = new Application();
         var authenticated = newApp.Authenticate(account.Address, "test");
         Assert.IsNotNull(authenticated);

         // This should throw an exception.
         var ex = Assert.Throws<COMException>(() => newApp.Domains.Add());
         StringAssert.Contains("You do not have access to this property / method.", ex.Message);
      }

      [Test]
      [Description("Issue 303, Normal users should not have permission to add new domains.")]
      public void NormalUserShouldNotBeAbleToAddDomain()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         // Create an account with normal privileges.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");

         var newApp = new Application();
         var authenticated = newApp.Authenticate(account.Address, "test");
         Assert.IsNotNull(authenticated);

         // This should throw an exception.
         var ex = Assert.Throws<COMException>(() => newApp.Domains.Add());
         StringAssert.Contains("You do not have access to this property / method.", ex.Message);
      }

      [Test]
      [Description("Issue 303, Server admin should be allowed to add domain.")]
      public void ServerAdminShouldBeAbleToAddDomain()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         // Create an account with normal privileges.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelServerAdmin;
         account.Save();

         var newApp = new Application();
         var authenticated = newApp.Authenticate(account.Address, "test");
         Assert.IsNotNull(authenticated);

         // This should throw an exception.
         var newDomain = newApp.Domains.Add();
      }

      [Test]
      public void DomainAdminShouldBeAbleToSaveDomain()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         // Create an account with normal privileges.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelDomainAdmin;
         account.Save();

         var newApp = new Application();
         var authenticated = newApp.Authenticate(account.Address, "test");
         Assert.IsNotNull(authenticated);

         Assert.AreEqual(1, newApp.Domains.Count);

         // Retrieve our domain.
         var newDomain = newApp.Domains[0];
         newDomain.Save();
      }

      [Test]
      public void UserShouldNotBeAbleToSaveDomain()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         // Create an account with normal privileges.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelNormal;
         account.Save();

         var newApp = new Application();
         var authenticated = newApp.Authenticate(account.Address, "test");
         Assert.IsNotNull(authenticated);

         Assert.AreEqual(1, newApp.Domains.Count);

         // Retrieve our domain.
         var newDomain = newApp.Domains[0];
         var ex = Assert.Throws<COMException>(() => newDomain.Save());
         StringAssert.Contains("You do not have access to this property / method.", ex.Message);
      }

      [Test]
      public void UserShouldNotAddRuleToOtherAccount()
      {
         var victim = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "victim@example.test", "test");
         var attackerApp = AuthenticateInOtherDomain(eAdminLevel.hAdminLevelNormal);

         IgnoreAccessDenied(() =>
            {
               var rule = attackerApp.Domains[0].Accounts[0].Rules.Add();
               rule.Name = "Steal";
               rule.Active = true;
               rule.AccountID = victim.ID;
               rule.Save();
            });

         Assert.AreEqual(0, GetAccount(victim.Address).Rules.Count);
      }

      [Test]
      public void UserShouldNotAddGlobalRule()
      {
         var attackerApp = AuthenticateInOtherDomain(eAdminLevel.hAdminLevelNormal);

         IgnoreAccessDenied(() =>
            {
               var rule = attackerApp.Domains[0].Accounts[0].Rules.Add();
               rule.Name = "Steal";
               rule.Active = true;
               rule.AccountID = 0;
               rule.Save();
            });

         Assert.AreEqual(0, _application.Rules.Count);
      }

      [Test]
      public void UserShouldNotAddRuleActionOrCriteriaToOtherAccountsRule()
      {
         var victim = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "victim@example.test", "test");
         var victimRule = victim.Rules.Add();
         victimRule.Name = "Victim rule";
         victimRule.Active = true;
         victimRule.Save();

         var attackerApp = AuthenticateInOtherDomain(eAdminLevel.hAdminLevelNormal);
         var attackerRule = attackerApp.Domains[0].Accounts[0].Rules.Add();
         attackerRule.Name = "Own rule";
         attackerRule.Save();

         IgnoreAccessDenied(() =>
            {
               var action = attackerRule.Actions.Add();
               action.Type = eRuleActionType.eRAForwardEmail;
               action.To = "attacker@other.test";
               action.RuleID = victimRule.ID;
               action.Save();
            });

         IgnoreAccessDenied(() =>
            {
               var criteria = attackerRule.Criterias.Add();
               criteria.UsePredefined = true;
               criteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
               criteria.MatchType = eRuleMatchType.eMTGreaterThan;
               criteria.MatchValue = "0";
               criteria.RuleID = victimRule.ID;
               criteria.Save();
            });

         var rule = GetAccount(victim.Address).Rules.get_ItemByDBID(victimRule.ID);
         Assert.AreEqual(0, rule.Actions.Count);
         Assert.AreEqual(0, rule.Criterias.Count);
      }

      [Test]
      public void UserShouldNotAddFetchAccountToOtherAccount()
      {
         var victim = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "victim@example.test", "test");
         var attackerApp = AuthenticateInOtherDomain(eAdminLevel.hAdminLevelNormal);

         IgnoreAccessDenied(() =>
            {
               var fetchAccount = attackerApp.Domains[0].Accounts[0].FetchAccounts.Add();
               fetchAccount.Enabled = false;
               fetchAccount.Name = "Inject";
               fetchAccount.ServerAddress = "localhost";
               fetchAccount.Port = 110;
               fetchAccount.Username = "user";
               fetchAccount.Password = "pass";
               fetchAccount.AccountID = victim.ID;
               fetchAccount.Save();
            });

         Assert.AreEqual(0, GetAccount(victim.Address).FetchAccounts.Count);
      }

      // Creates attacker@other.test and returns an Application authenticated as it.
      private static Application AuthenticateInOtherDomain(eAdminLevel adminLevel)
      {
         var otherDomain = SingletonProvider<TestSetup>.Instance.AddDomain("other.test");
         var attacker = SingletonProvider<TestSetup>.Instance.AddAccount(otherDomain, "attacker@other.test", "test");
         attacker.AdminLevel = adminLevel;
         attacker.Save();

         var app = new Application();
         Assert.IsNotNull(app.Authenticate(attacker.Address, "test"));
         return app;
      }

      private Account GetAccount(string address)
      {
         return _application.Domains.get_ItemByName("example.test").Accounts.get_ItemByAddress(address);
      }

      // A fix may either deny the call or ignore it, so the tests check the outcome instead.
      private static void IgnoreAccessDenied(System.Action action)
      {
         try
         {
            action();
         }
         catch (COMException)
         {
         }
      }
   }
}