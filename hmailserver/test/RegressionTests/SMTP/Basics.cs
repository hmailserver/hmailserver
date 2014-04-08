// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class Basics : TestFixtureBase
   {
      // helper
      private static void Send50Messages()
      {
         for (int i = 0; i < 50; i++)
         {
            var oSMTP = new SMTPClientSimulator();
            oSMTP.Send("test@test.com", "bigaccount@test.com", "Big account email", "Body av email");
         }
      }

      private string EncodeBase64(string s)
      {
         byte[] bytes = Encoding.UTF8.GetBytes(s);
         return Convert.ToBase64String(bytes);
      }

      [Test]
      [Description("Issue 325, Last logon time being updated at wrong point")]
      public void AuthShouldUpdateLastLogonTime()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         DateTime lastLogonTimeBefore = Convert.ToDateTime(account.LastLogonTime);

         Thread.Sleep(1000);
         var sock = new TcpSocket();
         sock.Connect(25);
         Assert.IsTrue(sock.Receive().StartsWith("220"));
         sock.Send("EHLO test.com\r\n");
         Assert.IsTrue(sock.Receive().StartsWith("250"));

         string base64EncodedUsername = EncodeBase64("test@test.com");
         sock.Send("AUTH LOGIN " + base64EncodedUsername + "\r\n");
         Assert.IsTrue(sock.Receive().StartsWith("334"));

         sock.Send(EncodeBase64("test") + "\r\n");
         Assert.IsTrue(sock.Receive().StartsWith("235"));

         DateTime lastLogonTimeAfter =
            Convert.ToDateTime(SingletonProvider<TestSetup>.Instance.GetApp().Domains[0].Accounts[0].LastLogonTime);
         Assert.AreNotEqual(lastLogonTimeBefore, lastLogonTimeAfter);
      }


      [Test]
      [Category("SMTP")]
      [Description("Issue 228. Bounce messages does not contain Subject or Date.")]
      public void BounceMessageShouldContainSubjectAndDate()
      {
         Account senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");
         Account recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@test.com",
                                                                                     "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         // Make sure that no bounce is sent.
         SMTPClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "MySubject", "Test");
         POP3Simulator.AssertGetFirstMessageText(recipientAccount.Address, "test");

         // Build a 2MB string.
         var builder = new StringBuilder();
         for (int i = 0; i < 11000; i++)
         {
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");
         }
         string text = builder.ToString();

         SMTPClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "Test subject", text);

         // Make sure the recipient did not receive it.
         TestSetup.AssertRecipientsInDeliveryQueue(0);
         POP3Simulator.AssertMessageCount(recipientAccount.Address, "test", 0);
         SingletonProvider<TestSetup>.Instance.AssertFilesInUserDirectory(recipientAccount, 0);

         // Make sure it bounced.
         string content = POP3Simulator.AssertGetFirstMessageText(senderAccount.Address, "test");
         Assert.IsTrue(content.Contains("Inbox is full"));
         Assert.IsTrue(content.Contains("Subject: Test subject"));

         // Make sure body contains year.
         int bodyStartPos = content.IndexOf("\r\n\r\n");
         int yearPos = content.IndexOf(DateTime.Now.Year.ToString(), bodyStartPos);
         Assert.IsTrue(yearPos >= 0);
      }

      [Test]
      [Description("Issue 226. Domain alias rewrites sender address.")]
      public void DomainAliasesShouldNotRewriteRecipientList()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();


         DomainAlias oDA = _domain.DomainAliases.Add();
         oDA.AliasName = "dummy-example.com";
         oDA.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "otherguy@test.com", "test");

         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();


            Route route = SMTPClientTests.AddRoutePointingAtLocalhost(1, smtpServerPort, true);

            // Now send a message from test@test.com to test@otherdomain.com.
            // Error scenario:
            // In the recipientlist, otherdomain.com gets rewritten to test.com 
            // since there's a domain alias named otherdomain.co on the domain 
            // test.com.
            //
            // This should not happen. Otherdomain.com is an alias for test.com,
            // but we shouldn't actually modify the recipient address just because
            // of this?
            var smtpClient = new SMTPClientSimulator();
            Assert.IsTrue(smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message"));

            TestSetup.AssertRecipientsInDeliveryQueue(0);

            POP3Simulator.AssertMessageCount(account.Address, "test", 0);

            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      [Description("Issue 325, Last logon time being updated at wrong point")]
      public void MailFromShouldNotUpdatedLastLogonTime()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "us'er@test.com", "test");
         DateTime lastLogonTimeBefore = Convert.ToDateTime(account.LastLogonTime);

         Thread.Sleep(1000);
         SMTPClientSimulator.StaticSend("someone@test.com", "us'er@test.com", "Test", "Test");

         TestSetup.AssertRecipientsInDeliveryQueue(0);
         DateTime lastLogonTimeAfter =
            Convert.ToDateTime(SingletonProvider<TestSetup>.Instance.GetApp().Domains[0].Accounts[0].LastLogonTime);
         Assert.AreEqual(lastLogonTimeBefore, lastLogonTimeAfter);
      }

      [Test]
      [Description("Issue 284. Sender to Alias to Route not working.")]
      public void SendMessageToAliasForwardToRoute()
      {
         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user@test.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort))
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

            var smtpClient = new SMTPClientSimulator();
            Assert.IsTrue(smtpClient.Send("example@example.com", "users@test.com", "Test", "Test message"));
            TestSetup.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      public void TestAntiVirusEnabled()
      {
         SecurityRange range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.EnableAntiVirus = true;
         range.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");

         SMTPClientSimulator.StaticSend("user@test.com", "user@test.com", "Test", "Test");
         POP3Simulator.AssertMessageCount(account.Address, "test", 1);

         Message message = account.IMAPFolders.get_ItemByName("INBOX").Messages[0];
         Assert.IsTrue(message.get_Flag(eMessageFlag.eMFVirusScan));
      }

      [Test]
      [Category("SMTP")]
      [Description("Confirm that deliveries are logged in the awstats log.")]
      public void TestAwstatsLog()
      {
         Settings settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;

         Logging logging = settings.Logging;
         logging.AWStatsEnabled = true;
         logging.Enabled = true;

         if (File.Exists(logging.CurrentAwstatsLog))
            File.Delete(logging.CurrentAwstatsLog);

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         IPAddress localAddress = TestSetup.GetLocalIPAddress();
         var oSMTP = new SMTPClientSimulator(false, 25, localAddress);

         // Delivery from external to local.
         Assert.IsTrue(oSMTP.Send("test@external.com", "test@test.com", "Mail 1", "Mail 1"));
         POP3Simulator.AssertMessageCount("test@test.com", "test", 1);
         string contents = TestSetup.ReadExistingTextFile(logging.CurrentAwstatsLog);
         TestSetup.AssertDeleteFile(logging.CurrentAwstatsLog);
         string expectedString = string.Format("\ttest@external.com\ttest@test.com\t{0}\t127.0.0.1\tSMTP\t?\t250\t",
                                               localAddress);
         Assert.IsTrue(contents.Contains(expectedString), contents);

         // Failed delivery from local to local.
         Assert.IsFalse(oSMTP.Send("test@test.com", "test@test.com", "Mail 1", "Mail 1"));
         contents = TestSetup.ReadExistingTextFile(logging.CurrentAwstatsLog);
         TestSetup.AssertDeleteFile(logging.CurrentAwstatsLog);
         expectedString = string.Format("\ttest@test.com\ttest@test.com\t{0}\t127.0.0.1\tSMTP\t?\t530\t",
                                        localAddress);
         Assert.IsTrue(contents.Contains(expectedString), contents);
      }

      [Test]
      [Description("Issue 291, Sloppy non-delivery report generated")]
      public void TestBounceMessageSyntax()
      {
         Account senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");
         Account recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@test.com",
                                                                                     "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         // Build a 2MB string.
         var builder = new StringBuilder();
         for (int i = 0; i < 11000; i++)
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");

         string text = builder.ToString();

         SMTPClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "", text);

         // Make sure the recipient did not receive it.
         TestSetup.AssertRecipientsInDeliveryQueue(0);

         // Check the syntax in the bounce message.
         string content = POP3Simulator.AssertGetFirstMessageText(senderAccount.Address, "test");

         // The bounce message should contain the MIME-version.
         Assert.IsTrue(content.Contains("MIME-Version: 1.0"));

         // Lines may not end with space if they are QP-encoded.
         Assert.IsTrue(content.Contains("  Subject:=20"));
      }

      

      [Test]
      [Category("SMTP")]
      [Description("Issue 181: Make sure that duplicate Message-ID's aren't added.")]
      public void TestDuplicateMessageIDs()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         string content = "SomeHeader: Text" + Environment.NewLine +
                          "Message-Id: <200903121212246.SM01264@server03>" + Environment.NewLine +
                          "" + Environment.NewLine +
                          "Test";

         SMTPClientSimulator.StaticSendRaw("test@test.com", "test@test.com", content);

         string test = POP3Simulator.AssertGetFirstMessageText("test@test.com", "test");

         Assert.IsTrue(test.Contains("Message-Id"));
         Assert.IsFalse(test.Contains("Message-ID"));
      }

      [Test]
      public void TestEHLOKeywords()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;
         settings.HostName = "examplify.com";

         var socket = new TcpSocket();
         Assert.IsTrue(socket.Connect(25));
         string result = socket.Receive();
         socket.Send("EHLO example.com\r\n");
         result = socket.Receive();
         socket.Disconnect();
         Assert.IsTrue(result.Contains("250-" + settings.HostName));
      }

      [Test]
      public void TestHelo()
      {
         var oSimulator = new SMTPClientSimulator();

         oSimulator.Connect(25);

         string sWelcome = oSimulator.Receive();

         oSimulator.Send("HELO\r\n");
         string sResponse = oSimulator.Receive();

         if (!sResponse.StartsWith("501"))
            throw new Exception("Invalid response to HELO");

         oSimulator.Send("HELO   \r\n");
         sResponse = oSimulator.Receive();

         if (!sResponse.StartsWith("501"))
            throw new Exception("Invalid response to HELO");

         oSimulator.Send("HELO TEST.COM\r\n");
         sResponse = oSimulator.Receive();

         if (!sResponse.StartsWith("250"))
            throw new Exception("Invalid response to HELO");


         oSimulator.Send("HELO   TEST.COM\r\n");
         sResponse = oSimulator.Receive();

         if (!sResponse.StartsWith("250"))
            throw new Exception("Invalid response to HELO");

         oSimulator.Send("EHLO TEST.COM\r\n");
         sResponse = oSimulator.Receive();

         if (!sResponse.StartsWith("250"))
            throw new Exception("Invalid response to HELO");

         oSimulator.Send("EHLO    TEST.COM\r\n");
         sResponse = oSimulator.Receive();

         if (!sResponse.StartsWith("250"))
            throw new Exception("Invalid response to HELO");

         oSimulator.Disconnect();
      }

      [Test]
      [Category("SMTP")]
      [Description("Confirm that it's OK to send MAIL FROM without the < and >")]
      public void TestMailFromSyntaxValidation()
      {
         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var oSMTP = new SMTPClientSimulator();
         oSMTP.Connect(25);

         Assert.IsTrue(oSMTP.Receive().StartsWith("220"));
         oSMTP.Send("HELO test\r\n");
         Assert.IsTrue(oSMTP.Receive().StartsWith("250"));

         // A few tests of invalid syntax.
         Assert.IsFalse(oSMTP.SendAndReceive("MAIL FROM: <test@test.com\r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("MAIL FROM: test@test.com>\r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("MAIL FROM: <    test@test.com    \r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("MAIL FROM: <        \r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("MAIL FROM: >        \r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("MAIL FROM: <sdsdfs@sdsdf.csd\r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("MAIL FROM <sdsdfs@sdsdf.csd>\r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("MAIL FROM  sdsdfs@sdsdf.csd\r\n").StartsWith("250"));

         // Valid syntax, < and >
         Assert.IsTrue(oSMTP.SendAndReceive("MAIL FROM: <test@test.com>\r\n").StartsWith("250"));
         Assert.IsTrue(oSMTP.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(oSMTP.SendAndReceive("MAIL FROM: test@test.com\r\n").StartsWith("250"));
         Assert.IsTrue(oSMTP.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(oSMTP.SendAndReceive("MAIL FROM:    test@test.com   \r\n").StartsWith("250"));
         Assert.IsTrue(oSMTP.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(oSMTP.SendAndReceive("MAIL FROM:test@test.com\r\n").StartsWith("250"));
         Assert.IsTrue(oSMTP.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(oSMTP.SendAndReceive("MAIL FROM:<test@test.com>\r\n").StartsWith("250"));
         Assert.IsTrue(oSMTP.SendAndReceive("RSET\r\n").StartsWith("250"));

         oSMTP.Disconnect();
      }

      [Test]
      [Category("SMTP")]
      [Description("Test account max size limitation.")]
      public void TestMaxSizeLimitation()
      {
         Account senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");
         Account recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@test.com",
                                                                                     "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         // Make sure that no bounce is sent.
         SMTPClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "MySubject", "Test");
         POP3Simulator.AssertGetFirstMessageText(recipientAccount.Address, "test");

         // Build a 2MB string.
         var builder = new StringBuilder();
         for (int i = 0; i < 11000; i++)
         {
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");
         }
         string text = builder.ToString();

         SMTPClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "MySubject", text);

         // Make sure the recipient did not receive it.
         TestSetup.AssertRecipientsInDeliveryQueue(0);
         POP3Simulator.AssertMessageCount(recipientAccount.Address, "test", 0);
         SingletonProvider<TestSetup>.Instance.AssertFilesInUserDirectory(recipientAccount, 0);

         // Make sure it bounced.
         string content = POP3Simulator.AssertGetFirstMessageText(senderAccount.Address, "test");
         Assert.IsTrue(content.Contains("Inbox is full"));
      }

      [Test]
      [Category("SMTP")]
      [Description("Test account max size limitation.")]
      public void TestMaxSizeLimitationMultipleSmallMessages()
      {
         Account senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");
         Account recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@test.com",
                                                                                     "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         // Make sure we can send several messages.
         Assert.IsTrue(SMTPClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "Test", "Test"));
         Assert.IsTrue(SMTPClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "Test", "Test"));

         POP3Simulator.AssertMessageCount(recipientAccount.Address, "test", 2);
      }

      [Test]
      public void TestMediumVolume()
      {
         // Fetch default domain
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "bigaccount@test.com", "test");

         var oSMTP = new SMTPClientSimulator();


         var oThreads = new ArrayList();
         for (int i = 0; i < 5; i++)
         {
            var oThread = new Thread(Send50Messages);
            oThreads.Add(oThread);
            oThread.Start();
         }

         // Join the threads
         for (int i = 0; i < 5; i++)
         {
            var oThread = (Thread) oThreads[i];
            oThread.Join();
         }

         IMAPSimulator.AssertMessageCount("bigaccount@test.com", "test", "Inbox", 250);
      }

      [Test]
      public void TestMultipleHostsLimitMXHosts()
      {
         _settings.MaxNumberOfMXHosts = 1;

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user1@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            Route route = SMTPClientTests.AddRoutePointingAtLocalhostMultipleHosts(2, smtpServerPort);

            // Send message to this route.
            var smtp = new SMTPClientSimulator();
            var recipients = new List<string>();

            recipients.Add("user1@dummy-example.com");

            Assert.IsTrue(SMTPClientSimulator.StaticSend("test@test.com", "user1@dummy-example.com", "Test", "Test"));

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            string log = TestSetup.ReadCurrentDefaultLog();
            Assert.IsTrue(log.Contains("Truncating MX server list."));
         }
      }

      [Test]
      public void TestPostmasterAddressIsAlias()
      {
         _domain.Postmaster = "alias@test.com";
         _domain.Save();

         Alias alias = SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "alias@test.com", "test@test.com");

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         Assert.IsTrue(SMTPClientSimulator.StaticSend("someone@example.com", "someone@test.com", "Test", "Test"));
         POP3Simulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      public void TestPostmasterAddressIsAliasButAliasMissing()
      {
         _domain.Postmaster = "alias@test.com";
         _domain.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         Assert.IsFalse(SMTPClientSimulator.StaticSend("someone@example.com", "someone@test.com", "Test", "Test"));
      }

      [Test]
      public void TestPostmasterInternalSender()
      {
         _domain.Postmaster = "test@test.com";
         _domain.Save();

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         Assert.IsTrue(SMTPClientSimulator.StaticSend("sender@test.com", "someone@test.com", "Test", "Test"));

         POP3Simulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      public void TestPostmasterOnSecondDomain()
      {
         _domain.Postmaster = "test@otherDomain.com";
         _domain.Save();

         Domain otherDomain =
            SingletonProvider<TestSetup>.Instance.AddDomain(SingletonProvider<TestSetup>.Instance.GetApp().Domains,
                                                            "otherDomain.com");

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(otherDomain, "test@otherDomain.com",
                                                                            "test");
         Assert.IsTrue(SMTPClientSimulator.StaticSend("sender@test.com", "someone@test.com", "Test", "Test"));

         POP3Simulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      public void TestPostmasterOnSecondDomainWithDomainAlias()
      {
         Domain otherDomain =
            SingletonProvider<TestSetup>.Instance.AddDomain(SingletonProvider<TestSetup>.Instance.GetApp().Domains,
                                                            "otherDomain.com");
         DomainAlias otherDomainAlias = otherDomain.DomainAliases.Add();
         otherDomainAlias.AliasName = "name2.com";
         otherDomainAlias.Save();

         _domain.Postmaster = "test@name2.com";
         _domain.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(otherDomain, "test@otherDomain.com",
                                                                            "test");
         Assert.IsTrue(SMTPClientSimulator.StaticSend("sender@test.com", "someone@test.com", "Test", "Test"));

         POP3Simulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      [Description("Test that we don't mess up external deliveries with the postmaster address.")]
      public void TestPostmasterRecipientExternal()
      {
         _domain.Postmaster = "test@test.com";
         _domain.Save();

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         Assert.IsFalse(SMTPClientSimulator.StaticSend("sender@example.com", "someone@example.com", "Test", "Test"));
      }

      [Test]
      public void TestPostmasterToPostmasterOnOtherDomain()
      {
         _domain.Postmaster = "nonexistant@otherDomain.com";
         _domain.Save();

         Domain otherDomain =
            SingletonProvider<TestSetup>.Instance.AddDomain(SingletonProvider<TestSetup>.Instance.GetApp().Domains,
                                                            "otherDomain.com");
         otherDomain.Postmaster = "account@otherDomain.com";
         otherDomain.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(otherDomain, "account@otherDomain.com",
                                                                            "test");
         Assert.IsTrue(SMTPClientSimulator.StaticSend("sender@test.com", "someone@test.com", "Test", "Test"));

         POP3Simulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      [Category("SMTP")]
      [Description("Confirm that it's OK to send MAIL FROM without the < and >")]
      public void TestRcptToSyntax()
      {
         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var oSMTP = new SMTPClientSimulator();
         oSMTP.Connect(25);

         Assert.IsTrue(oSMTP.Receive().StartsWith("220"));
         oSMTP.Send("HELO test\r\n");
         Assert.IsTrue(oSMTP.Receive().StartsWith("250"));

         // A few tests of invalid syntax.
         Assert.IsTrue(oSMTP.SendAndReceive("MAIL FROM: <test@test.com>\r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("RCPT TO: test@test.com>\r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("RCPT TO: <test@test.com\r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("RCPT TO <test@test.com\r\n").StartsWith("250"));
         Assert.IsFalse(oSMTP.SendAndReceive("RCPT TO<test@test.com\r\n").StartsWith("250"));

         Assert.IsTrue(oSMTP.SendAndReceive("RCPT TO: <test@test.com>\r\n").StartsWith("250"));
         Assert.IsTrue(oSMTP.SendAndReceive("RCPT TO: test@test.com\r\n").StartsWith("250"));

         oSMTP.Disconnect();
      }

      [Test]
      [Category("SMTP")]
      [Description("Confirm that it's OK to add the same recipient multiple times.")]
      public void TestSameRecipientMultipleTimes()
      {
         Logging logging = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Logging;
         logging.AWStatsEnabled = true;
         logging.Enabled = true;

         if (File.Exists(logging.CurrentAwstatsLog))
            File.Delete(logging.CurrentAwstatsLog);

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var oSMTP = new SMTPClientSimulator();
         oSMTP.Connect(25);
         Assert.IsTrue(oSMTP.Receive().StartsWith("220"));
         oSMTP.Send("HELO test\r\n");
         Assert.IsTrue(oSMTP.Receive().StartsWith("250"));
         oSMTP.Send("MAIL FROM: test@test.com\r\n");
         Assert.IsTrue(oSMTP.Receive().StartsWith("250"));
         oSMTP.Send("RCPT TO: knafve@gmail.com\r\n");
         Assert.IsTrue(oSMTP.Receive().StartsWith("250"));
         oSMTP.Send("RCPT TO: knafve@gmail.com\r\n");
         Assert.IsTrue(oSMTP.Receive().StartsWith("250"));

         oSMTP.Disconnect();
      }

      [Test]
      [Description(
         "Test send email from internal address to external, using no authentication on an IP range with does not require authentication"
         )]
      public void TestSendExternalToExternalPermitted()
      {
         SecurityRange range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthExternalToExternal = false;

         range.Save();

         var oSMTP = new SMTPClientSimulator();
         Assert.IsTrue(oSMTP.Send("test@sdag532sdfagdsa12fsdafdsa1.com",
                                  "test2@dsatwvbsdagdasfds423sdavsagasddas.com", "Mail 1", "Mail 1"));
      }

      [Test]
      public void TestSendToAddressWithQuote()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "us'er@test.com", "test");
         SMTPClientSimulator.StaticSend("someone@test.com", "us'er@test.com", "Test", "Test");
         POP3Simulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      public void TestSendToMultipleAccounts()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multi1@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multi2@test.com", "test");
         Account oAccount3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multi3@test.com", "test");

         var oSMTP = new SMTPClientSimulator();

         var lstRecipients = new List<string>();
         lstRecipients.Add("multi1@test.com");
         lstRecipients.Add("multi2@test.com");
         lstRecipients.Add("multi3@test.com");

         string sBody = "Test of sending same email to multiple accounts.";

         oSMTP.Send(oAccount1.Address, lstRecipients, "Multi test", sBody);

         var oPOP3 = new POP3Simulator();

         string sMessageData = POP3Simulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageData.IndexOf(sBody) < 0)
            throw new Exception("E-mail not found");

         sMessageData = POP3Simulator.AssertGetFirstMessageText(oAccount2.Address, "test");
         if (sMessageData.IndexOf(sBody) < 0)
            throw new Exception("E-mail not found");

         sMessageData = POP3Simulator.AssertGetFirstMessageText(oAccount3.Address, "test");
         if (sMessageData.IndexOf(sBody) < 0)
            throw new Exception("E-mail not found");
      }

      [Test]
      public void TestSendToPostmasterAddress()
      {
         _domain.Postmaster = "test@test.com";
         _domain.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         Assert.IsTrue(SMTPClientSimulator.StaticSend("someone@example.com", "someone@test.com", "Test", "Test"));
         POP3Simulator.AssertMessageCount(account.Address, "test", 1);
      }


      [Test]
      public void TestSendToPostmasterAddressDomainAlias()
      {
         DomainAlias alias = _domain.DomainAliases.Add();
         alias.AliasName = "alias.com";
         alias.Save();

         _domain.Postmaster = "test@alias.com";
         _domain.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         Assert.IsTrue(SMTPClientSimulator.StaticSend("someone@example.com", "someone@test.com", "Test", "Test"));

         POP3Simulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      [Category("SMTP")]
      [Description("Issue 188, SMTP: Permanent error response if disk full ")]
      public void TestTempErrorIfDiskFull()
      {
         Account senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");
         Account recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@test.com",
                                                                                     "test");

         Directories directories = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories;

         string origDataDir = directories.DataDirectory;

         try
         {
            directories.DataDirectory = "C:\\FolderWhichDoesNotExist";

            string result = "";

            var sim = new SMTPClientSimulator();
            sim.Send(senderAccount.Address, recipientAccount.Address, "MySubject", "Test", out result);

            Assert.IsTrue(result.StartsWith("4"), "Expected temporary error, but was: " + result);

            TestSetup.AssertReportedError();
         }
         finally
         {
            directories.DataDirectory = origDataDir;
         }
      }

      [Test]
      public void TestTooManyInvalidCommandsAUTH()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;

         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 3;

         var sim = new SMTPClientSimulator();
         sim.Connect(25);
         sim.Send("EHLO test.com\r\n");

         for (int i = 1; i <= 6; i++)
         {
            try
            {
               sim.Send("AUTH LOGIN\r\n");

               string result = sim.Receive();

               if (result.Contains("Too many invalid commands"))
                  return;

               if (i > 5)
                  break;

               sim.Send("YWNhZGVtaWE=\r\n");
               sim.Receive();
               sim.Send("abc\r\n");
               sim.Receive();
            }
            catch (Exception)
            {
               if (i < 5)
               {
                  Assert.Fail("Was disconnected prematurely.");
               }

               return;
            }
         }

         Assert.Fail("Wasn't disconnected");
      }

      [Test]
      public void TestTooManyInvalidCommandsHELO()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;
         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 3;

         var sim = new SMTPClientSimulator();
         sim.Connect(25);

         for (int i = 1; i <= 6; i++)
         {
            sim.Send("HELO\r\n");
            string result = sim.Receive();

            if (result.Contains("Too many invalid commands"))
               break;

            if (i > 5)
               Assert.Fail();
         }
      }

      /// <summary>
      /// If an error is rejected with a temp-error (4xx), that should not be counted
      /// as an error which makes the client get disconnected.
      /// </summary>
      [Test]
      [Description("Issue 189, Greylisting rejections (try again) are treated as errors ")]
      public void TestTooManyInvalidCommandsTempError()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         for (int i = 0; i < 10; i++)
            SingletonProvider<TestSetup>.Instance.AddAccount(_domain, string.Format("test{0}@test.com", i), "test");

         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;

         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 5;

         settings.AntiSpam.GreyListingEnabled = true;

         var sim = new SMTPClientSimulator();
         sim.Connect(25);
         string res = sim.Receive();
         sim.Send("EHLO test.com\r\n");
         res = sim.Receive();
         sim.Send("MAIL FROM: <test@test.com>\r\n");
         res = sim.Receive();
         for (int i = 1; i < 10; i++)
         {
            string address = string.Format("test{0}@test.com", i);

            sim.Send("RCPT TO: " + address + "\r\n");

            res = sim.Receive();

            Assert.AreEqual("451 Please try again later.\r\n", res);
         }

         sim.Disconnect();
      }

      [Test]
      [Description("Issue 291, Sloppy non-delivery report generated")]
      public void TestValidEmailAddress()
      {
         Account senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");
         Account recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@test.com",
                                                                                     "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         // Build a 2MB string.
         var builder = new StringBuilder();
         for (int i = 0; i < 11000; i++)
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");

         string text = builder.ToString();

         SMTPClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "", text);

         // Make sure the recipient did not receive it.
         TestSetup.AssertRecipientsInDeliveryQueue(0);

         // Check the syntax in the bounce message.
         string content = POP3Simulator.AssertGetFirstMessageText(senderAccount.Address, "test");

         // The bounce message should contain the MIME-version.
         Assert.IsTrue(content.Contains("MIME-Version: 1.0"));

         // Lines may not end with space if they are QP-encoded.
         Assert.IsTrue(content.Contains("  Subject:=20"));
      }

      [Test]
      public void TestWelcomeMessage()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         _settings.WelcomeSMTP = "HOWDYHO";

         var oSimulator = new SMTPClientSimulator();

         string sWelcomeMessage = oSimulator.GetWelcomeMessage();

         if (sWelcomeMessage != "220 HOWDYHO\r\n")
            throw new Exception("ERROR - Wrong welcome message.");
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
         using (var server = new SMTPServerSimulator(1, smtpServerPort))
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

            var smtpClient = new SMTPClientSimulator();
            Assert.IsTrue(smtpClient.Send("example@example.com", "users@test.com", "Test", "Test message"));
            TestSetup.AssertRecipientsInDeliveryQueue(0);

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
         using (var server = new SMTPServerSimulator(1, smtpServerPort))
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

            var smtpClient = new SMTPClientSimulator();

            var recipients = new List<string>()
               {
                  "user1@test.com",
                  "user2@test.com",
                  "user3@test.com",
                  "user4@test.com"
               };

            Assert.IsTrue(smtpClient.Send("example@example.com", recipients, "Test", "Test message"));
            TestSetup.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
            Assert.AreEqual(deliveryResults.Count, server.RcptTosReceived);
         }
      }

      [Test]
      [Description("If a message with 4 recipients on different domains, but all are to be sent through the same SMTP relay, only one message should be sent")]
      public void SmtpRelayShouldConsolidateRecipients()
      {
         // Set up a server listening on port 250 which accepts email for test@test.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user1@test1.com"] = 250;
         deliveryResults["user2@test2.com"] = 250;
         deliveryResults["user3@test3.com"] = 250;
         deliveryResults["user4@test4.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SMTPServerSimulator(1, smtpServerPort))
         {
            _application.Settings.SMTPRelayer = "localhost";
            _application.Settings.SMTPRelayerPort = smtpServerPort;

            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            var smtpClient = new SMTPClientSimulator();

            var recipients = new List<string>()
               {
                  "user1@test1.com",
                  "user2@test2.com",
                  "user3@test3.com",
                  "user4@test4.com"
               };

            Account senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");
            Assert.IsTrue(smtpClient.Send(senderAccount.Address, recipients, "Test", "Test message"));
            TestSetup.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
            Assert.AreEqual(deliveryResults.Count, server.RcptTosReceived);
         }
      }
   }
}