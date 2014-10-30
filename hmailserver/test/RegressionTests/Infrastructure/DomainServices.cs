// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class DomainServices : TestFixtureBase
   {
      [Test]
      public void TestDomainAliases()
      {
         // Create a test account
         // Fetch the default domain
         DomainAlias oDomainAlias = _domain.DomainAliases.Add();
         oDomainAlias.AliasName = "alias.com";
         oDomainAlias.Save();

         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "domain-alias-test@test.com",
                                                                             "test");

         // Send 5 messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         for (int i = 0; i < 5; i++)
            smtpClientSimulator.Send("domain-alias-test@alias.com", "domain-alias-test@alias.com", "INBOX", "Alias test message");

         Pop3ClientSimulator.AssertMessageCount("domain-alias-test@alias.com", "test", 5);

         {
            oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                        "domain-alias-test-account@test.com", "test");

            // Set up an alias pointing at the domain alias.
            SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "datestalias@test.com",
                                                           "domain-alias-test-account@alias.com");

            // Send to the alias
            for (int i = 0; i < 5; i++)
               smtpClientSimulator.Send(oAccount.Address, "datestalias@test.com", "INBOX", "Plus addressing message");
            // Wait for completion

            Pop3ClientSimulator.AssertMessageCount(oAccount.Address, "test", 5);
         }
      }

      [Test]
      [Description("Test of virtual domains - test sending to four accounts on two different domains.")]
      public void TestMultipleDomains()
      {
         Domains domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test1@test.com", "test");
         Account account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test2@test.com", "test");

         Domain domain2 = SingletonProvider<TestSetup>.Instance.AddDomain(domains, "test2.com");

         Account account3 = SingletonProvider<TestSetup>.Instance.AddAccount(domain2, "test1@test2.com", "test");
         Account account4 = SingletonProvider<TestSetup>.Instance.AddAccount(domain2, "test2@test2.com", "test");

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.Send("test@alias.com", account1.Address, "Test", "test1@test.com");
         smtpSimulator.Send("test@alias.com", account2.Address, "Test", "test2@test.com");
         smtpSimulator.Send("test@alias.com", account3.Address, "Test", "test1@test1.com");
         smtpSimulator.Send("test@alias.com", account4.Address, "Test", "test2@test1.com");

         Assert.IsTrue(Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test").Contains(account1.Address));
         Assert.IsTrue(Pop3ClientSimulator.AssertGetFirstMessageText(account2.Address, "test").Contains(account2.Address));
         Assert.IsTrue(Pop3ClientSimulator.AssertGetFirstMessageText(account3.Address, "test").Contains(account3.Address));
         Assert.IsTrue(Pop3ClientSimulator.AssertGetFirstMessageText(account4.Address, "test").Contains(account4.Address));
      }

      [Test]
      public void TestPlusAddressing()
      {
         // Create a test account
         // Fetch the default domain
         // Enable plus adressing
         _domain.PlusAddressingEnabled = true;
         _domain.PlusAddressingCharacter = "+";

         // Save the change
         _domain.Save();

         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "plustest@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "plustest2@test.com", "test");

         // Send 5 messages to this account, without using plus addressing.
         var smtpClientSimulator = new SmtpClientSimulator();
         for (int i = 0; i < 5; i++)
            smtpClientSimulator.Send("plustest@test.com", "plustest2@test.com", "INBOX", "Plus addressing message");
         // Wait for completion

         // Check using POP3 that 5 messages exists.
         Pop3ClientSimulator.AssertMessageCount("plustest2@test.com", "test", 5);

         // Send using plus addressing
         for (int i = 0; i < 5; i++)
            smtpClientSimulator.Send("plustest@test.com", "plustest2+hejsan@test.com", "INBOX", "Plus addressing message");
         // Wait for completion

         Pop3ClientSimulator.AssertMessageCount("plustest2@test.com", "test", 10);

         {
            Account oAccount3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "plustest3@test.com",
                                                                                 "test");

            SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "plusalias@test.com",
                                                           "plustest3+hejsan@test.com");

            // Send to the alias
            for (int i = 0; i < 5; i++)
               smtpClientSimulator.Send("plustest@test.com", "plusalias@test.com", "INBOX", "Plus addressing message");
            // Wait for completion

            Pop3ClientSimulator.AssertMessageCount("plustest3@test.com", "test", 5);
         }

         _domain.PlusAddressingEnabled = false;
      }
   }
}