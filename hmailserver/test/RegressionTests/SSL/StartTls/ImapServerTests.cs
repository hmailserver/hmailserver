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
            SslSetup.SetupSSLPorts(_application);

            Thread.Sleep(1000);
        }

        [Test]
        public void IfStartTlsNotEnabledStartTlsShouldNotBeShownInEhloResponse()
        {
           var imapSimulator = new IMAPSimulator(false, 143);
            imapSimulator.Connect();
            var data = imapSimulator.GetCapabilities();

            CustomAssert.IsFalse(data.Contains("STARTTLS"));
        }

        [Test]
        public void IfStartTlsIsEnabledStartTlsShouldBeShownInEhloResponse()
        {
            var imapSimulator = new IMAPSimulator(false, 14302);
            imapSimulator.Connect();
            var data = imapSimulator.GetCapabilities();

            CustomAssert.IsTrue(data.Contains("STARTTLS"));
        }

        [Test]
        public void StlsCommandShouldSwithToTls()
        {
           var imapSimulator = new IMAPSimulator(false, 14302);
           imapSimulator.Connect();
           var data = imapSimulator.GetCapabilities();
           imapSimulator.SendSingleCommand("A01 STARTTLS");
           imapSimulator.Handshake();

           // command is sent over TLS.
           imapSimulator.GetCapabilities();
        }
    }
}
