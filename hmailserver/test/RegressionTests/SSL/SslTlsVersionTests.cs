// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.ComponentModel;
using System.IO;
using System.Net;
using System.Security.Authentication;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SSL
{
   [TestFixture]
   public class SslTlsVersionTests : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
      }

      private Account _account;

      private void SetSslVersions(bool tlsv10, bool tlsv11, bool tlsv12, bool tlsv13)
      {
         SslSetup.SetupSSLPorts(_application, new SslVersions
         {
            Tls10 = tlsv10,
            Tls11 = tlsv11,
            Tls12 = tlsv12,
            Tls13 = tlsv13
         });

         Thread.Sleep(1000);
      }

      [Test]
      public void ItShouldBePossibleToDisableSslV3()
      {
         SetSslVersions(true, true, true, true);
         var smtpClientSimulator =
            new SmtpClientSimulator(true, SslProtocols.Ssl3, 25001, IPAddress.Parse("127.0.0.1"));

         try
         {
            string errorMessage;
            smtpClientSimulator.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test",
               "test",
               out errorMessage);

            Assert.Fail("Was able to establish SSLv3 connection");
         }
         catch (AuthenticationException)
         {
            // on windows 10
         }
         catch (Win32Exception)
         {
            // on newer windows 10
         }
         catch (IOException)
         {
            // on windows xp
         }
      }

      [Test]
      public void WhenSSL3IsDisabledTLSShouldWork()
      {
         SetSslVersions(true, true, true, true);
         var smtpClientSimulator = new SmtpClientSimulator(true, SslProtocols.Tls, 25001, IPAddress.Parse("127.0.0.1"));

         string errorMessage;
         smtpClientSimulator.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test", "test",
            out errorMessage);

         var message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
         Assert.IsTrue(message.Contains("version=TLSv1"), message);
      }

      [Test]
      public void Tls12ShouldWork()
      {
         SetSslVersions(false, false, true, false);
         var smtpClientSimulator = new SmtpClientSimulator(true, SslProtocols.Tls12, 25001, IPAddress.Parse("127.0.0.1"));

         string errorMessage;
         smtpClientSimulator.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test", "test",
            out errorMessage);

         var message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
         Assert.IsTrue(message.Contains("version=TLSv1.2"), message);
      }

      [Test]
      public void WhenTls12IsDisabled_ConnectionWithTls12ShouldFail()
      {
         SetSslVersions(false, false, false, true);
         var smtpClientSimulator = new SmtpClientSimulator(true, SslProtocols.Tls12, 25001, IPAddress.Parse("127.0.0.1"));

         try
         {
            string errorMessage;
            smtpClientSimulator.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test",
               "test",
               out errorMessage);

            Assert.Fail("Was able to establish TLS 1.2 connection when TLS 1.2 was disabled");
         }
         catch (AuthenticationException)
         {
         }
         catch (Win32Exception)
         {
         }
         catch (IOException)
         {
         }
      }
   }
}