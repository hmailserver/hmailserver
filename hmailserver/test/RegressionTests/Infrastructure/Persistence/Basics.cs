// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure.Persistence
{
   [TestFixture]
   public class Basics : TestFixtureBase
   {
      [Test]
      public void TestAccount()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "perste'ster@example.test", "test");
         if (account1.ID == 0)
            throw new Exception("Account not properly saved");

         _domain.Accounts.DeleteByDBID(account1.ID);
      }

      [Test]
      public void TestAlias()
      {
         var alias = SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "fr'om@example.test", "to@t'st.com");
         if (alias.ID == 0)
            throw new Exception("Account not properly saved");

         _domain.Aliases.DeleteByDBID(alias.ID);
      }

      [Test]
      public void TestBlockedAttachment()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var attachment = settings.AntiVirus.BlockedAttachments.Add();

         attachment.Description = "Some description";
         attachment.Wildcard = "*.some";
         attachment.Save();

         if (attachment.ID == 0)
            throw new Exception("Blocked attachment not properly saved");

         settings.AntiVirus.BlockedAttachments.DeleteByDBID(attachment.ID);
      }


      [Test]
      public void TestCaseInsensitivtyAccount()
      {
         var testAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "lowercase@example.test", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         var upperCase = testAccount.Address.ToUpper();
         smtpClientSimulator.Send("someone@dummy-example.com", upperCase, "test mail", "test body");

         Pop3ClientSimulator.AssertMessageCount("lowercase@example.test", "test", 1);
      }

      [Test]
      public void TestCaseInsensitivtyAlias()
      {
         var testAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "lowercase@example.test", "test");
         var testAlias = SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "sometext@example.test",
            "LowerCase@example.test");

         var smtpClientSimulator = new SmtpClientSimulator();
         var upperCase = testAlias.Name.ToUpper();
         smtpClientSimulator.Send("someone@dummy-example.com", upperCase, "test mail", "test body");

         Pop3ClientSimulator.AssertMessageCount("lowercase@example.test", "test", 1);
      }

      [Test]
      public void TestCaseInsensitivtyList()
      {
         var testAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "lowercase@example.test", "test");

         var recipients = new List<string>();
         recipients.Add(testAccount.Address);

         var list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "myList@example.test",
            recipients);

         var smtpClientSimulator = new SmtpClientSimulator();
         var upperCase = list.Address.ToUpper();
         smtpClientSimulator.Send("someone@dummy-example.com", upperCase, "test mail", "test body");

         Pop3ClientSimulator.AssertMessageCount("lowercase@example.test", "test", 1);
      }

      [Test]
      public void TestCaseInsensitivtyListRecipient()
      {
         var testAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "lowercase@example.test", "test");

         var recipients = new List<string>();
         recipients.Add(testAccount.Address);

         var list = SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "myList@example.test",
            recipients);

         var recipient = list.Recipients[0];
         recipient.RecipientAddress = testAccount.Address.ToUpper();
         recipient.Delete();
      }

      [Test]
      public void TestDNSBlackList()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var dnsBlackList = settings.AntiSpam.DNSBlackLists.Add();

         dnsBlackList.DNSHost = "somehost.com";
         dnsBlackList.RejectMessage = "somerejectmessage";
         dnsBlackList.Score = 5;

         dnsBlackList.Save();

         if (dnsBlackList.ID == 0)
            throw new Exception("DNS blacklist not saved properly");

         settings.AntiSpam.DNSBlackLists.DeleteByDBID(dnsBlackList.ID);
      }

      [Test]
      public void TestDistributionList()
      {
         var list = _domain.DistributionLists.Add();
         list.Address = "persis'tent-test-list@example.test";
         list.Active = true;
         list.Save();

         var recipient = list.Recipients.Add();
         recipient.RecipientAddress = "test@te'st.com";
         recipient.Save();

         recipient.RecipientAddress = "tes't2@example.test";
         recipient.Save();
         list.Delete();
      }

      [Test]
      public void TestDomain()
      {
         var domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;
         _domain = SingletonProvider<TestSetup>.Instance.AddDomain(domains, "test2.com");
         if (_domain.ID == 0)
            throw new Exception("Domain not properly saved");

         _domain.Delete();
      }

      [Test]
      public void TestDomainRenaming()
      {
         var domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;
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
         var domain = SingletonProvider<TestSetup>.Instance.AddDomain("example.com");

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(domain.Accounts, "test1@example.com",
            "secret");
         account.MaxSize = 1024 * 1024 * 2000;
         account.Save();

         var secondAccount = SingletonProvider<TestSetup>.Instance.AddAccount(domain.Accounts,
            "test2@example.com", "secret");
         secondAccount.MaxSize = 1024 * 1024 * 2000;
         secondAccount.Save();

         Assert.AreEqual(account.MaxSize + (long)secondAccount.MaxSize, domain.AllocatedSize);
      }

      [Test]
      public void TestFetchAccount()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "fatester@example.test", "test");

         var fetchAccount = account1.FetchAccounts.Add();
         fetchAccount.Name = "test";
         fetchAccount.Save();

         if (fetchAccount.ID == 0)
            throw new Exception("Fetch account could not be saved");

         account1.FetchAccounts.Delete(0);
      }

      [Test]
      public void TestGroup()
      {
         var groups = _application.Settings.Groups;
         var group = groups.Add();
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
         var groups = _application.Settings.Groups;

         var group = groups.Add();
         group.Name = "MyGroup";
         group.Save();

         if (group.ID == 0)
            throw new Exception("Group not properly saved");

         var member = group.Members.Add();
         member.AccountID = 1;
         member.Save();

         if (member.ID == 0)
            throw new Exception("Group member not properly saved");
      }

      [Test]
      public void TestIncomingRelays()
      {
         var incomingRelays = _application.Settings.IncomingRelays;
         Assert.AreEqual(0, incomingRelays.Count);

         var incomingRelay = incomingRelays.Add();
         incomingRelay.Name = "TestRelay";
         incomingRelay.LowerIP = "1.2.1.1";
         incomingRelay.UpperIP = "2.1.2.1";
         incomingRelay.Save();

         // Check that it was saved.
         Assert.AreNotEqual(0, incomingRelay.ID);

         // Confirm that settings were saved properly.
         incomingRelays.Refresh();
         var incomingRelay2 = incomingRelays.get_ItemByDBID(incomingRelay.ID);
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
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test message", "Test body");

         var folder = account.IMAPFolders.get_ItemByName("Inbox");
         CustomAsserts.AssertFolderMessageCount(folder, 1);
         var message = account.IMAPFolders.get_ItemByName("Inbox").Messages[0];

         // Move the message file to another folder.
         var domainPath = Path.Combine(_application.Settings.Directories.DataDirectory, _domain.Name);
         var accountPath = Path.Combine(domainPath, "test");
         var fileName = Path.Combine(accountPath, "randomMail.eml");
         File.Move(message.Filename, fileName);

         // Update the database with the full path.
         var sql = string.Format("update hm_messages set messagefilename = '{0}' where messageid = {1}",
            TestSetup.Escape(fileName), message.ID);
         SingletonProvider<TestSetup>.Instance.GetApp().Database.ExecuteSQL(sql);

         SingletonProvider<TestSetup>.Instance.GetApp().Settings.Cache.Clear();

         // Now try to change the name of the domain or account. Should fail.
         account.Address = "test2@example.test";
         var thrown = false;

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
         account.Address = "test@example.test";
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
         _domain.Name = "example.test";
         _domain.Save();

         _domain.Delete();
      }

      [Test]
      public void TestRenameAccountWithMessages()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account1@example.test", "test");

         var messageBody = Guid.NewGuid().ToString();
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Subj", messageBody);
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         account.Address = "account2@example.test";
         account.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText("account2@example.test", "test");
         Assert.IsTrue(messageText.Contains(messageBody), messageText);
      }

      [Test]
      [Description("Issue 343, Changing domain name doesn't change distribution list addresses")]
      public void TestRenameDomainWithAccountForward()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account1@example.test", "test");
         account1.ForwardAddress = "someone@example.test";
         account1.Save();

         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account2@example.test", "test");
         account2.ForwardAddress = "someone@external.com";
         account2.Save();

         _domain.Name = "example.foo";
         _domain.Save();

         Assert.AreEqual("someone@example.foo", _domain.Accounts[0].ForwardAddress);
         Assert.AreEqual("someone@external.com", _domain.Accounts[1].ForwardAddress);
      }

      [Test]
      [Description("Issue 343, Changing domain name doesn't change distribution list addresses")]
      public void TestRenameDomainWithAliases()
      {
         var alias1 = _domain.Aliases.Add();
         alias1.Name = "alias1@example.test";
         alias1.Value = "alias2@example.test";
         alias1.Save();

         var alias2 = _domain.Aliases.Add();
         alias2.Name = "alias2@example.test";
         alias2.Value = "account@example.test";
         alias2.Save();

         var alias3 = _domain.Aliases.Add();
         alias3.Name = "alias3@example.test";
         alias3.Value = "external@external.com";
         alias3.Save();

         _domain.Name = "new1.example.com";
         _domain.Save();

         Assert.AreEqual("alias1@new1.example.com", _domain.Aliases[0].Name);
         Assert.AreEqual("alias2@new1.example.com", _domain.Aliases[0].Value);

         Assert.AreEqual("alias2@new1.example.com", _domain.Aliases[1].Name);
         Assert.AreEqual("account@new1.example.com", _domain.Aliases[1].Value);

         Assert.AreEqual("alias3@new1.example.com", _domain.Aliases[2].Name);
         Assert.AreEqual("external@external.com", _domain.Aliases[2].Value);
      }

      [Test]
      [Description("Issue 343, Changing domain name doesn't change distribution list addresses")]
      public void TestRenameDomainWithList()
      {
         var list1 = _domain.DistributionLists.Add();
         list1.Address = "list@example.test";
         list1.Active = true;
         list1.Save();

         var recipient = list1.Recipients.Add();
         recipient.RecipientAddress = "recipient1@example.test";
         recipient.Save();

         recipient = list1.Recipients.Add();
         recipient.RecipientAddress = "recipient2@Example.test";
         recipient.Save();

         recipient = list1.Recipients.Add();
         recipient.RecipientAddress = "recipient3@otherdomain.example.test";
         recipient.Save();

         _domain.Name = "new2.example.com";
         _domain.Save();

         var list = _domain.DistributionLists[0];
         Assert.AreEqual("list@new2.example.com", list.Address);
         Assert.AreEqual("recipient1@new2.example.com", list.Recipients[0].RecipientAddress);
         Assert.AreEqual("recipient2@new2.example.com", list.Recipients[1].RecipientAddress);
         Assert.AreEqual("recipient3@otherdomain.example.test", list.Recipients[2].RecipientAddress);
      }

      [Test]
      public void TestRenameDomainWithMessages()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account1@example.test", "test");
         account.ForwardAddress = "someone@example.test";
         account.Save();

         var messageBody = Guid.NewGuid().ToString();
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Subj", messageBody);
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         _domain.Name = "new3.example.com";
         _domain.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText("account1@new3.example.com", "test");
         Assert.IsTrue(messageText.Contains(messageBody), messageText);
      }

      [Test]
      public void TestRoute()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var route = settings.Routes.Add();

         route.DomainName = "myroute.com";
         route.TargetSMTPHost = "somehost.com";
         route.TargetSMTPPort = 25;

         route.Save();

         if (route.ID == 0)
            throw new Exception("Route not saved properly");

         settings.Routes.DeleteByDBID(route.ID);
      }


      [Test]
      public void TestSSLCertificate()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var sslcert = settings.SSLCertificates.Add();

         sslcert.CertificateFile = "somefile.dat";
         sslcert.PrivateKeyFile = "someprivatefile.dat";
         sslcert.Save();

         if (sslcert.ID == 0)
            throw new Exception("SSL certificate not saved properly");

         settings.SSLCertificates.DeleteByDBID(sslcert.ID);
      }

      [Test]
      public void TestSURBLServer()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var surblServer = settings.AntiSpam.SURBLServers.Add();

         surblServer.DNSHost = "somehost.com";
         surblServer.RejectMessage = "somerejectmessage";
         surblServer.Score = 5;

         surblServer.Save();

         if (surblServer.ID == 0)
            throw new Exception("SURBL server not saved properly");

         settings.AntiSpam.SURBLServers.DeleteByDBID(surblServer.ID);
      }

      [Test]
      public void TestWhiteListAddress()
      {
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;
         var whiteAddress = settings.AntiSpam.WhiteListAddresses.Add();

         whiteAddress.Description = "My description of this entry";
         whiteAddress.EmailAddress = "myaddress@dummy-example.com";
         whiteAddress.Save();

         if (whiteAddress.ID == 0)
            throw new Exception("White list address not saved properly");

         settings.AntiSpam.WhiteListAddresses.DeleteByDBID(whiteAddress.ID);
      }
   }
}