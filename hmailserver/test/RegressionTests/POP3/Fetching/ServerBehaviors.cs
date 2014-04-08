// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections.Generic;
using System.IO;
using System.Threading;
using NUnit.Framework;
using RegressionTests.POP3.Fetching;
using RegressionTests.SMTP;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.POP3
{
   [TestFixture]
    public class ServerBehaviors : TestFixtureBase
   {
      const string _message = "Received: from example.com (example.com [1.2.3.4]) by mail.host.edu\r\n" +
                                "From: Martin@example.com\r\n" +
                                "To: Martin@example.com\r\n" +
                                "Subject: Test\r\n" +
                                "\r\n" +
                                "Hello!";

      private int _serverPort;

      private Account _account;


      [SetUp]
      public void SetUpTest()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@test.com", "test");

         _serverPort = TestSetup.GetNextFreePort();
      }

      private FetchAccount CreateFetchAccount()
      {
         FetchAccount fa = _account.FetchAccounts.Add();

         fa.Enabled = true;
         fa.MinutesBetweenFetch = 10;
         fa.Name = "Test";
         fa.Username = "test@example.com";
         fa.Password = "test";
         fa.UseSSL = false;
         fa.ServerAddress = "localhost";
         fa.Port = _serverPort;
         fa.ProcessMIMERecipients = false;
         fa.DaysToKeepMessages = -1;
         fa.UseAntiSpam = false;
         fa.UseAntiVirus = false;
         fa.Save();
         return fa;
      }

      
      private POP3Server CreateServer()
      {
         return CreateServer(_message);
      }


      private POP3Server CreateServer(string message)
      {
         var messages = new List<string> { message };
         return new POP3Server(1, _serverPort, messages);
      }


      [Test]
      [Description("Simulates that the POP3 server disconnects directly after having sent the entire message.")]
      public void TestDisconnectAfterRetrCommand()
      {
         
         var pop3Server = CreateServer();
         pop3Server.SecondsToWaitBeforeTerminate = 180;
         pop3Server.DisconnectAfterRetrCompletion = true;
         pop3Server.StartListen();

         // Connection will be dropped after we perform the RETR command.
         var fetchAccount = CreateFetchAccount();
         fetchAccount.DownloadNow();
         
         pop3Server.WaitForCompletion();
         LockHelper.WaitForUnlock(fetchAccount);

         // Simulate another download where the connection is not dropPed.
         pop3Server = CreateServer();
         pop3Server.SecondsToWaitBeforeTerminate = 180;
         pop3Server.DisconnectAfterRetrCompletion = false;
         pop3Server.StartListen();

         fetchAccount.DownloadNow();
         pop3Server.WaitForCompletion();

         POP3Simulator.AssertMessageCount(_account.Address, "test", 2);
      }

      [Test]
      [Description("Simulates that the POP3 server delivers the +OK, message and \r\n.\r\n in a single buffer.")]
      public void TestSingleBuffer()
      {
         FetchAccount fetchAccount;

         using (var pop3Server = CreateServer())
         {
            pop3Server.SendBufferMode = POP3Server.BufferMode.SingleBuffer;
            pop3Server.StartListen();

            // Fetch message
            fetchAccount = CreateFetchAccount();
            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);
         }
         
         // Do it again
         using (var pop3Server = CreateServer())
         {
            pop3Server.SendBufferMode = POP3Server.BufferMode.SingleBuffer;
            pop3Server.StartListen();

            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);

            POP3Simulator.AssertMessageCount(_account.Address, "test", 2);

            string downloadedMessage = POP3Simulator.AssertGetFirstMessageText(_account.Address, "test");
            Assert.IsTrue(downloadedMessage.Contains(_message));
         }
      }

      [Test]
      [Description("Simulates that the POP3 server delivers the message and \r\n.\r\n in a single buffer.")]
      public void TestMessageAndTerminationInSameBuffer()
      {
         FetchAccount fetchAccount;

         using (var pop3Server = CreateServer())
         {
            pop3Server.SendBufferMode = POP3Server.BufferMode.MessageAndTerminatonTogether;
            pop3Server.StartListen();

            fetchAccount = CreateFetchAccount();

            // Fetch message
            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);
         }
         

            // Do it again
         using (var pop3Server = CreateServer())
         {
            pop3Server.SendBufferMode = POP3Server.BufferMode.MessageAndTerminatonTogether;
            pop3Server.StartListen();

            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);

            POP3Simulator.AssertMessageCount(_account.Address, "test", 2);
            string downloadedMessage = POP3Simulator.AssertGetFirstMessageText(_account.Address, "test");
            Assert.IsTrue(downloadedMessage.Contains(_message));
         }
      }

      //[Test]
      //public void TestSampleMessages()
      //{
      //   string folder = @"C:\Temp\hMailServer IssueTracker ID 342 Example EML Files";

      //   var files = Directory.GetFiles(folder, "*.eml");

      //   foreach (var file in files)
      //   {
      //      string content = File.ReadAllText(file);

      //      TestMessage(content);

      //      SetUp();
      //      TestSetup();
      //   }
      //}

      private void TestMessage(string messageContent)
      {
         FetchAccount fetchAccount;

         using (var pop3Server = CreateServer(messageContent))
         {
            pop3Server.SendBufferMode = POP3Server.BufferMode.SingleBuffer;
            pop3Server.StartListen();

            fetchAccount = CreateFetchAccount();

            // Fetch message
            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);
         }

         // Do it again
         using (var pop3Server = CreateServer(messageContent))
         {
            pop3Server.SendBufferMode = POP3Server.BufferMode.MessageAndTerminatonTogether;
            pop3Server.StartListen();

            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);

            POP3Simulator.AssertMessageCount(_account.Address, "test", 2);
            string downloadedMessage = POP3Simulator.AssertGetFirstMessageText(_account.Address, "test");
            Assert.IsTrue(downloadedMessage.Contains(messageContent));
         }
      }
   }
}