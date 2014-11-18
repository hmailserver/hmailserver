using System;
using System.Collections.Generic;
using System.Net.Mail;
using System.Text;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

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


         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var oRecipients = new List<string>();
         oRecipients.Add("test@dummy-example.com");

         DistributionList oList3 = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list@test.com",
                                                                                             oRecipients);
         oList3.Mode = eDistributionListMode.eLMAnnouncement;
         oList3.RequireSenderAddress = "test@dummy-example.com";
         oList3.Save();

         // THIS MESSAGE SHOULD FAIL
         CustomAsserts.Throws<DeliveryFailedException>(()=> smtpClientSimulator.Send("test@test.com", "list@test.com", "Mail 1", "Mail 1"));

         DomainAlias oDA = _domain.DomainAliases.Add();
         oDA.AliasName = "dummy-example.com";
         oDA.Save();

         // THIS MESSAGE SHOULD SUCCEED
         smtpClientSimulator.Send("test@dummy-example.com", "list@dummy-example.com", "Mail 1", "Mail 1");
         ImapClientSimulator.AssertMessageCount("test@dummy-example.com", "test", "Inbox", 1);
      }

      [Test]
      public void TestDistributionListPointingAtItself()
      {
         // Add distribution list
         var oRecipients = new List<string>();
         oRecipients.Add("recipient1@test.com");
         oRecipients.Add("recipient2@test.com");
         oRecipients.Add("recipient4@test.com");
         oRecipients.Add("list1@test.com");

         SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@test.com", oRecipients);
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient1@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient2@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient3@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient4@test.com", "test");

         SmtpClientSimulator.StaticSend("test@test.com", "list1@test.com", "Mail 1", "Mail 1");

         ImapClientSimulator.AssertMessageCount("recipient1@test.com", "test", "Inbox", 1);
         ImapClientSimulator.AssertMessageCount("recipient2@test.com", "test", "Inbox", 1);
         ImapClientSimulator.AssertMessageCount("recipient4@test.com", "test", "Inbox", 1);
      }

      [Test]
      public void TestDistributionListWithEmptyAddress()
      {
         // Add distribution list
         var oRecipients = new List<string>();
         oRecipients.Add("recipient1@test.com");
         oRecipients.Add("recipient2@test.com");
         oRecipients.Add("");
         oRecipients.Add("recipient4@test.com");

         try
         {
            SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@test.com", oRecipients);
         }
         catch (Exception ex)
         {
            Assert.IsTrue(ex.Message.Contains("The recipient address is empty"));
            return;
         }

         Assert.Fail("No error reported when creating distribution list recipient with empty address");
      }

      [Test]
      public void TestDistributionListModePublic()
      {
         var recipients = new List<string>();
         recipients.Add("recipient1@test.com");
         recipients.Add("recipient2@test.com");
         recipients.Add("recipient3@test.com");

         var list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@test.com", recipients);

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient1@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient2@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient3@test.com", "test");

         var announcer = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "announcer@test.com", "test");

         // Switch list mode so that only a single announcer can send to list.
         list.Mode = eDistributionListMode.eLMPublic;
         list.RequireSMTPAuth = false;
         list.Save();

         var smtpClient = new SmtpClientSimulator();
         smtpClient.Send("test@test.com", list.Address, "Mail 1", "Mail 1");
         
         foreach (var recipient in recipients)
            ImapClientSimulator.AssertMessageCount(recipient, "test", "Inbox", 1);
      }


      [Test]
      public void TestDistributionListModeAnnouncer()
      {
         var recipients = new List<string>();
         recipients.Add("recipient1@test.com");
         recipients.Add("recipient2@test.com");
         recipients.Add("recipient3@test.com");

         var list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@test.com", recipients);

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient1@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient2@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient3@test.com", "test");

         var announcer = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "announcer@test.com", "test");

         // Switch list mode so that only a single announcer can send to list.
         list.Mode = eDistributionListMode.eLMAnnouncement;
         list.RequireSenderAddress = announcer.Address;
         list.RequireSMTPAuth = false;
         list.Save();

         var smtpClient = new SmtpClientSimulator();
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send("test@test.com", list.Address, "Mail 1", "Mail 1"));
         smtpClient.Send(announcer.Address, list.Address, "Mail 1", "Mail 1");

         foreach (var recipient in recipients)
            ImapClientSimulator.AssertMessageCount(recipient, "test", "Inbox", 1);
      }

      [Test]
      public void TestDistributionListModeMembers()
      {
         var recipients = new List<string>();
         recipients.Add("recipient1@test.com");
         recipients.Add("recipient2@test.com");
         recipients.Add("recipient3@test.com");

         var list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@test.com", recipients);

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient1@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient2@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient3@test.com", "test");

         var announcer = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "announcer@test.com", "test");

         // Switch list mode so that only a single announcer can send to list.
         list.Mode = eDistributionListMode.eLMMembership;
         list.RequireSenderAddress = announcer.Address;
         list.RequireSMTPAuth = false;
         list.Save();

         var smtpClient = new SmtpClientSimulator();
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send("test@test.com", list.Address, "Mail 1", "Mail 1"));
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClient.Send(announcer.Address, list.Address, "Mail 1", "Mail 1"));
         smtpClient.Send(recipients[0], list.Address, "Mail 1", "Mail 1");

         foreach (var recipient in recipients)
            ImapClientSimulator.AssertMessageCount(recipient, "test", "Inbox", 1);
      }

      [Test]
      public void TestDistributionListsMembershipDomainAliases()
      {
         var oIMAP = new ImapClientSimulator();
         var smtpClientSimulator = new SmtpClientSimulator();

         Application application = SingletonProvider<TestSetup>.Instance.GetApp();


         DomainAlias oDA = _domain.DomainAliases.Add();
         oDA.AliasName = "dummy-example.com";
         oDA.Save();

         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account1@test.com", "test");
         oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account2@test.com", "test");


         // 
         // TEST LIST SECURITY IN COMBINATION WITH DOMAIN NAME ALIASES
         // 


         var oRecipients = new List<string>();
         oRecipients.Clear();
         oRecipients.Add("vaffe@dummy-example.com");

         DistributionList oList3 = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list@test.com",
                                                                                             oRecipients);
         oList3.Mode = eDistributionListMode.eLMMembership;
         oList3.Save();

         // THIS MESSAGE SHOULD FAIL - Membership required, unknown sender domain
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send("account1@dummy-example.com", "list@test.com", "Mail 1", "Mail 1"));

         oList3.Delete();

         // THIS MESSAGE SHOULD SUCCED - Membership required, sender domain is now an alias for test.com.

         oRecipients = new List<string>();
         oRecipients.Clear();
         oRecipients.Add("account1@dummy-example.com");
         oRecipients.Add("account2@test.com");

         oList3 = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list@test.com", oRecipients);
         oList3.Mode = eDistributionListMode.eLMMembership;
         oList3.Save();

         smtpClientSimulator.Send("account1@dummy-example.com", "list@test.com", "Mail 1", "Mail 1");

         ImapClientSimulator.AssertMessageCount("account1@test.com", "test", "Inbox", 1);
         ImapClientSimulator.AssertMessageCount("account2@test.com", "test", "Inbox", 1);
      }


      [Test]
      public void TestListContainingLists()
      {
         var test = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "acc1@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "acc2@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "acc3@test.com", "test");

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "outsider1@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "outsider2@test.com", "test");

         var daRecipients = new List<string>()
            {
               "db@test.com",
               "dc@test.com"               
            };

         var dbRecipients = new List<string>
            {
               "acc2@test.com",
               "acc3@test.com",
            };

         var dcRecipients = new List<string>
            {
               "acc2@test.com",
               "acc3@test.com",
            };

         DistributionList daList = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "da@test.com", daRecipients);
         daList.Mode = eDistributionListMode.eLMPublic;
         daList.Save();
         
         DistributionList dbList = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "db@test.com", dbRecipients);
         dbList.Mode = eDistributionListMode.eLMPublic;
         dbList.Save();

         DistributionList dcList = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "dc@test.com", dcRecipients);
         dbList.Mode = eDistributionListMode.eLMPublic;
         dbList.Save();

         var recipients = new List<string>()
            {
               "da@test.com",
               "outsider1@test.com",
               "outsider2@test.com"               
            };

         var smtpClient = new SmtpClientSimulator();
         smtpClient.Send(test.Address, recipients, "test" , "test");

         ImapClientSimulator.AssertMessageCount("acc2@test.com", "test", "Inbox", 1); // Member in list
         ImapClientSimulator.AssertMessageCount("acc3@test.com", "test", "Inbox", 1); // Member in list
         ImapClientSimulator.AssertMessageCount("outsider1@test.com", "test", "Inbox", 1); // Included in To list
         ImapClientSimulator.AssertMessageCount("outsider2@test.com", "test", "Inbox", 1); // Included in To list
      }
   }
}
