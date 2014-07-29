using System;
using System.Collections.Generic;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure.Persistence
{
   [TestFixture]
   public class TCPIPPortTests : TestFixtureBase
   {
      [Test]
      [ExpectedException(ExpectedMessage = "Certificate must be specified.")]
      public void CertificateIsRequiredForSSL()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var port = settings.TCPIPPorts[0];

         if (port.SSLCertificateID > 0)
            Assert.Inconclusive("Test cannot run using port with SSL cert.");

         port.ConnectionSecurity = eConnectionSecurity.eCSTLS;

         port.Save();
      }

      [Test]
      [ExpectedException(ExpectedMessage = "Certificate must be specified.")]
      public void CertificateIsRequiredForStartTLSOptional()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var port = settings.TCPIPPorts[0];

         if (port.SSLCertificateID > 0)
            Assert.Inconclusive("Test cannot run using port with SSL cert.");

         port.ConnectionSecurity = eConnectionSecurity.eCSSTARTTLSOptional;

         port.Save();
      }

      [Test]
      [ExpectedException(ExpectedMessage = "Certificate must be specified.")]
      public void CertificateIsRequiredForStartTLSRequired()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var port = settings.TCPIPPorts[0];

         if (port.SSLCertificateID > 0)
            Assert.Inconclusive("Test cannot run using port with SSL cert.");

         port.ConnectionSecurity = eConnectionSecurity.eCSSTARTTLSRequired;

         port.Save();
      }
   }
}
