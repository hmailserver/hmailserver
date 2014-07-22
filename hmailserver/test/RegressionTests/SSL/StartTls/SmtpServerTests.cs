using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SSL.StartTls
{
   [TestFixture]
   public class SmtpServerTests : TestFixtureBase
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
         var smtpClientSimulator = new SMTPClientSimulator(false, 25);
         smtpClientSimulator.Connect();
         var data1 = smtpClientSimulator.Receive();
         var data = smtpClientSimulator.SendAndReceive("EHLO example.com\r\n");

         CustomAssert.IsFalse(data.Contains("STARTTLS"));
      }

      [Test]
      public void IfStartTlsIsEnabledStartTlsShouldBeShownInEhloResponse()
      {
         var smtpClientSimulator = new SMTPClientSimulator(false, 250);
         smtpClientSimulator.Connect();
         var data1 = smtpClientSimulator.Receive();
         var data = smtpClientSimulator.SendAndReceive("EHLO example.com\r\n");

         CustomAssert.IsTrue(data.Contains("STARTTLS"));
      }

      [Test]
      public void StartTlsCommandShouldSwithToTls()
      {
         var smtpClientSimulator = new SMTPClientSimulator(false, 250);
         smtpClientSimulator.Connect();
         var banner = smtpClientSimulator.Receive();
         var capabilities1 = smtpClientSimulator.SendAndReceive("EHLO example.com\r\n");
         CustomAssert.IsTrue(capabilities1.Contains("STARTTLS"));

         smtpClientSimulator.SendAndReceive("STARTTLS\r\n");
         smtpClientSimulator.Handshake();

         // Send a command over TLS.
         var capabilities2 = smtpClientSimulator.SendAndReceive("EHLO example.com\r\n");
         CustomAssert.IsFalse(capabilities2.Contains("STARTTLS"));

         // We're now on SSL.
      }

   }
}
