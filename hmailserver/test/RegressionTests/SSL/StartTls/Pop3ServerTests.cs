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
    public class Pop3ServerTests : TestFixtureBase
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
            var pop3Simulator = new POP3Simulator(false, 110);
            pop3Simulator.Connect();
            string banner;

            pop3Simulator.ReceiveBanner(out banner);

            var data = pop3Simulator.CAPA();

            CustomAssert.IsFalse(data.Contains("STLS"));
        }

        [Test]
        public void IfStartTlsIsEnabledStartTlsShouldBeShownInEhloResponse()
        {
            var pop3Simulator = new POP3Simulator(false, 11000);
            pop3Simulator.Connect();
            string banner;

            pop3Simulator.ReceiveBanner(out banner);

            var data = pop3Simulator.CAPA();

            CustomAssert.IsTrue(data.Contains("STLS"));
        }

        [Test]
        public void StlsCommandShouldSwithToTls()
        {
           var pop3Simulator = new POP3Simulator(false, 11000);
           pop3Simulator.Connect();
           string banner;
           pop3Simulator.ReceiveBanner(out banner);
           pop3Simulator.STLS();
           pop3Simulator.Handshake();

           // run over TLS.
           pop3Simulator.HELP();

        }
    }
}
