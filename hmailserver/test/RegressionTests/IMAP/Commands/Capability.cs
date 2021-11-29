using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.IMAP.Commands
{
   [TestFixture]
   public class Capability : TestFixtureBase
   {
      [Test]
      public void TestCapability()
      {
         Settings settings = _settings;

         settings.IMAPIdleEnabled = true;
         settings.IMAPQuotaEnabled = true;
         settings.IMAPSortEnabled = true;

         var simulator = new ImapClientSimulator();
         simulator.Connect();

         string sCapabilities = simulator.GetCapabilities();

         if (sCapabilities.IndexOf(" IDLE") == -1 ||
             sCapabilities.IndexOf(" QUOTA") == -1 ||
             sCapabilities.IndexOf(" SORT") == -1)
         {
            throw new Exception("ERROR - Wrong IMAP CAPABILITY.");
         }

         settings.IMAPIdleEnabled = false;
         settings.IMAPQuotaEnabled = true;
         settings.IMAPSortEnabled = true;
         sCapabilities = simulator.GetCapabilities();

         if (sCapabilities.IndexOf(" IDLE") != -1 ||
             sCapabilities.IndexOf(" QUOTA") == -1 ||
             sCapabilities.IndexOf(" SORT") == -1)
         {
            throw new Exception("ERROR - Wrong IMAP CAPABILITY.");
         }

         settings.IMAPIdleEnabled = false;
         settings.IMAPQuotaEnabled = false;
         settings.IMAPSortEnabled = true;
         sCapabilities = simulator.GetCapabilities();

         if (sCapabilities.IndexOf(" IDLE") != -1 ||
             sCapabilities.IndexOf(" QUOTA") != -1 ||
             sCapabilities.IndexOf(" SORT") == -1)
         {
            throw new Exception("ERROR - Wrong IMAP CAPABILITY.");
         }

         settings.IMAPIdleEnabled = false;
         settings.IMAPQuotaEnabled = false;
         settings.IMAPSortEnabled = false;
         sCapabilities = simulator.GetCapabilities();

         if (sCapabilities.IndexOf(" IDLE") != -1 ||
             sCapabilities.IndexOf(" QUOTA") != -1 ||
             sCapabilities.IndexOf(" SORT") != -1)
         {
            throw new Exception("ERROR - Wrong IMAP CAPABILITY.");
         }

         settings.IMAPIdleEnabled = true;
         settings.IMAPQuotaEnabled = false;
         settings.IMAPSortEnabled = false;
         sCapabilities = simulator.GetCapabilities();

         if (sCapabilities.IndexOf(" IDLE") == -1 ||
             sCapabilities.IndexOf(" QUOTA") != -1 ||
             sCapabilities.IndexOf(" SORT") != -1)
         {
            throw new Exception("ERROR - Wrong IMAP CAPABILITY.");
         }

         settings.IMAPIdleEnabled = true;
         settings.IMAPQuotaEnabled = true;
         settings.IMAPSortEnabled = false;
         sCapabilities = simulator.GetCapabilities();

         if (sCapabilities.IndexOf(" IDLE") == -1 ||
             sCapabilities.IndexOf(" QUOTA") == -1 ||
             sCapabilities.IndexOf(" SORT") != -1)
         {
            throw new Exception("ERROR - Wrong IMAP CAPABILITY.");
         }

         settings.IMAPACLEnabled = true;

         sCapabilities = simulator.GetCapabilities();
         Assert.IsTrue(sCapabilities.Contains(" ACL"));

         settings.IMAPACLEnabled = false;

         sCapabilities = simulator.GetCapabilities();
         Assert.IsFalse(sCapabilities.Contains(" ACL"));

         Assert.IsTrue(sCapabilities.Contains("UIDPLUS"));
      }

   }
}
