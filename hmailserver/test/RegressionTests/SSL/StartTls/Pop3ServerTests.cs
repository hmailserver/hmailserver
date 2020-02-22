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
      private hMailServer.Account _account;

      [TestFixtureSetUp]
      public new void TestFixtureSetUp()
      {
         SslSetup.SetupSSLPorts(_application);

         Thread.Sleep(1000);
      }

      [SetUp]
      public new void SetUp()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }

      [Test]
      public void IfStartTlsNotEnabledStartTlsShouldNotBeShownInEhloResponse()
      {
         var pop3Simulator = new Pop3ClientSimulator(false, 110);
         pop3Simulator.Connect();
         string banner;

         pop3Simulator.ReceiveBanner(out banner);

         var data = pop3Simulator.CAPA();

         Assert.IsFalse(data.Contains("STLS"));
      }

      [Test]
      public void IfStartTlsIsEnabledStartTlsShouldBeShownInEhloResponse()
      {
         var pop3Simulator = new Pop3ClientSimulator(false, 11002);
         pop3Simulator.Connect();
         string banner;

         pop3Simulator.ReceiveBanner(out banner);

         var data = pop3Simulator.CAPA();

         Assert.IsTrue(data.Contains("STLS"));
      }

      [Test]
      public void StlsCommandShouldSwithToTls()
      {
         var pop3Simulator = new Pop3ClientSimulator(false, 11002);
         pop3Simulator.Connect();
         string banner;
         pop3Simulator.ReceiveBanner(out banner);
         pop3Simulator.STLS();
         pop3Simulator.Handshake();

         // run over TLS.
         pop3Simulator.HELP();

      }

      [Test]
      public void IfStlsRequiredLogonShouldSucceedIfStls()
      {
         var pop3Simulator = new Pop3ClientSimulator(false, 11003);
         pop3Simulator.Connect();
         string banner;
         pop3Simulator.ReceiveBanner(out banner);
         pop3Simulator.STLS();
         pop3Simulator.Handshake();

         // run over TLS.
         var result = pop3Simulator.User("test@test.com");
         Assert.IsTrue(result.StartsWith("+OK"));
      }

      [Test]
      public void IfStlsRequiredLogonShouldFailIfNoStls()
      {
         var pop3Simulator = new Pop3ClientSimulator(false, 11003);
         pop3Simulator.Connect();
         string banner;
         pop3Simulator.ReceiveBanner(out banner);

         // run over TLS.
         var result = pop3Simulator.User("test@test.com");
         Assert.IsTrue(result.StartsWith("-ERR STLS is required."));
      }

      [Test]
      public void IfStlsOptionalButSslRequiredByIpRangeForAuthThenAuthShouldFail()
      {
         var range = SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSSLTLSForAuth = true;
         range.Save();

         var pop3Simulator = new Pop3ClientSimulator(false, 11002);
         pop3Simulator.Connect();
         string banner;
         pop3Simulator.ReceiveBanner(out banner);

         // run over TLS.
         var result = pop3Simulator.User("test@test.com");
         Assert.IsTrue(result.StartsWith("-ERR A SSL/TLS-connection is required for authentication."));
      }
   }
}
