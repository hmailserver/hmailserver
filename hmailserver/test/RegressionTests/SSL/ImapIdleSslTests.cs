using System;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SSL
{
   /// <summary>
   ///    IMAP IDLE notifications must reach the client while it is idling, also on TLS
   ///    connections. Regression test for issue #610, where a queued write was withheld on SSL
   ///    connections until the outstanding read completed - i.e. until the client sent DONE.
   /// </summary>
   [TestFixture]
   public class ImapIdleSslTests : TestFixtureBase
   {
      private const int ImapPlainPort = 143;
      private const int ImapTlsPort = 14301;
      private const int ImapStartTlsPort = 14302;

      private Account _account;

      [OneTimeSetUp]
      public new void TestFixtureSetUp()
      {
         SslSetup.SetupSSLPorts(_application);
      }

      [SetUp]
      public new void SetUp()
      {
         _settings.IMAPIdleEnabled = true;

         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "idle-ssl@example.test", "test");
      }

      [Test]
      [Description("A message delivered while idling on a plain connection is announced without DONE.")]
      public void NotificationIsPushedDuringIdleOnPlainPort()
      {
         var simulator = new ImapClientSimulator(false, ImapPlainPort);

         AssertNotificationPushedDuringIdle(simulator, false);
      }

      [Test]
      [Category("SSL")]
      [Description("A message delivered while idling on an implicit TLS connection is announced without DONE.")]
      public void NotificationIsPushedDuringIdleOnTlsPort()
      {
         var simulator = new ImapClientSimulator(true, ImapTlsPort);

         AssertNotificationPushedDuringIdle(simulator, false);
      }

      [Test]
      [Category("SSL")]
      [Description("A message delivered while idling after STARTTLS is announced without DONE.")]
      public void NotificationIsPushedDuringIdleAfterStartTls()
      {
         var simulator = new ImapClientSimulator(false, ImapStartTlsPort);

         AssertNotificationPushedDuringIdle(simulator, true);
      }

      private void AssertNotificationPushedDuringIdle(ImapClientSimulator simulator, bool useStartTls)
      {
         simulator.Connect();

         if (useStartTls)
         {
            simulator.SendSingleCommand("A01 STARTTLS");
            simulator.Handshake();
         }

         Assert.IsTrue(simulator.Logon(_account.Address, "test"), "Logon");
         Assert.IsTrue(simulator.SelectFolder("INBOX"), "SelectFolder");
         Assert.IsTrue(simulator.StartIdle(), "StartIdle");

         Assert.IsFalse(simulator.GetPendingDataExists(), "Unexpected data before delivery.");

         new SmtpClientSimulator().Send(_account.Address, _account.Address, "IDLE Test",
            "This is a test of IDLE");

         // No DONE is sent here - the notification has to arrive on its own.
         if (!simulator.AssertPendingDataExists())
            Assert.Fail("No notification was received while idling. Sent only after DONE?");

         var data = simulator.Receive();

         Assert.IsTrue(data.Contains("EXISTS"),
            "Expected an EXISTS notification while idling, received: " + data);

         string output;
         Assert.IsTrue(simulator.EndIdle(true, out output), "EndIdle");

         simulator.Disconnect();
      }
   }
}
