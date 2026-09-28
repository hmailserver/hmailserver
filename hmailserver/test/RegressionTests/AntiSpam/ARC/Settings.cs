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
      [Description("The ARC settings should be readable and writable through the API.")]
      public void TestARCSettingsCanBeChanged()
      {
         var antiSpam = _application.Settings.AntiSpam;

         antiSpam.ARCEnabled = true;
         antiSpam.ARCTrustedSealers = "google.com, microsoft.com";

         Assert.IsTrue(antiSpam.ARCEnabled);
         Assert.AreEqual("google.com, microsoft.com", antiSpam.ARCTrustedSealers);

         antiSpam.ARCEnabled = false;
         antiSpam.ARCTrustedSealers = "";

         Assert.IsFalse(antiSpam.ARCEnabled);
         Assert.AreEqual("", antiSpam.ARCTrustedSealers);
      }
   }
}
