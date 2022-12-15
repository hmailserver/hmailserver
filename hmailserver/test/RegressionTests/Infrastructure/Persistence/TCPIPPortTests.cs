using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
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
      public void CertificateIsRequiredForSSL()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var port = settings.TCPIPPorts[0];

         if (port.SSLCertificateID > 0)
            Assert.Inconclusive("Test cannot run using port with SSL cert.");

         port.ConnectionSecurity = eConnectionSecurity.eCSTLS;

         var ex = Assert.Throws<COMException>(() => port.Save());
         StringAssert.Contains("Certificate must be specified.", ex.Message);

      }

      [Test]
      public void CertificateIsRequiredForStartTLSOptional()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var port = settings.TCPIPPorts[0];

         if (port.SSLCertificateID > 0)
            Assert.Inconclusive("Test cannot run using port with SSL cert.");

         port.ConnectionSecurity = eConnectionSecurity.eCSSTARTTLSOptional;

         var ex = Assert.Throws<COMException>(() => port.Save());
         StringAssert.Contains("Certificate must be specified.", ex.Message);
      }

      [Test]
      public void CertificateIsRequiredForStartTLSRequired()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var port = settings.TCPIPPorts[0];

         if (port.SSLCertificateID > 0)
            Assert.Inconclusive("Test cannot run using port with SSL cert.");

         port.ConnectionSecurity = eConnectionSecurity.eCSSTARTTLSRequired;

         var ex = Assert.Throws<COMException>(() => port.Save());
         StringAssert.Contains("Certificate must be specified.", ex.Message);
      }
   }
}
