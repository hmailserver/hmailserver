using System.Runtime.InteropServices;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SSL.StartTls
{
   [TestFixture]
   class ImapServerTests : TestFixtureBase
   {
      private hMailServer.Account _account;

      [TestFixtureSetUp]
      public new void TestFixtureSetUp()
      {
         SslSetup.SetupSSLPorts(_application);

         
         Thread.Sleep(1000);
      }

      [SetUp]
      public void SetUp()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
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

         imapSimulator.Logon(_account.Address, "test");
      }

      [Test]
      public void IfStlsRequiredLogonShouldSucceedIfStls()
      {
         var imapSimulator = new IMAPSimulator(false, 14303);
         imapSimulator.Connect();
         imapSimulator.SendSingleCommand("A01 STARTTLS");
         imapSimulator.Handshake();

         // command is sent over TLS.
         imapSimulator.GetCapabilities();

         CustomAssert.IsTrue(imapSimulator.Logon(_account.Address, "test"));
      }

      [Test]
      public void IfStlsRequiredLogonShouldFailIfNoStls()
      {
         var imapSimulator = new IMAPSimulator(false, 14303);
         imapSimulator.Connect();

         string errorMessage;
         CustomAssert.IsFalse(imapSimulator.Logon(_account.Address, "test", out errorMessage));

         CustomAssert.IsTrue(errorMessage.Contains("A01 BAD STARTTLS is required."));
      }

      [Test]
      public void IfStlsOptionalButSslRequiredByIpRangeForAuthThenAuthShouldFail()
      {
         var range = SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSSLTLSForAuth = true;
         range.Save();

         var imapSimulator = new IMAPSimulator(false, 14302);
         imapSimulator.Connect();

         string errorMessage;
         CustomAssert.IsFalse(imapSimulator.Logon(_account.Address, "test", out errorMessage));
         CustomAssert.IsTrue(errorMessage.Contains("A01 BAD A SSL/TLS-connection is required for authentication."));
      }
   }
}


