// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections.Generic;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class BasicTests : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         SingletonProvider<TestSetup>.Instance.DisableSpamProtection();
      }

      #endregion

      [Test]
      public void TestAliases()
      {
         // Fetch default domain

         // Create another account
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test2@test.com", "test");

         // Add aliases
         SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "alias1@test.com", "test2@test.com");
         SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "alias2@test.com", "test2@test.com");
         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("test@test.com", "test2@test.com", "Mail 1", "Mail 1");
         smtpClientSimulator.Send("test@test.com", "alias1@test.com", "Mail 2", "Mail 2");
         smtpClientSimulator.Send("test@test.com", "alias2@test.com", "Mail 3", "Mail 3");

         ImapClientSimulator.AssertMessageCount("test2@test.com", "test", "Inbox", 3);
      }

      [Test]
      public void TestMirror()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mirror@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mirror-test@test.com", "test");

         _settings.MirrorEMailAddress = "mirror-test@test.com";
         _settings.AddDeliveredToHeader = true;

         // Send 5 messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         for (int i = 0; i < 5; i++)
            smtpClientSimulator.Send("test@test.com", "mirror@test.com", "INBOX", "Mirror test message");

         // Check using POP3 that 5 messages exists.
         Pop3ClientSimulator.AssertMessageCount("mirror-test@test.com", "test", 5);

         string message = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount2.Address, "test");

         Assert.IsTrue(message.Contains("Delivered-To: mirror@test.com"));
      }

      [Test]
      [Description("Issue 248, Delivered-To header invalid for mirror messages")]
      public void TestMirrorMultipleRecipients()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mirror1@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mirror2@test.com", "test");
         Account oAccount3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mirror3@test.com", "test");

         Account mirrorAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mirror-test@test.com",
                                                                                  "test");

         _settings.MirrorEMailAddress = "mirror-test@test.com";
         _settings.AddDeliveredToHeader = true;

         // Send 5 messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("test@test.com", new List<string> {oAccount1.Address, oAccount2.Address, oAccount3.Address},
                    "INBOX", "Mirror test message");

         Pop3ClientSimulator.AssertMessageCount(mirrorAccount.Address, "test", 1);

         string message = Pop3ClientSimulator.AssertGetFirstMessageText(mirrorAccount.Address, "test");

         Assert.IsTrue(message.Contains("Delivered-To: mirror1@test.com,mirror2@test.com,mirror3@test.com"));

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
      }

      [Test]
      [Description("Issue 248, Delivered-To header invalid for mirror messages")]
      public void TestMirrorMultipleRecipientsOver255Chars()
      {
         // Create a test account
         // Fetch the default domain
         var recipients = new List<string>();
         for (int i = 0; i < 20; i++)
         {
            string address = string.Format("mirror{0}@test.com", i);
            SingletonProvider<TestSetup>.Instance.AddAccount(_domain, address, "test");
            recipients.Add(address);
         }

         Account mirrorAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mirror-test@test.com",
                                                                                  "test");

         _settings.MirrorEMailAddress = "mirror-test@test.com";
         _settings.AddDeliveredToHeader = true;

         // Send 1 messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("test@test.com", recipients, "INBOX", "Mirror test message");

         Pop3ClientSimulator.AssertMessageCount(mirrorAccount.Address, "test", 1);

         string message = Pop3ClientSimulator.AssertGetFirstMessageText(mirrorAccount.Address, "test");

         Assert.IsTrue(
            message.Contains(
               "Delivered-To: mirror0@test.com,mirror1@test.com,mirror2@test.com,mirror3@test.com,mirror4@test.com,mirror5@test.com,mirror6@test.com,mirror7@test.com,mirror8@test.com,mirror9@test.com,mirror10@test.com,mirror11@test.com,mirror12@test.com,mirror13@test.com,mirror14@test\r\n"));

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
      }

      [Test]
      public void DeliveryShouldSucceedAfterClearingDeliveryQueue()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         _application.GlobalObjects.DeliveryQueue.Clear();

         Assert.IsTrue(LogHandler.DefaultLogContains("Delivery queue cleared."));

         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("test@test.com", "test@test.com", "INBOX", "Mirror test message");

         Pop3ClientSimulator.AssertMessageCount("test@test.com", "test", 1);
      }
   }
}