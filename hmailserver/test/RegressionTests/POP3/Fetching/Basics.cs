// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.POP3.Fetching
{
   [TestFixture]
   public class Basics : TestFixtureBase
   {
      private static FetchAccount CreateFetchAccount(Account account, int port, bool antiSpam, bool antiVirus)
      {
         var fa = account.FetchAccounts.Add();

         fa.Enabled = true;
         fa.MinutesBetweenFetch = 10;
         fa.Name = "Test";
         fa.Username = "test@example.com";
         fa.Password = "test";
         fa.UseSSL = false;
         fa.ServerAddress = "localhost";
         fa.Port = port;
         fa.ProcessMIMERecipients = false;
         fa.DaysToKeepMessages = -1;
         fa.UseAntiSpam = antiSpam;
         fa.UseAntiVirus = antiVirus;
         fa.Save();
         return fa;
      }

      [Test]
      public void TestAntiVirusDisabled()
      {
         var messages = new List<string>();

         var messageText = "From: spftest@openspf.org\r\n" +
                           "To: Martin@example.com\r\n" +
                           "Subject: Test\r\n" +
                           "\r\n" +
                           "Should be blocked by SPF.";

         messages.Add(messageText);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = CreateFetchAccount(account, port, false, false);

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

            var message = account.IMAPFolders.get_ItemByName("INBOX").Messages[0];
            Assert.IsFalse(message.get_Flag(eMessageFlag.eMFVirusScan));
         }
      }

      [Test]
      public void TestAntiVirusEnabled()
      {
         var messages = new List<string>();

         var messageText = "From: spftest@openspf.org\r\n" +
                           "To: Martin@example.com\r\n" +
                           "Subject: Test\r\n" +
                           "\r\n" +
                           "Should be blocked by SPF.";

         messages.Add(messageText);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = CreateFetchAccount(account, port, false, true);

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

            var message = account.IMAPFolders.get_ItemByName("INBOX").Messages[0];
            Assert.IsTrue(message.get_Flag(eMessageFlag.eMFVirusScan));
         }
      }

      [Test]
      public void TestBasicExternalAccount()
      {
         var messages = new List<string>();

         var message = "Received: from example.com (example.com [1.2.3.4]) by mail.host.edu\r\n" +
                       "From: Martin@example.com\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Hello!";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            var downloadedMessage = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

            StringAssert.Contains("Subject: Test", downloadedMessage);
            StringAssert.Contains("Hello!", downloadedMessage);
         }
      }

      [Test]
      public void TestFetchFromInvalidHostName()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
         var fa = account.FetchAccounts.Add();

         fa.Enabled = true;
         fa.MinutesBetweenFetch = 10;
         fa.Name = "Test";
         fa.Username = "test@example.com";
         fa.Password = "test";
         fa.UseSSL = false;
         fa.ServerAddress = "nonexistant.example.com";
         fa.Port = 110;
         fa.ProcessMIMERecipients = false;
         fa.Save();

         fa.DownloadNow();

         RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
         {
            var
               log = LogHandler.ReadCurrentDefaultLog();

            if (!log.Contains("The IP address for external account Test could not be resolved. Aborting fetch."))
               throw new Exception("Expected message not appearing in log.");
         });

         fa.Delete();
      }


      [Test]
      public void TestDelete()
      {
         var messages = new List<string>();

         var message = "From: Martin@example.com\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Hello!";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = -1;
            fa.Save();


            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            var downloadedMessage = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

            StringAssert.Contains("Hello!", downloadedMessage);
            Assert.AreEqual(1, pop3Server.DeletedMessages.Count);
         }
      }

      [Test]
      public void TestDeleteMutliple()
      {
         var messages = new List<string>();

         var message = "From: Martin@example.com\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Hello!";

         messages.Add(message);
         messages.Add(message);
         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = -1;
            fa.Save();


            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 3);

            Assert.AreEqual(3, pop3Server.DeletedMessages.Count);
         }
      }

      [Test]
      [Description("Issue 215, Mail not delivered to MIME recipients (if external). Test option disabled.")]
      public void TestDeliverToExternalMimeRecipientsDisabled()
      {
         var messages = new List<string>();

         var message = "From: Martin@example.com\r\n" +
                       "To: \"Test\" <test1@example.test>, \"ExternalGuy\" <external@dummy-example.com>\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Hello!";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var deliveryResults = new Dictionary<string, int>();
            deliveryResults["external@dummy-example.com"] = 250;

            var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test1@example.test", "test");
            var catchallAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "catchall@example.test",
               "test");

            _domain.Postmaster = catchallAccount.Address;
            _domain.Save();

            var fa = account1.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = true;
            fa.MIMERecipientHeaders = "To,CC,X-RCPT-TO,X-Envelope-To";
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            var downloadedMessage1 = Pop3ClientSimulator.AssertGetFirstMessageText(account2.Address, "test");
            Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 0);
            StringAssert.Contains("Hello!", downloadedMessage1);

            Pop3ClientSimulator.AssertMessageCount(account2.Address, "test", 0);
            Pop3ClientSimulator.AssertMessageCount(catchallAccount.Address, "test", 0);
         }
      }

      [Test]
      [Description("Issue 215, Mail not delivered to MIME recipients (if external)")]
      public void TestDeliverToExternalMimeRecipientsEnabled()
      {
         var messages = new List<string>();

         var message = "From: Martin@example.com\r\n" +
                       "To: \"Test\" <test1@example.test>, \"ExternalGuy\" <external@dummy-example.com>\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Hello!";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var deliveryResults = new Dictionary<string, int>();
            deliveryResults["external@dummy-example.com"] = 250;

            var smtpServerPort = TestSetup.GetNextFreePort();
            using (var smtpServer = new SmtpServerSimulator(1, smtpServerPort))
            {
               smtpServer.AddRecipientResult(deliveryResults);
               smtpServer.StartListen();

               // Add a route so we can connect to localhost.
               var route = TestSetup.AddRoutePointingAtLocalhost(1, smtpServerPort, false);
               route.TreatSecurityAsLocalDomain = true;
               route.Save();

               var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
               var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test1@example.test", "test");
               var catchallAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "catchall@example.test",
                  "test");

               _domain.Postmaster = catchallAccount.Address;
               _domain.Save();

               var fa = account1.FetchAccounts.Add();

               fa.Enabled = true;
               fa.MinutesBetweenFetch = 10;
               fa.Name = "Test";
               fa.Username = "test@example.com";
               fa.Password = "test";
               fa.UseSSL = false;
               fa.ServerAddress = "localhost";
               fa.Port = port;
               fa.ProcessMIMERecipients = true;
               fa.MIMERecipientHeaders = "To,CC,X-RCPT-TO,X-Envelope-To";
               fa.EnableRouteRecipients = true;
               fa.Save();

               fa.DownloadNow();

               pop3Server.WaitForCompletion();

               fa.Delete();

               var downloadedMessage1 = Pop3ClientSimulator.AssertGetFirstMessageText(account2.Address, "test");
               Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 0);
               Pop3ClientSimulator.AssertMessageCount(catchallAccount.Address, "test", 0);
               StringAssert.Contains("Hello!", downloadedMessage1);

               // Make sure the exernal list has received his copy.
               smtpServer.WaitForCompletion();
               var messageData = smtpServer.MessageData;
               Assert.IsTrue(messageData.Contains(messageData), messageData);

               CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);
            }
         }
      }

      [Test]
      [Description(
         "Issue 215, Mail not delivered to MIME recipients (if external). Test to deliver when the route is external."
      )]
      public void TestDeliverToExternalMimeRecipientsEnabledRouteAsExternal()
      {
         var messages = new List<string>();

         var message = "From: Martin@example.com\r\n" +
                       "To: \"Test\" <test1@example.test>, \"ExternalGuy\" <external@dummy-example.com>\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Hello!";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var userAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var recipientAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test1@example.test",
               "test");
            var catchallAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "catchall@example.test",
               "test");

            _domain.Postmaster = catchallAccount.Address;
            _domain.Save();

            var fa = userAccount.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = true;
            fa.MIMERecipientHeaders = "To,CC,X-RCPT-TO,X-Envelope-To";
            fa.EnableRouteRecipients = true;
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            var downloadedMessage1 = Pop3ClientSimulator.AssertGetFirstMessageText(recipientAccount1.Address, "test");
            StringAssert.Contains("Hello!", downloadedMessage1);

            CustomAsserts.AssertRecipientsInDeliveryQueue(0, false);
         }
      }

      [Test]
      public void TestDeliverToMIMERecipients()
      {
         var messages = new List<string>();

         var message = "From: Martin@example.com\r\n" +
                       "To: \"Test\" <test1@example.test>, \"Test 2\" <test2@example.test>\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Hello!";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test1@example.test", "test");
            var account3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test2@example.test", "test");
            var catchallAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "catchall@example.test",
               "test");

            _domain.Postmaster = catchallAccount.Address;
            _domain.Save();


            var fa = account1.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = true;
            fa.MIMERecipientHeaders = "To,CC,X-RCPT-TO,X-Envelope-To";
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            var downloadedMessage1 = Pop3ClientSimulator.AssertGetFirstMessageText(account2.Address, "test");
            var downloadedMessage2 = Pop3ClientSimulator.AssertGetFirstMessageText(account3.Address, "test");
            Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 0);
            Pop3ClientSimulator.AssertMessageCount(catchallAccount.Address, "test", 0);

            StringAssert.Contains("Hello!", downloadedMessage1);
            StringAssert.Contains("Hello!", downloadedMessage2);
         }
      }

      [Test]
      [Description("Issue 313, External fetch thread hangs on -ERR response")]
      public void TestServerNotSupportingUIDL()
      {
         var messages = new List<string>();

         var message = "Received: from example.com (example.com [1.2.3.4]) by mail.host.edu\r\n" +
                       "From: Martin@example.com\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Hello!";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.SupportsUIDL = false;
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.Save();
            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
            {
               var error = LogHandler.ReadCurrentDefaultLog();

               RetryableAssert.IsTrue(error.Contains("-ERR unhandled command"));
               RetryableAssert.IsTrue(error.Contains("Completed retrieval of messages from external account."));
            });
         }
      }

      [Test]
      public void TestSpamProtectionDisabled()
      {
         _application.Settings.AntiSpam.SpamMarkThreshold = 1;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 100;
         _application.Settings.AntiSpam.AddHeaderReason = true;
         _application.Settings.AntiSpam.AddHeaderSpam = true;
         _application.Settings.AntiSpam.PrependSubject = true;
         _application.Settings.AntiSpam.PrependSubjectText = "ThisIsSpam";


         _application.Settings.AntiSpam.UseSPF = true;
         _application.Settings.AntiSpam.UseSPFScore = 5;

         var messages = new List<string>();

         var message = "From: spftest@openspf.org\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Should be blocked by SPF.";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = -1;
            fa.UseAntiSpam = false;
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            var downloadedMessage = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

            Assert.IsFalse(downloadedMessage.Contains("X-hMailServer-Spam: YES"));
         }
      }

      [Test]
      [Description("Issue 249: POP3 download may fail on spam message")]
      public void TestSpamProtectionNoTagging()
      {
         CustomAsserts.AssertSpamAssassinIsRunning();

         _application.Settings.AntiSpam.SpamMarkThreshold = 5;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 9999;
         _application.Settings.AntiSpam.MaximumMessageSize = 1024 * 1024;
         _application.Settings.AntiSpam.AddHeaderReason = false;
         _application.Settings.AntiSpam.AddHeaderSpam = false;
         _application.Settings.AntiSpam.PrependSubject = false;
         _application.Settings.AntiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SpamAssassin
         _application.Settings.AntiSpam.SpamAssassinEnabled = true;
         _application.Settings.AntiSpam.SpamAssassinHost = "localhost";
         _application.Settings.AntiSpam.SpamAssassinPort = 783;
         _application.Settings.AntiSpam.SpamAssassinMergeScore = true;
         _application.Settings.AntiSpam.SpamAssassinScore = 5;

         var messages = new List<string>();

         var message = "From: Martin@example.com\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = -1;
            fa.UseAntiSpam = true;

            fa.Save();
            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
         }
      }

      [Test]
      public void TestSpamProtectionPostTransmission()
      {
         _application.Settings.AntiSpam.SpamMarkThreshold = 1;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 100;
         _application.Settings.AntiSpam.AddHeaderReason = true;
         _application.Settings.AntiSpam.AddHeaderSpam = true;
         _application.Settings.AntiSpam.PrependSubject = true;
         _application.Settings.AntiSpam.PrependSubjectText = "ThisIsSpam";


         var surblServer = _application.Settings.AntiSpam.SURBLServers[0];
         surblServer.Active = true;
         surblServer.Score = 5;
         surblServer.Save();

         var messages = new List<string>();

         var message = "Received: from example.com (example.com [1.2.3.4]) by mail.host.edu\r\n" +
                       "From: Martin@example.com\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "http://surbl-org-permanent-test-point.com/";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = -1;
            fa.UseAntiSpam = true;

            fa.Save();
            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            var downloadedMessage = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

            Assert.IsTrue(downloadedMessage.Contains("X-hMailServer-Spam: YES"));
         }
      }


      [Test]
      public void TestSpamProtectionPreTransmissionHELODelete()
      {
         _application.Settings.AntiSpam.SpamMarkThreshold = 1;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 100;
         _application.Settings.AntiSpam.AddHeaderReason = true;
         _application.Settings.AntiSpam.AddHeaderSpam = true;
         _application.Settings.AntiSpam.PrependSubject = true;
         _application.Settings.AntiSpam.PrependSubjectText = "ThisIsSpam";

         _application.Settings.AntiSpam.CheckHostInHelo = true;
         _application.Settings.AntiSpam.CheckHostInHeloScore = 105;

         var messages = new List<string>();

         var message = "Received: from openspf.org (openspf.org [1.2.1.1]) by mail.host.edu\r\n" +
                       "From: spftest@openspf.org\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Should be blocked by SPF.";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = 0;
            fa.UseAntiSpam = true;
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 0);
         }
      }

      [Test]
      public void TestSpamProtectionPreTransmissionHELOPass()
      {
         _application.Settings.AntiSpam.SpamMarkThreshold = 1;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 100;
         _application.Settings.AntiSpam.AddHeaderReason = true;
         _application.Settings.AntiSpam.AddHeaderSpam = true;
         _application.Settings.AntiSpam.PrependSubject = true;
         _application.Settings.AntiSpam.PrependSubjectText = "ThisIsSpam";

         _application.Settings.AntiSpam.CheckHostInHelo = true;
         _application.Settings.AntiSpam.CheckHostInHeloScore = 105;

         var messages = new List<string>();

         var message = "Received: from mail.hmailserver.com (mail.hmailserver.com [" +
                       TestSetup.GethMailServerCOMIPaddress() + "]) by mail.host.edu\r\n" +
                       "From: spftest@openspf.org\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Should not be blocked.";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = 0;
            fa.UseAntiSpam = true;
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
         }
      }

      [Test]
      [Description("Test that the spam test skips hosts which are listed as incoming relays.")]
      public void TestSpamProtectionPreTransmissionHELOPassFirst()
      {
         _application.Settings.AntiSpam.SpamMarkThreshold = 1;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 100;
         _application.Settings.AntiSpam.AddHeaderReason = true;
         _application.Settings.AntiSpam.AddHeaderSpam = true;
         _application.Settings.AntiSpam.PrependSubject = true;
         _application.Settings.AntiSpam.PrependSubjectText = "ThisIsSpam";

         _application.Settings.AntiSpam.CheckHostInHelo = true;
         _application.Settings.AntiSpam.CheckHostInHeloScore = 105;

         var incomingRelay = _application.Settings.IncomingRelays.Add();
         incomingRelay.LowerIP = "1.2.1.2";
         incomingRelay.UpperIP = "1.2.1.3";
         incomingRelay.Name = "Test";
         incomingRelay.Save();

         var messages = new List<string>();

         var message = "Received: from example.com (example.com [1.2.1.2]) by mail.host.edu\r\n" +
                       "Received: from mail.hmailserver.com (mail.hmailserver.com [" +
                       TestSetup.GethMailServerCOMIPaddress() + "]) by mail.host.edu\r\n" +
                       "From: spftest@openspf.org\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Should be blocked by SPF.";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = 0;
            fa.UseAntiSpam = true;
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
         }
      }

      [Test]
      public void TestSpamProtectionPreTransmissionSPFDelete()
      {
         _application.Settings.AntiSpam.SpamMarkThreshold = 1;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 100;
         _application.Settings.AntiSpam.AddHeaderReason = true;
         _application.Settings.AntiSpam.AddHeaderSpam = true;
         _application.Settings.AntiSpam.PrependSubject = true;
         _application.Settings.AntiSpam.PrependSubjectText = "ThisIsSpam";

         _application.Settings.AntiSpam.UseSPF = true;
         _application.Settings.AntiSpam.UseSPFScore = 105;

         var messages = new List<string>();

         var message = "Received: from openspf.org (openspf.org [1.2.3.4]) by mail.host.edu\r\n" +
                       "From: spftest@openspf.org\r\n" +
                       "To: Martin@example.com\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Should be blocked by SPF.";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = 0;
            fa.UseAntiSpam = true;
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 0);
         }
      }

      [Test]
      public void TestSpamProtectionPreTransmissionSPFPass()
      {
         _application.Settings.AntiSpam.SpamMarkThreshold = 1;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 100;
         _application.Settings.AntiSpam.AddHeaderReason = true;
         _application.Settings.AntiSpam.AddHeaderSpam = true;
         _application.Settings.AntiSpam.PrependSubject = true;
         _application.Settings.AntiSpam.PrependSubjectText = "ThisIsSpam";

         _application.Settings.AntiSpam.UseSPF = true;
         _application.Settings.AntiSpam.UseSPFScore = 105;

         var messages = new List<string>();

         var message =
            "Received: from nonexistent.hmailserver.com (nonexistent.hmailserver.com [1.2.3]) by nonexistent.hmailserver.com\r\n" +
            "From: example@nonexistent.hmailserver.com\r\n" +
            "To: Martin@example.com\r\n" +
            "Subject: Test\r\n" +
            "\r\n" +
            "Should NOT be blocked by SPF.";

         messages.Add(message);

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = 0;
            fa.UseAntiSpam = true;
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
         }
      }


      [Test]
      [Description("Issue 14, Potentially invalid sender address when fetching from external account")]
      public void TestFetchMessageWithValidFromAddress()
      {
         var message = string.Format("From: A@example.com\r\n" +
                                     "To: someone@example.com\r\n" +
                                     "Subject: Test\r\n" +
                                     "\r\n" +
                                     "Hello!");

         var messages = new List<string> { message };


         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.SendBufferMode = Pop3ServerSimulator.BufferMode.SingleBuffer;
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            LockHelper.WaitForUnlock(fa);

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

            var log = LogHandler.ReadCurrentDefaultLog();
            Assert.IsTrue(log.Contains("Delivering message from A@example.com to user@example.test."));
         }
      }

      [Test]
      [Description("Issue 14, Potentially invalid sender address when fetching from external account")]
      public void TestFetchMessageWithInvalidFromAddress()
      {
         var message = string.Format("From: A\r\n" +
                                     "To: someone@example.com\r\n" +
                                     "Subject: Test\r\n" +
                                     "\r\n" +
                                     "Hello!");

         var messages = new List<string> { message };


         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.SendBufferMode = Pop3ServerSimulator.BufferMode.SingleBuffer;
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.Save();

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            LockHelper.WaitForUnlock(fa);

            fa.Delete();

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

            var log = LogHandler.ReadCurrentDefaultLog();
            Assert.IsTrue(log.Contains("Delivering message from <Empty> to user@example.test."));
         }
      }

      // https://github.com/hmailserver/hmailserver/issues/168
      //
      // When a message is downloaded from an external account, the IP address used for the IP
      // based spam tests is parsed out of the Received headers of the message. The host name the
      // sender presented in HELO/EHLO ends up in those headers, but it is supplied by the sender
      // and must not affect which IP address is tested.
      //
      // In the messages below, 203.0.113.99 is the address the message was received from, and
      // 198.51.100.7 is an address the sender put in the message. The black list host doesn't
      // resolve - the tests assert on which lookups hMailServer makes, not on their result.
      private const string OriginatingIPBlackList = "dnsbl.example.test";

      private void EnableOriginatingIPBlackList()
      {
         _application.Settings.AntiSpam.SpamMarkThreshold = 1;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 100;

         var dnsBlackList = _application.Settings.AntiSpam.DNSBlackLists.Add();
         dnsBlackList.DNSHost = OriginatingIPBlackList;
         dnsBlackList.RejectMessage = "Blocked";
         dnsBlackList.Score = 5;
         dnsBlackList.Active = true;
         dnsBlackList.Save();
      }

      private Account DownloadMessageFromExternalAccount(string message)
      {
         var messages = new List<string> {message};

         var port = TestSetup.GetNextFreePort();

         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
            var fa = CreateFetchAccount(account, port, true, false);

            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            LockHelper.WaitForUnlock(fa);

            fa.Delete();

            return account;
         }
      }

      private string FetchMessageAndReadLog(string message)
      {
         LogHandler.DeleteCurrentDefaultLog();

         var account = DownloadMessageFromExternalAccount(message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         return LogHandler.ReadCurrentDefaultLog();
      }

      [Test]
      [Description("Issue 168: The DNSBL lookup should be made against the IP address the message " +
                   "was received from, not against an address literal presented in HELO.")]
      public void TestSpamTestsUseReceivedIPWhenHeloContainsAddressLiteral()
      {
         EnableOriginatingIPBlackList();

         var message = "Received: from [198.51.100.7] (unknown [203.0.113.99])\r\n" +
                       "\tby mail.example.test with ESMTP\r\n" +
                       "\t; Fri, 06 May 2016 03:49:14 +0200\r\n" +
                       "From: sender@example.com\r\n" +
                       "To: user@example.test\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Test body.";

         var log = FetchMessageAndReadLog(message);

         Assert.IsTrue(log.Contains("DNS lookup: 99.113.0.203." + OriginatingIPBlackList), log);
         Assert.IsFalse(log.Contains("DNS lookup: 7.100.51.198." + OriginatingIPBlackList), log);
      }

      [Test]
      [Description("Issue 168: A HELO host name which isn't a valid domain name should not cause " +
                   "the spam tests to be skipped.")]
      public void TestSpamTestsAreRunWhenHeloIsNotAValidDomainName()
      {
         EnableOriginatingIPBlackList();

         var message = "Received: from my_pc (unknown [203.0.113.99])\r\n" +
                       "\tby mail.example.test with ESMTP\r\n" +
                       "\t; Fri, 06 May 2016 03:49:14 +0200\r\n" +
                       "From: sender@example.com\r\n" +
                       "To: user@example.test\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Test body.";

         var log = FetchMessageAndReadLog(message);

         Assert.IsTrue(log.Contains("DNS lookup: 99.113.0.203." + OriginatingIPBlackList), log);
      }

      [Test]
      [Description("Issue 168: A HELO host name which isn't a valid domain name should not cause " +
                   "hMailServer to fall back on a Received header written by the sender.")]
      public void TestSpamTestsIgnoreReceivedHeadersBelowTheOriginatingOne()
      {
         EnableOriginatingIPBlackList();

         var message = "Received: from my_pc (unknown [203.0.113.99])\r\n" +
                       "\tby mail.example.test with ESMTP\r\n" +
                       "\t; Fri, 06 May 2016 03:49:14 +0200\r\n" +
                       "Received: from forged.example.test (forged.example.test [198.51.100.7])\r\n" +
                       "\tby forged.example.test with ESMTP\r\n" +
                       "\t; Fri, 06 May 2016 03:49:13 +0200\r\n" +
                       "From: sender@example.com\r\n" +
                       "To: user@example.test\r\n" +
                       "Subject: Test\r\n" +
                       "\r\n" +
                       "Test body.";

         var log = FetchMessageAndReadLog(message);

         Assert.IsTrue(log.Contains("DNS lookup: 99.113.0.203." + OriginatingIPBlackList), log);
         Assert.IsFalse(log.Contains("DNS lookup: 7.100.51.198." + OriginatingIPBlackList), log);
      }

      // The message below was received from 203.0.113.99, but the sender presented the address
      // literal [198.51.100.7] in HELO. The HELO host name test compares the two, and since they
      // don't match, the message should be classified as spam.
      //
      // The test doesn't depend on DNS: when the HELO host name is an address literal, the HELO
      // host name test is a plain comparison against the originating IP address.
      private const string MessageWithAddressLiteralInHelo =
         "Received: from [198.51.100.7] (unknown [203.0.113.99])\r\n" +
         "\tby mail.example.test with ESMTP\r\n" +
         "\t; Fri, 06 May 2016 03:49:14 +0200\r\n" +
         "From: sender@example.com\r\n" +
         "To: user@example.test\r\n" +
         "Subject: Test\r\n" +
         "\r\n" +
         "Test body.";

      [Test]
      [Description("Issue 168: A message whose HELO host name does not match the IP address it was " +
                   "received from should be marked as spam, also when the HELO host name is an " +
                   "address literal.")]
      public void TestMessageWithAddressLiteralInHeloIsMarkedAsSpam()
      {
         _application.Settings.AntiSpam.SpamMarkThreshold = 1;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 100;
         _application.Settings.AntiSpam.AddHeaderReason = true;
         _application.Settings.AntiSpam.AddHeaderSpam = true;
         _application.Settings.AntiSpam.PrependSubject = true;
         _application.Settings.AntiSpam.PrependSubjectText = "ThisIsSpam";

         _application.Settings.AntiSpam.CheckHostInHelo = true;
         _application.Settings.AntiSpam.CheckHostInHeloScore = 5;

         var account = DownloadMessageFromExternalAccount(MessageWithAddressLiteralInHelo);

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(messageText.Contains("Subject: ThisIsSpam Test"), messageText);
         Assert.IsTrue(messageText.Contains("X-hMailServer-Spam: YES"), messageText);
         Assert.IsTrue(messageText.Contains("X-hMailServer-Reason-1:"), messageText);
         Assert.IsTrue(messageText.Contains("The host name specified in HELO"), messageText);
      }

      [Test]
      [Description("Issue 168: A message whose HELO host name does not match the IP address it was " +
                   "received from should be deleted when it reaches the delete threshold, also when " +
                   "the HELO host name is an address literal.")]
      public void TestMessageWithAddressLiteralInHeloIsDeleted()
      {
         _application.Settings.AntiSpam.SpamMarkThreshold = 1;
         _application.Settings.AntiSpam.SpamDeleteThreshold = 100;

         _application.Settings.AntiSpam.CheckHostInHelo = true;
         _application.Settings.AntiSpam.CheckHostInHeloScore = 105;

         var account = DownloadMessageFromExternalAccount(MessageWithAddressLiteralInHelo);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 0);
      }
   }
}