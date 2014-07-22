using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SSL.StartTls
{
    [TestFixture]
    class ImapServerTests : TestFixtureBase
    {
        [TestFixtureSetUp]
        public new void TestFixtureSetUp()
        {
            SslSetup.SetupSSLPorts(_application, eConnectionSecurity.eCSSTARTTLS);

            Thread.Sleep(1000);
        }

        [Test]
        public void IfStartTlsNotEnabledStartTlsShouldNotBeShownInEhloResponse()
        {
            var smtpClientSimulator = new IMAPSimulator(false, 143);
            smtpClientSimulator.Connect();
            var data = smtpClientSimulator.GetCapabilities();

            CustomAssert.IsFalse(data.Contains("STARTTLS"));
        }

        [Test]
        public void IfStartTlsIsEnabledStartTlsShouldBeShownInEhloResponse()
        {
            var smtpClientSimulator = new IMAPSimulator(false, 14300);
            smtpClientSimulator.Connect();
            var data = smtpClientSimulator.GetCapabilities();

            CustomAssert.IsTrue(data.Contains("STARTTLS"));
        }
    }
}
