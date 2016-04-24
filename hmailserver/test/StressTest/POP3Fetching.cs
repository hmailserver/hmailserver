using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace StressTest
{
   [TestFixture]
   public class POP3Fetching : TestFixtureBase
   {
     
      [Test]
      public void TestProcessLargeNumberOfMessages()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         var messages = new List<string>();

         string message = "From: Martin@example.com\r\n" +
                          "To: Martin@example.com\r\n" +
                          "Subject: Test\r\n" +
                          "\r\n" +
                          "Hello!";

         const int msgCount = 30000;

         for (int i = 0; i < msgCount; i++)
            messages.Add(message);

         int port = TestSetup.GetNextFreePort();
         using (var pop3Server = new Pop3ServerSimulator(1, port, messages) {SecondsToWaitBeforeTerminate = 3600})
         {
            pop3Server.StartListen();


            FetchAccount fa = account.FetchAccounts.Add();

            fa.Enabled = true;
            fa.MinutesBetweenFetch = 10;
            fa.Name = "Test";
            fa.Username = "test@example.com";
            fa.Password = "test";
            fa.UseSSL = false;
            fa.ServerAddress = "localhost";
            fa.Port = port;
            fa.ProcessMIMERecipients = false;
            fa.DaysToKeepMessages = -1;
            fa.Save();

            fa.DownloadNow();

            Pop3ClientSimulator.AssertMessageCount("test@test.com", "test", msgCount, TimeSpan.FromMinutes(60));

            pop3Server.WaitForCompletion();

            fa.Delete();

            
         }
      }

   }
}
