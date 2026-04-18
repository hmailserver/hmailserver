// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text.RegularExpressions;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.Rules
{
   [TestFixture]
   public class Rules : TestFixtureBase
   {
      private void CreatePrintRecipientCountRule(hMailServer.Rules rules)
      {
         var rule = rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRARunScriptFunction;
         ruleAction.ScriptFunction = "PrintRecipientCount";
         ruleAction.Save();

         rule.Save();

         File.WriteAllText(_settings.Scripting.CurrentScriptFile,
            "Sub PrintRecipientCount(message)" + Environment.NewLine +
            " Call EventLog.Write(message.Recipients.Count)" + Environment.NewLine +
            "End Sub");

         _settings.Scripting.Reload();
      }

      [Test]
      public void ActionAccountRuleMoveToExistingPublicFolder()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();

         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");
         var publicFolders = _settings.PublicFolders;
         var folder = publicFolders.Add("Share1");
         folder.Save();

         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = account1.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionExpunge, true);
         permission.Save();

         var rule = account1.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "#public.Share1";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeString",
            "Detta ska hamna i public folder.");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "#public.Share1", 1);
      }

      [Test]
      public void ActionAccountRuleMoveToExistingPublicFolderSubStructureWithCreatePermission()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();

         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "account1@example.test", "test");
         var publicFolders = _settings.PublicFolders;
         var folder = publicFolders.Add("Share1");
         folder.Save();

         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = account1.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionCreate, true);
         permission.set_Permission(eACLPermission.ePermissionLookup, true);
         permission.Save();

         var rule = account1.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "#public.Share1.Sub";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "account1@example.test", "SomeString",
            "This should end up in the #public.share1.sub since user lacks right.");

         ImapClientSimulator.AssertMessageCount("account1@example.test", "test", "#public.Share1.Sub", 1);
      }

      [Test]
      public void ActionAccountRuleMoveToNonExistingPublicFolder()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();

         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");
         var publicFolders = _settings.PublicFolders;

         var rule = account1.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "#public.Share1";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeString",
            "This should end up in the inbox since user lacks right.");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "INBOX", 1);
      }

      [Test]
      [Description("Tessts the bind to address rule action.")]
      public void ActionBindToAddress()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = SingletonProvider<TestSetup>.Instance.GetApp().Rules.Add();
         rule.Name = "Global rule test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTEquals;
         ruleCriteria.MatchValue = "SomeString";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRABindToAddress;
         ruleAction.Value = "255.254.253.252";
         ruleAction.Save();
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "knafve@gmail.com", "SomeString",
            "This mail should not be delivered - Test ActionBindToAddress.");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         var errorLog = LogHandler.ReadAndDeleteErrorLog();

         Assert.IsTrue(errorLog.Contains("Failed to bind to IP address 255.254.253.252."));
      }

      [Test]
      [Description("Test to delete using a rule. Confirm that the account folder does not contain the files")]
      public void ActionDelete()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTContains;
         ruleCriteria.MatchValue = "TestString";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRADeleteEmail;
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "TestString", "Test 1");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "a", "Test 2");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "TestString", "Test 3");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "b", "Test 2");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 2);

         var fileCount = 0;
         var domainDir = Path.Combine(_application.Settings.Directories.DataDirectory, "example.test");
         var userDir = Path.Combine(domainDir, "ruletest");

         RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
         {
            var dirs = Directory.GetDirectories(userDir);
            foreach (var dir in dirs)
            {
               var files = Directory.GetFiles(dir);
               fileCount += files.Length;
            }

            RetryableAssert.AreEqual(2, fileCount);
         });

         RetryHelper.TryAction(TimeSpan.FromSeconds(10), delegate
         {
            var logContent = LogHandler.ReadCurrentDefaultLog();
            var loggedDeletionCount =
               new Regex(Regex.Escape("Delivery to this account was canceled by an account rule")).Matches(logContent)
                  .Count;

            RetryableAssert.AreEqual(2, loggedDeletionCount);
         });
      }

      [Test]
      public void ActionGlobalMoveToIMAPFolder()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
            "ActionGlobalMoveToIMAPFolder@example.test",
            "test");

         var rule = SingletonProvider<TestSetup>.Instance.GetApp().Rules.Add();
         rule.Name = "Global rule test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTNotEquals;
         ruleCriteria.MatchValue = "SomeString";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.GlobalBox";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ActionGlobalMoveToIMAPFolder@example.test",
            "ActionGlobalMoveToIMAPFolder@example.test", "SomeString",
            "Detta ska inte hamna i mappen Inbox\\NotEquals");
         smtpClientSimulator.Send("ActionGlobalMoveToIMAPFolder@example.test",
            "ActionGlobalMoveToIMAPFolder@example.test", "SomeStringA",
            "Detta ska hamna i mappen Inbox\\NotEquals");
         smtpClientSimulator.Send("ActionGlobalMoveToIMAPFolder@example.test",
            "ActionGlobalMoveToIMAPFolder@example.test", "somestring",
            "Detta ska inte hamna i mappen Inbox\\NotEquals");

         ImapClientSimulator.AssertMessageCount("ActionGlobalMoveToIMAPFolder@example.test", "test", "Inbox.GlobalBox",
            1);
         ImapClientSimulator.AssertMessageCount("ActionGlobalMoveToIMAPFolder@example.test", "test", "Inbox", 2);
      }

      [Test]
      public void ActionGlobalRuleMoveToIMAPFolderPublicFolder()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();

         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");
         var publicFolders = _settings.PublicFolders;
         var folder = publicFolders.Add("Share1");
         folder.Save();

         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = account1.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionExpunge, true);
         permission.Save();

         var rule = application.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "#public.Share1";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeString",
            "Detta ska hamna i public folder.");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "#public.Share1", 1);
      }

      [Test]
      public void ActionGlobalRuleMoveToIMAPFolderPublicFolderNonExistant()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = application.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "#public.MyFolder";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeString",
            "Detta ska hamna i public folder.");

         // Wait for the folder to be created.
         var folder = CustomAsserts.AssertFolderExists(_settings.PublicFolders, "MyFolder");

         // Wait for the message to appear.
         CustomAsserts.AssertFolderMessageCount(folder, 1);

         // Make sure we can't access it.
         var imap = new ImapClientSimulator();
         Assert.IsTrue(imap.ConnectAndLogon("ruletest@example.test", "test"));

         Assert.Throws<ArgumentException>(() => imap.GetMessageCount("#public.MyFolder"));

         // Set permissions on this folder.
         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = account1.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionExpunge, true);
         permission.Save();

         // Make sure we can access it now.
         imap.GetMessageCount("#public.MyFolder");
      }

      [Test]
      public void ActionMoveToIMAPFolder()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTNotEquals;
         ruleCriteria.MatchValue = "SomeString";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.NotEquals";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeString",
            "Detta ska inte hamna i mappen Inbox\\NotEquals");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeStringA",
            "Detta ska hamna i mappen Inbox\\NotEquals");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "somestring",
            "Detta ska inte hamna i mappen Inbox\\NotEquals");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.NotEquals", 1);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 2);
      }

      [Test]
      public void ActionOverrideMoveToIMAPFolder()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = SingletonProvider<TestSetup>.Instance.GetApp().Rules.Add();
         rule.Name = "Global rule test";
         rule.Active = true;
         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTNotEquals;
         ruleCriteria.MatchValue = "SomeString";
         ruleCriteria.Save();
         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.GlobalBox";
         ruleAction.Save();
         rule.Save();

         // Account-level rule
         var accountRule = account.Rules.Add();
         accountRule.Name = "Criteria test";
         accountRule.Active = true;

         ruleCriteria = accountRule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTNotEquals;
         ruleCriteria.MatchValue = "SomeString";
         ruleCriteria.Save();

         // Add action
         ruleAction = accountRule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.Overriden.Test";
         ruleAction.Save();

         // Save the rule in the database
         accountRule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeString",
            "Detta ska inte hamna i mappen Inbox.Overriden.Test");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeStringA",
            "Detta ska hamna i mappen Inbox.Overriden.Test");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "somestring",
            "Detta ska inte hamna i mappen Inbox.Overriden.Test");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.Overriden.Test", 1);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 2);
      }

      [Test]
      public void ActionSendUsingRoute()
      {
         var smtpServerPort = TestSetup.GetNextFreePort();

         // add an account to send from
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // Add a route so we can conenct to localhost.
         var route = TestSetup.AddRoutePointingAtLocalhost(5, smtpServerPort, false);

         // Add a global send-using-route rule
         var rule = _application.Rules.Add();
         rule.Name = "Send using route";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTContains;
         ruleCriteria.MatchValue = "TestString";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRASendUsingRoute;
         ruleAction.RouteID = route.ID;
         ruleAction.Save();

         rule.Save();

         // Send message and confirm that the route does not affect it.
         var smtp = new SmtpClientSimulator();
         var recipients = new List<string>();
         recipients.Add("test@example.test");
         smtp.Send("test@example.test", recipients, "Test", "Test message");

         var message = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         Assert.IsTrue(message.Contains("Test message"));

         // Send a message and confirm that the rule affects it.
         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@nonexistantdomain.com"] = 550;


         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Send the actual message
            recipients = new List<string>();
            recipients.Add("test@nonexistantdomain.com");
            smtp.Send("test@example.test", recipients, "TestString", "Test message");


            server.WaitForCompletion();
         }

         // Submit the bounce message...
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         // Download it.
         message = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(message.Contains("550"));
         Assert.IsTrue(message.Contains("test@nonexistantdomain.com"));
      }

      [Test]
      public void ActionSetHeaderContents()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTContains;
         ruleCriteria.MatchValue = "TestString";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRASetHeaderValue;
         ruleAction.HeaderName = "SomeHeader";
         ruleAction.Value = "SomeValue";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "TestString", "Test 1");

         var sContents = Pop3ClientSimulator.AssertGetFirstMessageText("ruletest@example.test", "test");

         if (sContents.IndexOf("SomeHeader: SomeValue") <= 0)
            throw new Exception("Message header not set");
      }

      [Test]
      public void ActionSetHeaderContents_MACRO_ORIGINAL_HEADER()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTContains;
         ruleCriteria.MatchValue = "TestString";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRASetHeaderValue;
         ruleAction.HeaderName = "Subject";
         ruleAction.Value = "Foo: %MACRO_ORIGINAL_HEADER%";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "TestString", "Test 1");

         var sContents = Pop3ClientSimulator.AssertGetFirstMessageText("ruletest@example.test", "test");

         StringAssert.Contains("Foo: TestString", sContents, sContents);
      }

      [Test]
      public void CriteriaContains()
      {
         var watch = new Stopwatch();
         watch.Start();

         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTContains;
         ruleCriteria.MatchValue = "TestString";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.Wildcard";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "TestString",
            "Detta ska hamna i mappen Inbox\\Wildcard");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "TestStri",
            "Detta ska inte hamna Inbox\\Wildcard");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "VaffeTestStringBaffe",
            "Detta ska hamna i mappen Inbox\\Wildcard");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.Wildcard", 2);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 1);


         Trace.WriteLine(watch.ElapsedMilliseconds);
      }

      [Test]
      [Description("2009-12-08: The Body criteria didn't include the HTML body. It should.")]
      public void CriteriaContainsHTMLBody()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTBody;
         ruleCriteria.MatchType = eRuleMatchType.eMTContains;
         ruleCriteria.MatchValue = "MyHTMLBody";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.Wildcard";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         var message = "From: Someone <Someone@example.org>" + Environment.NewLine +
                       "Content-Type: text/html; charset=\"Windows-1251\"" + Environment.NewLine +
                       "Content-Transfer-Encoding: quoted-printable" + Environment.NewLine +
                       Environment.NewLine +
                       "<HTML><Center>MyHTMLBody</Center></HTML>" + Environment.NewLine;

         smtpClientSimulator.SendRaw("someone@example.org", account.Address, message);

         // The message should be placed in the Wildcard folder, since the HTML body of the message contains MyHTMLBody.
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox.Wildcard", 1);
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 0);
      }

      [Test]
      public void CriteriaEquals()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTEquals;
         ruleCriteria.MatchValue = "TestString";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.Wildcard";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "TestString",
            "Detta ska hamna i mappen Inbox\\Wildcard");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "teststring",
            "Detta ska hamna Inbox\\Wildcard");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "Testar",
            "Detta ska inte hamna i mappen Inbox\\Wildcard");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "teststring vaffe",
            "Detta ska inte hamna i mappen Inbox\\Wildcard");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.Wildcard", 2);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 2);
      }


      [Test]
      public void CriteriaGreaterThan()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "2";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.GreaterThan";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "0",
            "Detta ska inte hamna i mappen Inbox");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "1",
            "Detta ska inte hamna i mappen Inbox");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "2",
            "Detta ska inte hamna i mappen Inbox");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "3",
            "Detta ska hamna i mappen Inbox\\GreaterThan");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "4",
            "Detta ska hamna i mappen Inbox\\GreaterThan");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 3);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.GreaterThan", 2);
      }

      [Test]
      public void CriteriaLessThan()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTLessThan;
         ruleCriteria.MatchValue = "2";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.LessThan";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "0",
            "Detta ska hamna i mappen Inbox\\LessThan");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "2",
            "Detta ska hamna i mappen Inbox");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "3",
            "Detta ska hamna i mappen Inbox");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "4",
            "Detta ska hamna i mappen Inbox");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.LessThan", 1);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 3);
      }

      [Test]
      public void CriteriaNotEquals()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTNotEquals;
         ruleCriteria.MatchValue = "SomeString";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.NotEquals";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeString",
            "Detta ska inte hamna i mappen Inbox\\NotEquals");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeStringA",
            "Detta ska hamna i mappen Inbox\\NotEquals");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "somestring",
            "Detta ska inte hamna i mappen Inbox\\NotEquals");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.NotEquals", 1);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 2);
      }

      [Test]
      public void CriteriaRegEx()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTRegExMatch;
         ruleCriteria.MatchValue = "[a-f]*";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.RegEx";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "abc",
            "Detta ska hamna i mappen Inbox\\Wildcard");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "abcdef",
            "Detta ska hamna i mappen Inbox\\Wildcard");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "abcdefghi",
            "Detta ska inte hamna i mappen Inbox\\Wildcard");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.RegEx", 2);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 1);
      }

      [Test]
      public void CriteriaWildcardExactMatch()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTWildcard;
         ruleCriteria.MatchValue = "Exact wildcard";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.Wildcard";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "Exact wildcard",
            "Detta ska hamna i mappen Inbox\\Wildcard");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "Exact wildcard",
            "Detta ska hamna i mappen Inbox\\Wildcard");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.Wildcard", 2);
      }

      [Test]
      public void CriteriaWildcardNoCase()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain,
            "CriteriaWildcardNoCase@example.test",
            "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTWildcard;
         ruleCriteria.MatchValue = "Exact*Match";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.Wildcard";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("CriteriaWildcardNoCase@example.test", "CriteriaWildcardNoCase@example.test",
            "exact Test match",
            "Detta ska hamna i mappen Inbox\\Wildcard");

         ImapClientSimulator.AssertMessageCount("CriteriaWildcardNoCase@example.test", "test", "Inbox.Wildcard", 1);
      }

      [Test]
      public void CriteriaWildcardPartialMatch()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTWildcard;
         ruleCriteria.MatchValue = "Exact*Match";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.Wildcard";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "Exact Test Match",
            "Detta ska hamna i mappen Inbox\\Wildcard");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "ExactMatchArInte",
            "Detta ska inte hamna Inbox\\Wildcard");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.Wildcard", 1);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 1);
      }

      [Test]
      public void MovedToRenamedPublicFolder()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();

         _settings.IMAPPublicFolderName = "Public";

         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");
         var publicFolders = _settings.PublicFolders;
         var folder = publicFolders.Add("Share1");
         folder.Save();

         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = account1.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionExpunge, true);
         permission.Save();

         var rule = application.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "Public.Share1";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeString",
            "Detta ska hamna i public folder.");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Public.Share1", 1);
      }

      [Test]
      public void TestBasics()
      {
         // Fetch the default domain

         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");

         // Add a rule to this account.
         AddSpamRule(account);
         AddCorporateRule(account);
         AddExactMatchRule(account);

         var smtpClientSimulator = new SmtpClientSimulator();


         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "**SPAM** INBOX->SPAM",
            "Detta ska hamna i mappen Inbox\\Spam");

         // Corporate folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "**CORPORATE** INBOX->CORPORATE",
            "Detta ska hamna i mappen Inbox\\Corporate");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "CORPORATE EXACT MATCH",
            "Detta ska hamna i mappen Inbox\\Corporate");

         // Inbox folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "**CORPORATE EXACT MATCH**",
            "Detta ska hamna i mappen Inbox");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "INBOX",
            "Detta ska hamna i mappen Inbox");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "INBOX",
            "Detta ska hamna i mappen Inbox");
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "INBOX",
            "Detta ska hamna i mappen Inbox");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.Spam", 1);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox.Corporate", 2);
         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "Inbox", 4);

         // Test move to imap with mail with multiple recipients.

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest-m1@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest-m2@example.test", "test");
         AddSpamRule(account1);

         // Send email to both recipients
         var lstRecipients = new List<string> { "ruletest-m1@example.test", "ruletest-m2@example.test" };

         const string sBody = "Test of sending same email to multiple accounts.";

         smtpClientSimulator.Send(account1.Address, lstRecipients, "**SPAM** INBOX->SPAM", sBody);

         ImapClientSimulator.AssertMessageCount(account1.Address, "test", "Inbox.Spam", 1);
         ImapClientSimulator.AssertMessageCount(account2.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account2.Address, "test");
         Assert.IsFalse(sim.SelectFolder("Inbox.Spam"));
      }

      [Test]
      [Description("Test that creating a message copy in a rule works using a account rule.")]
      public void TestCreateCopyAccountRule()
      {
         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest1@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest2@example.test", "test");
         var account3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest3@example.test", "test");

         // Set up a rule to forward from account1 to 2 and 3.
         var rule = account1.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;
         rule.UseAND = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Only send one copy.
         ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "X-hMailServer-LoopCount";
         ruleCriteria.MatchType = eRuleMatchType.eMTLessThan;
         ruleCriteria.MatchValue = "1";
         ruleCriteria.Save();

         // Set up the actions to forward.
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRACreateCopy;
         ruleAction.To = "ruletest2@example.test";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Test to send the message to account 1. Make sure a copy is created by this rule.
         smtpClientSimulator.Send(account1.Address, account1.Address, "Test", "Test message.");
         CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);
         ImapClientSimulator.AssertMessageCount(account1.Address, "test", "Inbox", 2);

         var firstTemp = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         var secondTemp = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");

         // This is where it gets really ugly. The order of the two deliveries
         // are not defined. The message created by the rule could be delivered
         // before the first message.
         var first = "";
         var second = "";

         if (secondTemp.Contains("X-hMailServer-LoopCount"))
         {
            first = firstTemp;
            second = secondTemp;
         }
         else
         {
            first = secondTemp;
            second = firstTemp;
         }

         Assert.IsFalse(first.Contains("X-hMailServer-LoopCount: 1"), first);
         Assert.IsFalse(first.Contains("X-CopyRule: Criteria test"), first);
         Assert.IsTrue(second.Contains("X-hMailServer-LoopCount"), second);
         Assert.IsTrue(second.Contains("X-CopyRule: Criteria test"), second);
      }

      [Test]
      [Description("Test that creating a message copy in a rule works using a global rule.")]
      public void TestCreateCopyGlobalRule()
      {
         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest1@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest2@example.test", "test");

         // Set up a rule to forward from account1 to 2 and 3.
         var rule = _application.Rules.Add();
         rule.Name = "CriteriaTest";
         rule.Active = true;
         rule.UseAND = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Only send one copy.
         ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "X-hMailServer-LoopCount";
         ruleCriteria.MatchType = eRuleMatchType.eMTLessThan;
         ruleCriteria.MatchValue = "1";
         ruleCriteria.Save();

         // Set up the actions to create a copy
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRACreateCopy;
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Test to send the message to account 1. Make sure a copy is created by this rule.
         smtpClientSimulator.Send(account1.Address, new List<string> { account1.Address, account2.Address }, "Test",
            "Test message.");
         CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);
         ImapClientSimulator.AssertMessageCount(account1.Address, "test", "Inbox", 2);
         ImapClientSimulator.AssertMessageCount(account2.Address, "test", "Inbox", 2);

         /*
          * The delivery order is not guaranteed. The copied message may be delivered
          * before the original message. Check both situations.
          * 
          */
         var first = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         var second = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");

         if (first.Contains("X-hMailServer-LoopCount: 1"))
         {
            Assert.IsFalse(second.Contains("X-CopyRule: CriteriaTest"), first);
            Assert.IsTrue(first.Contains("X-hMailServer-LoopCount"), first);
         }
         else
         {
            Assert.IsTrue(second.Contains("X-hMailServer-LoopCount: 1"), second);
            Assert.IsTrue(second.Contains("X-CopyRule: CriteriaTest"), first);
         }

         first = Pop3ClientSimulator.AssertGetFirstMessageText(account2.Address, "test");
         second = Pop3ClientSimulator.AssertGetFirstMessageText(account2.Address, "test");

         if (first.Contains("X-hMailServer-LoopCount: 1"))
         {
            Assert.IsFalse(second.Contains("X-hMailServer-LoopCount"), first);
            Assert.IsTrue(first.Contains("X-CopyRule: CriteriaTest"), first);
         }
         else
         {
            Assert.IsTrue(second.Contains("X-hMailServer-LoopCount: 1"), second);
            Assert.IsTrue(second.Contains("X-CopyRule: CriteriaTest"), first);
         }
      }

      [Test]
      [Description("Test the delivery attempts criteria.")]
      public void TestDeliveryAttempts()
      {
         // Add an account
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         var adminAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "admin@example.test", "test");

         // Set up a rule to forward from account1 to 2 and 3.
         var rule = _application.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;
         rule.UseAND = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTDeliveryAttempts;
         ruleCriteria.MatchType = eRuleMatchType.eMTEquals;
         ruleCriteria.MatchValue = "2";
         ruleCriteria.Save();

         // The second time we try to deliver an email, forward a copy to the admin!
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAForwardEmail;
         ruleAction.To = adminAccount.Address;
         ruleAction.Save();

         // And then delete it.
         ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRADeleteEmail;
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();


         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["ahem@dummy-example.com"] = 452;

         var smtpServerPort = TestSetup.GetNextFreePort();
         using (var smtpServer = new SmtpServerSimulator(1, smtpServerPort))
         {
            smtpServer.AddRecipientResult(deliveryResults);
            smtpServer.StartListen();

            // Add a route so we can connect to localhost.
            TestSetup.AddRoutePointingAtLocalhost(2, smtpServerPort, false);

            var smtpClientSimulator = new SmtpClientSimulator();

            // Test to send the message to account 1. Make sure a copy is created by this rule.
            smtpClientSimulator.Send(account.Address, new List<string> { "ahem@dummy-example.com" }, "Test",
               "Test message.");

            smtpServer.WaitForCompletion();
         }

         CustomAsserts.AssertRecipientsInDeliveryQueue(0, true);

         var first = Pop3ClientSimulator.AssertGetFirstMessageText(adminAccount.Address, "test");

         Assert.IsTrue(first.Contains("X-hMailServer-LoopCount: 1"), first);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 0);
      }

      [Test]
      [Description("Test that forwarding set up in a rule works.")]
      public void TestForward()
      {
         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest1@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest2@example.test", "test");
         var account3 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest3@example.test", "test");

         // Set up a rule to forward from account1 to 2 and 3.
         var rule = account1.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Set up the actions to forward.
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAForwardEmail;
         ruleAction.To = "ruletest2@example.test";
         ruleAction.Save();

         var ruleAction2 = rule.Actions.Add();
         ruleAction2.Type = eRuleActionType.eRAForwardEmail;
         ruleAction2.To = "ruletest3@example.test";
         ruleAction2.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Test to send the messge to account 1.
         smtpClientSimulator.Send(account1.Address, account1.Address, "Test", "Test message.");

         ImapClientSimulator.AssertMessageCount(account1.Address, "test", "Inbox", 1);
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         ImapClientSimulator.AssertMessageCount(account3.Address, "test", "Inbox", 1);
         ImapClientSimulator.AssertMessageCount(account2.Address, "test", "Inbox", 1);
      }

      [Test]
      [Description("Test to move to a public folder without permission.")]
      public void TestMoveToPublicFolderWithoutPermission()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();

         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest@example.test", "test");
         var publicFolders = _settings.PublicFolders;
         var folder = publicFolders.Add("Share1");
         folder.Save();

         var permission = folder.Permissions.Add();
         permission.PermissionAccountID = account1.ID;
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.set_Permission(eACLPermission.ePermissionRead, true);
         permission.set_Permission(eACLPermission.ePermissionExpunge, true);
         permission.Save();

         var rule = account1.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "#public.Share1.Sub";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Spam folder
         smtpClientSimulator.Send("ruletest@example.test", "ruletest@example.test", "SomeString",
            "This should end up in the inbox since user lacks right.");

         ImapClientSimulator.AssertMessageCount("ruletest@example.test", "test", "INBOX", 1);
      }

      [Test]
      [Description("Issue 338, Recipient Count is wrong under certain circumstances")]
      public void TestRecipientCountInAccountLevelRule()
      {
         _settings.Scripting.Enabled = true;

         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest1@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest2@example.test", "test");

         CreatePrintRecipientCountRule(account1.Rules);

         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "SomeString",
            "Detta ska inte hamna i mappen Inbox.Overriden.Test");
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         // This should print a single list.
         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         CustomAsserts.AssertDeleteFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("\"1\""), eventLogText);

         // Send message to two recipients. Recipient should still be one, since it's an account-level rule.
         SmtpClientSimulator.StaticSend(account1.Address, new List<string> { account1.Address, account2.Address },
            "SomeString", "Detta ska inte hamna i mappen Inbox.Overriden.Test");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         // This should print a single list.
         eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         CustomAsserts.AssertDeleteFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("\"1\""), eventLogText);
      }

      [Test]
      [Description("Issue 338, Recipient Count is wrong under certain circumstances")]
      public void TestRecipientCountInGlobalRule()
      {
         _settings.Scripting.Enabled = true;

         // Add an account
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest1@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest2@example.test", "test");

         CreatePrintRecipientCountRule(_application.Rules);

         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "SomeString",
            "Detta ska inte hamna i mappen Inbox.Overriden.Test");
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         // This should print a single list.

         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         CustomAsserts.AssertDeleteFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("\"1\""), eventLogText);

         // Send message to two recipients. 
         SmtpClientSimulator.StaticSend(account1.Address, new List<string> { account1.Address, account2.Address },
            "SomeString", "Detta ska inte hamna i mappen Inbox.Overriden.Test");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         // This should print a two recipients. Global rule is affected before message reaches recipients.
         eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         CustomAsserts.AssertDeleteFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("\"2\""), eventLogText);
      }

      [Test]
      [Description("Test that replying using rule works.")]
      public void TestReply()
      {
         // Add accounts
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest1@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruletest2@example.test", "test");

         // Set up a rule to reply to any message sent to account2.
         var rule = account2.Rules.Add();
         rule.Name = "Criteria test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         // Set up the actions to forward.
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAReply;
         ruleAction.FromAddress = account2.Address;
         ruleAction.FromName = "Rule Test 2";
         ruleAction.Subject = "Autoreply";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();

         var smtpClientSimulator = new SmtpClientSimulator();

         // Test to send the message to account 2.
         smtpClientSimulator.Send(account1.Address, account2.Address, "Test", "Test message.");
         ImapClientSimulator.AssertMessageCount(account2.Address, "test", "Inbox", 1);

         // Make sure a reply is sent back to account 1.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         var message = CustomAsserts.AssertGetFirstMessage(account1, "Inbox");

         Assert.AreEqual("ruletest2@example.test", message.FromAddress);
         Assert.AreEqual("auto-replied", message.get_HeaderValue("Auto-Submitted"));
      }

      [Test]
      [Description("Rule REPLY body must be QP-wrapped to 76 chars per RFC 2045 section 6.7 (issue #171).")]
      public void WhenRuleReplyBodyExceedsQPLineLimitShouldWrapToRfc2045()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "qpwrap-src@example.test", "test");
         var account2 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "qpwrap-dst@example.test", "test");

         var rule = account2.Rules.Add();
         rule.Name = "QP wrap test";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = true;
         ruleCriteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         ruleCriteria.MatchType = eRuleMatchType.eMTGreaterThan;
         ruleCriteria.MatchValue = "0";
         ruleCriteria.Save();

         const string longLine = "Please note my normal office hours are Monday and Tuesday only.  I will respond to your email when I return.";

         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAReply;
         ruleAction.FromAddress = account2.Address;
         ruleAction.Subject = "Autoreply";
         ruleAction.Body = longLine;
         ruleAction.Save();

         rule.Save();

         SmtpClientSimulator.StaticSend(account1.Address, account2.Address, "Test", "Trigger.");
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         var rawMessage = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");

         foreach (var line in rawMessage.Split(new[] { "\r\n", "\n" }, StringSplitOptions.None))
            Assert.LessOrEqual(line.Length, 76, $"QP line exceeds 76 chars: {line}");
      }

      private void AddExactMatchRule(Account account)
      {
         var rule = account.Rules.Add();
         rule.Name = "TestRule 3";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTEquals;
         ruleCriteria.MatchValue = "CORPORATE EXACT MATCH";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.Corporate";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();
      }

      public void AddCorporateRule(Account account)
      {
         var rule = account.Rules.Add();
         rule.Name = "TestRule 2";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTContains;
         ruleCriteria.MatchValue = "**CORPORATE**";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.Corporate";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();
      }

      public void AddSpamRule(Account account)
      {
         var rule = account.Rules.Add();
         rule.Name = "TestRule 1";
         rule.Active = true;

         var ruleCriteria = rule.Criterias.Add();
         ruleCriteria.UsePredefined = false;
         ruleCriteria.HeaderField = "Subject";
         ruleCriteria.MatchType = eRuleMatchType.eMTContains;
         ruleCriteria.MatchValue = "**SPAM**";
         ruleCriteria.Save();

         // Add action
         var ruleAction = rule.Actions.Add();
         ruleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         ruleAction.IMAPFolder = "INBOX.Spam";
         ruleAction.Save();

         // Save the rule in the database
         rule.Save();
      }
   }
}