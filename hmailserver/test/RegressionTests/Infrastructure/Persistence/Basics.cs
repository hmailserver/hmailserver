// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Infrastructure.Persistence
{
   [TestFixture]
   public class Basics : TestFixtureBase
   {
      [Test]
      public void TestAccount()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "perste'ster@test.com", "test");
         if (oAccount1.ID == 0)
            throw new Exception("Account not properly saved");

         _domain.Accounts.DeleteByDBID(oAccount1.ID);
      }

      [Test]
      public void TestAlias()
      {
         Alias oAlias = SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "fr'om@test.com", "to@t'st.com");
         if (oAlias.ID == 0)
            throw new Exception("Account not properly saved");

         _domain.Aliases.DeleteByDBID(oAlias.ID);
      }

      [Test]
      public void TestBlockedAttachment()
      {
         Settings oSettings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         BlockedAttachment attachment = oSettings.AntiVirus.BlockedAttachments.Add();

         attachment.Description = "Some description";
         attachment.Wildcard = "*.some";
         attachment.Save();

         if (attachment.ID == 0)
            throw new Exception("Blocked attachment not properly saved");

         oSettings.AntiVirus.BlockedAttachments.DeleteByDBID(attachment.ID);
      }


      [Test]
      public void TestCaseInsensitivtyAccount()
      {
         Account testAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "lowercase@test.com", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         string upperCase = testAccount.Address.ToUpper();
         smtpClientSimulator.Send("someone@dummy-example.com", upperCase, "test mail", "test body");

         Pop3ClientSimulator.AssertMessageCount("lowercase@test.com", "test", 1);
      }

      [Test]
      public void TestCaseInsensitivtyAlias()
      {
         Account testAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "lowercase@test.com", "test");
         Alias testAlias = SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "sometext@test.com",
                                                                          "LowerCase@test.com");

         var smtpClientSimulator = new SmtpClientSimulator();
         string upperCase = testAlias.Name.ToUpper();
         smtpClientSimulator.Send("someone@dummy-example.com", upperCase, "test mail", "test body");

         Pop3ClientSimulator.AssertMessageCount("lowercase@test.com", "test", 1);
      }

      [Test]
      public void TestCaseInsensitivtyList()
      {
         Account testAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "lowercase@test.com", "test");

         var recipients = new List<string>();
         recipients.Add(testAccount.Address);

         DistributionList list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "myList@test.com",
                                                                                           recipients);

         var smtpClientSimulator = new SmtpClientSimulator();
         string upperCase = list.Address.ToUpper();
         smtpClientSimulator.Send("someone@dummy-example.com", upperCase, "test mail", "test body");

         Pop3ClientSimulator.AssertMessageCount("lowercase@test.com", "test", 1);
      }

      [Test]
      public void TestCaseInsensitivtyListRecipient()
      {
         Account testAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "lowercase@test.com", "test");

         var recipients = new List<string>();
         recipients.Add(testAccount.Address);

         DistributionList list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "myList@test.com",
                                                                                           recipients);

         DistributionListRecipient recipient = list.Recipients[0];
         recipient.RecipientAddress = testAccount.Address.ToUpper();

         recipient.Delete();
      }

      [Test]
      public void TestDNSBlackList()
      {
         Settings oSettings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         DNSBlackList dnsBlackList = oSettings.AntiSpam.DNSBlackLists.Add();

         dnsBlackList.DNSHost = "somehost.com";
         dnsBlackList.RejectMessage = "somerejectmessage";
         dnsBlackList.Score = 5;

         dnsBlackList.Save();

         if (dnsBlackList.ID == 0)
            throw new Exception("DNS blacklist not saved properly");

         oSettings.AntiSpam.DNSBlackLists.DeleteByDBID(dnsBlackList.ID);
      }

      [Test]
      public void TestDistributionList()
      {
         DistributionList oList = _domain.DistributionLists.Add();
         oList.Address = "persis'tent-test-list@test.com";
         oList.Active = true;
         oList.Save();

         DistributionListRecipient oRecipient = oList.Recipients.Add();
         oRecipient.RecipientAddress = "test@te'st.com";
         oRecipient.Save();

         oRecipient.RecipientAddress = "tes't2@test.com";
         oRecipient.Save();
         oList.Delete();
      }

      [Test]
      public void TestDomain()
      {
         Domains domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;
         _domain = SingletonProvider<TestSetup>.Instance.AddDomain(domains, "test2.com");
         if (_domain.ID == 0)
            throw new Exception("Domain not properly saved");

         _domain.Delete();
      }

      [Test]
      public void TestDomainRenaming()
      {
         Domains domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;
         _domain = SingletonProvider<TestSetup>.Instance.AddDomain(domains, "test2.com");

         // Add a few accounts
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test1@test2.com", "t1");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test2@test2.com", "t1");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test3@test2.com", "t1");
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test2.com@test2.com", "t1");

         SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "alias1@test2.com", "t1");
         SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "alias2@test2.com", "t1");
         SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "alias3@test2.com", "t1");

         var list = new List<string>();
         SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list1@test2.com", list);
         SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list2@test2.com", list);
         SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list3@test2.com", list);

         _domain.Name = "test3.com";
         _domain.Save();

         Assert.IsNotNull(_domain.Accounts.get_ItemByAddress("test1@test3.com"));
         Assert.IsNotNull(_domain.Accounts.get_ItemByAddress("test2@test3.com"));
         Assert.IsNotNull(_domain.Accounts.get_ItemByAddress("test3@test3.com"));
         Assert.IsNotNull(_domain.Accounts.get_ItemByAddress("test2.com@test3.com"));

         Assert.IsNotNull(_domain.Aliases.get_ItemByName("alias1@test3.com"));
         Assert.IsNotNull(_domain.Aliases.get_ItemByName("alias2@test3.com"));
         Assert.IsNotNull(_domain.Aliases.get_ItemByName("alias3@test3.com"));

         Assert.IsNotNull(_domain.DistributionLists.get_ItemByAddress("list1@test3.com"));
         Assert.IsNotNull(_domain.DistributionLists.get_ItemByAddress("list2@test3.com"));
         Assert.IsNotNull(_domain.DistributionLists.get_ItemByAddress("list3@test3.com"));
      }


      [Test]
      public void TestDomainWithLargeAccounts()
      {
         Domain domain = SingletonProvider<TestSetup>.Instance.AddDomain("example.com");

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(domain.Accounts, "test1@example.com",
                                                                            "secret");
         account.MaxSize = 1024*1024*2000;
         account.Save();

         Account secondAccount = SingletonProvider<TestSetup>.Instance.AddAccount(domain.Accounts,
                                                                                  "test2@example.com", "secret");
         secondAccount.MaxSize = 1024*1024*2000;
         secondAccount.Save();

         Assert.AreEqual(account.MaxSize + (long) secondAccount.MaxSize, domain.AllocatedSize);
      }

      [Test]
      public void TestFetchAccount()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "fatester@test.com", "test");

         FetchAccount oFA = oAccount1.FetchAccounts.Add();
         oFA.Name = "test";
         oFA.Save();

         if (oFA.ID == 0)
            throw new Exception("Fetch account could not be saved");

         oAccount1.FetchAccounts.Delete(0);
      }

      [Test]
      public void TestGroup()
      {
         Groups groups = _application.Settings.Groups;
         Group group = groups.Add();
         group.Name = "MyGroup";
         group.Save();

         if (group.ID == 0)
            throw new Exception("Group not properly saved");

         _application.Settings.Groups.DeleteByDBID(group.ID);

         if (groups.Count != 0)
            throw new Exception("Group not properly deleted");
      }

      [Test]
      public void TestGroupMember()
      {
         Groups groups = _application.Settings.Groups;

         Group group = groups.Add();
         group.Name = "MyGroup";
         group.Save();

         if (group.ID == 0)
            throw new Exception("Group not properly saved");

         GroupMember member = group.Members.Add();
         member.AccountID = 1;
         member.Save();

         if (member.ID == 0)
            throw new Exception("Group member not properly saved");
      }

      [Test]
      public void TestIncomingRelays()
      {
         IncomingRelays incomingRelays = _application.Settings.IncomingRelays;
         Assert.AreEqual(0, incomingRelays.Count);

         IncomingRelay incomingRelay = incomingRelays.Add();
         incomingRelay.Name = "TestRelay";
         incomingRelay.LowerIP = "1.2.1.1";
         incomingRelay.UpperIP = "2.1.2.1";
         incomingRelay.Save();

         // Check that it was saved.
         Assert.AreNotEqual(0, incomingRelay.ID);

         // Confirm that settings were saved properly.
         incomingRelays.Refresh();
         IncomingRelay incomingRelay2 = incomingRelays.get_ItemByDBID(incomingRelay.ID);
         Assert.AreEqual(incomingRelay.ID, incomingRelay2.ID);
         Assert.AreEqual(incomingRelay.Name, incomingRelay2.Name);
         Assert.AreEqual(incomingRelay.LowerIP, incomingRelay2.LowerIP);
         Assert.AreEqual(incomingRelay.UpperIP, incomingRelay2.UpperIP);

         // Delete it again.
         incomingRelays.Delete(0);

         Assert.AreEqual(0, incomingRelays.Count);
      }

      [Test]
      public void TestRenameAccountOrDomainWithMessagesWithFullPath()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test message", "Test body");

         IMAPFolder folder = account.IMAPFolders.get_ItemByName("Inbox");
         CustomAsserts.AssertFolderMessageCount(folder, 1);
         Message message = account.IMAPFolders.get_ItemByName("Inbox").Messages[0];

         // Move the message file to another folder.
         string domainPath = Path.Combine(_application.Settings.Directories.DataDirectory, _domain.Name);
         string accountPath = Path.Combine(domainPath, "test");
         string fileName = Path.Combine(accountPath, "randomMail.eml");
         File.Move(message.Filename, fileName);

         // Update the database with the full path.
         string sql = string.Format("update hm_messages set messagefilename = '{0}' where messageid = {1}",
                                    TestSetup.Escape(fileName), message.ID);
         SingletonProvider<TestSetup>.Instance.GetApp().Database.ExecuteSQL(sql);

         // Now try to change the name of the domain or account. Should fail.
         account.Address = "test2@test.com";
         bool thrown = false;

         try
         {
            account.Save();
         }
         catch (Exception)
         {
            thrown = true;
         }

         Assert.IsTrue(thrown);

         // Saving account is OK, unless its address is changed.
         account.Address = "test@test.com";
         account.Save();

         thrown = false;

         _domain.Name = "example.com";

         try
         {
            _domain.Save();
         }
         catch (Exception)
         {
            thrown = true;
         }

         Assert.IsTrue(thrown);

         // Saving domain is OK, unless its address is changed.
         _domain.Name = "test.com";
         _domain.Save();
      }

      [Test]
      public void TestRenameAccountWithMessages()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account1@test.com", "test");

         string messageBody = Guid.NewGuid().ToString();
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Subj", messageBody);
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         account.Address = "account2@test.com";
         account.Save();

         string messageText = Pop3ClientSimulator.AssertGetFirstMessageText("account2@test.com", "test");
         Assert.IsTrue(messageText.Contains(messageBody), messageText);
      }

      [Test]
      [Description("Issue 343, Changing domain name doesn't change distribution list addresses")]
      public void TestRenameDomainWithAccountForward()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account1@test.com", "test");
         oAccount1.ForwardAddress = "someone@test.com";
         oAccount1.Save();

         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account2@test.com", "test");
         oAccount2.ForwardAddress = "someone@external.com";
         oAccount2.Save();

         _domain.Name = "example.com";
         _domain.Save();

         Assert.AreEqual("someone@example.com", _domain.Accounts[0].ForwardAddress);
         Assert.AreEqual("someone@external.com", _domain.Accounts[1].ForwardAddress);
      }

      [Test]
      [Description("Issue 343, Changing domain name doesn't change distribution list addresses")]
      public void TestRenameDomainWithAliases()
      {
         Alias alias1 = _domain.Aliases.Add();
         alias1.Name = "alias1@test.com";
         alias1.Value = "alias2@test.com";
         alias1.Save();

         Alias alias2 = _domain.Aliases.Add();
         alias2.Name = "alias2@test.com";
         alias2.Value = "account@test.com";
         alias2.Save();

         Alias alias3 = _domain.Aliases.Add();
         alias3.Name = "alias3@test.com";
         alias3.Value = "external@external.com";
         alias3.Save();

         _domain.Name = "example.com";
         _domain.Save();

         Assert.AreEqual("alias1@example.com", _domain.Aliases[0].Name);
         Assert.AreEqual("alias2@example.com", _domain.Aliases[0].Value);

         Assert.AreEqual("alias2@example.com", _domain.Aliases[1].Name);
         Assert.AreEqual("account@example.com", _domain.Aliases[1].Value);

         Assert.AreEqual("alias3@example.com", _domain.Aliases[2].Name);
         Assert.AreEqual("external@external.com", _domain.Aliases[2].Value);
      }

      [Test]
      [Description("Issue 343, Changing domain name doesn't change distribution list addresses")]
      public void TestRenameDomainWithList()
      {
         DistributionList oList = _domain.DistributionLists.Add();
         oList.Address = "list@test.com";
         oList.Active = true;
         oList.Save();

         DistributionListRecipient oRecipient = oList.Recipients.Add();
         oRecipient.RecipientAddress = "recipient1@test.com";
         oRecipient.Save();

         oRecipient = oList.Recipients.Add();
         oRecipient.RecipientAddress = "recipient2@Test.com";
         oRecipient.Save();

         oRecipient = oList.Recipients.Add();
         oRecipient.RecipientAddress = "recipient3@otherdomain.com";
         oRecipient.Save();

         _domain.Name = "example.com";
         _domain.Save();

         DistributionList list = _domain.DistributionLists[0];
         Assert.AreEqual("list@example.com", list.Address);
         Assert.AreEqual("recipient1@example.com", list.Recipients[0].RecipientAddress);
         Assert.AreEqual("recipient2@example.com", list.Recipients[1].RecipientAddress);
         Assert.AreEqual("recipient3@otherdomain.com", list.Recipients[2].RecipientAddress);
      }

      [Test]
      public void TestRenameDomainWithMessages()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account1@test.com", "test");
         account.ForwardAddress = "someone@test.com";
         account.Save();

         string messageBody = Guid.NewGuid().ToString();
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Subj", messageBody);
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         _domain.Name = "example.com";
         _domain.Save();

         string messageText = Pop3ClientSimulator.AssertGetFirstMessageText("account1@example.com", "test");
         Assert.IsTrue(messageText.Contains(messageBody), messageText);
      }

      [Test]
      public void TestRoute()
      {
         Settings oSettings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         Route route = oSettings.Routes.Add();

         route.DomainName = "myroute.com";
         route.TargetSMTPHost = "somehost.com";
         route.TargetSMTPPort = 25;

         route.Save();

         if (route.ID == 0)
            throw new Exception("Route not saved properly");

         oSettings.Routes.DeleteByDBID(route.ID);
      }


      [Test]
      public void TestSSLCertificate()
      {
         Settings oSettings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         SSLCertificate sslcert = oSettings.SSLCertificates.Add();

         sslcert.CertificateFile = "somefile.dat";
         sslcert.PrivateKeyFile = "someprivatefile.dat";
         sslcert.Save();

         if (sslcert.ID == 0)
            throw new Exception("SSL certificate not saved properly");

         oSettings.SSLCertificates.DeleteByDBID(sslcert.ID);
      }

      [Test]
      public void TestSURBLServer()
      {
         Settings oSettings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         SURBLServer surblServer = oSettings.AntiSpam.SURBLServers.Add();

         surblServer.DNSHost = "somehost.com";
         surblServer.RejectMessage = "somerejectmessage";
         surblServer.Score = 5;

         surblServer.Save();

         if (surblServer.ID == 0)
            throw new Exception("SURBL server not saved properly");

         oSettings.AntiSpam.SURBLServers.DeleteByDBID(surblServer.ID);
      }

      [Test]
      public void TestWhiteListAddress()
      {
         Settings oSettings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         WhiteListAddress whiteAddress = oSettings.AntiSpam.WhiteListAddresses.Add();

         whiteAddress.Description = "My description of this entry";
         whiteAddress.EmailAddress = "myaddress@dummy-example.com";
         whiteAddress.Save();

         if (whiteAddress.ID == 0)
            throw new Exception("White list address not saved properly");

         oSettings.AntiSpam.WhiteListAddresses.DeleteByDBID(whiteAddress.ID);
      }
   }
}