// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Net;
using System.Security.Authentication;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SSL
{
   [TestFixture]
   public class SslTlsVersionTests : TestFixtureBase
   {
      private hMailServer.Account _account;

      private void SetSslVersions(bool sslv3, bool tlsv10, bool tlsv11, bool tlsv12)
      {
         SslSetup.SetupSSLPorts(_application, new SslVersions()
            {
               Ssl30 = sslv3,
               Tls10 = tlsv10,
               Tls11 = tlsv11,
               Tls12 = tlsv12
            });

         Thread.Sleep(1000);
      }

      [SetUp]
      public new void SetUp()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }

      [Test]
      public void ItShouldBePossibleToEnableSslV3()
      {
         SetSslVersions(true, false, false, false);
         var smtpClientSimulator = new SmtpClientSimulator(true, SslProtocols.Ssl3, 25001, IPAddress.Parse("127.0.0.1"));


         string errorMessage;
         smtpClientSimulator.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test", "test", out errorMessage);


         var message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
         Assert.IsTrue(message.Contains("version=SSLv3"), message);

      }

      [Test]
      public void ItShouldBePossibleToDisableSslV3()
      {
         SetSslVersions(false, true, true, true);
         var smtpClientSimulator = new SmtpClientSimulator(true, SslProtocols.Ssl3, 25001, IPAddress.Parse("127.0.0.1"));

         try
         {
            string errorMessage;
            smtpClientSimulator.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test", "test",
               out errorMessage);

            Assert.Fail("Was able to establish SSLv3 connection");
         }
         catch (System.IO.IOException)
         {
            // expected  
         }

         RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
            {
               var defaultLog = LogHandler.ReadCurrentDefaultLog();
               Assert.IsTrue(defaultLog.Contains("wrong version number"));
            });
      }

      [Test]
      public void WhenSSL3IsDisabledTLSShouldWork()
      {
         SetSslVersions(false, true, true, true);
         var smtpClientSimulator = new SmtpClientSimulator(true, SslProtocols.Tls, 25001, IPAddress.Parse("127.0.0.1"));

         string errorMessage;
         smtpClientSimulator.Send(false, _account.Address, "test", _account.Address, _account.Address, "Test", "test", out errorMessage);

         var message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
         Assert.IsTrue(message.Contains("version=TLSv1"), message);
      }

   
   }
}