using System;
using System.Collections.Generic;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
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
         using (var server = new SmtpServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSTLS))
         {
            server.SetCertificate(SslSetup.GetCertificate());
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSTLS);

            var smtpClient = new SmtpClientSimulator();
            smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
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
         using (var smtpServer = new SmtpServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSSTARTTLSRequired))
         {
            smtpServer.SetCertificate(SslSetup.GetCertificate());
            smtpServer.AddRecipientResult(deliveryResults);
            smtpServer.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSSTARTTLSRequired);

            var smtpClient = new SmtpClientSimulator();
            smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            smtpServer.WaitForCompletion();

            Assert.IsTrue(smtpServer.MessageData.Contains("Test message"), smtpServer.MessageData);
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
         using (var server = new SmtpServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSSTARTTLSRequired))
         {
            server.SetCertificate(SslSetup.GetCertificate());
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSSTARTTLSOptional);

            var smtpClient = new SmtpClientSimulator();
            smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
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
         using (var server = new SmtpServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSNone))
         {
            server.SetCertificate(SslSetup.GetCertificate());
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSSTARTTLSRequired);

            var smtpClient = new SmtpClientSimulator();
            smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            var msg = Pop3ClientSimulator.AssertGetFirstMessageText("sender@test.com", "test");

            Assert.IsTrue(msg.Contains("Server does not support STARTTLS"));
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
         using (var server = new SmtpServerSimulator(1, smtpServerPort, eConnectionSecurity.eCSNone))
         {
            server.SetCertificate(SslSetup.GetCertificate());
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSSTARTTLSOptional);

            var smtpClient = new SmtpClientSimulator();
            smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

   }
}
