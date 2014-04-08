// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class AccountServices : TestFixtureBase
   {
      [Test]
      [Category("Accounts")]
      [Description("Ensure that only a single return-path setting exists after forwarding has been done")]
      public void ConfirmSingleReturnPathAfterAccountForward()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account1@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account2@test.com", "test");

         oAccount1.ForwardAddress = oAccount2.Address;
         oAccount1.ForwardEnabled = true;
         oAccount1.Save();

         // Send a message...
         var oSMTP = new SMTPClientSimulator();
         oSMTP.Send("original-address@test.com", oAccount1.Address, "Test message", "This is the body");

         TestSetup.AssertRecipientsInDeliveryQueue(0);
         _application.SubmitEMail();

         // Wait for the auto-reply.
         string text = POP3Simulator.AssertGetFirstMessageText(oAccount2.Address, "test");

         Assert.IsFalse(text.Contains("Return-Path: account2@test.com"));
         Assert.IsFalse(text.Contains("Return-Path: account1@test.com"));
         Assert.IsTrue(text.Contains("Return-Path: original-address@test.com"));
      }

      [Test]
      [Category("Accounts")]
      [Description("Ensure that messges aren't forwarded if they re deleted using a rule.")]
      public void ConfirmSingleReturnPathAfterRuleForward()
      {
         // Create a test account
         // Fetch the default _domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account-a@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account-b@test.com", "test");

         // Set up a rule to trash the message.
         Rule oRule = oAccount1.Rules.Add();
         oRule.Name = "Criteria test";
         oRule.Active = true;

         RuleCriteria oRuleCriteria = oRule.Criterias.Add();
         oRuleCriteria.UsePredefined = true;
         oRuleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         oRuleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         oRuleCriteria.MatchValue = "0";
         oRuleCriteria.Save();

         // Add action
         RuleAction oRuleAction = oRule.Actions.Add();
         oRuleAction.Type = eRuleActionType.eRAForwardEmail;
         oRuleAction.To = oAccount2.Address;
         oRuleAction.Save();

         // Save the rule in the database
         oRule.Save();

         // Make sure that that a forward is made if no rule is set up.
         SMTPClientSimulator.StaticSend("external@test.com", oAccount1.Address, "Test message", "This is the body");
         POP3Simulator.AssertMessageCount(oAccount1.Address, "test", 1);
         _application.SubmitEMail();

         // Wait for the auto-reply.
         string text = POP3Simulator.AssertGetFirstMessageText(oAccount2.Address, "test");

         Assert.IsTrue(text.Contains("Return-Path: account-a@test.com"));
         Assert.IsFalse(text.Contains("Return-Path: account2@test.com"));
         Assert.IsFalse(text.Contains("Return-Path: external@test.com"));
      }

      [Test]
      [Category("Accounts")]
      [Description("Test usage of accounts containing single quote.")]
      public void TestAddressContainingSingleQuote()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "Addr'ess1@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "Addr'ess2@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "alias2'quoted@test.com", "Addr'ess2@test.com");

         // Send 5 messages to this account.
         var oSMTP = new SMTPClientSimulator();
         for (int i = 0; i < 5; i++)
            oSMTP.Send(oAccount1.Address, "alias2'quoted@test.com", "INBOX", "Quoted message test message");

         POP3Simulator.AssertMessageCount(oAccount2.Address, "test", 5);
      }

      [Test]
      [Category("Accounts")]
      [Description("Ensure that auto-replies can be sent.")]
      public void TestAutoReply()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");

         oAccount2.VacationMessageIsOn = true;
         oAccount2.VacationMessage = "I'm on vacation";
         oAccount2.VacationSubject = "Out of office!";
         oAccount2.Save();

         // Send 2 messages to this account.
         var oSMTP = new SMTPClientSimulator();
         oSMTP.Send(oAccount1.Address, oAccount2.Address, "Test message", "This is the body");

         var oPOP3 = new POP3Simulator();
         POP3Simulator.AssertMessageCount(oAccount1.Address, "test", 1);
         POP3Simulator.AssertMessageCount(oAccount2.Address, "test", 1);
         string s = oPOP3.GetFirstMessageText(oAccount1.Address, "test");
         if (s.IndexOf("Out of office!") < 0)
            throw new Exception("ERROR - Auto reply subject not set properly.");

         oAccount2.VacationMessageIsOn = false;
         oAccount2.Save();

         oAccount2.VacationSubject = "";
         oAccount2.VacationMessageIsOn = true;
         oAccount2.Save();

         // Send another
         oSMTP.Send(oAccount1.Address, oAccount2.Address, "Test message", "This is the body");

         POP3Simulator.AssertMessageCount(oAccount2.Address, "test", 2);
         POP3Simulator.AssertMessageCount(oAccount1.Address, "test", 1);

         s = oPOP3.GetFirstMessageText(oAccount1.Address, "test");
         if (s.ToLower().IndexOf("re: test message") < 0)
            throw new Exception("ERROR - Auto reply subject not set properly.");

         Assert.IsTrue(s.Contains("Auto-Submitted: auto-replied"));

         oAccount2.VacationMessageIsOn = false;
         oAccount2.Save();
      }

      [Test]
      [Category("Accounts")]
      [Description("Ensure that auto-replies are sent even if account forwarding is on.")]
      public void TestAutoReplyCombinedWithForwarding()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");
         Account oAccount3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");

         oAccount2.VacationMessageIsOn = true;
         oAccount2.VacationMessage = "I'm on vacation";
         oAccount2.VacationSubject = "Out of office!";

         oAccount2.ForwardAddress = oAccount3.Address;
         oAccount2.ForwardEnabled = true;
         oAccount2.ForwardKeepOriginal = true;
         oAccount2.Save();

         // Send a message...
         var oSMTP = new SMTPClientSimulator();
         oSMTP.Send(oAccount1.Address, oAccount2.Address, "Test message", "This is the body");

         SingletonProvider<TestSetup>.Instance.GetApp().SubmitEMail();
         TestSetup.AssertRecipientsInDeliveryQueue(0);

         // Wait for the auto-reply.
         POP3Simulator.AssertMessageCount(oAccount1.Address, "test", 1);
         POP3Simulator.AssertMessageCount(oAccount2.Address, "test", 1);
         POP3Simulator.AssertMessageCount(oAccount3.Address, "test", 1);
      }

      [Test]
      [Category("Accounts")]
      [Description("Ensure that the %Subject% macro in auto-replies can be used in subject.")]
      public void TestAutoReplySubject()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");

         oAccount2.VacationMessageIsOn = true;
         oAccount2.VacationMessage = "I'm on vacation";
         oAccount2.VacationSubject = "Auto-Reply: %SUBJECT%";
         oAccount2.Save();

         // Send 1 message to this account
         var oSMTP = new SMTPClientSimulator();
         oSMTP.Send(oAccount1.Address, oAccount2.Address, "Test message", "This is the body");

         // Wait a second to be sure that the message
         // are delivered.

         // Check using POP3 that 2 messages exists.
         var oPOP3 = new POP3Simulator();

         POP3Simulator.AssertMessageCount(oAccount1.Address, "test", 1);
         string s = oPOP3.GetFirstMessageText(oAccount1.Address, "test");
         if (s.IndexOf("Subject: Auto-Reply: Test message") < 0)
            throw new Exception("ERROR - Auto reply subject not set properly.");
      }

      [Test]
      [Category("Accounts")]
      [Description("Ensure that the %Subject% macro in auto-replies can be used in body.")]
      public void TestAutoReplySubjectInBody()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");

         oAccount2.VacationMessageIsOn = true;
         oAccount2.VacationMessage = "Your message regarding -%SUBJECT%- was not received.";
         oAccount2.VacationSubject = "Auto-Reply: Out of office";
         oAccount2.Save();

         // Send 1 message to this account
         var oSMTP = new SMTPClientSimulator();
         oSMTP.Send(oAccount1.Address, oAccount2.Address, "Test message", "This is the body");

         string s = POP3Simulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (s.IndexOf("Your message regarding -Test message- was not received.") < 0)
            throw new Exception("ERROR - Auto reply subject not set properly.");
      }


      [Test]
      [Category("Accounts")]
      [Description("Test account forwarding")]
      public void TestForwarding()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "Forward1@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "Forward2@test.com", "test");

         // Set up account 1 to forward to account2.
         oAccount1.ForwardEnabled = true;
         oAccount1.ForwardAddress = "Forward2@test.com";
         oAccount1.ForwardKeepOriginal = true;
         oAccount1.Save();

         // Send 2 messages to this account.
         var oSMTP = new SMTPClientSimulator();
         for (int i = 0; i < 2; i++)
            oSMTP.Send("Forward1@test.com", "Forward1@test.com", "INBOX", "POP3 test message");

         POP3Simulator.AssertMessageCount(oAccount1.Address, "test", 2);

         // Tell hMailServer to deliver now, so that the forward takes effect.
         SingletonProvider<TestSetup>.Instance.GetApp().SubmitEMail();

         POP3Simulator.AssertMessageCount(oAccount2.Address, "test", 2);
      }

      [Test]
      [Category("Accounts")]
      [Description("Test that message file is deleted if a message is forwarding and original not kept")]
      public void TestForwardingAndDelete()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "Forward1@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "Forward2@test.com", "test");

         // Set up account 1 to forward to account2.
         oAccount1.ForwardEnabled = true;
         oAccount1.ForwardAddress = "Forward2@test.com";
         oAccount1.ForwardKeepOriginal = false;
         oAccount1.Save();

         // Send 2 messages to this account.
         var oSMTP = new SMTPClientSimulator();
         oSMTP.Send("Forward1@test.com", "Forward1@test.com", "INBOX", "POP3 test message");
         TestSetup.AssertRecipientsInDeliveryQueue(0);
         POP3Simulator.AssertMessageCount(oAccount2.Address, "test", 1);

         string domainDir = Path.Combine(_settings.Directories.DataDirectory, "test.com");
         string userDir = Path.Combine(domainDir, "Forward1");

         string[] dirs = Directory.GetDirectories(userDir);
         foreach (string dir in dirs)
         {
            string[] files = Directory.GetFiles(dir);

            Assert.AreEqual(0, files.Length);
         }
      }

      [Test]
      [Category("Accounts")]
      [Description("Ensure that messges aren't forwarded if they re deleted using a rule.")]
      public void TestForwardingCombinedWithAccountRule()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");
         Account oAccount3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              TestSetup.RandomString() + "@test.com",
                                                                              "test");

         oAccount2.ForwardAddress = oAccount3.Address;
         oAccount2.ForwardEnabled = true;
         oAccount2.ForwardKeepOriginal = true;
         oAccount2.Save();

         var oSMTP = new SMTPClientSimulator();
         Assert.IsTrue(oSMTP.Send(oAccount1.Address, oAccount2.Address, "Test message", "This is the body"));

         // Make sure that that a forward is made if no rule is set up.
         POP3Simulator.AssertMessageCount(oAccount2.Address, "test", 1);
         _application.SubmitEMail();
         POP3Simulator.AssertMessageCount(oAccount3.Address, "test", 1);

         // Start over again.
         oAccount2.DeleteMessages();
         oAccount3.DeleteMessages();

         // Set up a rule to trash the message.
         Rule oRule = oAccount2.Rules.Add();
         oRule.Name = "Criteria test";
         oRule.Active = true;

         RuleCriteria oRuleCriteria = oRule.Criterias.Add();
         oRuleCriteria.UsePredefined = true;
         oRuleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         oRuleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         oRuleCriteria.MatchValue = "0";
         oRuleCriteria.Save();

         // Add action
         RuleAction oRuleAction = oRule.Actions.Add();
         oRuleAction.Type = eRuleActionType.eRADeleteEmail;
         oRuleAction.Save();

         // Save the rule in the database
         oRule.Save();

         // Make sure that that a forward is made if no rule is set up.
         Assert.IsTrue(oSMTP.Send(oAccount1.Address, oAccount2.Address, "Test message", "This is the body"));
         POP3Simulator.AssertMessageCount(oAccount2.Address, "test", 0);
         _application.SubmitEMail();
         POP3Simulator.AssertMessageCount(oAccount3.Address, "test", 0);
      }

      [Test]
      [Category("Accounts")]
      [Description("Test usage of very long email addresses")]
      public void TestLongEmailAddresses()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              "Account1123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890@test.com",
                                                                              "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
                                                                              "Account2123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890@test.com",
                                                                              "test");

         // Send 5 messages to this account.
         var oSMTP = new SMTPClientSimulator();
         for (int i = 0; i < 5; i++)
            oSMTP.Send(oAccount1.Address, oAccount2.Address, "INBOX", "POP3 test message");

         POP3Simulator.AssertMessageCount(oAccount2.Address, "test", 5);
      }


      [Test]
      [Category("Accounts")]
      [Description("Test cache refresh when renaming account.")]
      public void TestRefreshOfCache()
      {
         // Create a test account
         // Fetch the default domain
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "Addr'ess1@test.com", "test");
         Account oAccount2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "Addr'ess2@test.com", "test");
         SingletonProvider<TestSetup>.Instance.AddAlias(_domain, "alias2'quoted@test.com", "Addr'ess2@test.com");

         // Send 5 messages to this account.
         var oSMTP = new SMTPClientSimulator();
         for (int i = 0; i < 5; i++)
            oSMTP.Send(oAccount1.Address, "alias2'quoted@test.com", "INBOX", "Quoted message test message");

         POP3Simulator.AssertMessageCount(oAccount2.Address, "test", 5);
      }
   }
}