// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using NUnit.Framework;
using RegressionTests.Infrastructure;
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

         CustomAsserts.Throws<DeliveryFailedException>(
            () => SmtpClientSimulator.StaticSend("test@localhost.hmailserver.com", oAccount1.Address, "Test",
               "Body"));

         _antiSpam.BypassGreylistingOnMailFromMX = true;

         SmtpClientSimulator.StaticSend("test@localhost.hmailserver.com", oAccount1.Address, "Test",
                                                      "Body");

         Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
      }

      [Test]
      public void TestGreyListing()
      {
         _antiSpam.GreyListingEnabled = false;

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "grey@test.com", "test");

         var smtp = new SmtpClientSimulator();
         var recipients = new List<string>();
         recipients.Add(oAccount1.Address);
         smtp.Send("test@test.com", recipients, "Test", "Body");
         
         Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         _antiSpam.GreyListingEnabled = true;

         CustomAsserts.Throws<DeliveryFailedException>(() => smtp.Send("test@test.com", recipients, "Test", "Body"));
         

         _antiSpam.GreyListingEnabled = false;

         smtp.Send("test@test.com", recipients, "Test", "Body");
         
         Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
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

         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSend("external@example.com", account.Address, "Test", "Test"));

         whiteAddress.IPAddress = "*";
         whiteAddress.Save();

         SmtpClientSimulator.StaticSend("external@example.com", account.Address, "Test", "Test");

         Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
      }

      [Test]
      public void TestGreyListingWithDomainAliases()
      {
         _antiSpam.GreyListingEnabled = false;

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "grey@test.com", "test");

         var smtp = new SmtpClientSimulator();
         var recipients = new List<string>();
         recipients.Add(oAccount1.Address);
         smtp.Send("test@test.com", recipients, "Test", "Body");
         
         Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         _antiSpam.GreyListingEnabled = true;

         CustomAsserts.Throws<DeliveryFailedException>(() => smtp.Send("test@test.com", recipients, "Test", "Body"));


         _domain.AntiSpamEnableGreylisting = false;
         _domain.Save();

         smtp.Send("test@test.com", recipients, "Test", "Body");

         Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         DomainAlias da = _domain.DomainAliases.Add();
         da.AliasName = "test2.com";
         da.Save();

         recipients = new List<string>();
         recipients.Add("grey@test2.com");

         smtp.Send("test@test.com", recipients, "Test", "Body");
         Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         _domain.AntiSpamEnableGreylisting = true;
         _domain.Save();

         CustomAsserts.Throws<DeliveryFailedException>(() => smtp.Send("test@test.com", recipients, "Test", "Body"));
      }
   }
}