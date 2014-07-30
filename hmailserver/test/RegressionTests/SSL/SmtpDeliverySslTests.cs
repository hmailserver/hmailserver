using System;
using System.Collections.Generic;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;
using RegressionTests.SMTP;

namespace RegressionTests.SSL
{
   public class SmtpDeliverySslTests : TestFixtureBase
   {
      [Test]
      public void SmtpServerSupportingSSL()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");

         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSTLS))
         {
            server.SetCertificate(SslSetup.GetCertificate());
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = SMTPClientTests.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSTLS);

            var smtpClient = new SMTPClientSimulator();
            CustomAssert.IsTrue(smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message"));

            TestSetup.AssertRecipientsInDeliveryQueue(0);

            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            CustomAssert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      public void SmtpServerSupportingStartTls_StartTlsRequired()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");

         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSSTARTTLSRequired))
         {
            server.SetCertificate(SslSetup.GetCertificate());
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = SMTPClientTests.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSSTARTTLSRequired);

            var smtpClient = new SMTPClientSimulator();
            CustomAssert.IsTrue(smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message"));

            TestSetup.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            CustomAssert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      public void SmtpServerSupportingStartTls_StartTlsOptional()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");

         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSSTARTTLSRequired))
         {
            server.SetCertificate(SslSetup.GetCertificate());
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = SMTPClientTests.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSSTARTTLSOptional);

            var smtpClient = new SMTPClientSimulator();
            CustomAssert.IsTrue(smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message"));

            TestSetup.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            CustomAssert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      public void SmtpServerNOTSupportingStartTls_StartTlsRequired()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");

         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSNone))
         {
            server.SetCertificate(SslSetup.GetCertificate());
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = SMTPClientTests.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSSTARTTLSRequired);

            var smtpClient = new SMTPClientSimulator();
            CustomAssert.IsTrue(smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message"));

            TestSetup.AssertRecipientsInDeliveryQueue(0);

            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            var msg = POP3Simulator.AssertGetFirstMessageText("sender@test.com", "test");

            CustomAssert.IsTrue(msg.Contains("Server does not support STARTTLS"));
         }
      }

      [Test]
      public void SmtpServerNOTSupportingStartTls_StartTlsOptional()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");

         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSNone))
         {
            server.SetCertificate(SslSetup.GetCertificate());
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = SMTPClientTests.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSSTARTTLSOptional);

            var smtpClient = new SMTPClientSimulator();
            CustomAssert.IsTrue(smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message"));

            TestSetup.AssertRecipientsInDeliveryQueue(0);

            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

   }
}
