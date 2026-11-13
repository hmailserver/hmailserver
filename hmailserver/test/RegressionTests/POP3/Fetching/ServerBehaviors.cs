// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections.Generic;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.POP3.Fetching;
using RegressionTests.Shared;

namespace RegressionTests.POP3
{
   [TestFixture]
   public class ServerBehaviors : TestFixtureBase
   {
      [SetUp]
      public void SetUpTest()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");

         _serverPort = TestSetup.GetNextFreePort();
      }

      private const string _message = "Received: from example.com (example.com [1.2.3.4]) by mail.host.edu\r\n" +
                                      "From: Martin@example.com\r\n" +
                                      "To: Martin@example.com\r\n" +
                                      "Subject: Test\r\n" +
                                      "\r\n" +
                                      "Hello!";

      private int _serverPort;

      private Account _account;

      private FetchAccount CreateFetchAccount()
      {
         return CreateFetchAccount(_account, _serverPort);
      }

      private static FetchAccount CreateFetchAccount(Account account, int serverPort)
      {
         var fa = account.FetchAccounts.Add();

         fa.Enabled = true;
         fa.MinutesBetweenFetch = 10;
         fa.Name = "Test";
         fa.Username = "test@example.com";
         fa.Password = "test";
         fa.UseSSL = false;
         fa.ServerAddress = "localhost";
         fa.Port = serverPort;
         fa.ProcessMIMERecipients = false;
         fa.DaysToKeepMessages = -1;
         fa.UseAntiSpam = false;
         fa.UseAntiVirus = false;
         fa.Save();
         return fa;
      }


      private Pop3ServerSimulator CreateServer()
      {
         return CreateServer(_message);
      }


      private Pop3ServerSimulator CreateServer(string message)
      {
         var messages = new List<string> { message };
         return new Pop3ServerSimulator(1, _serverPort, messages);
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

         Pop3ClientSimulator.AssertMessageCount(_account.Address, "test", 2);
      }

      [Test]
      [Description("Simulates that the POP3 server delivers the +OK, message and \r\n.\r\n in a single buffer.")]
      public void TestSingleBuffer()
      {
         FetchAccount fetchAccount;

         using (var pop3Server = CreateServer())
         {
            pop3Server.SendBufferMode = Pop3ServerSimulator.BufferMode.SingleBuffer;
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
            pop3Server.SendBufferMode = Pop3ServerSimulator.BufferMode.SingleBuffer;
            pop3Server.StartListen();

            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);

            Pop3ClientSimulator.AssertMessageCount(_account.Address, "test", 2);

            var downloadedMessage = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

            StringAssert.Contains("Received: from example.com", downloadedMessage);
            StringAssert.Contains("Hello!", downloadedMessage);
         }
      }

      [Test]
      [Description("Simulates that the POP3 server delivers the message and \r\n.\r\n in a single buffer.")]
      public void TestMessageAndTerminationInSameBuffer()
      {
         FetchAccount fetchAccount;

         using (var pop3Server = CreateServer())
         {
            pop3Server.SendBufferMode = Pop3ServerSimulator.BufferMode.MessageAndTerminatonTogether;
            pop3Server.StartListen();

            fetchAccount = CreateFetchAccount();

            // Fetch message
            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);
         }


         // Do it again, to make sure 
         using (var pop3Server = CreateServer())
         {
            pop3Server.SendBufferMode = Pop3ServerSimulator.BufferMode.MessageAndTerminatonTogether;
            pop3Server.StartListen();

            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);
         }

         Pop3ClientSimulator.AssertMessageCount(_account.Address, "test", 2);
         var downloadedMessage = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

         StringAssert.Contains("Received: from example.com", downloadedMessage);
         StringAssert.Contains("Hello!", downloadedMessage);
      }

      [Test]
      [Description("A large mailbox gives a UIDL response over 20 MB. It should still be downloaded.")]
      public void TestLargeUidlResponse()
      {
         using (var pop3Server = CreateServer())
         {
            // About 24 MB. The server disconnects when the first padding message is requested.
            pop3Server.PaddingUidCount = 300000;
            pop3Server.SecondsToWaitBeforeTerminate = 120;
            pop3Server.StartListen();

            var fetchAccount = CreateFetchAccount();
            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);
         }

         Pop3ClientSimulator.AssertMessageCount(_account.Address, "test", 1);
         var downloadedMessage = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
         StringAssert.Contains("Hello!", downloadedMessage);
      }

      [TestCase(Pop3ServerSimulator.OversizedResponseType.Capa, "CAPA")]
      [TestCase(Pop3ServerSimulator.OversizedResponseType.UidlLine, "UIDL")]
      [TestCase(Pop3ServerSimulator.OversizedResponseType.UidlLongLines, "UIDL")]
      [TestCase(Pop3ServerSimulator.OversizedResponseType.RetrFirstLine, "RETR")]
      [Description("hMailServer should disconnect instead of buffering a never-ending response.")]
      public void TestOversizedResponseShouldDisconnect(Pop3ServerSimulator.OversizedResponseType responseType,
         string command)
      {
         AssertOversizedResponseDisconnects(_account, _serverPort, responseType, command);
      }

      internal static void AssertOversizedResponseDisconnects(Account account, int serverPort,
         Pop3ServerSimulator.OversizedResponseType responseType, string command)
      {
         FetchAccount fetchAccount;

         using (var pop3Server = new Pop3ServerSimulator(1, serverPort, new List<string> { _message }))
         {
            pop3Server.OversizedResponse = responseType;
            // The UIDL total size limit is 100 MB.
            pop3Server.OversizedResponseSize = responseType == Pop3ServerSimulator.OversizedResponseType.UidlTotalSize
               ? 120 * 1024 * 1024
               : 50 * 1024 * 1024;
            pop3Server.SecondsToWaitBeforeTerminate = 120;
            pop3Server.StartListen();

            fetchAccount = CreateFetchAccount(account, serverPort);

            // CAPA is only sent when STARTTLS may be used.
            if (responseType == Pop3ServerSimulator.OversizedResponseType.Capa)
            {
               fetchAccount.ConnectionSecurity = eConnectionSecurity.eCSSTARTTLSOptional;
               fetchAccount.Save();
            }

            fetchAccount.DownloadNow();
            pop3Server.WaitForCompletion();

            Assert.IsTrue(pop3Server.ClientDisconnectedDuringOversizedResponse,
               "hMailServer kept reading the oversized response");
         }

         LockHelper.WaitForUnlock(fetchAccount);
         ServiceRestartDetector.ValidateProcessId();

         var expectedMessage = string.Format(
            "The download of messages from external account {0} failed. The response to {1} was too large.",
            fetchAccount.Name, command);
         Assert.IsTrue(LogHandler.DefaultLogContains(expectedMessage), "Log did not contain: " + expectedMessage);
      }
   }
}