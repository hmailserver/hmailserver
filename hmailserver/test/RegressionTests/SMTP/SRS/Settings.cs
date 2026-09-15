using System.Runtime.InteropServices;
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
      [Description("Rotating the secret replaces it with a new one, and is what has to be asked for to lose the old one.")]
      public void TheSecretIsOnlyReplacedWhenRotatingIsAskedFor()
      {
         var originalSecret = Secret;

         // Every address already handed out stops being reversible when the secret
         // changes, so clearing it is refused rather than quietly taken as a rotation.
         Assert.Throws<COMException>(() => _settings.SRSSecret = "");
         Assert.AreEqual(originalSecret, Secret);

         _settings.RotateSRSSecret();

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

         _settings.SRSHashLength = 12;
         Assert.AreEqual(12, _settings.SRSHashLength);
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

      [Test]
      [Description("A setting outside the range the server goes by is reported as the value it goes by.")]
      public void OutOfRangeSettingsAreReportedAsTheyAreApplied()
      {
         _settings.SRSHashLength = 1;
         Assert.AreEqual(SrsAddress.MinHashLength, _settings.SRSHashLength);

         _settings.SRSHashLength = 100;
         Assert.AreEqual(SrsAddress.MaxHashLength, _settings.SRSHashLength);

         _settings.SRSMaxAgeDays = 100000;
         Assert.AreEqual(SrsAddress.MaxMaxAgeDays, _settings.SRSMaxAgeDays);
      }
   }
}
