// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.API
{
   [TestFixture]
   public class Events : TestFixtureBase
   {
      [Test]
      public void TestOnAcceptMessageJScript()
      {
         _settings.Scripting.Language = "JScript";
         // First set up a script
         string script =
            @"function OnAcceptMessage(oClient, oMessage)
                           {
                              oMessage.HeaderValue('X-SpamResult') = 'TEST';
                              oMessage.Save();
                           }";

         Scripting scripting = _settings.Scripting;
         string file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SmtpClientSimulator.StaticSend(oAccount1.Address, oAccount1.Address, "Test", "SampleBody");

         // Check that the message exists
         string message = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         Assert.IsNotEmpty(message);

         Assert.Less(0, message.IndexOf("X-SpamResult: TEST"));
      }

      [Test]
      public void TestOnAcceptMessageVBScript()
      {
         string eventLogFile = _settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         // First set up a script
         string script =
            @"Sub OnAcceptMessage(oClient, oMessage)
                               oMessage.HeaderValue(""X-SpamResult"") = ""TEST""
                               oMessage.Save()
                               EventLog.Write(""Port: "" & oClient.Port)
                               EventLog.Write(""Address: "" & oClient.IPAddress)
                               EventLog.Write(""Username: "" & oClient.Username)
                              End Sub";

         Scripting scripting = _settings.Scripting;
         string file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SmtpClientSimulator.StaticSend(oAccount1.Address, oAccount1.Address, "Test", "SampleBody");

         // Check that the message exists
         string message = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         Assert.IsNotEmpty(message);

         Assert.Less(0, message.IndexOf("X-SpamResult: TEST"));


         // Check that the message exists
         message = TestSetup.ReadExistingTextFile(eventLogFile);

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains("Port: 25"));
         Assert.IsTrue(message.Contains("Address: 127"));
         Assert.IsTrue(message.Contains("Username: \"")); // Should be empty, Username isn't available at this time.
      }


      [Test]
      public void TestOnBackupCompletedJScript()
      {
         Scripting scripting = _settings.Scripting;
         scripting.Language = "JScript";

         // First set up a script
         string script =
            @"function OnBackupCompleted()
                           {
                               EventLog.Write('Backup process completed')
                           }";


         string file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         var back = new BackupRestore();
         back.InitializeBackupSettings();
         back.TestWithMessages();


         string eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("Backup process completed"));
      }

      [Test]
      public void TestOnBackupCompletedVBScript()
      {
         // First set up a script
         string script =
            @"Sub OnBackupCompleted()
                               EventLog.Write(""Backup process completed"")
                           End Sub";

         Scripting scripting = _settings.Scripting;
         string file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         var back = new BackupRestore();
         back.InitializeBackupSettings();
         back.TestWithMessages();


         string eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("Backup process completed"));
      }

      [Test]
      public void TestOnBackupFailedJScript()
      {
         Scripting scripting = _settings.Scripting;
         scripting.Language = "JScript";

         // First set up a script
         string script =
            @"function OnBackupFailed(reason)     
                           {
                               EventLog.Write('Failed: ' + reason)
                           }";


         string file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         var back = new BackupRestore();
         back.InitializeBackupSettings();
         back.SetBackupDir(@"C:\some-non-existant-directory");
         Assert.IsFalse(back.Execute());

         CustomAsserts.AssertReportedError("BACKUP ERROR: The specified backup directory is not accessible:");
         string eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("The specified backup directory is not accessible"));
      }

      [Test]
      public void TestOnBackupFailedVBScript()
      {
         // First set up a script
         string script =
            @"Sub OnBackupFailed(reason)
                               EventLog.Write(""Failed: "" & reason)
                           End Sub";

         Scripting scripting = _settings.Scripting;
         string file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         var back = new BackupRestore();
         back.InitializeBackupSettings();
         back.SetBackupDir(@"C:\some-non-existant-directory");
         Assert.IsFalse(back.Execute());

         CustomAsserts.AssertReportedError("BACKUP ERROR: The specified backup directory is not accessible:");
         string eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("The specified backup directory is not accessible"));
      }

      [Test]
      public void TestOnClientConnectJScript()
      {
         Application app = SingletonProvider<TestSetup>.Instance.GetApp();
         Scripting scripting = app.Settings.Scripting;

         scripting.Language = "JScript";

         string script = "function OnClientConnect(oClient) " + Environment.NewLine +
                         "{" + Environment.NewLine +
                         " EventLog.Write('Port: ' + oClient.Port); " + Environment.NewLine +
                         " EventLog.Write('Address: ' + oClient.IPAddress); " + Environment.NewLine +
                         " EventLog.Write('Username: ' + oClient.Username); " + Environment.NewLine +
                         "}" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         string eventLogFile = app.Settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         var socket = new TcpConnection();
         Assert.IsTrue(socket.IsPortOpen(25));

         // Check that the message exists
         string message = TestSetup.ReadExistingTextFile(eventLogFile);

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains("Port: 25"));
         Assert.IsTrue(message.Contains("Address: 127"));
         Assert.IsTrue(message.Contains("Username: \"")); // Should be empty, Username isn't available at this time.
      }

      [Test]
      public void TestOnClientConnectVBScript()
      {
         Application app = SingletonProvider<TestSetup>.Instance.GetApp();
         Scripting scripting = app.Settings.Scripting;

         string script = "Sub OnClientConnect(oClient) " + Environment.NewLine +
                         " EventLog.Write(\"Port: \" & oClient.Port) " + Environment.NewLine +
                         " EventLog.Write(\"Address: \" & oClient.IPAddress) " + Environment.NewLine +
                         " EventLog.Write(\"Username: \" & oClient.Username) " + Environment.NewLine +
                         "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         string eventLogFile = app.Settings.Logging.CurrentEventLog;
         if (File.Exists(eventLogFile))
            File.Delete(eventLogFile);

         var socket = new TcpConnection();
         Assert.IsTrue(socket.IsPortOpen(25));

         // Check that the message exists
         string message = TestSetup.ReadExistingTextFile(eventLogFile);

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains("Port: 25"));
         Assert.IsTrue(message.Contains("Address: 127"));
         Assert.IsTrue(message.Contains("Username: \"")); // Should be empty, Username isn't available at this time.
      }

      [Test]
      public void TestOnDeliverMessageJScript()
      {
         Scripting scripting = _settings.Scripting;
         scripting.Language = "JScript";
         // First set up a script
         string script =
            @"function OnDeliverMessage(oMessage)
                           {
                               oMessage.HeaderValue('X-SpamResult') = 'TEST2';
                               oMessage.Save();
                           }";


         string file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SmtpClientSimulator.StaticSend(oAccount1.Address, oAccount1.Address, "Test", "SampleBody");

         // Check that the message exists
         string message = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         Assert.IsNotEmpty(message);

         Assert.Less(0, message.IndexOf("X-SpamResult: TEST2"));
      }

      [Test]
      public void TestOnDeliveryFailedJScript()
      {
         Scripting scripting = _settings.Scripting;
         scripting.Language = "JScript";

         // First set up a script
         string script = "function OnDeliveryFailed(oMessage, sRecipient, sErrorMessage) {" + Environment.NewLine +
                         " EventLog.Write('File: ' + oMessage.FileName); " + Environment.NewLine +
                         " EventLog.Write('Recipient: ' + sRecipient); " + Environment.NewLine +
                         " EventLog.Write('Error: ' + sErrorMessage); " + Environment.NewLine +
                         "}";


         string file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend(oAccount1.Address, "user@dummy.example.com", "Test", "SampleBody");

         // Make sure that the message is deliverd and bounced.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         string eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("File: "), eventLogText);
         Assert.IsTrue(eventLogText.Contains("Recipient: user@dummy.example.com"), eventLogText);
         Assert.IsTrue(eventLogText.Contains("No mail servers appear to exists"), eventLogText);
      }

      [Test]
      public void TestOnDeliveryFailedVBScript()
      {
         // First set up a script
         string script = "Sub OnDeliveryFailed(oMessage, sRecipient, sErrorMessage)" + Environment.NewLine +
                         " EventLog.Write(\"File: \" & oMessage.FileName) " + Environment.NewLine +
                         " EventLog.Write(\"Recipient: \" & sRecipient) " + Environment.NewLine +
                         " EventLog.Write(\"Error: \" & sErrorMessage) " + Environment.NewLine +
                         " End Sub";

         Scripting scripting = _settings.Scripting;
         string file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         // Add an account and send a message to it.
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend(oAccount1.Address, "user@dummy.example.com", "Test", "SampleBody");

         // Make sure that the message is deliverd and bounced.
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         string eventLogText = TestSetup.ReadExistingTextFile(LogHandler.GetEventLogFileName());
         Assert.IsTrue(eventLogText.Contains("File: "));
         Assert.IsTrue(eventLogText.Contains("Recipient: user@dummy.example.com"));
         Assert.IsTrue(eventLogText.Contains("No mail servers appear to exists"));
      }

      [Test]
      public void TestOnDeliveryStartVBScript()
      {
         Application app = SingletonProvider<TestSetup>.Instance.GetApp();
         Scripting scripting = app.Settings.Scripting;

         string script = "Sub OnDeliveryStart(oMessage) " + Environment.NewLine +
                         " EventLog.Write(\"Delivering message: \" & oMessage.FileName) " + Environment.NewLine +
                         "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");

         // Wait for the message to be delivered.
         Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         string eventLogText = TestSetup.ReadExistingTextFile(app.Settings.Logging.CurrentEventLog);
         Assert.IsTrue(eventLogText.Contains("Delivering message"));
      }

      [Test]
      public void TestOnErrorJScript()
      {
         Application app = SingletonProvider<TestSetup>.Instance.GetApp();
         Scripting scripting = app.Settings.Scripting;
         scripting.Language = "JScript";
         string script = "function OnError(iSeverity, iError, sSource, sDescription) {" + Environment.NewLine +
                         " EventLog.Write('Severity: ' + iSeverity) " + Environment.NewLine +
                         " EventLog.Write('Error: ' + iError) " + Environment.NewLine +
                         " EventLog.Write('Source: ' + sSource) " + Environment.NewLine +
                         " EventLog.Write('Description: ' + sDescription) " + Environment.NewLine +
                         "}" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         IMAPFolder inbox = account.IMAPFolders.get_ItemByName("Inbox");


         string deletedMessageText = app.Settings.ServerMessages.get_ItemByName("MESSAGE_FILE_MISSING").Text;
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");

         CustomAsserts.AssertFolderMessageCount(inbox, 1);
         hMailServer.Message message = inbox.Messages[0];
         File.Delete(message.Filename);
         string text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         Assert.IsTrue(text.Contains(deletedMessageText.Replace("%MACRO_FILE%", message.Filename)));
         CustomAsserts.AssertReportedError("Message retrieval failed because message file");

         string eventLogText = TestSetup.ReadExistingTextFile(app.Settings.Logging.CurrentEventLog);
         Assert.IsTrue(eventLogText.Contains("Description: Message retrieval failed"));
      }

      [Test]
      public void TestOnErrorVBScript()
      {
         Application app = SingletonProvider<TestSetup>.Instance.GetApp();
         Scripting scripting = app.Settings.Scripting;

         string script = "Sub OnError(iSeverity, iError, sSource, sDescription) " + Environment.NewLine +
                         " EventLog.Write(\"Severity: \" & iSeverity) " + Environment.NewLine +
                         " EventLog.Write(\"Error: \" & iError) " + Environment.NewLine +
                         " EventLog.Write(\"Source: \" & sSource) " + Environment.NewLine +
                         " EventLog.Write(\"Description: \" & sDescription) " + Environment.NewLine +
                         "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);

         scripting.Enabled = true;
         scripting.Reload();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         IMAPFolder inbox = account.IMAPFolders.get_ItemByName("Inbox");


         string deletedMessageText = app.Settings.ServerMessages.get_ItemByName("MESSAGE_FILE_MISSING").Text;
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody");

         CustomAsserts.AssertFolderMessageCount(inbox, 1);
         hMailServer.Message message = inbox.Messages[0];
         File.Delete(message.Filename);
         string text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         Assert.IsTrue(text.Contains(deletedMessageText.Replace("%MACRO_FILE%", message.Filename)));
         CustomAsserts.AssertReportedError("Message retrieval failed because message file");

         string eventLogText = TestSetup.ReadExistingTextFile(app.Settings.Logging.CurrentEventLog);
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
         string script = "Sub OnExternalAccountDownload(oFetchAccount, oMessage, sRemoteUID)" + Environment.NewLine +
                         " EventLog.Write(\"UID: \" & sRemoteUID) " + Environment.NewLine +
                         " EventLog.Write(\"FetchAccount: \" & oFetchAccount.Name) " + Environment.NewLine +
                         " If Not oMessage Is Nothing Then " + Environment.NewLine +
                         "   EventLog.Write(\"From: \" & oMessage.FromAddress) " + Environment.NewLine +
                         "   EventLog.Write(\"Filename: \" & oMessage.FileName) " + Environment.NewLine +
                         " Else " + Environment.NewLine +
                         "   EventLog.Write(\"Message details missing\") " + Environment.NewLine +
                         " End If" + Environment.NewLine +
                         " if sRemoteUID = \"UniqueID-" + messages[1].GetHashCode() + "\" Then " +
                         Environment.NewLine +
                         "   Result.Value = 2  " + Environment.NewLine +
                         "   Result.Parameter = 5  " + Environment.NewLine +
                         " End If " + Environment.NewLine +
                         " End Sub";

         Scripting scripting = _settings.Scripting;
         string file = scripting.CurrentScriptFile;
         File.WriteAllText(file, script);
         scripting.Enabled = true;
         scripting.Reload();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");

         FetchAccount fa;

         int port = TestSetup.GetNextFreePort();
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

            string eventLogFile = _settings.Logging.CurrentEventLog;
            string logContents = TestSetup.ReadExistingTextFile(eventLogFile);

            Assert.IsTrue(logContents.Contains("FetchAccount: " + fa.Name));

            Assert.IsTrue(logContents.Contains("From: Martin@example1.com"));
            Assert.IsTrue(logContents.Contains("From: Martin@example2.com"));
            Assert.IsTrue(logContents.Contains("From: Martin@example3.com"));

            string appLogContent = LogHandler.ReadCurrentDefaultLog();

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
   }
}