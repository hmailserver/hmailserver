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

         _application.Settings.TlsPreferServerCiphers = false;
         _application.Settings.TlsPrioritizeChaCha = false;
      }

      private Account _account;

      [Test]
      public void TlsPreferServerCiphers_DefaultsToFalse()
      {
         Assert.IsFalse(_application.Settings.TlsPreferServerCiphers);
      }

      [Test]
      public void TlsPrioritizeChaCha_DefaultsToFalse()
      {
         Assert.IsFalse(_application.Settings.TlsPrioritizeChaCha);
      }

      [Test]
      public void TlsPreferServerCiphers_CanBeToggled()
      {
         _application.Settings.TlsPreferServerCiphers = true;
         Assert.IsTrue(_application.Settings.TlsPreferServerCiphers);

         _application.Settings.TlsPreferServerCiphers = false;
         Assert.IsFalse(_application.Settings.TlsPreferServerCiphers);
      }

      [Test]
      public void TlsPrioritizeChaCha_CanBeToggled()
      {
         _application.Settings.TlsPrioritizeChaCha = true;
         Assert.IsTrue(_application.Settings.TlsPrioritizeChaCha);

         _application.Settings.TlsPrioritizeChaCha = false;
         Assert.IsFalse(_application.Settings.TlsPrioritizeChaCha);
      }

      [Test]
      public void EnableTlsPreferServerCiphers_DoesNotBreakConnectivity()
      {
         _application.Settings.TlsPreferServerCiphers = true;

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
         _application.Settings.TlsPreferServerCiphers = true;
         _application.Settings.TlsPrioritizeChaCha = true;

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
