using System;
using System.Collections.Generic;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class DistributionLists : TestFixtureBase
   {
      [Test]
      public void TestDistributionListAnnouncementFromDomainAlias()
      {
         var smtpClientSimulator = new SmtpClientSimulator();

         // 
         // TEST LIST SECURITY IN COMBINATION WITH DOMAIN NAME ALIASES
         // 


         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var recipients = new List<string>();
         recipients.Add("test@dummy-example.com");

         var list3 = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list@example.test",
            recipients);
         list3.Mode = eDistributionListMode.eLMAnnouncement;
         list3.RequireSenderAddress = "test@dummy-example.com";
         list3.Save();

         // THIS MESSAGE SHOULD FAIL
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send("test@example.test", "list@example.test", "Mail 1", "Mail 1"));

         var domainAlias = _domain.DomainAliases.Add();
         domainAlias.AliasName = "dummy-example.com";
         domainAlias.Save();

         // THIS MESSAGE SHOULD SUCCEED
         smtpClientSimulator.Send("test@dummy-example.com", "list@dummy-example.com", "Mail 1", "Mail 1");
         ImapClientSimulator.AssertMessageCount("test@dummy-example.com", "test", "Inbox", 1);
      }

      [Test]
      public void TestDistributionListPointingAtItself()
      {
         // Add distribution list
         var recipients = new List<string>();
         recipients.Add("recipient1@example.test");
         recipients.Add("recipient2@example.test");
         recipients.Add("recipient4@example.test");
         recipients.Add("list1@example.test");

         SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@example.test", recipients);
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient1@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient2@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient3@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient4@example.test", "test");

         SmtpClientSimulator.StaticSend("test@example.test", "list1@example.test", "Mail 1", "Mail 1");

         ImapClientSimulator.AssertMessageCount("recipient1@example.test", "test", "Inbox", 1);
         ImapClientSimulator.AssertMessageCount("recipient2@example.test", "test", "Inbox", 1);
         ImapClientSimulator.AssertMessageCount("recipient4@example.test", "test", "Inbox", 1);
      }

      [Test]
      public void TestDistributionListWithEmptyAddress()
      {
         // Add distribution list
         var recipients = new List<string>();
         recipients.Add("recipient1@example.test");
         recipients.Add("recipient2@example.test");
         recipients.Add("");
         recipients.Add("recipient4@example.test");

         try
         {
            SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@example.test", recipients);
         }
         catch (Exception ex)
         {
            Assert.IsTrue(ex.Message.Contains("The recipient address is empty"), ex.Message);
            return;
         }

         Assert.Fail("No error reported when creating distribution list list with empty address");
      }

      [Test]
      public void TestDistributionListModePublic()
      {
         var recipients = new List<string>();
         recipients.Add("recipient1@example.test");
         recipients.Add("recipient2@example.test");
         recipients.Add("recipient3@example.test");

         var list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@example.test",
            recipients);

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient1@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient2@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient3@example.test", "test");

         var announcer = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "announcer@example.test", "test");

         // Switch list mode so that only a single announcer can send to list.
         list.Mode = eDistributionListMode.eLMPublic;
         list.RequireSMTPAuth = false;
         list.Save();

         var smtpClient = new SmtpClientSimulator();
         smtpClient.Send("test@example.test", list.Address, "Mail 1", "Mail 1");

         foreach (var recipientAddress in recipients)
            ImapClientSimulator.AssertMessageCount(recipientAddress, "test", "Inbox", 1);
      }


      [Test]
      public void TestDistributionListModeAnnouncer()
      {
         var recipients = new List<string>();
         recipients.Add("recipient1@example.test");
         recipients.Add("recipient2@example.test");
         recipients.Add("recipient3@example.test");

         var list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@example.test",
            recipients);

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient1@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient2@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient3@example.test", "test");

         var announcer = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "announcer@example.test", "test");

         // Switch list mode so that only a single announcer can send to list.
         list.Mode = eDistributionListMode.eLMAnnouncement;
         list.RequireSenderAddress = announcer.Address;
         list.RequireSMTPAuth = false;
         list.Save();

         var smtpClient = new SmtpClientSimulator();
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClient.Send("test@example.test", list.Address, "Mail 1", "Mail 1"));
         smtpClient.Send(announcer.Address, list.Address, "Mail 1", "Mail 1");

         foreach (var recipientAddress in recipients)
            ImapClientSimulator.AssertMessageCount(recipientAddress, "test", "Inbox", 1);
      }

      [Test]
      public void TestDistributionListModeMembers()
      {
         var recipients = new List<string>();
         recipients.Add("recipient1@example.test");
         recipients.Add("recipient2@example.test");
         recipients.Add("recipient3@example.test");

         var list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@example.test",
            recipients);

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient1@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient2@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient3@example.test", "test");

         var announcer = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "announcer@example.test", "test");

         // Switch list mode so that only a single announcer can send to list.
         list.Mode = eDistributionListMode.eLMMembership;
         list.RequireSenderAddress = announcer.Address;
         list.RequireSMTPAuth = false;
         list.Save();

         var smtpClient = new SmtpClientSimulator();
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClient.Send("test@example.test", list.Address, "Mail 1", "Mail 1"));
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClient.Send(announcer.Address, list.Address, "Mail 1", "Mail 1"));
         smtpClient.Send(recipients[0], list.Address, "Mail 1", "Mail 1");

         foreach (var recipientAddress in recipients)
            ImapClientSimulator.AssertMessageCount(recipientAddress, "test", "Inbox", 1);
      }

      [Test]
      public void TestDistributionListModeDomainMembers()
      {
         var recipients = new List<string>();
         recipients.Add("recipient1@example.test");
         recipients.Add("recipient2@example.test");
         recipients.Add("recipient3@example.test");

         var list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@example.test",
            recipients);

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient1@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient2@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient3@example.test", "test");

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // Switch list mode so that only a single announcer can send to list.
         list.Mode = eDistributionListMode.eLMDomainMembers;
         list.RequireSMTPAuth = false;
         list.Save();

         var smtpClient = new SmtpClientSimulator();

         Assert.DoesNotThrow(() => smtpClient.Send("test@example.test", list.Address, "Mail 1", "Mail 1"));
         Assert.DoesNotThrow(() => smtpClient.Send("non-existent@example.test", list.Address, "Mail 1", "Mail 1"));

         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClient.Send("external@example.com", list.Address, "Mail 1", "Mail 1"));

         foreach (var recipientAddress in recipients)
            ImapClientSimulator.AssertMessageCount(recipientAddress, "test", "Inbox", 2);
      }

      [Test]
      public void TestDistributionListsMembershipDomainAliases()
      {
         var imap = new ImapClientSimulator();
         var smtpClientSimulator = new SmtpClientSimulator();

         var application = SingletonProvider<TestSetup>.Instance.GetApp();


         var domainAlias = _domain.DomainAliases.Add();
         domainAlias.AliasName = "dummy-example.com";
         domainAlias.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account1@example.test", "test");
         account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account2@example.test", "test");


         // 
         // TEST LIST SECURITY IN COMBINATION WITH DOMAIN NAME ALIASES
         // 


         var recipients = new List<string>();
         recipients.Clear();
         recipients.Add("vaffe@dummy-example.com");

         var list3 = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list@example.test",
            recipients);
         list3.Mode = eDistributionListMode.eLMMembership;
         list3.Save();

         // THIS MESSAGE SHOULD FAIL - Membership required, unknown sender domain
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            smtpClientSimulator.Send("account1@dummy-example.com", "list@example.test", "Mail 1", "Mail 1"));

         list3.Delete();

         // THIS MESSAGE SHOULD SUCCEED - Membership required, sender domain is now an alias for test.com.

         recipients = new List<string>();
         recipients.Clear();
         recipients.Add("account1@dummy-example.com");
         recipients.Add("account2@example.test");

         list3 = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list@example.test", recipients);
         list3.Mode = eDistributionListMode.eLMMembership;
         list3.Save();

         smtpClientSimulator.Send("account1@dummy-example.com", "list@example.test", "Mail 1", "Mail 1");

         ImapClientSimulator.AssertMessageCount("account1@example.test", "test", "Inbox", 1);
         ImapClientSimulator.AssertMessageCount("account2@example.test", "test", "Inbox", 1);
      }


      [Test]
      public void TestListContainingLists()
      {
         var test = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "acc1@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "acc2@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "acc3@example.test", "test");

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "outsider1@example.test", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "outsider2@example.test", "test");

         var daRecipients = new List<string>
         {
            "db@example.test",
            "dc@example.test"
         };

         var dbRecipients = new List<string>
         {
            "acc2@example.test",
            "acc3@example.test"
         };

         var dcRecipients = new List<string>
         {
            "acc2@example.test",
            "acc3@example.test"
         };

         var daList =
            SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "da@example.test", daRecipients);
         daList.Mode = eDistributionListMode.eLMPublic;
         daList.Save();

         var dbList =
            SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "db@example.test", dbRecipients);
         dbList.Mode = eDistributionListMode.eLMPublic;
         dbList.Save();

         var dcList =
            SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "dc@example.test", dcRecipients);
         dbList.Mode = eDistributionListMode.eLMPublic;
         dbList.Save();

         var recipients = new List<string>
         {
            "da@example.test",
            "outsider1@example.test",
            "outsider2@example.test"
         };

         var smtpClient = new SmtpClientSimulator();
         smtpClient.Send(test.Address, recipients, "test", "test");

         ImapClientSimulator.AssertMessageCount("acc2@example.test", "test", "Inbox", 1); // Member in list
         ImapClientSimulator.AssertMessageCount("acc3@example.test", "test", "Inbox", 1); // Member in list
         ImapClientSimulator.AssertMessageCount("outsider1@example.test", "test", "Inbox", 1); // Included in To list
         ImapClientSimulator.AssertMessageCount("outsider2@example.test", "test", "Inbox", 1); // Included in To list
      }
   }
}