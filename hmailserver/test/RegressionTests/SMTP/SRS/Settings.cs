using NUnit.Framework;

namespace RegressionTests.SMTP.SRS
{
   [TestFixture]
   public class Settings : SrsTestBase
   {
      [Test]
      [Description("The server generates a secret of its own, so that SRS works without anyone configuring one.")]
      public void ASecretIsGeneratedAutomatically()
      {
         Assert.IsNotEmpty(Secret);
      }

      [Test]
      [Description("Clearing the secret rotates it rather than leaving the server without one.")]
      public void ClearingTheSecretGeneratesANewOne()
      {
         var originalSecret = Secret;

         _settings.SRSSecret = "";

         Assert.IsNotEmpty(Secret);
         Assert.AreNotEqual(originalSecret, Secret);

         // A secret which is set is kept as it is.
         _settings.SRSSecret = "a secret of my own";
         Assert.AreEqual("a secret of my own", Secret);
      }

      [Test]
      [Description("The settings are stored and read back.")]
      public void TheSettingsAreStoredAndReadBack()
      {
         Assert.IsFalse(_settings.SRSEnabled);

         _settings.SRSEnabled = true;
         Assert.IsTrue(_settings.SRSEnabled);

         _settings.SRSMaxAgeDays = 5;
         Assert.AreEqual(5, _settings.SRSMaxAgeDays);

         _settings.SRSHashLength = 6;
         Assert.AreEqual(6, _settings.SRSHashLength);
      }

      [Test]
      [Description("Zero selects the default rather than a value nothing could be validated with.")]
      public void ZeroSelectsTheDefaults()
      {
         _settings.SRSMaxAgeDays = 0;
         Assert.AreEqual(SrsAddress.DefaultMaxAgeDays, _settings.SRSMaxAgeDays);

         _settings.SRSHashLength = 0;
         Assert.AreEqual(SrsAddress.DefaultHashLength, _settings.SRSHashLength);
      }
   }
}
