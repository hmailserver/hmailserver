// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections.Generic;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.AntiSpam
{
   [TestFixture]
   public class GreyListing : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         _antiSpam = _settings.AntiSpam;
      }

      #endregion

      private hMailServer.AntiSpam _antiSpam;

      [Test]
      [Description("Test that grey listing can be enabled if message arrives from A or MX record.")]
      public void ItShouldBePossibleToBypassGreylistingOnMessagesArrivingFromMXorA()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "grey@test.com", "test");

         _antiSpam.GreyListingEnabled = true;

         Assert.IsFalse(SMTPClientSimulator.StaticSend("test@localhost.hmailserver.com", oAccount1.Address, "Test",
                                                       "Body"));

         _antiSpam.BypassGreylistingOnMailFromMX = true;

         Assert.IsTrue(SMTPClientSimulator.StaticSend("test@localhost.hmailserver.com", oAccount1.Address, "Test",
                                                      "Body"));

         POP3Simulator.AssertGetFirstMessageText(oAccount1.Address, "test");
      }

      [Test]
      public void TestGreyListing()
      {
         _antiSpam.GreyListingEnabled = false;

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "grey@test.com", "test");

         var smtp = new SMTPClientSimulator();
         var recipients = new List<string>();
         recipients.Add(oAccount1.Address);
         bool result = smtp.Send("test@test.com", recipients, "Test", "Body");
         Assert.IsTrue(result);
         POP3Simulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         _antiSpam.GreyListingEnabled = true;

         result = smtp.Send("test@test.com", recipients, "Test", "Body");
         Assert.IsFalse(result);

         _antiSpam.GreyListingEnabled = false;

         result = smtp.Send("test@test.com", recipients, "Test", "Body");
         Assert.IsTrue(result);
         POP3Simulator.AssertGetFirstMessageText(oAccount1.Address, "test");
      }

      [Test]
      public void TestGreyListingWhiteListWildcard()
      {
         _antiSpam.GreyListingEnabled = true;

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "grey@test.com", "test");

         GreyListingWhiteAddresses whiteAddresses = _antiSpam.GreyListingWhiteAddresses;
         GreyListingWhiteAddress whiteAddress = whiteAddresses.Add();
         whiteAddress.IPAddress = "127.0.0.5";
         whiteAddress.Save();

         Assert.IsFalse(SMTPClientSimulator.StaticSend("external@example.com", account.Address, "Test", "Test"));

         whiteAddress.IPAddress = "*";
         whiteAddress.Save();

         Assert.IsTrue(SMTPClientSimulator.StaticSend("external@example.com", account.Address, "Test", "Test"));

         POP3Simulator.AssertGetFirstMessageText(account.Address, "test");
      }

      [Test]
      public void TestGreyListingWithDomainAliases()
      {
         _antiSpam.GreyListingEnabled = false;

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "grey@test.com", "test");

         var smtp = new SMTPClientSimulator();
         var recipients = new List<string>();
         recipients.Add(oAccount1.Address);
         bool result = smtp.Send("test@test.com", recipients, "Test", "Body");
         Assert.IsTrue(result);
         POP3Simulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         _antiSpam.GreyListingEnabled = true;

         result = smtp.Send("test@test.com", recipients, "Test", "Body");
         Assert.IsFalse(result);

         _domain.AntiSpamEnableGreylisting = false;
         _domain.Save();

         result = smtp.Send("test@test.com", recipients, "Test", "Body");
         Assert.IsTrue(result);
         POP3Simulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         DomainAlias da = _domain.DomainAliases.Add();
         da.AliasName = "test2.com";
         da.Save();

         recipients = new List<string>();
         recipients.Add("grey@test2.com");

         result = smtp.Send("test@test.com", recipients, "Test", "Body");
         Assert.IsTrue(result);
         POP3Simulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         _domain.AntiSpamEnableGreylisting = true;
         _domain.Save();

         result = smtp.Send("test@test.com", recipients, "Test", "Body");
         Assert.IsFalse(result);
      }
   }
}