using System.Collections.Generic;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class SMTPClientStartTLSTests : TestFixtureBase
   {
      [Test]
      public void UseStartTlsIfEnabledAndAvailable()
      {
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSSTARTTLSOptional))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = SMTPClientTests.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSSTARTTLSOptional);

            // Send message to this route.
            var smtp = new SMTPClientSimulator();
            if (!smtp.Send("test@test.com", "test@dummy-example.com", "Test", "Test message"))
               CustomAssert.Fail("Delivery failed");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            TestSetup.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsNotNullOrEmpty(server.MessageData);

            CustomAssert.IsTrue(TestSetup.DefaultLogContains("220 Ready to start TLS"));
         }
      }

      [Test]
      public void DoNotUseStartTlsIfEnabledButNotAvailable()
      {
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSNone))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = SMTPClientTests.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSSTARTTLSOptional);

            // Send message to this route.
            var smtp = new SMTPClientSimulator();
            if (!smtp.Send("test@test.com", "test@dummy-example.com", "Test", "Test message"))
               CustomAssert.Fail("Delivery failed");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            TestSetup.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsNotNullOrEmpty(server.MessageData);

            CustomAssert.IsFalse(TestSetup.DefaultLogContains("220 Ready to start TLS"));
         }
      }

      [Test]
      public void DoNotUseStartTlsIfNotEnabledButAvailable()
      {
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSSTARTTLSOptional))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = SMTPClientTests.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSNone);

            // Send message to this route.
            var smtp = new SMTPClientSimulator();
            if (!smtp.Send("test@test.com", "test@dummy-example.com", "Test", "Test message"))
               CustomAssert.Fail("Delivery failed");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            TestSetup.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsNotNullOrEmpty(server.MessageData);

            CustomAssert.IsFalse(TestSetup.DefaultLogContains("220 Ready to start TLS"));
         }
      }

      [Test]
      public void DoNotUseStartTlsIfNotEnabledAndNotAvailable()
      {
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSNone))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = SMTPClientTests.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSNone);

            // Send message to this route.
            var smtp = new SMTPClientSimulator();
            if (!smtp.Send("test@test.com", "test@dummy-example.com", "Test", "Test message"))
               CustomAssert.Fail("Delivery failed");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            TestSetup.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsNotNullOrEmpty(server.MessageData);

            CustomAssert.IsFalse(TestSetup.DefaultLogContains("220 Ready to start TLS"));
         }
      }
   }
}
