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
         SslSetup.SetupSSLPorts(_application);

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
         var smtpClientSimulator = new SMTPClientSimulator(false, 25002);
         smtpClientSimulator.Connect();
         var data1 = smtpClientSimulator.Receive();
         var data = smtpClientSimulator.SendAndReceive("EHLO example.com\r\n");

         CustomAssert.IsTrue(data.Contains("STARTTLS"));
      }

      [Test]
      public void StartTlsCommandShouldSwithToTls()
      {
         var smtpClientSimulator = new SMTPClientSimulator(false, 25002);
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

      [Test]
      public void IfStlsRequiredLogonShouldSucceedIfStls()
      {
         var smtpClientSimulator = new SMTPClientSimulator(false, 25003);
         smtpClientSimulator.Connect();
         var banner = smtpClientSimulator.Receive();
         var capabilities1 = smtpClientSimulator.SendAndReceive("EHLO example.com\r\n");
         CustomAssert.IsTrue(capabilities1.Contains("STARTTLS"));

         smtpClientSimulator.SendAndReceive("STARTTLS\r\n");
         smtpClientSimulator.Handshake();

         var loginResult = smtpClientSimulator.SendAndReceive("AUTH LOGIN\r\n");
         CustomAssert.IsTrue(loginResult.StartsWith("334"));
      }

      [Test]
      public void IfStlsRequiredLogonShouldFailIfNoStls()
      {
         var smtpClientSimulator = new SMTPClientSimulator(false, 25003);
         smtpClientSimulator.Connect();
         var banner = smtpClientSimulator.Receive();
         var capabilities1 = smtpClientSimulator.SendAndReceive("EHLO example.com\r\n");
         CustomAssert.IsTrue(capabilities1.Contains("STARTTLS"));

         var loginResult = smtpClientSimulator.SendAndReceive("AUTH LOGIN\r\n");
         CustomAssert.IsTrue(loginResult.StartsWith("530 Must issue STARTTLS first."));
      }

      [Test]
      public void IfStlsOptionalButSslRequiredByIpRangeForAuthThenAuthShouldFail()
      {
         var range = SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSSLTLSForAuth = true;
         range.Save();

         var smtpClientSimulator = new SMTPClientSimulator(false, 25002);
         smtpClientSimulator.Connect();
         var banner = smtpClientSimulator.Receive();
         var capabilities1 = smtpClientSimulator.SendAndReceive("EHLO example.com\r\n");
         CustomAssert.IsTrue(capabilities1.Contains("STARTTLS"));

         var loginResult = smtpClientSimulator.SendAndReceive("AUTH LOGIN\r\n");
         CustomAssert.IsTrue(loginResult.StartsWith("530 A SSL/TLS-connection is required for authentication.")); // must run starttls first.
      }
   }
}
