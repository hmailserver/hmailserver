// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.AntiSpam.ARC
{
   [TestFixture]
   public class Settings : TestFixtureBase
   {
      [Test]
      [Description("The ARC setting should be readable and writable through the API.")]
      public void TestARCEnabledCanBeChanged()
      {
         var antiSpam = _application.Settings.AntiSpam;

         antiSpam.ARCEnabled = true;
         Assert.IsTrue(antiSpam.ARCEnabled);

         antiSpam.ARCEnabled = false;
         Assert.IsFalse(antiSpam.ARCEnabled);
      }

      [Test]
      [Description("Trusted sealers should be added, changed and removed through the API.")]
      public void TestTrustedSealersCanBeManaged()
      {
         var sealers = _application.Settings.AntiSpam.ARCTrustedSealers;
         Assert.AreEqual(0, sealers.Count);

         var sealer = sealers.Add();
         sealer.Domain = "google.com";
         sealer.Description = "Gmail forwarding";
         sealer.Save();

         sealer = sealers.Add();
         sealer.Domain = " microsoft.com ";
         sealer.Description = "Microsoft 365";
         sealer.Save();

         // A fresh collection reads from the database, sorted by domain.
         sealers = _application.Settings.AntiSpam.ARCTrustedSealers;
         Assert.AreEqual(2, sealers.Count);
         Assert.AreEqual("google.com", sealers[0].Domain);
         Assert.AreEqual("Gmail forwarding", sealers[0].Description);
         Assert.AreEqual("microsoft.com", sealers[1].Domain, "The domain should be stored without surrounding spaces.");

         var google = sealers.get_ItemByName("google.com");
         google.Description = "Changed";
         google.Save();

         sealers = _application.Settings.AntiSpam.ARCTrustedSealers;
         Assert.AreEqual("Changed", sealers.get_ItemByDBID(google.ID).Description);

         sealers.DeleteByDBID(google.ID);

         sealers = _application.Settings.AntiSpam.ARCTrustedSealers;
         Assert.AreEqual(1, sealers.Count);
         Assert.AreEqual("microsoft.com", sealers[0].Domain);
      }
   }
}
