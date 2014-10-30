using System;
using System.Collections.Generic;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class Routes : TestFixtureBase
   {
      [Test]
      [Description("Issue 284. Sender to Alias to Route not working.")]
      public void SendMessageToAliasForwardToRoute()
      {
         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user@test.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route pointing at localhost
            Route route = _settings.Routes.Add();
            route.DomainName = "test.com";
            route.TargetSMTPHost = "localhost";
            route.TargetSMTPPort = smtpServerPort;
            route.NumberOfTries = 1;
            route.MinutesBetweenTry = 5;
            route.TreatRecipientAsLocalDomain = true;
            route.TreatSenderAsLocalDomain = true;
            route.AllAddresses = false;
            route.Save();

            // Make sure only the specific user is valid.
            RouteAddress routeAddress = route.Addresses.Add();
            routeAddress.Address = "user@" + _domain.Name;
            routeAddress.Save();

            SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "users@test.com", "user@test.com");

            var smtpClient = new SmtpClientSimulator();
            smtpClient.Send("example@example.com", "users@test.com", "Test", "Test message");
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }


      [Test]
      [Description("If both route and SMTP relay is in use, route should have higher priortiy..")]
      public void RoutesShouldHaveHigherPrioThanSMTPRelay()
      {
         // Set up a server listening on port 250 which accepts email for test@test.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user@test.com"] = 250;

         // We set the SMTP relayer here, but this should be ignored since the recipient's
         // address matches a route set up (test.com).
         _application.Settings.SMTPRelayer = "example.com";

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route pointing at localhost
            Route route = _settings.Routes.Add();
            route.DomainName = "test.com";
            route.TargetSMTPHost = "localhost";
            route.TargetSMTPPort = smtpServerPort;
            route.NumberOfTries = 1;
            route.MinutesBetweenTry = 5;
            route.TreatRecipientAsLocalDomain = true;
            route.TreatSenderAsLocalDomain = true;
            route.AllAddresses = false;
            route.Save();

            // Make sure only the specific user is valid.
            RouteAddress routeAddress = route.Addresses.Add();
            routeAddress.Address = "user@" + _domain.Name;
            routeAddress.Save();

            SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "users@test.com", "user@test.com");

            var smtpClient = new SmtpClientSimulator();
            smtpClient.Send("example@example.com", "users@test.com", "Test", "Test message");
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      [Description("If a message with 4 recipients on the same domain is is delivered via a route, only one message should be delivered.")]
      public void RoutesShouldConsolidateRecipients()
      {
         // Set up a server listening on port 250 which accepts email for test@test.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user1@test.com"] = 250;
         deliveryResults["user2@test.com"] = 250;
         deliveryResults["user3@test.com"] = 250;
         deliveryResults["user4@test.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route pointing at localhost
            Route route = _settings.Routes.Add();
            route.DomainName = "test.com";
            route.TargetSMTPHost = "localhost";
            route.TargetSMTPPort = smtpServerPort;
            route.NumberOfTries = 1;
            route.MinutesBetweenTry = 5;
            route.TreatRecipientAsLocalDomain = true;
            route.TreatSenderAsLocalDomain = true;
            route.AllAddresses = true;
            route.Save();

            var smtpClient = new SmtpClientSimulator();

            var recipients = new List<string>()
               {
                  "user1@test.com",
                  "user2@test.com",
                  "user3@test.com",
                  "user4@test.com"
               };

            smtpClient.Send("example@example.com", recipients, "Test", "Test message");
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
            Assert.AreEqual(deliveryResults.Count, server.RcptTosReceived);
         }
      }

      [Test]
      public void RoutesShouldSupportWildcardDomain()
      {
         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user@stuff.example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route pointing at localhost
            Route route = _settings.Routes.Add();
            route.DomainName = "*.example.com";
            route.TargetSMTPHost = "localhost";
            route.TargetSMTPPort = smtpServerPort;
            route.NumberOfTries = 1;
            route.MinutesBetweenTry = 5;
            route.TreatRecipientAsLocalDomain = true;
            route.TreatSenderAsLocalDomain = true;
            route.AllAddresses = true;
            route.Save();

            // Make sure only the specific user is valid.
            RouteAddress routeAddress = route.Addresses.Add();
            routeAddress.Address = "user@" + _domain.Name;
            routeAddress.Save();

            var smtpClient = new SmtpClientSimulator();
            smtpClient.Send("example@example.com", "user@stuff.example.com", "Test", "Test message");
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      [Description("If a client attempts to deliver to a route, but the recipient is not in the route list an error should be returned.")]
      public void RecipientNotInListShouldReturnError()
      {
         // Add a route pointing at localhost
         Route route = _settings.Routes.Add();
         route.DomainName = "test.com";
         route.TargetSMTPHost = "localhost";
         route.TargetSMTPPort = 255;
         route.NumberOfTries = 1;
         route.MinutesBetweenTry = 5;
         route.TreatRecipientAsLocalDomain = true;
         route.TreatSenderAsLocalDomain = true;
         route.AllAddresses = false; // only to recipients in list.
         route.Save();

         var smtpClient = new SmtpClientSimulator();

         string resultMessage = "";
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send("example@example.com", "user1@test.com", "Test", "Test message", out resultMessage));
         Assert.AreEqual("550 Recipient not in route list.", resultMessage);
      }

      [Test]
      public void ShouldBePossibleToSendToRouteWithTargetIPAddress()
      {
         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSNone);
            route.TargetSMTPHost = "127.0.0.1";
            route.Save();
          
            var smtpSimulator = new SmtpClientSimulator();
            smtpSimulator.Send("test@test.com",
                                           "test@dummy-example.com", "Mail 1", "Test message");


            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();
            var log = LogHandler.ReadCurrentDefaultLog();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }
   }
}
