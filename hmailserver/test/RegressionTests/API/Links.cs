// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections.Generic;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.API
{
   [TestFixture]
   public class Links : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         _links = _application.Links;
      }

      #endregion

      private hMailServer.Links _links;

      [Test]
      public void TestAccountLink()
      {
         Account account =
            SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         Assert.AreEqual(account.Address, _links.get_Account(account.ID).Address);
      }

      [Test]
      public void TestAliasLink()
      {
         Alias alias =
            SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "mytest@test.com", "test2");

         Assert.AreEqual(alias.Value, _links.get_Alias(alias.ID).Value);
      }

      [Test]
      public void TestDistributionListLink()
      {
         var recipients = new List<string>();
         recipients.Add("Vaffe");
         recipients.Add("Baffe");
         recipients.Add("Laffe");

         DistributionList list =
            SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "mytest@test.com", recipients);

         Assert.AreEqual(recipients.Count, _links.get_DistributionList(list.ID).Recipients.Count);
      }

      [Test]
      public void TestDomainLink()
      {
         Assert.AreEqual(_domain.Name, _links.get_Domain(_domain.ID).Name);
      }
   }
}