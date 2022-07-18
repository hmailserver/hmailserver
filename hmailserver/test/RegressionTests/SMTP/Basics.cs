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
using RegressionTests.Infrastructure;
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
            var smtpClientSimulator = new SmtpClientSimulator();
            smtpClientSimulator.Send("test@test.com", "bigaccount@test.com", "Big account email", "Body av email");
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
         var sock = new TcpConnection();
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
         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "MySubject", "Test");
         Pop3ClientSimulator.AssertGetFirstMessageText(recipientAccount.Address, "test");

         // Build a 2MB string.
         var builder = new StringBuilder();
         for (int i = 0; i < 11000; i++)
         {
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");
         }
         string text = builder.ToString();

         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "Test subject", text);

         // Make sure the recipient did not receive it.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         Pop3ClientSimulator.AssertMessageCount(recipientAccount.Address, "test", 0);
         CustomAsserts.AssertFilesInUserDirectory(recipientAccount, 0);

         // Make sure it bounced.
         string content = Pop3ClientSimulator.AssertGetFirstMessageText(senderAccount.Address, "test");
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
         DomainAlias oDA = _domain.DomainAliases.Add();
         oDA.AliasName = "dummy-example.com";
         oDA.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "otherguy@test.com", "test");

         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();


            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, true, eConnectionSecurity.eCSNone);

            // Now send a message from test@test.com to test@otherdomain.com.
            // Error scenario:
            // In the recipientlist, otherdomain.com gets rewritten to test.com 
            // since there's a domain alias named otherdomain.co on the domain 
            // test.com.
            //
            // This should not happen. Otherdomain.com is an alias for test.com,
            // but we shouldn't actually modify the recipient address just because
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
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "us'er@test.com", "test");
         DateTime lastLogonTimeBefore = Convert.ToDateTime(account.LastLogonTime);

         Thread.Sleep(1000);
         SmtpClientSimulator.StaticSend("someone@test.com", "us'er@test.com", "Test", "Test");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         DateTime lastLogonTimeAfter =
            Convert.ToDateTime(SingletonProvider<TestSetup>.Instance.GetApp().Domains[0].Accounts[0].LastLogonTime);
         Assert.AreEqual(lastLogonTimeBefore, lastLogonTimeAfter);
      }

      [Test]
      public void TestAntiVirusEnabled()
      {
         SecurityRange range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.EnableAntiVirus = true;
         range.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");

         SmtpClientSimulator.StaticSend("user@test.com", "user@test.com", "Test", "Test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

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

         IPAddress localAddress = TestSetup.GetLocalIpAddress();
         var smtpClientSimulator = new SmtpClientSimulator(false, 25, localAddress);

         // Delivery from external to local.
         smtpClientSimulator.Send("test@external.com", "test@test.com", "Mail 1", "Mail 1");
         Pop3ClientSimulator.AssertMessageCount("test@test.com", "test", 1);
         string contents = TestSetup.ReadExistingTextFile(logging.CurrentAwstatsLog);
         CustomAsserts.AssertDeleteFile(logging.CurrentAwstatsLog);
         string expectedString = string.Format("\ttest@external.com\ttest@test.com\t{0}\t127.0.0.1\tSMTP\t?\t250\t",
                                               localAddress);
         Assert.IsTrue(contents.Contains(expectedString), contents);

         // Failed delivery from local to local.
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send("test@test.com", "test@test.com", "Mail 1", "Mail 1"));
         contents = TestSetup.ReadExistingTextFile(logging.CurrentAwstatsLog);
         CustomAsserts.AssertDeleteFile(logging.CurrentAwstatsLog);
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

         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "", text);

         // Make sure the recipient did not receive it.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         // Check the syntax in the bounce message.
         string content = Pop3ClientSimulator.AssertGetFirstMessageText(senderAccount.Address, "test");

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

         SmtpClientSimulator.StaticSendRaw("test@test.com", "test@test.com", content);

         string test = Pop3ClientSimulator.AssertGetFirstMessageText("test@test.com", "test");

         Assert.IsTrue(test.Contains("Message-Id"));
         Assert.IsFalse(test.Contains("Message-ID"));
      }

      [Test]
      public void TestEHLOKeywords()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Settings settings = _settings;
         settings.HostName = "examplify.com";

         var socket = new TcpConnection();
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
         var oSimulator = new TcpConnection();

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

         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);

         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));

         // A few tests of invalid syntax.
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: <test@test.com\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: test@test.com>\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: <    test@test.com    \r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: <        \r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: >        \r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM: <sdsdfs@sdsdf.csd\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM <sdsdfs@sdsdf.csd>\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("MAIL FROM  sdsdfs@sdsdf.csd\r\n").StartsWith("250"));

         // Valid syntax, < and >
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM: <test@test.com>\r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM: test@test.com\r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM:    test@test.com   \r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM:test@test.com\r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RSET\r\n").StartsWith("250"));

         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM:<test@test.com>\r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RSET\r\n").StartsWith("250"));

         smtpClientSimulator.Disconnect();
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
         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "MySubject", "Test");
         Pop3ClientSimulator.AssertGetFirstMessageText(recipientAccount.Address, "test");

         // Build a 2MB string.
         var builder = new StringBuilder();
         for (int i = 0; i < 11000; i++)
         {
            builder.Append(
               "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890\r\n");
         }
         string text = builder.ToString();

         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "MySubject", text);

         // Make sure the recipient did not receive it.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         Pop3ClientSimulator.AssertMessageCount(recipientAccount.Address, "test", 0);
         CustomAsserts.AssertFilesInUserDirectory(recipientAccount, 0);

         // Make sure it bounced.
         string content = Pop3ClientSimulator.AssertGetFirstMessageText(senderAccount.Address, "test");
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
         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "Test", "Test");
         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(recipientAccount.Address, "test", 2);
      }

      [Test]
      public void TestMediumVolume()
      {
         // Fetch default domain
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "bigaccount@test.com", "test");

         var smtpClientSimulator = new SmtpClientSimulator();


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

         ImapClientSimulator.AssertMessageCount("bigaccount@test.com", "test", "Inbox", 250);
      }

      [Test]
      public void TestMultipleHostsLimitMXHosts()
      {
         _settings.MaxNumberOfMXHosts = 1;

         // No valid recipients...
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["user1@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            Route route = TestSetup.AddRoutePointingAtLocalhostMultipleHosts(2, smtpServerPort);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();

            recipients.Add("user1@dummy-example.com");

            SmtpClientSimulator.StaticSend("test@test.com", "user1@dummy-example.com", "Test", "Test");

            // Wait for the client to disconnect.
            server.WaitForCompletion();

            string log = LogHandler.ReadCurrentDefaultLog();
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
         SmtpClientSimulator.StaticSend("someone@example.com", "someone@test.com", "Test", "Test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      public void TestPostmasterAddressIsAliasButAliasMissing()
      {
         _domain.Postmaster = "alias@test.com";
         _domain.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("someone@example.com", "someone@test.com", "Test", "Test"));
      }

      [Test]
      public void TestPostmasterInternalSender()
      {
         _domain.Postmaster = "test@test.com";
         _domain.Save();

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend("sender@test.com", "someone@test.com", "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
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
         SmtpClientSimulator.StaticSend("sender@test.com", "someone@test.com", "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
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
         SmtpClientSimulator.StaticSend("sender@test.com", "someone@test.com", "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      [Description("Test that we don't mess up external deliveries with the postmaster address.")]
      public void TestPostmasterRecipientExternal()
      {
         _domain.Postmaster = "test@test.com";
         _domain.Save();

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("sender@example.com", "someone@example.com", "Test", "Test"));
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
         SmtpClientSimulator.StaticSend("sender@test.com", "someone@test.com", "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      [Category("SMTP")]
      [Description("Confirm that it's OK to send MAIL FROM without the < and >")]
      public void TestRcptToSyntax()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);

         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));

         // A few tests of invalid syntax.
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("MAIL FROM: <test@test.com>\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("RCPT TO: test@test.com>\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("RCPT TO: <test@test.com\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("RCPT TO <test@test.com\r\n").StartsWith("250"));
         Assert.IsFalse(smtpClientSimulator.SendAndReceive("RCPT TO<test@test.com\r\n").StartsWith("250"));

         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RCPT TO: <test@test.com>\r\n").StartsWith("250"));
         Assert.IsTrue(smtpClientSimulator.SendAndReceive("RCPT TO: test@test.com\r\n").StartsWith("250"));

         smtpClientSimulator.Disconnect();
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

         var smtpClientSimulator = new TcpConnection();
         smtpClientSimulator.Connect(25);
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("220"));
         smtpClientSimulator.Send("HELO test\r\n");
         Assert.IsTrue(smtpClientSimulator.Receive().StartsWith("250"));
         smtpClientSimulator.Send("MAIL FROM: test@test.com\r\n");
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
         SecurityRange range =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthExternalToExternal = false;
         range.Save();


         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test2@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            Route route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false, eConnectionSecurity.eCSNone);

            var smtpClientSimulator = new SmtpClientSimulator();
            smtpClientSimulator.Send("test@sdag532sdfagdsa12fsdafdsa1.com",
                                           "test2@dummy-example.com", "Mail 1", "Test message");


            // This should now be processed via the rule -> route -> external server we've set up.
            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
         }
      }

      [Test]
      public void TestSendExternalMailToMailboxContainingQuote()
      {
         var range = SingletonProvider<TestSetup>.Instance.GetApp().Settings.SecurityRanges.get_ItemByName("My computer");
         range.RequireSMTPAuthExternalToExternal = false;
         range.Save();


         // Set up a server listening on port 250 which accepts email for test@otherdomain.com
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["\"John Smith\"@dummy-example.com"] = 250;

         int smtpServerPort = TestSetup.GetNextFreePort();
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
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "us'er@test.com", "test");
         SmtpClientSimulator.StaticSend("someone@test.com", "us'er@test.com", "Test", "Test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      public void TestSendToMultipleAccounts()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multi1@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multi2@test.com", "test");
         Account oAccount3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multi3@test.com", "test");

         var smtpClientSimulator = new SmtpClientSimulator();

         var lstRecipients = new List<string>();
         lstRecipients.Add("multi1@test.com");
         lstRecipients.Add("multi2@test.com");
         lstRecipients.Add("multi3@test.com");

         string sBody = "Test of sending same email to multiple accounts.";

         smtpClientSimulator.Send(oAccount1.Address, lstRecipients, "Multi test", sBody);

         var pop3ClientSimulator = new Pop3ClientSimulator();

         string sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageData.IndexOf(sBody) < 0)
            throw new Exception("E-mail not found");

         sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount2.Address, "test");
         if (sMessageData.IndexOf(sBody) < 0)
            throw new Exception("E-mail not found");

         sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount3.Address, "test");
         if (sMessageData.IndexOf(sBody) < 0)
            throw new Exception("E-mail not found");
      }

      [Test]
      public void TestSendToPostmasterAddress()
      {
         _domain.Postmaster = "test@test.com";
         _domain.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend("someone@example.com", "someone@test.com", "Test", "Test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
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
         SmtpClientSimulator.StaticSend("someone@example.com", "someone@test.com", "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
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

            var sim = new SmtpClientSimulator();
            CustomAsserts.Throws<DeliveryFailedException>( () => sim.Send(senderAccount.Address, recipientAccount.Address, "MySubject", "Test", out result));

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
         Settings settings = _settings;

         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 3;

         var sim = new TcpConnection();
         sim.Connect(25);
         sim.Send("EHLO test.com\r\n");
         sim.ReadUntil("250 HELP\r\n");

         for (int i = 1; i <= 5; i++)
         {
            sim.Send("AUTH LOGIN\r\n");

            // Send invalid username/password
            string usernamePrompt = sim.Receive();

            if (i == 5)
            {
               StringAssert.Contains("Too many invalid commands", usernamePrompt);
               return;
            }
            else
            {
               StringAssert.Contains("334 VXNlcm5hbWU6", usernamePrompt); // Base64 encoded "Username" prompt
            }


            // Send a invalid username
            sim.Send("YWNhZGVtaWE=\r\n");
            string passwordPrompt = sim.Receive();
            StringAssert.Contains("334 UGFzc3dvcmQ6", passwordPrompt); // Base64 encoded "Password" prompt

            // Send a invalid password
            sim.Send("abc\r\n");
            var loginResult = sim.Receive();
            
            StringAssert.Contains("535 Authentication failed. Restarting authentication process.", loginResult);
         }

         Assert.Fail("Wasn't disconnected");
      }

      [Test]
      public void TestTooManyInvalidCommandsHELO()
      {
         Settings settings = _settings;
         settings.DisconnectInvalidClients = true;
         settings.MaxNumberOfInvalidCommands = 3;

         var sim = new TcpConnection();
         sim.Connect(25);
         sim.Receive(); // banner

         sim.SendAndReceive("HELO\r\n");
         sim.SendAndReceive("HELO\r\n");
         sim.SendAndReceive("HELO\r\n");
         sim.SendAndReceive("HELO\r\n");
         var result = sim.SendAndReceive("HELO\r\n");
         Assert.IsTrue(result.Contains("Too many invalid commands"), result);
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

         var sim = new TcpConnection();
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

         SmtpClientSimulator.StaticSend(senderAccount.Address, recipientAccount.Address, "", text);

         // Make sure the recipient did not receive it.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         // Check the syntax in the bounce message.
         string content = Pop3ClientSimulator.AssertGetFirstMessageText(senderAccount.Address, "test");

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

         var oSimulator = new SmtpClientSimulator();

         string sWelcomeMessage = oSimulator.GetWelcomeMessage();

         if (sWelcomeMessage != "220 HOWDYHO\r\n")
            throw new Exception("ERROR - Wrong welcome message.");
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
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            _application.Settings.SMTPRelayer = "localhost";
            _application.Settings.SMTPRelayerPort = smtpServerPort;
            
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            var smtpClient = new SmtpClientSimulator();

            var recipients = new List<string>()
               {
                  "user1@test1.com",
                  "user2@test2.com",
                  "user3@test3.com",
                  "user4@test4.com"
               };

            Account senderAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@test.com", "test");
            smtpClient.Send(senderAccount.Address, recipients, "Test", "Test message");
            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.IsTrue(server.MessageData.Contains("Test message"));
            Assert.AreEqual(deliveryResults.Count, server.RcptTosReceived);
         }
      }
   }
}