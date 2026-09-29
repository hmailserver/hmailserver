// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Security.Authentication;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.API
{
   [TestFixture]
   public class Events : TestFixtureBase
   {
      [Test]
      public void TestOnAcceptMessageJScript()
      {
         LogHandler.DeleteEventLog();

         _settings.Scripting.Language = "JScript";
         // First set up a script
         var script =
            @"function OnAcceptMessage(oClient, message)
                           {
                              message.HeaderValue('X-SpamResult') = 'TEST';
                              message.Save();
                           }";

         var scripting = _settings.Scripting;
         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Test", "SampleBody");

         // Check that the message exists
         var message = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         Assert.IsNotEmpty(message);

         Assert.Less(0, message.IndexOf("X-SpamResult: TEST"));
      }

      [Test]
      public void TestOnAcceptMessageVBScript()
      {
         LogHandler.DeleteEventLog();

         // First set up a script
         var script =
            @"Sub OnAcceptMessage(oClient, message)
                               message.HeaderValue(""X-SpamResult"") = ""TEST""
                               message.Save()
                               EventLog.Write(""Port: "" & oClient.Port)
                               EventLog.Write(""Address: "" & oClient.IPAddress)
                               EventLog.Write(""Username: "" & oClient.Username)
                               EventLog.Write(""SessionId: "" & oClient.SessionID)
                              End Sub";

         var scripting = _settings.Scripting;
         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Test", "SampleBody");

         // Check that the message exists
         var message = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         Assert.IsNotEmpty(message);

         Assert.Less(0, message.IndexOf("X-SpamResult: TEST"));


         // Check that the message exists
         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());

         Assert.IsNotEmpty(eventLogText);
         Assert.IsTrue(eventLogText.Contains("Port: 25"));
         Assert.IsTrue(eventLogText.Contains("Address: 127"));
         Assert.IsTrue(
            eventLogText.Contains("Username: \"")); // Should be empty, Username isn't available at this time.
         StringAssert.IsMatch(".*\"SessionId: \\d+\"", eventLogText);
      }

      [Test]
      public void TestOnRecipientUnknownVBScript()
      {
         var eventLogFile = _settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         // First set up a script
         var script =
            @"Sub OnRecipientUnknown(oClient, oMessage)
                               EventLog.Write(""OnRecipientUnknown "")
                              End Sub";

         var scripting = _settings.Scripting;
         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         try
         {
            SmtpClientSimulator.StaticSend(account1.Address, "nonexistent@example.test", "Test", "SampleBody");
         }
         catch (DeliveryFailedException)
         {
            // Expected, since recipient does not exist.
         }

         // Check that the event was triggered
         var message = TestSetup.ReadExistingTextFile(eventLogFile);
         Assert.IsTrue(message.Contains("OnRecipientUnknown"));
      }

      [Test]
      public void TestOnTooManyInvalidCommands()
      {
         var maxInvalid = 5;

         _settings.MaxNumberOfInvalidCommands = maxInvalid;
         _settings.DisconnectInvalidClients = true;

         var eventLogFile = _settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         // First set up a script
         var script =
            @"Sub OnTooManyInvalidCommands(oClient, oMessage)
                               EventLog.Write(""OnTooManyInvalidCommands "")
                              End Sub";

         var scripting = _settings.Scripting;
         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var client = new SmtpClientSimulator();
         client.Connect();
         client.Receive(); // Welcome banner
         var ehloResponse = client.SendAndReceive("EHLO example.com\r\n");

         for (var i = 0; i < maxInvalid + 1; i++) client.SendAndReceive("MAIL FROM\r\n");

         // Check that the event was triggered
         var message = TestSetup.ReadExistingTextFile(eventLogFile);
         Assert.IsTrue(message.Contains("OnTooManyInvalidCommands"));
      }


      [Test]
      public void TestOnBackupCompletedJScript()
      {
         LogHandler.DeleteEventLog();

         var scripting = _settings.Scripting;
         scripting.Language = "JScript";

         // First set up a script
         var script =
            @"function OnBackupCompleted()
                           {
                               EventLog.Write('Backup process completed')
                           }";


         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         var back = new BackupRestore();
         back.InitializeBackupSettings();
         back.TestWithMessages();


         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("Backup process completed"));
      }

      [Test]
      public void TestOnBackupCompletedVBScript()
      {
         LogHandler.DeleteEventLog();

         // First set up a script
         var script =
            @"Sub OnBackupCompleted()
                               EventLog.Write(""Backup process completed"")
                           End Sub";

         var scripting = _settings.Scripting;
         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         var back = new BackupRestore();
         back.InitializeBackupSettings();
         back.TestWithMessages();


         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("Backup process completed"));
      }

      [Test]
      public void TestOnBackupFailedJScript()
      {
         LogHandler.DeleteEventLog();

         var scripting = _settings.Scripting;
         scripting.Language = "JScript";

         // First set up a script
         var script =
            @"function OnBackupFailed(reason)     
                           {
                               EventLog.Write('Failed: ' + reason)
                           }";


         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         var back = new BackupRestore();
         back.InitializeBackupSettings();
         back.SetBackupDir(@"C:\some-non-existant-directory");
         Assert.IsFalse(back.Execute());

         CustomAsserts.AssertReportedError("BACKUP ERROR: The specified backup directory is not accessible:");
         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("The specified backup directory is not accessible"));
      }

      [Test]
      public void TestOnBackupFailedVBScript()
      {
         LogHandler.DeleteEventLog();

         // First set up a script
         var script =
            @"Sub OnBackupFailed(reason)
                               EventLog.Write(""Failed: "" & reason)
                           End Sub";

         var scripting = _settings.Scripting;
         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         var back = new BackupRestore();
         back.InitializeBackupSettings();
         back.SetBackupDir(@"C:\some-non-existant-directory");
         Assert.IsFalse(back.Execute());

         CustomAsserts.AssertReportedError("BACKUP ERROR: The specified backup directory is not accessible:");
         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("The specified backup directory is not accessible"));
      }

      [Test]
      public void TestOnClientConnectJScript()
      {
         LogHandler.DeleteEventLog();

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         scripting.Language = "JScript";

         var script = "function OnClientConnect(oClient) " + Environment.NewLine +
                      "{" + Environment.NewLine +
                      " EventLog.Write('Port: ' + oClient.Port); " + Environment.NewLine +
                      " EventLog.Write('Address: ' + oClient.IPAddress); " + Environment.NewLine +
                      " EventLog.Write('Username: ' + oClient.Username); " + Environment.NewLine +
                      "}" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var eventLogFile = app.Settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         var socket = new TcpConnection();
         Assert.IsTrue(socket.IsPortOpen(25));

         // Check that the message exists
         var message = TestSetup.ReadExistingTextFile(eventLogFile);

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains("Port: 25"));
         Assert.IsTrue(message.Contains("Address: 127"));
         Assert.IsTrue(message.Contains("Username: \"")); // Should be empty, Username isn't available at this time.
      }

      [Test]
      public void TestOnClientConnectVBScript()
      {
         LogHandler.DeleteEventLog();

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         var script = "Sub OnClientConnect(oClient) " + Environment.NewLine +
                      " EventLog.Write(\"Port: \" & oClient.Port) " + Environment.NewLine +
                      " EventLog.Write(\"Address: \" & oClient.IPAddress) " + Environment.NewLine +
                      " EventLog.Write(\"Username: \" & oClient.Username) " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var socket = new TcpConnection();
         Assert.IsTrue(socket.IsPortOpen(25));

         // Check that the message exists
         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());

         Assert.IsNotEmpty(eventLogText);
         Assert.IsTrue(eventLogText.Contains("Port: 25"));
         Assert.IsTrue(eventLogText.Contains("Address: 127"));
         Assert.IsTrue(
            eventLogText.Contains("Username: \"")); // Should be empty, Username isn't available at this time.
      }

      [Test]
      public void TestOnDeliverMessageJScript()
      {
         LogHandler.DeleteEventLog();

         var scripting = _settings.Scripting;
         scripting.Language = "JScript";
         // First set up a script
         var script =
            @"function OnDeliverMessage(message)
                           {
                               message.HeaderValue('X-SpamResult') = 'TEST2';
                               message.Body = 'This is the body text.';
                               message.Save();
                           }";


         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Test", "SampleBody");

         // Check that the message exists
         var message = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         Assert.IsNotEmpty(message);

         StringAssert.Contains("X-SpamResult: TEST2", message);
         StringAssert.Contains("This is the body text.", message);
      }

      [Test]
      public void TestOnDeliveryFailedJScript()
      {
         LogHandler.DeleteEventLog();

         var scripting = _settings.Scripting;
         scripting.Language = "JScript";

         // First set up a script
         var script = "function OnDeliveryFailed(message, sRecipient, sErrorMessage) {" + Environment.NewLine +
                      " EventLog.Write('File: ' + message.FileName); " + Environment.NewLine +
                      " EventLog.Write('Recipient: ' + sRecipient); " + Environment.NewLine +
                      " EventLog.Write('Error: ' + sErrorMessage); " + Environment.NewLine +
                      "}";


         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account1.Address, "user@dummy.example.com", "Test", "SampleBody");

         // Make sure that the message is deliverd and bounced.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("File: "), eventLogText);
         Assert.IsTrue(eventLogText.Contains("Recipient: user@dummy.example.com"), eventLogText);
         Assert.IsTrue(eventLogText.Contains("No mail servers appear to exists") ||
                       eventLogText.Contains("Unable to find the recipient's email server"), eventLogText);
      }

      [Test]
      public void TestOnDeliveryFailedVBScript()
      {
         LogHandler.DeleteEventLog();

         // First set up a script
         var script = "Sub OnDeliveryFailed(message, sRecipient, sErrorMessage)" + Environment.NewLine +
                      " EventLog.Write(\"File: \" & message.FileName) " + Environment.NewLine +
                      " EventLog.Write(\"Recipient: \" & sRecipient) " + Environment.NewLine +
                      " EventLog.Write(\"Error: \" & sErrorMessage) " + Environment.NewLine +
                      " End Sub";

         var scripting = _settings.Scripting;
         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account1.Address, "user@dummy.example.com", "Test", "SampleBody");

         // Make sure that the message is deliverd and bounced.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("File: "));
         Assert.IsTrue(eventLogText.Contains("Recipient: user@dummy.example.com"));
         Assert.IsTrue(eventLogText.Contains("No mail servers appear to exists") ||
                       eventLogText.Contains("Unable to find the recipient's email server"), eventLogText);
      }

      [Test]
      public void TestOnDeliveryStartVBScript()
      {
         LogHandler.DeleteEventLog();

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         var script = "Sub OnDeliveryStart(message) " + Environment.NewLine +
                      " EventLog.Write(\"Delivering message: \" & message.FileName) " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");

         // Wait for the message to be delivered.
         Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var eventLogText = TestSetup.ReadExistingTextFile(app.Settings.Logging.CurrentEventLog);
         Assert.IsTrue(eventLogText.Contains("Delivering message"));
      }

      [Test]
      public void TestOnDeliveryStart_SetHtmlBodyEmpty()
      {
         LogHandler.DeleteEventLog();

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         var script = "Sub OnDeliveryStart(message) " + Environment.NewLine +
                      " message.HTMLBody = \"\" " + Environment.NewLine +
                      " EventLog.Write(\"HTMLBody: '\" & message.HTMLBody & \"'\")" + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");

         // Wait for the message to be delivered.
         Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var eventLogText = TestSetup.ReadExistingTextFile(app.Settings.Logging.CurrentEventLog);
         StringAssert.Contains("HTMLBody: ''", eventLogText);
      }

      [Test]
      public void TestOnErrorJScript()
      {
         LogHandler.DeleteEventLog();

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;
         scripting.Language = "JScript";
         var script = "function OnError(iSeverity, iError, sSource, sDescription) {" + Environment.NewLine +
                      " EventLog.Write('Severity: ' + iSeverity) " + Environment.NewLine +
                      " EventLog.Write('Error: ' + iError) " + Environment.NewLine +
                      " EventLog.Write('Source: ' + sSource) " + Environment.NewLine +
                      " EventLog.Write('Description: ' + sDescription) " + Environment.NewLine +
                      "}" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         var inbox = account.IMAPFolders.get_ItemByName("Inbox");


         var deletedMessageText = app.Settings.ServerMessages.get_ItemByName("MESSAGE_FILE_MISSING").Text;
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");

         CustomAsserts.AssertFolderMessageCount(inbox, 1);
         var message = inbox.Messages[0];
         File.Delete(message.Filename);
         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         Assert.IsTrue(text.Contains(deletedMessageText.Replace("%MACRO_FILE%", message.Filename)));
         CustomAsserts.AssertReportedError("Message retrieval failed because message file");

         var eventLogText = TestSetup.ReadExistingTextFile(app.Settings.Logging.CurrentEventLog);
         Assert.IsTrue(eventLogText.Contains("Description: Message retrieval failed"));
      }

      [Test]
      public void TestOnErrorVBScript()
      {
         LogHandler.DeleteEventLog();

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         var script = "Sub OnError(iSeverity, iError, sSource, sDescription) " + Environment.NewLine +
                      " EventLog.Write(\"Severity: \" & iSeverity) " + Environment.NewLine +
                      " EventLog.Write(\"Error: \" & iError) " + Environment.NewLine +
                      " EventLog.Write(\"Source: \" & sSource) " + Environment.NewLine +
                      " EventLog.Write(\"Description: \" & sDescription) " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         var inbox = account.IMAPFolders.get_ItemByName("Inbox");


         var deletedMessageText = app.Settings.ServerMessages.get_ItemByName("MESSAGE_FILE_MISSING").Text;
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");

         CustomAsserts.AssertFolderMessageCount(inbox, 1);
         var message = inbox.Messages[0];
         File.Delete(message.Filename);
         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         Assert.IsTrue(text.Contains(deletedMessageText.Replace("%MACRO_FILE%", message.Filename)));
         CustomAsserts.AssertReportedError("Message retrieval failed because message file");

         var eventLogText = TestSetup.ReadExistingTextFile(app.Settings.Logging.CurrentEventLog);
         Assert.IsTrue(eventLogText.Contains("Description: Message retrieval failed"));
      }

      [Test]
      public void TestOnExternalAccountDownload()
      {
         LogHandler.DeleteCurrentDefaultLog();

         var messages = new List<string>();

         messages.Add("From: Martin@example1.com\r\n" +
                      "To: Martin@example.com\r\n" +
                      "Subject: Message 1\r\n" +
                      "\r\n" +
                      "Message 1!");

         messages.Add("From: Martin@example2.com\r\n" +
                      "To: Martin@example.com\r\n" +
                      "Subject: Message 2\r\n" +
                      "\r\n" +
                      "Message 2!");

         messages.Add("From: Martin@example3.com\r\n" +
                      "To: Martin@example.com\r\n" +
                      "Subject: Message 3\r\n" +
                      "\r\n" +
                      "Message 3!");


         // The second message should be deleted after 5 days.
         var script = "Sub OnExternalAccountDownload(oFetchAccount, message, sRemoteUID)" + Environment.NewLine +
                      " EventLog.Write(\"UID: \" & sRemoteUID) " + Environment.NewLine +
                      " EventLog.Write(\"FetchAccount: \" & oFetchAccount.Name) " + Environment.NewLine +
                      " If Not message Is Nothing Then " + Environment.NewLine +
                      "   EventLog.Write(\"From: \" & message.FromAddress) " + Environment.NewLine +
                      "   EventLog.Write(\"Filename: \" & message.FileName) " + Environment.NewLine +
                      " Else " + Environment.NewLine +
                      "   EventLog.Write(\"Message details missing\") " + Environment.NewLine +
                      " End If" + Environment.NewLine +
                      " if sRemoteUID = \"UniqueID-" + messages[1].GetHashCode() + "\" Then " +
                      Environment.NewLine +
                      "   Result.Value = 2  " + Environment.NewLine +
                      "   Result.Parameter = 5  " + Environment.NewLine +
                      " End If " + Environment.NewLine +
                      " End Sub";

         var scripting = _settings.Scripting;
         var file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");

         FetchAccount fa;

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();


            fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "TestFA";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = -1;
            fa.Save();
            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            var eventLogFile = _settings.Logging.CurrentEventLog;
            var logContents = TestSetup.ReadExistingTextFile(eventLogFile);

            Assert.IsTrue(logContents.Contains("FetchAccount: " + fa.Name));

            Assert.IsTrue(logContents.Contains("From: Martin@example1.com"));
            Assert.IsTrue(logContents.Contains("From: Martin@example2.com"));
            Assert.IsTrue(logContents.Contains("From: Martin@example3.com"));

            var appLogContent = LogHandler.ReadCurrentDefaultLog();

            Assert.IsTrue(pop3Server.DeletedMessages.Contains(1));
            Assert.IsFalse(pop3Server.DeletedMessages.Contains(2));
            Assert.IsTrue(pop3Server.DeletedMessages.Contains(3));

            Assert.IsTrue(pop3Server.RetrievedMessages.Contains(1));
            Assert.IsTrue(pop3Server.RetrievedMessages.Contains(2));
            Assert.IsTrue(pop3Server.RetrievedMessages.Contains(3));

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 3);
         }

         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            pop3Server.StartListen();

            // Download again...
            fa.DownloadNow();

            pop3Server.WaitForCompletion();

            // Make sure that no messages are deleted.
            Assert.AreEqual(0, pop3Server.DeletedMessages.Count);
            Assert.AreEqual(0, pop3Server.RetrievedMessages.Count);

            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 3);
         }
      }

      [Test]
      public void TestOnClientLogon_POP3()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();
         SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         var script = "Sub OnClientLogon(oClient) " + Environment.NewLine +
                      " EventLog.Write(\"OnClientLogin-POP3\")" + Environment.NewLine +
                      " EventLog.Write(\"IsAuthenticated: \" & oClient.Authenticated) " + Environment.NewLine +
                      " EventLog.Write(\"Username: \" & oClient.Username) " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var eventLogFile = app.Settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         // Log on once to trigger event.
         Pop3ClientSimulator.AssertMessageCount("test@example.test", "test", 0);

         // Check that the message exists
         var message = TestSetup.ReadExistingTextFile(eventLogFile);

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains("OnClientLogin-POP3"));
         Assert.IsTrue(message.Contains("IsAuthenticated: True"));
         Assert.IsTrue(
            message.Contains("Username: test@example.test")); // Should be empty, Username isn't available at this time.
      }

      [Test]
      public void TestOnClientLogon_IMAP()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();
         SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         var script = "Sub OnClientLogon(oClient) " + Environment.NewLine +
                      " EventLog.Write(\"OnClientLogin-IMAP\")" + Environment.NewLine +
                      " EventLog.Write(\"IsAuthenticated: \" & oClient.Authenticated) " + Environment.NewLine +
                      " EventLog.Write(\"Username: \" & oClient.Username) " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var eventLogFile = app.Settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         // Log on once to trigger event.
         ImapClientSimulator.AssertMessageCount("test@example.test", "test", "Inbox", 0);

         // Check that the message exists
         var message = TestSetup.ReadExistingTextFile(eventLogFile);

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains("OnClientLogin-IMAP"));
         Assert.IsTrue(message.Contains("IsAuthenticated: True"));
         Assert.IsTrue(
            message.Contains("Username: test@example.test")); // Should be empty, Username isn't available at this time.
      }

      [Test]
      public void TestOnClientLogon_IMAP_Authenticate()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();
         SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         app.Settings.IMAPSASLPlainEnabled = true;

         var script = "Sub OnClientLogon(oClient) " + Environment.NewLine +
                      " EventLog.Write(\"OnClientLogin-IMAP-Authenticate\")" + Environment.NewLine +
                      " EventLog.Write(\"IsAuthenticated: \" & oClient.Authenticated) " + Environment.NewLine +
                      " EventLog.Write(\"Username: \" & oClient.Username) " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var eventLogFile = app.Settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         // Log on using AUTHENTICATE PLAIN to trigger the event.
         var token =
            Convert.ToBase64String(Encoding.ASCII.GetBytes("\0test@example.test\0test"));

         var simulator = new ImapClientSimulator();
         simulator.Connect();
         var result = simulator.SendSingleCommand("A01 AUTHENTICATE PLAIN " + token);
         Assert.IsTrue(result.Contains("A01 OK LOGIN completed"), result);
         simulator.Disconnect();

         // Check that the message exists
         var message = TestSetup.ReadExistingTextFile(eventLogFile);

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains("OnClientLogin-IMAP-Authenticate"));
         Assert.IsTrue(message.Contains("IsAuthenticated: True"));
         Assert.IsTrue(message.Contains("Username: test@example.test"));
      }

      [Test]
      public void TestOnClientLogon_SMTP()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();
         SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         var script = "Sub OnClientLogon(oClient) " + Environment.NewLine +
                      " EventLog.Write(\"OnClientLogin-SMTP\")" + Environment.NewLine +
                      " EventLog.Write(\"IsAuthenticated: \" & oClient.Authenticated) " + Environment.NewLine +
                      " EventLog.Write(\"Username: \" & oClient.Username) " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var eventLogFile = app.Settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         // Log on once to trigger event.

         var base64Username = Convert.ToBase64String(Encoding.ASCII.GetBytes("test@example.test"));
         var base64Password = Convert.ToBase64String(Encoding.ASCII.GetBytes("test"));

         var clientSimulator = new SmtpClientSimulator();
         string errMsg;
         clientSimulator.ConnectAndLogon(base64Username, base64Password, out errMsg);

         // Check that the message exists
         var message = TestSetup.ReadExistingTextFile(eventLogFile);

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains("OnClientLogin-SMTP"));
         Assert.IsTrue(message.Contains("IsAuthenticated: True"));
         Assert.IsTrue(
            message.Contains("Username: test@example.test")); // Should be empty, Username isn't available at this time.
      }

      [Test]
      public void TestOnHelo_WithHelo()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();
         SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         var script = "Sub OnHelo(oClient) " + Environment.NewLine +
                      " EventLog.Write(\"OnHelo\")" + Environment.NewLine +
                      " EventLog.Write(\"HeloHost: \" & oClient.HELO) " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var eventLogFile = app.Settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         // Log on once to trigger event.
         var clientSimulator = new SmtpClientSimulator();
         clientSimulator.Connect();
         clientSimulator.SendAndReceive("HELO WORLD\r\n");

         // Check that the message exists
         var message = TestSetup.ReadExistingTextFile(eventLogFile);

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains("OnHelo"));
         Assert.IsTrue(message.Contains("HeloHost: WORLD"));
      }

      [Test]
      public void TestOnHelo_WithEhlo()
      {
         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();
         SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@example.test", "test");

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         var script = "Sub OnHelo(oClient) " + Environment.NewLine +
                      " EventLog.Write(\"OnHelo\")" + Environment.NewLine +
                      " EventLog.Write(\"HeloHost: \" & oClient.HELO) " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         var eventLogFile = app.Settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         // Log on once to trigger event.
         var clientSimulator = new SmtpClientSimulator();
         clientSimulator.Connect();
         clientSimulator.SendAndReceive("EHLO WORLD2\r\n");

         // Check that the message exists
         var message = TestSetup.ReadExistingTextFile(eventLogFile);

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains("OnHelo"));
         Assert.IsTrue(message.Contains("HeloHost: WORLD2"));
      }

      [Test]
      public void TestOnClientValidatePasswordVBScript_ValidPassword()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // First verify log on works with proper password ("test") but fails with incorrect ("MySecretPassword")
         Assert.IsTrue(ImapClientSimulator.ValidatePassword("test@example.test", "test"));
         Assert.IsFalse(ImapClientSimulator.ValidatePassword("test@example.test", "MySecretPassword"));

         // Create a script which override password validation to allow MySecretPassword as valid password
         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;

         var script =
            @"Sub OnClientValidatePassword(account, password)  
                 EventLog.Write(""Account: "" & account.Address)
                 EventLog.Write(""Password: "" & password)
                 If password = ""MySecretPassword"" Then
                   Result.Value = 0
                 Else
                   Result.Value = 1
                 End If
              End Sub";


         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         // Now verify we can log on using the new password
         Assert.IsTrue(ImapClientSimulator.ValidatePassword("test@example.test", "MySecretPassword"));
         Assert.IsFalse(ImapClientSimulator.ValidatePassword("test@example.test", "test"));

         var eventLogText = TestSetup.ReadExistingTextFile(app.Settings.Logging.CurrentEventLog);
         Assert.IsTrue(eventLogText.Contains("Account: test@example.test"));
         Assert.IsTrue(eventLogText.Contains("Password: MySecretPassword"));
      }

      [Test]
      public void TestOnClientValidatePasswordJScript_BackslashCannotBreakOutOfString()
      {
         LogHandler.DeleteEventLog();
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;
         scripting.Language = "JScript";

         var script =
            @"function OnClientValidatePassword(account, password) {
                 EventLog.Write('Password: [' + password + ']');
              }";

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();

         var pop3 = new Pop3ClientSimulator();
         Assert.IsFalse(pop3.ConnectAndLogon("test@example.test", JScriptInjection));

         var eventLogText = TestSetup.ReadExistingTextFile(app.Settings.Logging.CurrentEventLog);
         Assert.IsFalse(eventLogText.Contains("INJECTED"), eventLogText);
         StringAssert.Contains("Password: [" + JScriptInjection + "]", eventLogText);
      }

      [Test]
      public void TestOnClientValidatePasswordVBScript_QuoteAndLineBreakArePassedIntact()
      {
         LogHandler.DeleteEventLog();
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var app = SingletonProvider<TestSetup>.Instance.GetApp();
         var scripting = app.Settings.Scripting;
         scripting.Language = "VBScript";

         var script =
            @"Sub OnClientValidatePassword(account, password)
                 EventLog.Write(""Intact: "" & (password = ""pass""""word"" & vbCrLf & ""line2""))
              End Sub";

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();

         // AUTH LOGIN is base64 encoded, so the password can contain a line break.
         const string password = "pass\"word\r\nline2";
         var smtp = new SmtpClientSimulator();
         Assert.Throws<AuthenticationException>(() =>
            smtp.ConnectAndLogon(EncodeBase64("test@example.test"), EncodeBase64(password), out _));

         var eventLogText = TestSetup.ReadExistingTextFile(app.Settings.Logging.CurrentEventLog);
         StringAssert.Contains("Intact: True", eventLogText);
      }

      [Test]
      public void TestOnDeliveryFailedJScript_BackslashCannotBreakOutOfString()
      {
         LogHandler.DeleteEventLog();
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var scripting = _settings.Scripting;
         scripting.Language = "JScript";

         var script =
            @"function OnDeliveryFailed(message, recipient, errorMessage) {
                 EventLog.Write('Error: [' + errorMessage + ']');
              }";

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();

         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@dummy-example.com"] = 550;

         var smtpServerPort = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, smtpServerPort))
         {
            // The remote server controls the error message passed to the script.
            server.RecipientReplyText = JScriptInjection;
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            TestSetup.AddRoutePointingAtLocalhost(0, smtpServerPort, false);

            SmtpClientSimulator.StaticSend("test@example.test", "test@dummy-example.com", "Test", "Test message");

            server.WaitForCompletion();
         }

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsFalse(eventLogText.Contains("INJECTED"), eventLogText);
         StringAssert.Contains(JScriptInjection, eventLogText);
      }

      [Test]
      public void TestOnExternalAccountDownloadJScript_BackslashCannotBreakOutOfString()
      {
         LogHandler.DeleteEventLog();

         var scripting = _settings.Scripting;
         scripting.Language = "JScript";

         var script =
            @"function OnExternalAccountDownload(fetchAccount, message, remoteUid) {
                 EventLog.Write('UID: [' + remoteUid + ']');
              }";

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");

         var messages = new List<string>
         {
            "From: Martin@example1.com\r\n" +
            "To: Martin@example.com\r\n" +
            "Subject: Message 1\r\n" +
            "\r\n" +
            "Message 1!"
         };

         var port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages))
         {
            // The remote server controls the UID passed to the script.
            pop3Server.Uids = new List<string> { JScriptInjection };
            pop3Server.StartListen();

            var fa = account.FetchAccounts.Add();
            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "TestFA";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.Save();
            fa.DownloadNow();

            pop3Server.WaitForCompletion();
         }

         var eventLogText = TestSetup.ReadExistingTextFile(_settings.Logging.CurrentEventLog);
         Assert.IsFalse(eventLogText.Contains("INJECTED"), eventLogText);
         StringAssert.Contains("UID: [" + JScriptInjection + "]", eventLogText);
      }

      [Test]
      public void TestOnBackupFailedJScript_BackslashIsPassedIntact()
      {
         LogHandler.DeleteEventLog();

         var scripting = _settings.Scripting;
         scripting.Language = "JScript";

         var script =
            @"function OnBackupFailed(reason) {
                 EventLog.Write('Failed: [' + reason + ']');
              }";

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();

         // \t in the path must not become a tab.
         const string backupDir = @"C:\some-non-existant-directory\test";
         var back = new BackupRestore();
         back.InitializeBackupSettings();
         back.SetBackupDir(backupDir);
         Assert.IsFalse(back.Execute());

         CustomAsserts.AssertReportedError("BACKUP ERROR: The specified backup directory is not accessible:");
         var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         StringAssert.Contains(backupDir, eventLogText);
      }

      [Test]
      public void TestRuleScriptFunctionNameSetByUserCannotRunCode()
      {
         LogHandler.DeleteEventLog();

         var scripting = _settings.Scripting;
         scripting.Language = "VBScript";
         File.WriteAllText(scripting.CurrentScriptFile, "Sub Harmless(message)\r\nEnd Sub");
         scripting.Enabled = true;
         scripting.Reload();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         account.AdminLevel = eAdminLevel.hAdminLevelNormal;
         account.Save();

         // A normal user logs on to the COM API and adds a rule to their own account.
         var userApp = new Application();
         Assert.IsNotNull(userApp.Authenticate(account.Address, "test"));
         var userAccount = userApp.Domains[0].Accounts.ItemByAddress[account.Address];

         var rule = userAccount.Rules.Add();
         rule.Name = "Injection";
         rule.Active = true;

         var criteria = rule.Criterias.Add();
         criteria.UsePredefined = true;
         criteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         criteria.MatchType = eRuleMatchType.eMTGreaterThan;
         criteria.MatchValue = "0";
         criteria.Save();

         var action = rule.Actions.Add();
         action.Type = eRuleActionType.eRARunScriptFunction;
         action.ScriptFunction = "Harmless(Nothing) : EventLog.Write(\"INJ\" & \"ECTED\") : Rem ";
         Assert.Throws<COMException>(() => action.Save());

         try
         {
            rule.Save();
         }
         catch (COMException)
         {
            // Refusing the rule is also acceptable.
         }

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Test");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         if (File.Exists(LogHandler.GetEventLogFileName()))
         {
            var eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
            Assert.IsFalse(eventLogText.Contains("INJECTED"), eventLogText);
         }
      }

      // Ends a JScript string with \' if the backslash is not escaped. The marker is split so
      // it only appears in the log when the injected code runs.
      private const string JScriptInjection = @"\');EventLog.Write(/INJ/.source+/ECTED/.source);//";

      private static string EncodeBase64(string value)
      {
         return Convert.ToBase64String(Encoding.UTF8.GetBytes(value));
      }
   }
}