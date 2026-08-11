// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class Basics : TestFixtureBase
   {
      // helper
      private static void Send50Messages()
      {
         for (var i = 0; i < 50; i++)
         {
            var smtpClientSimulator = new SmtpClientSimulator();
            smtpClientSimulator.Send("test@example.test", "bigaccount@example.test", "Big account email",
               "Body av email");
         }
      }

      private string EncodeBase64(string s)
      {
         var bytes = Encoding.UTF8.GetBytes(s);
         return Convert.ToBase64String(bytes);
      }

      [Test]
      [Description("Issue 325, Last logon time being updated at wrong point")]
      public void AuthShouldUpdateLastLogonTime()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         var lastLogonTimeBefore = Convert.ToDateTime(account.LastLogonTime);

         Thread.Sleep(1000);
         var sock = new TcpConnection();
         sock.Connect(25);
         Assert.IsTrue(sock.Receive().StartsWith("220"));
         sock.Send("EHLO test.com\r\n");
         Assert.IsTrue(sock.Receive().StartsWith("250"));

         var base64EncodedUsername = EncodeBase64("test@example.test");
         sock.Send("AUTH LOGIN " + base64EncodedUsername + "\r\n");
         Assert.IsTrue(sock.Receive().StartsWith("334"));

         sock.Send(EncodeBase64("test") + "\r\n");
         Assert.IsTrue(sock.Receive().StartsWith("235"));

         var lastLogonTimeAfter =
            Convert.ToDateTime(SingletonProvider<TestSetup>.Instance.GetApp().Domains[0].Accounts[0].LastLogonTime);
         Assert.AreNotEqual(lastLogonTimeBefore, lastLogonTimeAfter);
      }

      [Test]
      public void AuthLoginShouldOnlyBeAllowedOnce()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var sock = new TcpConnection();
         sock.Connect(25);
         Assert.IsTrue(sock.Receive().StartsWith("220"));
         sock.Send("EHLO test.com\r\n");
         Assert.IsTrue(sock.Receive().StartsWith("250"));

         // Login a first time
         var base64EncodedUsername = EncodeBase64("test@example.test");
         sock.Send("AUTH LOGIN " + base64EncodedUsername + "\r\n");
         Assert.IsTrue(sock.Receive().StartsWith("334"));
         sock.Send(EncodeBase64("test") + "\r\n");
         Assert.IsTrue(sock.Receive().StartsWith("235"));

         // Login a second time
         sock.Send("AUTH LOGIN " + base64EncodedUsername + "\r\n");
         Assert.IsTrue(sock.Receive().StartsWith("503 Already authenticated."));
      }


      [Test]
      [Category("SMTP")]
      [Description("Issue 228. Bounce messages does not contain Subject or Date.")]
      public void BounceMessageShouldContainSubjectAndDate()
      {
         var senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");
         var recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "list@example.test",
            "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         // Make sure that no bounce is sent.
         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "MySubject", "Test");
         Pop3ClientSimulator.AssertGetFirstMessageText(recipientAccount.Address, "test");

         // Build a 2MB string.
         var builder = new StringBuilder();
         for (var i = 0; i < 11000; i++)
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");
         var text = builder.ToString();

         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "Test subject", text);

         // Make sure the list did not receive it.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         Pop3ClientSimulator.AssertMessageCount(recipientAccount.Address, "test", 0);
         CustomAsserts.AssertFilesInUserDirectory(recipientAccount, 0);

         // Make sure it bounced.
         var content = Pop3ClientSimulator.AssertGetFirstMessageText(senderAccount.Address, "test");
         Assert.IsTrue(content.Contains("Inbox is full"));
         Assert.IsTrue(content.Contains("Subject: Test subject"));

         // Make sure body contains year.
         var bodyStartPos = content.IndexOf("\r\n\r\n");
         var yearPos = content.IndexOf(DateTime.Now.Year.ToString(), bodyStartPos);
         Assert.IsTrue(yearPos >= 0);
      }

      [Test]
      [Category("SMTP")]
      public void BounceMessageShouldReferenceOriginalMessageID()
      {
         var senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");
         var recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "list@example.test",
            "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         const string messageID = "<dsn-threading@example.test>";
         var builder = new StringBuilder();
         for (var i = 0; i < 11000; i++)
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");

         var content = "Message-ID: " + messageID + "\r\n" +
                       "Subject: Test subject\r\n" +
                       "\r\n" +
                       builder;
         SmtpClientSimulator.StaticSendRaw(senderAccount.Address, recipientAccount.Address, content);

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         var bounce = Pop3ClientSimulator.AssertGetFirstMessageText(senderAccount.Address, "test");
         Assert.IsTrue(bounce.Contains("In-Reply-To: " + messageID));
         Assert.IsTrue(bounce.Contains("References: " + messageID));
      }

      [Test]
      [Description("Issue 226. Domain alias rewrites sender address.")]
      public void DomainAliasesShouldNotRewriteRecipientList()
      {
         var domainAlias = _domain.DomainAliases.Add();
         domainAlias.AliasName = "dummy-example.com";
         domainAlias.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "otherguy@example.test", "test");

         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         var smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();


            var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSNone);

            // Now send a message from test@example.test to test@otherdomain.com.
            // Error scenario:
            // In the recipientlist, otherdomain.com gets rewritten to test.com 
            // since there's a domain alias named otherdomain.co on the domain 
            // test.com.
            //
            // This should not happen. Otherdomain.com is an alias for test.com,
            // but we shouldn't actually modify the list address just because
            // of this.
            var smtpClient = new SmtpClientSimulator();
            smtpClient.Send(account.Address, "test@dummy-example.com", "Test", "Test message");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 0);

            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      [Description("Issue 325, Last logon time being updated at wrong point")]
      public void MailFromShouldNotUpdatedLastLogonTime()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "us'er@example.test", "test");
         var lastLogonTimeBefore = Convert.ToDateTime(account.LastLogonTime);

         Thread.Sleep(1000);
         SmtpClientSimulator.StaticSend("someone@example.test", "us'er@example.test", "Test", "Test");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         var lastLogonTimeAfter =
            Convert.ToDateTime(SingletonProvider<TestSetup>.Instance.GetApp().Domains[0].Accounts[0].LastLogonTime);
         Assert.AreEqual(lastLogonTimeBefore, lastLogonTimeAfter);
      }

      [Test]
      public void TestAntiVirusEnabled()
      {
         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.EnableAntiVirus = true;
         range.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");

         SmtpClientSimulator.StaticSend("user@example.test", "user@example.test", "Test", "Test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var message = account.IMAPFolders.get_ItemByName("INBOX").Messages[0];
         Assert.IsTrue(message.get_Flag(eMessageFlag.eMFVirusScan));
      }


      [Test]
      [Description("Issue 291, Sloppy non-delivery report generated")]
      public void TestBounceMessageSyntax()
      {
         var senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");
         var recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "list@example.test",
            "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         // Build a 2MB string.
         var builder = new StringBuilder();
         for (var i = 0; i < 11000; i++)
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");

         var text = builder.ToString();

         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "", text);

         // Make sure the list did not receive it.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         // Check the syntax in the bounce message.
         var content = Pop3ClientSimulator.AssertGetFirstMessageText(senderAccount.Address, "test");

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
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var content = "SomeHeader: Text" + Environment.NewLine +
                       "Message-Id: <200903121212246.SM01264@server03>" + Environment.NewLine +
                       "" + Environment.NewLine +
                       "Test";

         SmtpClientSimulator.StaticSendRaw("test@example.test", "test@example.test", content);

         var test = Pop3ClientSimulator.AssertGetFirstMessageText("test@example.test", "test");

         Assert.IsTrue(test.Contains("Message-Id"));
         Assert.IsFalse(test.Contains("Message-ID"));
      }

      [Test]
      public void TestEHLOKeywords()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();
         var settings = _settings;
         settings.HostName = "examplify.com";

         var socket = new TcpConnection();
         Assert.IsTrue(socket.Connect(25));
         var result = socket.Receive();
         socket.Send("EHLO example.com\r\n");
         result = socket.Receive();
         socket.Disconnect();
         Assert.IsTrue(result.Contains("250-" + settings.HostName));
      }

      [Test]
      public void TestHelo()
      {
         var simulator = new TcpConnection();

         simulator.Connect(25);

         var sWelcome = simulator.Receive();

         simulator.Send("HELO\r\n");
         var sResponse = simulator.Receive();

         if (!sResponse.StartsWith("501"))
            throw new Exception("Invalid response to HELO");

         simulator.Send("HELO   \r\n");
         sResponse = simulator.Receive();

         if (!sResponse.StartsWith("501"))
            throw new Exception("Invalid response to HELO");

         simulator.Send("HELO TEST.COM\r\n");
         sResponse = simulator.Receive();

         if (!sResponse.StartsWith("250"))
            throw new Exception("Invalid response to HELO");


         simulator.Send("HELO   TEST.COM\r\n");
         sResponse = simulator.Receive();

         if (!sResponse.StartsWith("250"))
            throw new Exception("Invalid response to HELO");

         simulator.Send("EHLO TEST.COM\r\n");
         sResponse = simulator.Receive();

         if (!sResponse.StartsWith("250"))
            throw new Exception("Invalid response to HELO");

         simulator.Send("EHLO    TEST.COM\r\n");
         sResponse = simulator.Receive();

         if (!sResponse.StartsWith("250"))
            throw new Exception("Invalid response to HELO");

         simulator.Disconnect();
      }

      [Test]
      [Category("SMTP")]
      [Description("Confirm that it's OK to send MAIL FROM without the < and >")]
      public void TestMailFromSyntaxValidation()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);

         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));

         // A few tests of invalid syntax.
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: <test@example.test\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: test@example.test>\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: <    test@example.test    \r\n")
            .StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: <        \r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: >        \r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: <sdsdfs@sdsdf.csd\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM <sdsdfs@sdsdf.csd>\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM  sdsdfs@sdsdf.csd\r\n").StartsWith("250"));

         // Valid syntax, < and >
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM: <test@example.test>\r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM: test@example.test\r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM:    test@example.test   \r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM:test@example.test\r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM:<test@example.test>\r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RSET\r\n").StartsWith("250"));

         smtpClientSimulator.Disconnect();
      }

      [Test]
      [Category("SMTP")]
      [Description("Test account max size limitation.")]
      public void TestMaxSizeLimitation()
      {
         var senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");
         var recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "list@example.test",
            "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         // Make sure that no bounce is sent.
         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "MySubject", "Test");
         Pop3ClientSimulator.AssertGetFirstMessageText(recipientAccount.Address, "test");

         // Build a 2MB string.
         var builder = new StringBuilder();
         for (var i = 0; i < 11000; i++)
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");
         var text = builder.ToString();

         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "MySubject", text);

         // Make sure the list did not receive it.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         Pop3ClientSimulator.AssertMessageCount(recipientAccount.Address, "test", 0);
         CustomAsserts.AssertFilesInUserDirectory(recipientAccount, 0);

         // Make sure it bounced.
         var content = Pop3ClientSimulator.AssertGetFirstMessageText(senderAccount.Address, "test");
         Assert.IsTrue(content.Contains("Inbox is full"));
      }

      [Test]
      [Category("SMTP")]
      [Description("Test account max size limitation.")]
      public void TestMaxSizeLimitation_AnnouncedText()
      {
         _settings.MaxMessageSize = 102400;

         // Make sure that no bounce is sent.
         var smtpClient = new SmtpClientSimulator();
         smtpClient.Connect();
         smtpClient.Receive(); // Receive banner

         var ehloResponse = smtpClient.SendAndReceive("EHLO example.com\r\n");

         StringAssert.Contains("250-SIZE 104857600", ehloResponse, ehloResponse);
      }

      [Test]
      [Category("SMTP")]
      [Description("Test account max size limitation.")]
      public void TestMaxSizeLimitationMultipleSmallMessages()
      {
         var senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");
         var recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "list@example.test",
            "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         // Make sure we can send several messages.
         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "Test", "Test");
         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(recipientAccount.Address, "test", 2);
      }

      [Test]
      public void TestMediumVolume()
      {
         // Fetch default domain
         var application = SingletonProvider<TestSetup>.Instance.GetApp();

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "bigaccount@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();


         var oThreads = new ArrayList();
         for (var i = 0; i < 5; i++)
         {
            var oThread = new Thread(Send50Messages);
            oThreads.Add(oThread);
            oThread.Start();
         }

         // Join the threads
         for (var i = 0; i < 5; i++)
         {
            var oThread = (Thread)oThreads[i];
            oThread.Join();
         }

         ImapClientSimulator.AssertMessageCount("bigaccount@example.test", "test", "Inbox", 250);
      }

      [Test]
      public void TestMultipleHostsLimitMXHosts()
      {
         _settings.MaxNumberOfMXHosts = 1;

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user1@dummy-example.com"] = 250;

         var smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            var route = TestSetup.AddRoutePointingAtLocalhostMultipleHosts(2, smtpServerPort);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();

            recipients.Add("user1@dummy-example.com");

            SmtpClientSimulator.StaticSend("test@example.test", "user1@dummy-example.com", "Test", "Test");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            var log = LogHandler.ReadCurrentDefaultLog();
            Assert.IsTrue(log.Contains("Truncating MX server list."));
         }
      }

      [Test]
      public void TestPostmasterAddressIsAlias()
      {
         _domain.Postmaster = "alias@example.test";
         _domain.Save();

         var alias = SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "alias@example.test", "test@example.test");

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend("someone@example.com", "someone@example.test", "Test", "Test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      public void TestPostmasterAddressIsAliasButAliasMissing()
      {
         _domain.Postmaster = "alias@example.test";
         _domain.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            SmtpClientSimulator.StaticSend("someone@example.com", "someone@example.test", "Test", "Test"));
      }

      [Test]
      public void TestPostmasterInternalSender()
      {
         _domain.Postmaster = "test@example.test";
         _domain.Save();

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend("sender@example.test", "someone@example.test", "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      public void TestPostmasterOnSecondDomain()
      {
         _domain.Postmaster = "test@otherDomain.com";
         _domain.Save();

         var otherDomain =
            SingletonProvider<TestSetup>.Instance.AddDomain(SingletonProvider<TestSetup>.Instance.GetApp().Domains,
               "otherDomain.com");

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(otherDomain, "test@otherDomain.com",
            "test");
         SmtpClientSimulator.StaticSend("sender@example.test", "someone@example.test", "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      public void TestPostmasterOnSecondDomainWithDomainAlias()
      {
         var otherDomain =
            SingletonProvider<TestSetup>.Instance.AddDomain(SingletonProvider<TestSetup>.Instance.GetApp().Domains,
               "otherDomain.com");
         var otherDomainAlias = otherDomain.DomainAliases.Add();
         otherDomainAlias.AliasName = "name2.com";
         otherDomainAlias.Save();

         _domain.Postmaster = "test@name2.com";
         _domain.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(otherDomain, "test@otherDomain.com",
            "test");
         SmtpClientSimulator.StaticSend("sender@example.test", "someone@example.test", "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      [Description("Test that we don't mess up external deliveries with the postmaster address.")]
      public void TestPostmasterRecipientExternal()
      {
         _domain.Postmaster = "test@example.test";
         _domain.Save();

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            SmtpClientSimulator.StaticSend("sender@example.com", "someone@example.com", "Test", "Test"));
      }

      [Test]
      public void TestPostmasterToPostmasterOnOtherDomain()
      {
         _domain.Postmaster = "nonexistant@otherDomain.com";
         _domain.Save();

         var otherDomain =
            SingletonProvider<TestSetup>.Instance.AddDomain(SingletonProvider<TestSetup>.Instance.GetApp().Domains,
               "otherDomain.com");
         otherDomain.Postmaster = "account@otherDomain.com";
         otherDomain.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(otherDomain, "account@otherDomain.com",
            "test");
         SmtpClientSimulator.StaticSend("sender@example.test", "someone@example.test", "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }


      [Test]
      [Category("SMTP")]
      public void MailFromWithAuthParameterShouldBeAccepted()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);

         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));

         // A few tests of invalid syntax.
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM: <test@example.test> AUTH=<>\r\n")
            .StartsWith("250"));

         smtpClientSimulator.Disconnect();
      }


      [Test]
      [Category("SMTP")]
      [Description("Confirm that it's OK to send MAIL FROM without the < and >")]
      public void TestRcptToSyntax()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);

         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));

         // A few tests of invalid syntax.
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM: <test@example.test>\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("RCPT TO: test@example.test>\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("RCPT TO: <test@example.test\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("RCPT TO <test@example.test\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("RCPT TO<test@example.test\r\n").StartsWith("250"));

         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RCPT TO: <test@example.test>\r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RCPT TO: test@example.test\r\n").StartsWith("250"));

         smtpClientSimulator.Disconnect();
      }

      [Test]
      [Category("SMTP")]
      [Description("Confirm that it's OK to add the same list multiple times.")]
      public void TestSameRecipientMultipleTimes()
      {
         var logging = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Logging;
         logging.AWStatsEnabled = true;
         logging.Enabled = true;

         if (File.Exists(logging.CurrentAwstatsLog))
            File.Delete(logging.CurrentAwstatsLog);

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));
         smtpClientSimulator.Send("MAIL FROM: test@example.test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));
         smtpClientSimulator.Send("RCPT TO: knafve@gmail.com\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));
         smtpClientSimulator.Send("RCPT TO: knafve@gmail.com\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));

         smtpClientSimulator.Disconnect();
      }

      [Test]
      [Description(
         "Test send email from internal address to external, using no authentication on an IP range with does not require authentication"
      )]
      public void TestSendExternalToExternalPermitted()
      {
         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthExternalToExternal = false;
         range.Save();


         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test2@dummy-example.com"] = 250;

         var smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSNone);

            var smtpClientSimulator = new SmtpClientSimulator();
            smtpClientSimulator.Send("test@sdag532sdfagdsa12fsdafdsa1.com",
               "test2@dummy-example.com", "Mail 1", "Test message");


            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      [Description(
         "Test send email from internal address to external. When not allowed, error message should state this (and not bring up authentication).")]
      public void TestSendExternalToExternalNotPermitted_ErrorMessage()
      {
         var range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.AllowDeliveryFromRemoteToRemote = false;
         range.Save();

         var smtpClientSimulator = new SmtpClientSimulator();
         var ex = Assert.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send(
            "test@sdag532sdfagdsa12fsdafdsa1.com",
            "test2@dummy-example.com", "Mail 1", "Test message"));

         StringAssert.Contains("550 Delivery is not allowed to this address.", ex.Message, ex.Message);
      }

      [Test]
      public void TestSendExternalMailToMailboxContainingQuote()
      {
         var range = SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges
            .get_ItemByName("My computer");
         range.RequireSMTPAuthExternalToExternal = false;
         range.Save();


         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["\"John Smith\"@dummy-example.com"] = 250;

         var smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSNone);

            var smtpClientSimulator = new SmtpClientSimulator();
            smtpClientSimulator.Send("\"John Smith\"@example.com",
               "\"John Smith\"@dummy-example.com", "Mail 1", "Test message");


            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      public void TestSendToAddressWithQuote()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "us'er@example.test", "test");
         SmtpClientSimulator.StaticSend("someone@example.test", "us'er@example.test", "Test", "Test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      public void TestSendToMultipleAccounts()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multi1@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multi2@example.test", "test");
         var account3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multi3@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();

         var lstRecipients = new List<string>();
         lstRecipients.Add("multi1@example.test");
         lstRecipients.Add("multi2@example.test");
         lstRecipients.Add("multi3@example.test");

         var sBody = "Test of sending same email to multiple accounts.";

         smtpClientSimulator.Send(account1.Address, lstRecipients, "Multi test", sBody);

         var pop3ClientSimulator = new Pop3ClientSimulator();

         var sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         if (sMessageData.IndexOf(sBody) < 0)
            throw new Exception("E-mail not found");

         sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(account2.Address, "test");
         if (sMessageData.IndexOf(sBody) < 0)
            throw new Exception("E-mail not found");

         sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(account3.Address, "test");
         if (sMessageData.IndexOf(sBody) < 0)
            throw new Exception("E-mail not found");
      }

      [Test]
      public void TestSendToPostmasterAddress()
      {
         _domain.Postmaster = "test@example.test";
         _domain.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend("someone@example.com", "someone@example.test", "Test", "Test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }


      [Test]
      public void TestSendToPostmasterAddressDomainAlias()
      {
         var alias = _domain.DomainAliases.Add();
         alias.AliasName = "alias.com";
         alias.Save();

         _domain.Postmaster = "test@alias.com";
         _domain.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend("someone@example.com", "someone@example.test", "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      [Category("SMTP")]
      [Description("Issue 188, SMTP: Permanent error response if disk full ")]
      public void TestTempErrorIfDiskFull()
      {
         var senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");
         var recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "list@example.test",
            "test");

         var directories = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories;

         var origDataDir = directories.DataDirectory;

         try
         {
            directories.DataDirectory = "C:\\FolderWhichDoesNotExist";

            var result = "";

            var sim = new SmtpClientSimulator();
            CustomAsserts.Throws<DeliveryFailedException>(() =>
               sim.Send(senderAccount.Address, recipientAccount.Address, "MySubject", "Test", out result));

            Assert.IsTrue(result.StartsWith("4"), "Expected temporary error, but was: " + result);

            CustomAsserts.AssertReportedError("Failed to write to the file",
               "Rejected message because no mail data has been saved in file");
         }
         finally
         {
            directories.DataDirectory = origDataDir;
         }
      }

      [Test]
      public void TestTooManyInvalidCommandsAUTH()
      {
         var settings = _settings;

         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 3;

         var sim = new TcpConnection();
         sim.Connect(25);
         sim.Send("EHLO test.com\r\n");
         sim.ReadUntil("250 HELP\r\n");

         for (var i = 1; i <= 5; i++)
         {
            sim.Send("AUTH LOGIN\r\n");

            // Send invalid username/password
            var usernamePrompt = sim.Receive();

            // Send a invalid username
            sim.Send("YWNhZGVtaWE=\r\n");
            var passwordPrompt = sim.Receive();
            StringAssert.Contains("334 UGFzc3dvcmQ6", passwordPrompt); // Base64 encoded "Password" prompt

            // Send a invalid password
            sim.Send("abc\r\n");
            var loginResult = sim.Receive();


            if (i == 4)
            {
               StringAssert.Contains("Too many invalid commands", loginResult);
               return;
            }

            StringAssert.Contains("535 Authentication failed. Restarting authentication process.", loginResult);
         }

         Assert.Fail("Wasn't disconnected");
      }

      [Test]
      public void TestTooManyInvalidCommandsHELO()
      {
         var settings = _settings;
         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 3;

         var sim = new TcpConnection();
         sim.Connect(25);
         sim.Receive(); // banner

         sim.SendAndReceive("HELO\r\n");
         sim.SendAndReceive("HELO\r\n");
         sim.SendAndReceive("HELO\r\n");
         var result = sim.SendAndReceive("HELO\r\n");
         Assert.IsTrue(result.Contains("Too many invalid commands"), result);
      }

      [Test]
      public void TestTooManyInvalidCommandsHELOLastCommandOK()
      {
         var settings = _settings;
         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 3;

         var sim = new TcpConnection();
         sim.Connect(25);
         sim.Receive(); // banner

         sim.SendAndReceive("HELO\r\n");
         sim.SendAndReceive("HELO\r\n");
         sim.SendAndReceive("HELO\r\n");
         var result = sim.SendAndReceive("HELO test.com\r\n");
         Assert.IsTrue(result.Contains("250 Hello."), result);
      }

      [Test]
      public void TestTooManyInvalidCommandsUnknownRcptShouldBeCounted()
      {
         var settings = _settings;
         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 3;

         var sim = new TcpConnection();
         sim.Connect(25);
         sim.Receive(); // banner

         sim.SendAndReceive("HELO example.com\r\n");
         sim.SendAndReceive("MAIL FROM: example@example.com\r\n");
         var result = sim.SendAndReceive("RCPT TO: unknown@example.test\r\n");
         Assert.IsTrue(result.Contains("550 Unknown user"), result);
         result = sim.SendAndReceive("RCPT TO: unknown@example.test\r\n");
         Assert.IsTrue(result.Contains("550 Unknown user"), result);
         result = sim.SendAndReceive("RCPT TO: unknown@example.test\r\n");
         Assert.IsTrue(result.Contains("550 Unknown user"), result);
         result = sim.SendAndReceive("RCPT TO: unknown@example.test\r\n");
         Assert.IsTrue(result.Contains("Too many invalid commands"), result);
      }

      [Test]
      public void TestTooManyInvalidCommandsHELOSuccesfullCommandDoesNotResetCounter()
      {
         var settings = _settings;
         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 3;

         var sim = new TcpConnection();
         sim.Connect(25);
         sim.Receive(); // banner

         sim.SendAndReceive("HELO\r\n");
         sim.SendAndReceive("HELO\r\n");
         sim.SendAndReceive("HELO\r\n");
         var result = sim.SendAndReceive("HELO test.com\r\n");
         Assert.IsTrue(result.Contains("250 Hello."), result);

         result = sim.SendAndReceive("HELO\r\n");
         Assert.IsTrue(result.Contains("Too many invalid commands"), result);
      }

      /// <summary>
      ///    If an error is rejected with a temp-error (4xx), that should not be counted
      ///    as an error which makes the client get disconnected.
      /// </summary>
      [Test]
      [Description("Issue 189, Greylisting rejections (try again) are treated as errors ")]
      public void TestTooManyInvalidCommandsTempError()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         for (var i = 0; i < 10; i++)
            SingletonProvider<TestSetup>.Instance.AddAccount(_domain, string.Format("test{0}@example.test", i), "test");

         var application = SingletonProvider<TestSetup>.Instance.GetApp();
         var settings = _settings;

         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 5;

         settings.AntiSpam.GreyListingEnabled = true;

         var sim = new TcpConnection();
         sim.Connect(25);
         var res = sim.Receive();
         sim.Send("EHLO test.com\r\n");
         res = sim.Receive();
         sim.Send("MAIL FROM: <test@example.test>\r\n");
         res = sim.Receive();
         for (var i = 1; i < 10; i++)
         {
            var address = string.Format("test{0}@example.test", i);

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
         var senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");
         var recipientAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "list@example.test",
            "test");

         recipientAccount.MaxSize = 1;
         recipientAccount.Save();

         // Build a 2MB string.
         var builder = new StringBuilder();
         for (var i = 0; i < 11000; i++)
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");

         var text = builder.ToString();

         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "", text);

         // Make sure the list did not receive it.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         // Check the syntax in the bounce message.
         var content = Pop3ClientSimulator.AssertGetFirstMessageText(senderAccount.Address, "test");

         // The bounce message should contain the MIME-version.
         Assert.IsTrue(content.Contains("MIME-Version: 1.0"));

         // Lines may not end with space if they are QP-encoded.
         Assert.IsTrue(content.Contains("  Subject:=20"));
      }

      [Test]
      public void TestTooLongEmailAddress()
      {
         var senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");

         var tooLongAddress = new string('i', 260) + "@example.com";
         var ex = Assert.Throws<DeliveryFailedException>(() =>
            SmtpClientSimulator.StaticSend(senderAccount.Address, tooLongAddress, "", "foobar"));

         StringAssert.Contains("550 A valid address is required.", ex.Message);
      }

      [Test]
      public void TestWelcomeMessage()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();
         _settings.WelcomeSMTP = "HOWDYHO";

         var simulator = new SmtpClientSimulator();

         var sWelcomeMessage = simulator.GetWelcomeMessage();

         if (sWelcomeMessage != "220 HOWDYHO ESMTP\r\n")
            throw new Exception("ERROR - Wrong welcome message.");
      }

      [Test]
      [Description(
         "If a message with 4 recipients on different domains, but all are to be sent through the same SMTP relay, only one message should be sent")]
      public void SmtpRelayShouldConsolidateRecipients()
      {
         // Set up a server listening on port 250 which accepts email for test@example.test
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user1@test1.com"] = 250;
         deliveryResults["user2@test2.com"] = 250;
         deliveryResults["user3@test3.com"] = 250;
         deliveryResults["user4@test4.com"] = 250;

         var smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            _application.Settings.SMTPRelayer = "localhost";
            _application.Settings.SMTPRelayerPort = smtpServerPort;

            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            var smtpClient = new SmtpClientSimulator();

            var recipients = new List<string>
            {
               "user1@test1.com",
               "user2@test2.com",
               "user3@test3.com",
               "user4@test4.com"
            };

            var senderAccount =
               SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");
            smtpClient.Send(senderAccount.Address, recipients, "Test", "Test message");
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
            Assert.AreEqual(deliveryResults.Count, server.RcptTosReceived);
         }
      }
   }
}