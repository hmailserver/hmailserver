// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Shared;

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
         var oDomainAlias = _domain.DomainAliases.Add();
         oDomainAlias.AliasName = "alias.com";
         oDomainAlias.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "domain-alias-test@example.test",
            "test");

         // Send 5 messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         for (var i = 0; i < 5; i++)
            smtpClientSimulator.Send("domain-alias-test@alias.com", "domain-alias-test@alias.com", "INBOX",
               "Alias test message");

         Pop3ClientSimulator.AssertMessageCount("domain-alias-test@alias.com", "test", 5);

         {
            account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
               "domain-alias-test-account@example.test", "test");

            // Set up an alias pointing at the domain alias.
            SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "datestalias@example.test",
               "domain-alias-test-account@alias.com");

            // Send to the alias
            for (var i = 0; i < 5; i++)
               smtpClientSimulator.Send(account.Address, "datestalias@example.test", "INBOX",
                  "Plus addressing message");
            // Wait for completion

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 5);
         }
      }

      [Test]
      [Description("Test of virtual domains - test sending to four accounts on two different domains.")]
      public void TestMultipleDomains()
      {
         var domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test1@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test2@example.test", "test");

         var domain2 = SingletonProvider<TestSetup>.Instance.AddDomain(domains, "test2.com");

         var account3 = SingletonProvider<TestSetup>.Instance.AddAccount(domain2, "test1@test2.com", "test");
         var account4 = SingletonProvider<TestSetup>.Instance.AddAccount(domain2, "test2@test2.com", "test");

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.Send("test@alias.com", account1.Address, "Test", "test1@example.test");
         smtpSimulator.Send("test@alias.com", account2.Address, "Test", "test2@example.test");
         smtpSimulator.Send("test@alias.com", account3.Address, "Test", "test1@test1.com");
         smtpSimulator.Send("test@alias.com", account4.Address, "Test", "test2@test1.com");

         Assert.IsTrue(Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test")
            .Contains(account1.Address));
         Assert.IsTrue(Pop3ClientSimulator.AssertGetFirstMessageText(account2.Address, "test")
            .Contains(account2.Address));
         Assert.IsTrue(Pop3ClientSimulator.AssertGetFirstMessageText(account3.Address, "test")
            .Contains(account3.Address));
         Assert.IsTrue(Pop3ClientSimulator.AssertGetFirstMessageText(account4.Address, "test")
            .Contains(account4.Address));
      }

      [Test]
      public void TestPlusAddressing()
      {
         // Create a test account
         // Fetch the default domain
         // Enable plus addressing
         _domain.PlusAddressingEnabled = true;
         _domain.PlusAddressingCharacter = "+";

         // Save the change
         _domain.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "plustest@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "plustest2@example.test", "test");

         // Send 5 messages to this account, without using plus addressing.
         var smtpClientSimulator = new SmtpClientSimulator();
         for (var i = 0; i < 5; i++)
            smtpClientSimulator.Send("plustest@example.test", "plustest2@example.test", "INBOX",
               "Plus addressing message");
         // Wait for completion

         // Check using POP3 that 5 messages exists.
         Pop3ClientSimulator.AssertMessageCount("plustest2@example.test", "test", 5);

         // Send using plus addressing
         for (var i = 0; i < 5; i++)
            smtpClientSimulator.Send("plustest@example.test", "plustest2+hejsan@example.test", "INBOX",
               "Plus addressing message");
         // Wait for completion

         Pop3ClientSimulator.AssertMessageCount("plustest2@example.test", "test", 10);

         {
            var account3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "plustest3@example.test",
               "test");

            SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "plusalias@example.test",
               "plustest3+hejsan@example.test");

            // Send to the alias
            for (var i = 0; i < 5; i++)
               smtpClientSimulator.Send("plustest@example.test", "plusalias@example.test", "INBOX",
                  "Plus addressing message");
            // Wait for completion

            Pop3ClientSimulator.AssertMessageCount("plustest3@example.test", "test", 5);
         }

         _domain.PlusAddressingEnabled = false;
      }
   }
}