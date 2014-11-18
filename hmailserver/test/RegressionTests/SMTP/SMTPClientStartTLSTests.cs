using System.Collections.Generic;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class SMTPClientStartTLSTests : TestFixtureBase
   {
      private Status _status;
      private Account _account;

      [SetUp]
      public new void SetUp()
      {
         _status = _application.Status;

         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }

      [Test]
      public void UseStartTlsIfEnabledAndAvailable()
      {
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSSTARTTLSOptional))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSSTARTTLSOptional);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            smtp.Send("test@test.com", "test@dummy-example.com", "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsNotNullOrEmpty(server.MessageData);

            Assert.IsTrue(LogHandler.DefaultLogContains("220 Ready to start TLS"));
         }
      }

      [Test]
      public void DoNotUseStartTlsIfEnabledButNotAvailable()
      {
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSNone))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSSTARTTLSOptional);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            smtp.Send("test@test.com", "test@dummy-example.com", "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsNotNullOrEmpty(server.MessageData);

            Assert.IsFalse(LogHandler.DefaultLogContains("220 Ready to start TLS"));
         }
      }

      [Test]
      public void DoNotUseStartTlsIfNotEnabledButAvailable()
      {
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSSTARTTLSOptional))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSNone);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            smtp.Send("test@test.com", "test@dummy-example.com", "Test", "Test message");
               

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsNotNullOrEmpty(server.MessageData);

            Assert.IsFalse(LogHandler.DefaultLogContains("220 Ready to start TLS"));
         }
      }

      [Test]
      public void DoNotUseStartTlsIfNotEnabledAndNotAvailable()
      {
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSNone))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSNone);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            smtp.Send("test@test.com", "test@dummy-example.com", "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsNotNullOrEmpty(server.MessageData);

            Assert.IsFalse(LogHandler.DefaultLogContains("220 Ready to start TLS"));
         }
      }

      

      [Test]
      public void TestDelivertoServerNotSupportingEHLOOptionalConnectionSecurity()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         var deliveryResults = new Dictionary<string, int>()
            {
               {"user1@dummy-example.com", 250}
            };

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.ServerSupportsEhlo = false;
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can conenct to localhost.
            TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSSTARTTLSOptional);

            // Send message to this route.

            SmtpClientSimulator.StaticSend("test@test.com", "user1@dummy-example.com", "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      public void TestDeliverToServerNotSupportingEHLORequiredConnectionSecurity()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         var deliveryResults = new Dictionary<string, int>()
            {
               {"user1@dummy-example.com", 250}
            };

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.ServerSupportsEhlo = false;
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can conenct to localhost.
            TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSSTARTTLSRequired);

            // Send message to this route.

            SmtpClientSimulator.StaticSend("test@test.com", "user1@dummy-example.com", "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

            var msg = Pop3ClientSimulator.AssertGetFirstMessageText("test@test.com", "test");
            Assert.IsTrue(msg.Contains("Server does not support EHLO command."));
         }
      }
   }
}
