// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections.Generic;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class SMTPClientTests : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         _queue = _application.GlobalObjects.DeliveryQueue;
         _status = _application.Status;
         // add an account to send from
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }

      #endregion

      private DeliveryQueue _queue;
      private Status _status;
      private Account _account;

      [Test]
      [Description("Make sure that the bounce message doesn't include a SMTP auth password")]
      public void TestAuthFailurePasswordInBounce()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.SimulatedError = SimulatedErrorType.ForceAuthenticationFailure;
            server.StartListen();

            // Add a route so we can connect to localhost.
            Route route = TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);
            route.RelayerRequiresAuth = true;
            route.RelayerAuthUsername = "user@example.com";
            route.SetRelayerAuthPassword("MySecretPassword");

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            string messageText = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

            Assert.IsFalse(messageText.Contains("MySecretPassword"));
            Assert.IsTrue(messageText.Contains("<Password removed>"));
         }
      }

      [Test]
      [Description(
         "Test that hMailServer does not try to connect to itself, if a route is set up incorrectly or DNS records are pointing at localhost"
         )]
      public void TestDeliverToMyselfOnLocalPort()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // Add a route so we can conenct to localhost.
         TestSetup.AddRoutePointingAtLocalhost(1, 25, false);

         // Send message to this route.
         SmtpClientSimulator.StaticSend("test@test.com", "test@dummy-example.com", "subject", "body");

         for (int i = 0; i < 40; i++)
         {
            string s = _status.UndeliveredMessages;
            if (s.Contains("\t\ttest@test.com"))
               break;

            Thread.Sleep(250);
         }

         // Wait for the bounce message to be delivered.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

         string message = Pop3ClientSimulator.AssertGetFirstMessageText("test@test.com", "test");
         Assert.IsTrue(message.Contains("this would mean connecting to myself."));
      }

      [Test]
      [Description("Issue 227, Restarting server doesn't refresh local address list")]
      public void TestDeliverToMyselfOnLocalPortAfterChangedLocalPort()
      {
         TCPIPPorts oPorts = _application.Settings.TCPIPPorts;
         for (int i = 0; i < oPorts.Count; i++)
         {
            TCPIPPort oTestPort = oPorts[i];
            if (oTestPort.Protocol == eSessionType.eSTIMAP)
               oTestPort.PortNumber = 14300;
            else if (oTestPort.Protocol == eSessionType.eSTSMTP)
               oTestPort.PortNumber = 11000;
            else if (oTestPort.Protocol == eSessionType.eSTPOP3)
               oTestPort.PortNumber = 2500;

            oTestPort.Save();
         }

         _application.Stop();
         _application.Start();

         // Set up a server and start listening.
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

            // Send message to this route.
            var sim = new SmtpClientSimulator(false, 11000);
            sim.Send("test@test.com", "user@dummy-example.com", "Test", "Test message");


            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }

         oPorts.SetDefault();
         _application.Stop();
         _application.Start();
      }

      [Test]
      public void TestDeliverySuccess250Recipients()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         for (int i = 0; i < 250; i++)
            deliveryResults["user" + i.ToString() + "@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(3, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.AddRecipientResult(deliveryResults);
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();

            for (int i = 0; i < 250; i++)
               recipients.Add("user" + i.ToString() + "@dummy-example.com");

            smtp.Send("test@test.com", recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      public void TestDeliverySuccess50Recipients()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);


         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         for (int i = 0; i < 50; i++)
            deliveryResults["user" + i.ToString() + "@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can conenct to localhost.
            TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();

            for (int i = 0; i < 50; i++)
               recipients.Add("user" + i.ToString() + "@dummy-example.com");

            smtp.Send("test@test.com", recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }


      [Test]
      [Description(
         "Test delivery to a server which accepts the message but then disconnects immediately before we've sent QUIT."
         )]
      public void TestDeliverySuccessDisconnectAfterMessageAccept()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();
            server.SimulatedError = SimulatedErrorType.DisconnectAfterMessageAccept;

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", "DisconnectAfterAcceptBeforeQuit");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsTrue(server.MessageData.Contains("DisconnectAfterAcceptBeforeQuit"));
         }
      }

      [Test]
      public void TestDeliverySuccessSingleRecipient()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            Assert.IsTrue(server.Conversation.Contains("\r\nDATA\r\n"));

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      public void TestLargeMessageSuccess()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

            // Send message to this route.
            var messageBody = TestSetup.CreateLargeDummyMailBody();

            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", messageBody);

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            Assert.IsTrue(server.Conversation.Contains("\r\nDATA\r\n"));

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsTrue(server.MessageData.Contains(messageBody));
         }
      }

      [Test]
      [Description("Test delivery to a server which accepts the message but does not respond to QUIT.")]
      public void TestDeliverySuccessSingleRecipientMissingQuitResponse()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();
            server.SimulatedError = SimulatedErrorType.DisconnectWithoutReplyOnQuit;

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", "DeliverySuccessNoQuitResponse");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsTrue(server.MessageData.Contains("DeliverySuccessNoQuitResponse"));
         }
      }

      [Test]
      [Description("In this test, the server will respond with a permanent-error on the MAIL FROM command.")]
      public void TestErrorOnMailFrom()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.MailFromResult = 561;
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can conenct to localhost.
            TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            string bounceMessage = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

            Assert.IsTrue(bounceMessage.Contains("MAIL FROM:<test@test.com>"));
            Assert.IsTrue(bounceMessage.Contains("Remote server replied: 561"));
         }
      }

      [Test]
      [Description("Test that an error after the message delivery has started causes a retry.")]
      public void TestFailureAfterConnect()
      {
         _application.Settings.SMTPNoOfTries = 3;
         _application.Settings.SMTPMinutesBetweenTry = 60;

         Assert.AreEqual(0, _status.UndeliveredMessages.Length);
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.SimulatedError = SimulatedErrorType.DisconnectAfterSessionStart;
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send(_account.Address, recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            // Force the message to be bounced.
            CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

            string bounce = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

            Assert.IsTrue(bounce.Contains("test@dummy-example.com"));
            Assert.IsTrue(bounce.Contains("Remote server closed connection."));
            Assert.IsTrue(bounce.Contains("Tried 1 time(s)"));
         }
      }


      [Test]
      [Description("Test that an error after the message delivery completion doens't affect the delivery.")]
      public void TestFailureAfterDelivery()
      {
         _application.Settings.SMTPNoOfTries = 3;
         _application.Settings.SMTPMinutesBetweenTry = 60;

         Assert.AreEqual(0, _status.UndeliveredMessages.Length);
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.QuitResult = 421;
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", "test of error after accepted delivery");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            Assert.IsTrue(server.MessageData.Contains("test of error after accepted delivery"));
         }
      }

      [Test]
      [Description("Test that an error after the message delivery has started causes a retry.")]
      public void TestFailureAfterDeliveryStarted()
      {
         _application.Settings.SMTPNoOfTries = 3;
         _application.Settings.SMTPMinutesBetweenTry = 60;

         Assert.AreEqual(0, _status.UndeliveredMessages.Length);
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.SimulatedError = SimulatedErrorType.DisconnectAfterDeliveryStarted;
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send(_account.Address, recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            // Force the message to be bounced.
            CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

            string bounce = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

            Assert.IsTrue(bounce.Contains("test@dummy-example.com"));
            Assert.IsTrue(bounce.Contains("Remote server closed connection."));
            Assert.IsTrue(bounce.Contains("Tried 1 time(s)"));
         }
      }

      [Test]
      [Description("Test that an error after the message delivery has started causes a retry.")]
      public void TestFailureAfterReceivedHelloBanner()
      {
         _application.Settings.SMTPNoOfTries = 3;
         _application.Settings.SMTPMinutesBetweenTry = 60;

         Assert.AreEqual(0, _status.UndeliveredMessages.Length);
         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.SimulatedError = SimulatedErrorType.DisconnectAfterSessionStart;
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send(_account.Address, recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            // Force the message to be bounced.
            CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

            string bounce = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

            Assert.IsTrue(bounce.Contains("test@dummy-example.com"));
            Assert.IsTrue(bounce.Contains("Remote server closed connection."));
            Assert.IsTrue(bounce.Contains("Tried 1 time(s)"));
         }
      }

      [Test]
      public void TestFatalDeliveryFailure()
      {
         _application.Settings.Logging.Enabled = true;
         _application.Settings.Logging.LogSMTP = true;
         _application.Settings.Logging.LogTCPIP = true;
         _application.Settings.Logging.LogDebug = true;

         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 550;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can conenct to localhost.
            TestSetup.AddRoutePointingAtLocalhost(0, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", "Test message");

            string undeliveredMessages = _status.UndeliveredMessages;

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            // wait for the message to be delivered.
            for (int i = 1; i <= 40; i++)
            {
               Assert.IsFalse(i == 40);

               if (_status.UndeliveredMessages.Contains("\ttest@test.com") || _status.UndeliveredMessages.Length == 0)
                  break;

               Thread.Sleep(250);
            }

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

            string bounceMessage = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

            Assert.IsTrue(bounceMessage.Contains("Remote server (127.0.0.1) issued an error."));
            Assert.IsTrue(bounceMessage.Contains("550 test@dummy-example.com"));
         }
      }

      /// <summary>
      /// 3 recipients. On the first server, 2 recipients are successful and one temp fail.
      /// On the second, the 3'rd one fails with a permanent error.
      /// </summary>
      [Test]
      public void TestMultipleHostsHalfDeliveryOnFirstPermanentOnSecond()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user1@dummy-example.com"] = 250;
         deliveryResults["user2@dummy-example.com"] = 250;
         deliveryResults["user3@dummy-example.com"] = 450;

         var deliveryResultsSecond = new Dictionary<string, int>();
         deliveryResultsSecond["user1@dummy-example.com"] = 550;
         deliveryResultsSecond["user2@dummy-example.com"] = 550;
         deliveryResultsSecond["user3@dummy-example.com"] = 500;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(2, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.AddRecipientResult(deliveryResultsSecond);
            server.StartListen();


            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhostMultipleHosts(1, smtpServerPort);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();

            recipients.Add("user1@dummy-example.com");
            recipients.Add("user2@dummy-example.com");
            recipients.Add("user3@dummy-example.com");

            smtp.Send("test@test.com", recipients, "Test", "Accepted message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            // Trigger a sending of the bounce message.
            CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

            string bounceMessage = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

            Assert.IsFalse(bounceMessage.Contains("RCPT TO:<user1@dummy-example.com>"));
            Assert.IsFalse(bounceMessage.Contains("RCPT TO:<user2@dummy-example.com>"));
            Assert.IsTrue(bounceMessage.Contains("RCPT TO:<user3@dummy-example.com>"));
            Assert.IsTrue(bounceMessage.Contains("500 user3@dummy-example.com"));
         }
      }

      [Test]
      public void TestMultipleHostsTemporaryFailure()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user1@dummy-example.com"] = 250;
         deliveryResults["user2@dummy-example.com"] = 250;
         deliveryResults["user3@dummy-example.com"] = 499;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(2, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            Route route = TestSetup.AddRoutePointingAtLocalhostMultipleHosts(2, smtpServerPort);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();

            recipients.Add("user1@dummy-example.com");
            recipients.Add("user2@dummy-example.com");
            recipients.Add("user3@dummy-example.com");

            smtp.Send("test@test.com", recipients, "Test", "Test message");


            // Wait for the client to disconnect.
            server.WaitForCompletion();
         }


         CustomAsserts.AssertRecipientsInDeliveryQueue(1);

         // Check so that only user 3 remains in the queue.
         Assert.AreEqual(-1, _status.UndeliveredMessages.IndexOf("user1@dummy-example.com"));
         Assert.AreEqual(-1, _status.UndeliveredMessages.IndexOf("user2@dummy-example.com"));
         Assert.AreNotEqual(-1, _status.UndeliveredMessages.IndexOf("user3@dummy-example.com"));

         using (var server = new SmtpServerSimulator(2, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

            server.WaitForCompletion();

            string bounceMessage = Pop3ClientSimulator.AssertGetFirstMessageText("test@test.com", "test");

            Assert.IsFalse(bounceMessage.Contains("user1@dummy-example.com"));
            Assert.IsFalse(bounceMessage.Contains("user2@dummy-example.com"));
            Assert.IsTrue(bounceMessage.Contains("user3@dummy-example.com"));
            Assert.IsTrue(bounceMessage.Contains("499 user3@dummy-example.com"));
            Assert.IsTrue(bounceMessage.Contains("Tried 2 time(s)"));
         }
      }

      [Test]
      public void TestMultipleHostsTemporaryFailureDeliveryOnSecondServer()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user1@dummy-example.com"] = 250;
         deliveryResults["user2@dummy-example.com"] = 250;
         deliveryResults["user3@dummy-example.com"] = 450;

         var deliveryResultsSecond = new Dictionary<string, int>();
         deliveryResultsSecond["user1@dummy-example.com"] = 250;
         deliveryResultsSecond["user2@dummy-example.com"] = 250;
         deliveryResultsSecond["user3@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(2, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.AddRecipientResult(deliveryResultsSecond);
            server.StartListen();


            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhostMultipleHosts(2, smtpServerPort);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();

            recipients.Add("user1@dummy-example.com");
            recipients.Add("user2@dummy-example.com");
            recipients.Add("user3@dummy-example.com");

            smtp.Send("test@test.com", recipients, "Test", "Accepted message");



            // Wait for the client to disconnect.
            server.WaitForCompletion();

            // Trigger a sending of the bounce message.
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            Assert.IsTrue(server.MessageData.Contains("Accepted message"));
         }
      }

      [Test]
      public void TestPartialTemporaryErrorFailure()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user1@dummy-example.com"] = 250;
         deliveryResults["user2@dummy-example.com"] = 250;
         deliveryResults["user3@dummy-example.com"] = 400;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(2, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();

            recipients.Add("user1@dummy-example.com");
            recipients.Add("user2@dummy-example.com");
            recipients.Add("user3@dummy-example.com");

            smtp.Send("test@test.com", recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            // Trigger a sending of the bounce message.
            CustomAsserts.AssertRecipientsInDeliveryQueue(1);

            Assert.AreEqual(-1, _status.UndeliveredMessages.IndexOf("user1@dummy-example.com"));
            Assert.AreEqual(-1, _status.UndeliveredMessages.IndexOf("user2@dummy-example.com"));
            Assert.AreNotEqual(-1, _status.UndeliveredMessages.IndexOf("user3@dummy-example.com"));

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }

         // Attempt to deliver the message again.
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();
            // Force sending of messages
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);
            server.WaitForCompletion();
            
            string bounceMessage = Pop3ClientSimulator.AssertGetFirstMessageText("test@test.com", "test");

            Assert.IsTrue(bounceMessage.Contains("400 user3@dummy-example.com"));
            Assert.IsTrue(bounceMessage.Contains("Tried 2 time(s)"));

            Assert.IsFalse(bounceMessage.Contains("user2@dummy-example.com"));
            Assert.IsFalse(bounceMessage.Contains("user1@dummy-example.com"));
         }
      }

      [Test]
      public void TestPermanentFailure()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 542;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            Assert.IsFalse(server.Conversation.Contains("\r\nDATA\r\n"));
         }

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         string bounce = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

         Assert.IsTrue(bounce.Contains("Remote server replied: 542 test@dummy-example.com"));
      }

      [Test]
      [Description("Tests that the SMTP client times out after 10 minutes.")]
      [Ignore]
      public void TestSMTPClientTimeout()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.SimulatedError = SimulatedErrorType.Sleep15MinutesAfterSessionStart;
            server.SecondsToWaitBeforeTerminate = 20*60;
            server.StartListen();

            // Add a route so we can connect to localhost.
            Route route = TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);
            route.RelayerRequiresAuth = true;
            route.RelayerAuthUsername = "user@example.com";
            route.SetRelayerAuthPassword("MySecretPassword");

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();
         }

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
      }

      [Test]
      public void TestTemporaryFailure()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 452;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(2, smtpServerPort, false);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@dummy-example.com");
            smtp.Send("test@test.com", recipients, "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            Assert.AreNotEqual(0, _status.UndeliveredMessages.Length);
         }
         
         using (var server = new SmtpServerSimulator(1, smtpServerPort))// Start to listen again.
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Deliver the message to the server and then deliver the bounce message.
            CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            Assert.IsFalse(server.Conversation.Contains("\r\nDATA\r\n"));
         }

         // Now the message has bounced.
         string message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

         Assert.IsTrue(message.Contains("452 test@dummy-example.com"));
         Assert.IsTrue(message.Contains("Tried 2 time(s)"));
      }

      [Test]
      public void TestDeliverToServerNotSupportingEHLO()
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
            TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSNone);

            
            SmtpClientSimulator.StaticSend("test@test.com", "user1@dummy-example.com", "Test", "Test message");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      public void TestDeliverToServerNotSupportingHELO()
      {
         Assert.AreEqual(0, _status.UndeliveredMessages.Length);

         var deliveryResults = new Dictionary<string, int>()
            {
               {"user1@dummy-example.com", 250}
            };

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.ServerSupportsHelo = false;
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can conenct to localhost.
            TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSNone);

            // Send message to this route.

            SmtpClientSimulator.StaticSend("test@test.com", "user1@dummy-example.com", "Test", "Test message");


            // Wait for the client to disconnect.
            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

            var msg = Pop3ClientSimulator.AssertGetFirstMessageText("test@test.com", "test");
            Assert.IsTrue(msg.Contains("Remote server replied: 550 Command HELO not recognized."));
         }
      }
   }
}