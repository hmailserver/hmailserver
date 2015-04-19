using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class Cache : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _settings.Cache.Clear();
         _settings.Cache.Enabled = true;

         while (_application.Domains.Count > 0)
            _application.Domains.DeleteByDBID(_application.Domains[0].ID);

      }


      [Test]
      public void CacheShouldHaveMaxSize()
      {
         _settings.Cache.DomainCacheMaxSizeKb = 40;
         _settings.Cache.AccountCacheMaxSizeKb = 20;


         for (int i = 0; i < 41; i++)
         {
            var domain = _application.Domains.Add();
            domain.Name = string.Format("{0}.example.com", i);
            domain.Active = true;
            domain.Save();

            var account = domain.Accounts.Add();
            account.Address = "test@" + domain.Name;
            account.Password = "test";
            account.Active = true;
            account.Save();
            
            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 0);
         }

         // Before the 41 domain is placed in cache, 10% of the items should be removed,
         // so at this point we should have 40-10%+1 = 37 items in cache
         Assert.AreEqual(37, _settings.Cache.DomainCacheSizeKb);

         // Account max size is set to 20 above, so 10% removal leaves 18, plus the one we just added.
         Assert.AreEqual(19, _settings.Cache.AccountCacheSizeKb);
      }
   }
}
