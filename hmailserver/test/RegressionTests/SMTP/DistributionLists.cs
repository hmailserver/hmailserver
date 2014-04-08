using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using NUnit.Framework;
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
         var oIMAP = new IMAPSimulator();
         var oSMTP = new SMTPClientSimulator();

         Application application = SingletonProvider<TestSetup>.Instance.GetApp();


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
         Assert.IsFalse(oSMTP.Send("test@test.com", "list@test.com", "Mail 1", "Mail 1"));

         DomainAlias oDA = _domain.DomainAliases.Add();
         oDA.AliasName = "dummy-example.com";
         oDA.Save();

         // THIS MESSAGE SHOULD SUCCEED
         Assert.IsTrue(oSMTP.Send("test@dummy-example.com", "list@dummy-example.com", "Mail 1", "Mail 1"));
         IMAPSimulator.AssertMessageCount("test@dummy-example.com", "test", "Inbox", 1);
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

         Assert.IsTrue(SMTPClientSimulator.StaticSend("test@test.com", "list1@test.com", "Mail 1", "Mail 1"));

         IMAPSimulator.AssertMessageCount("recipient1@test.com", "test", "Inbox", 1);
         IMAPSimulator.AssertMessageCount("recipient2@test.com", "test", "Inbox", 1);
         IMAPSimulator.AssertMessageCount("recipient4@test.com", "test", "Inbox", 1);
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
      public void TestDistributionLists()
      {
         // Fetch default domain


         // Add distribution list
         var oRecipients = new List<string>();
         oRecipients.Add("recipient1@test.com");
         oRecipients.Add("recipient2@test.com");
         oRecipients.Add("recipient3@test.com");

         SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@test.com", oRecipients);

         Account oAccount;
         oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient1@test.com", "test");
         oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient2@test.com", "test");
         oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient3@test.com", "test");

         // Add alias pointing at the distribution list.
         SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "listalias@test.com", "list1@test.com");

         var oSMTP = new SMTPClientSimulator();

         Assert.IsTrue(oSMTP.Send("test@test.com", "list1@test.com", "Mail 1", "Mail 1"));
         Assert.IsTrue(oSMTP.Send("test@test.com", "listalias@test.com", "Mail 2", "Mail 2"));
         Assert.IsTrue(oSMTP.Send("test@test.com", "listalias@test.com", "Mail 3", "Mail 3"));

         IMAPSimulator.AssertMessageCount("recipient1@test.com", "test", "Inbox", 3);
         IMAPSimulator.AssertMessageCount("recipient2@test.com", "test", "Inbox", 3);
         IMAPSimulator.AssertMessageCount("recipient3@test.com", "test", "Inbox", 3);

         oRecipients.Add("recipient4@test.com");
         oRecipients.Add("recipient5@test.com");
         oRecipients.Add("recipient6@test.com");
         oRecipients.Add("recipient7@test.com");
         oRecipients.Add("recipient8@test.com");

         SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list2@test.com", oRecipients);
         oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient7@test.com", "test");
         oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient8@test.com", "test");
         oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         Assert.IsTrue(oSMTP.Send("test@test.com", "list2@test.com", "Mail 1", "Mail 1"));

         IMAPSimulator.AssertMessageCount("recipient7@test.com", "test", "Inbox", 1);

         // Wait for the message to be completely delivered. The above assertion isn't enough to confirm that.
         Thread.Sleep(1000);

         oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "announcer@test.com", "test");
         DistributionList oList = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain,
                                                                                            "list3-security@test.com",
                                                                                            oRecipients);
         oList.Mode = eDistributionListMode.eLMAnnouncement;
         oList.RequireSenderAddress = oAccount.Address;
         oList.RequireSMTPAuth = false;
         oList.Save();

         Assert.IsFalse(oSMTP.Send("test@test.com", "list3-security@test.com", "Mail 1", "Mail 1"));
         Assert.IsTrue(oSMTP.Send(oAccount.Address, "list3-security@test.com", "Mail 1", "Mail 1"));
         IMAPSimulator.AssertMessageCount("recipient7@test.com", "test", "Inbox", 2);

         oList.Mode = eDistributionListMode.eLMMembership;
         oList.Save();

         Assert.IsFalse(oSMTP.Send(oAccount.Address, "list3-security@test.com", "Mail 1", "Mail 1"));

         // THIS MESSAGE SHOULD SUCCED 
         Assert.IsTrue(oSMTP.Send("recipient5@test.com", "list3-security@test.com", "Mail 1", "Mail 1"));
         IMAPSimulator.AssertMessageCount("recipient7@test.com", "test", "Inbox", 3);
      }

      [Test]
      public void TestDistributionListsMembershipDomainAliases()
      {
         var oIMAP = new IMAPSimulator();
         var oSMTP = new SMTPClientSimulator();

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
         Assert.IsFalse(oSMTP.Send("account1@dummy-example.com", "list@test.com", "Mail 1", "Mail 1"));

         oList3.Delete();

         // THIS MESSAGE SHOULD SUCCED - Membership required, sender domain is now an alias for test.com.

         oRecipients = new List<string>();
         oRecipients.Clear();
         oRecipients.Add("account1@dummy-example.com");
         oRecipients.Add("account2@test.com");

         oList3 = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list@test.com", oRecipients);
         oList3.Mode = eDistributionListMode.eLMMembership;
         oList3.Save();

         Assert.IsTrue(oSMTP.Send("account1@dummy-example.com", "list@test.com", "Mail 1", "Mail 1"));

         IMAPSimulator.AssertMessageCount("account1@test.com", "test", "Inbox", 1);
         IMAPSimulator.AssertMessageCount("account2@test.com", "test", "Inbox", 1);
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

         var smtpClient = new SMTPClientSimulator();
         Assert.IsTrue(smtpClient.Send(test.Address, recipients, "test" , "test"));

         IMAPSimulator.AssertMessageCount("acc2@test.com", "test", "Inbox", 1); // Member in list
         IMAPSimulator.AssertMessageCount("acc3@test.com", "test", "Inbox", 1); // Member in list
         IMAPSimulator.AssertMessageCount("outsider1@test.com", "test", "Inbox", 1); // Included in To list
         IMAPSimulator.AssertMessageCount("outsider2@test.com", "test", "Inbox", 1); // Included in To list
      }
   }
}
