// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.AntiSpam.DMARC
{
   [TestFixture]
   public class Settings : TestFixtureBase
   {
      [Test]
      [Description("The DMARC settings should be readable and writable through the API.")]
      public void TestDMARCSettingsCanBeChanged()
      {
         var antiSpam = _application.Settings.AntiSpam;

         antiSpam.DMARCEnabled = true;
         antiSpam.DMARCFailureScore = 17;
         antiSpam.DMARCHonorPolicy = true;
         antiSpam.AddAuthenticationResultsHeader = true;

         Assert.IsTrue(antiSpam.DMARCEnabled);
         Assert.AreEqual(17, antiSpam.DMARCFailureScore);
         Assert.IsTrue(antiSpam.DMARCHonorPolicy);
         Assert.IsTrue(antiSpam.AddAuthenticationResultsHeader);

         antiSpam.DMARCEnabled = false;
         antiSpam.DMARCFailureScore = 5;
         antiSpam.DMARCHonorPolicy = false;
         antiSpam.AddAuthenticationResultsHeader = false;

         Assert.IsFalse(antiSpam.DMARCEnabled);
         Assert.AreEqual(5, antiSpam.DMARCFailureScore);
         Assert.IsFalse(antiSpam.DMARCHonorPolicy);
         Assert.IsFalse(antiSpam.AddAuthenticationResultsHeader);
      }
   }
}
