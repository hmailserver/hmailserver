// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System.Net;
using System.Security.Authentication;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SSL
{
   [TestFixture]
   public class TlsOptionsTests : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         _application.Settings.TlsOptionPreferServerCiphersEnabled = false;
         _application.Settings.TlsOptionPrioritizeChaChaEnabled = false;
      }

      private Account _account;

      [Test]
      public void TlsOptionPreferServerCiphersEnabled_DefaultsToFalse()
      {
         Assert.IsFalse(_application.Settings.TlsOptionPreferServerCiphersEnabled);
      }

      [Test]
      public void TlsOptionPrioritizeChaChaEnabled_DefaultsToFalse()
      {
         Assert.IsFalse(_application.Settings.TlsOptionPrioritizeChaChaEnabled);
      }

      [Test]
      public void TlsOptionPreferServerCiphersEnabled_CanBeToggled()
      {
         _application.Settings.TlsOptionPreferServerCiphersEnabled = true;
         Assert.IsTrue(_application.Settings.TlsOptionPreferServerCiphersEnabled);

         _application.Settings.TlsOptionPreferServerCiphersEnabled = false;
         Assert.IsFalse(_application.Settings.TlsOptionPreferServerCiphersEnabled);
      }

      [Test]
      public void TlsOptionPrioritizeChaChaEnabled_CanBeToggled()
      {
         _application.Settings.TlsOptionPrioritizeChaChaEnabled = true;
         Assert.IsTrue(_application.Settings.TlsOptionPrioritizeChaChaEnabled);

         _application.Settings.TlsOptionPrioritizeChaChaEnabled = false;
         Assert.IsFalse(_application.Settings.TlsOptionPrioritizeChaChaEnabled);
      }

      [Test]
      public void EnableTlsOptionPreferServerCiphersEnabled_DoesNotBreakConnectivity()
      {
         _application.Settings.TlsOptionPreferServerCiphersEnabled = true;

         SslSetup.SetupSSLPorts(_application);
         Thread.Sleep(1000);

         var smtpClientSimulator = new SmtpClientSimulator(true, SslProtocols.Tls12, 25001, IPAddress.Parse("127.0.0.1"));

         string errorMessage;
         smtpClientSimulator.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test", "test",
            out errorMessage);

         Pop3ClientSimulator.AssertMessageCount(_account.Address, "test", 1);
      }

      [Test]
      public void EnableBothTlsOptions_DoesNotBreakConnectivity()
      {
         _application.Settings.TlsOptionPreferServerCiphersEnabled = true;
         _application.Settings.TlsOptionPrioritizeChaChaEnabled = true;

         SslSetup.SetupSSLPorts(_application);
         Thread.Sleep(1000);

         var smtpClientSimulator = new SmtpClientSimulator(true, SslProtocols.Tls12, 25001, IPAddress.Parse("127.0.0.1"));

         string errorMessage;
         smtpClientSimulator.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test", "test",
            out errorMessage);

         Pop3ClientSimulator.AssertMessageCount(_account.Address, "test", 1);
      }
   }
}
