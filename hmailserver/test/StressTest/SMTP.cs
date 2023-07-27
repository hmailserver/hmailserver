// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Net;
using System.Text;
using hMailServer;
using NUnit.Framework;
using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Net.Mail;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using Attachment = System.Net.Mail.Attachment;
using RetryHelper = hMailServer.Test.Infrastructure.RetryHelper;

namespace StressTest
{
   [TestFixture]
   public class Smtp : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }

      [Test]
      [Description("Test to send to distribution list with 1000 members.")]
      public void TestLargeDistributionList()
      {
         // Add distribution list
         hMailServer.Accounts accounts = _domain.Accounts;

         var recipients = new List<string>();
         for (int i = 0; i < 1000; i++)
         {
            hMailServer.Account account =
                SingletonProvider<TestSetup>.Instance.AddAccount(accounts, string.Format("recipient{0}@test.com", i), "test");

            recipients.Add(account.Address);

            Marshal.ReleaseComObject(account);
         }

         Marshal.ReleaseComObject(accounts);

         SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list@test.com", recipients);

         var sw = new Stopwatch();
         sw.Start();
         SmtpClientSimulator.StaticSend("test@test.com", "list@test.com", "Test", "Test message");
         sw.Stop();

         Console.WriteLine("Elapsed time: " + sw.Elapsed.TotalSeconds);

         string dataDir =
             Path.Combine(SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories.DataDirectory, "test.com");

         // Check number of accounts.
         RetryHelper.TryAction(() =>
         {
            if (IsFolderCountReached(dataDir, 1000))
               return;

            throw new Exception("Not enough files in folder yet.");
         }, TimeSpan.FromSeconds(10), TimeSpan.FromMinutes(5));
      }

      private bool IsFolderCountReached(string folder, int expectedCount)
      {
         string[] accountFolders = Directory.GetDirectories(folder);

         if (accountFolders.Length == expectedCount)
            return true;
         
         return false;
      }

      [Test]
      [Description("Test to send to 1000 members.")]
      public void TestSendToManyRecipients()
      {
         // Add distribution list
         hMailServer.Accounts accounts = _domain.Accounts;

         var recipients = new List<string>();
         for (int i = 0; i < 1000; i++)
         {
            hMailServer.Account account =
                SingletonProvider<TestSetup>.Instance.AddAccount(accounts, string.Format("recipient{0}@test.com", i), "test");

            recipients.Add(account.Address);

            Marshal.ReleaseComObject(account);

            if (i % 10 == 0)
            {
               TestTracer.WriteTraceInfo("Creating accounts: {0}/{1}", i, 1000);
            }
         }

         Marshal.ReleaseComObject(accounts);

         var sw = new Stopwatch();
         sw.Start();
         SmtpClientSimulator.StaticSend("test@test.com", recipients, "Test", "Test message");
         sw.Stop();

         Console.WriteLine("Elapsed time: " + sw.Elapsed.TotalSeconds);

         string dataDir =
             Path.Combine(SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories.DataDirectory, "test.com");

         // Check number of accounts.
         RetryHelper.TryAction(() =>
         {
            if (IsFolderCountReached(dataDir, 1000))
               return;
            
            throw new Exception("Not enough files in folder yet.");
         }, TimeSpan.FromSeconds(5), TimeSpan.FromMinutes(2));
      }

      [Test]
      [Description("Try to send to 51 000 recipients and confirm that you're stopped after 50 000 recipients.")]
      public void TestSendToTooManyRecipients()
      {
         hMailServer.Accounts accounts = _domain.Accounts;
         SingletonProvider<TestSetup>.Instance.AddAccount(accounts, string.Format("sender@test.com"), "test");

         var sock = new TcpConnection();
         sock.Connect(25);
         sock.ReadUntil("\r\n");
         string result;
         sock.Send("EHLO test.com\r\n");
         result = sock.ReadUntil("\r\n");
         Assert.IsTrue(result.StartsWith("250"));
         sock.Send("MAIL FROM: sender@test.com\r\n");
         result = sock.ReadUntil("\r\n");
         Assert.IsTrue(result.StartsWith("250"));

         const int recipientCount = 51000;

         for (int i = 1; i <= recipientCount; i++)
         {
            string address = string.Format("test{0}@gmail.com", i);

            sock.Send(string.Format("RCPT TO: <{0}>\r\n", address));
            result = sock.ReadUntil("\r\n");
            if (i <= 50000)
            {
               Assert.IsTrue(result.StartsWith("250"));
            }
            else
            {
               Assert.IsFalse(result.StartsWith("250"));
            }

            if (i % 100 == 0)
            {
               TestTracer.WriteTraceInfo("{0}/{1}", i, recipientCount);
            }

         }

      }

      [Test]
      [Description("Test to send 200 000 messages and confirm that the memory usage does not increase over 3MB.")]
      public void TestSend200KMessages()
      {
         string dataDir =
             Path.Combine(SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories.DataDirectory, "test.com");

         string accountDir = Path.Combine(dataDir, "test");

         int memoryUsage = Shared.GetCurrentMemoryUsage();
         int maxMemoryUsage = memoryUsage + 20;

         const int numberOfMessages = 200000;
         var stopwatch = Stopwatch.StartNew();

         for (int i = 1; i <= numberOfMessages; i++)
         {
            SmtpClientSimulator.StaticSend("test@test.com", "test@test.com", "Test", "Test message");

            if (i % 100 == 0)
            {
               double messagesPerSecond = 100 / stopwatch.Elapsed.TotalSeconds;
               long bytesUsed = Shared.AssertLowMemoryUsage(maxMemoryUsage);
               TestTracer.WriteTraceInfo("{0}/{1}. Messages per second: {2}, Bytes used: {3}", i, numberOfMessages, messagesPerSecond, bytesUsed);

               stopwatch.Reset();
               stopwatch.Start();
               ;
            }
         }

         // Check number of delivered messages.
         RetryHelper.TryAction(() =>
            {
               Shared.AssertLowMemoryUsage(maxMemoryUsage);
               
               int actualNumberOfMessages = GetNumberOfFilesInFolder(accountDir);
               TestTracer.WriteTraceInfo("{0}/{1}", actualNumberOfMessages, numberOfMessages);

               Assert.AreEqual(numberOfMessages, actualNumberOfMessages);
            }, TimeSpan.FromSeconds(30), TimeSpan.FromHours(12));

         _domain.Delete();

      }

      [Test]
      [Description("Test a bunch of messages with a 5MB attachment and check that memory does not increase")]
      public void TestSendAttachments()
      {
         string dataDir =
             Path.Combine(SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories.DataDirectory, "test.com");

         string accountDir = Path.Combine(dataDir, "test");

         int memoryUsage = Shared.GetCurrentMemoryUsage();
         int maxMemoryUsage = memoryUsage + 5;

         const int numberOfMessages = 1000;

         string executableName = Shared.GetExecutableName();

         var mail = new MailMessage {From = new MailAddress("test@test.com")};
         mail.To.Add("test@test.com");
         mail.Subject = "Automatic test";
         mail.Body = "Automatic test";
         mail.BodyEncoding = Encoding.GetEncoding(1252);
         mail.SubjectEncoding = Encoding.GetEncoding(1252);
         mail.Attachments.Add(new Attachment(executableName));

         for (int i = 1; i <= numberOfMessages; i++)
         {
            var oClient = new SmtpClient("127.0.0.1", 25);
            oClient.Send(mail);

            if (i%10 == 0)
            {
               TestTracer.WriteTraceInfo("{0}/{1}", i, 1000);
               
               WaitForFilesInFolder(accountDir, i);
               Shared.AssertLowMemoryUsage(maxMemoryUsage);
            }
         }

         


         _domain.Delete();

      }


      /// <summary>
      /// What happens if we connect to port 25 and send a 1000000000 character string with no new line.
      /// </summary>
      [Test]
      public void TestLongSMTPCommand()
      {
         var socket = new TcpConnection();
         socket.Connect(25);

         // Build a large string...
         var sb = new StringBuilder();
         sb.Append("A01");
         for (int i = 0; i < 1000000; i++)
         {
            sb.Append("01234567890");
         }

         sb.Append(Environment.NewLine);

         for (int i = 0; i < 100; i++)
         {
            try
            {
               socket.Send(sb.ToString());
            }
            catch (Exception)
            {
               return;
            }


            if ((i % 10) == 0)
            {
               TestTracer.WriteTraceInfo("{0}/{1}", i, 100);
            }
         }

         socket.Send("\r\n");
         socket.Receive();

         socket.Disconnect();
      }

      /// <summary>
      /// What happens if we start a SMTP data session and send 100MB with newline.
      /// The memory usage should be below 20MB.
      /// </summary>
      [Test]
      public void TestLongSMTPDataSessionIncludingNewline()
      {
         long memoryUsage = Shared.GetCurrentMemoryUsage();

         _application.Settings.MaxMessageSize = 0;

         var socket = new TcpConnection();
         socket.Connect(25);
         socket.Receive();

         socket.Send("HELO test.com\r\n");
         socket.Receive();

         // Build a large string...
         var sb = new StringBuilder();
         sb.Append("A01");
         for (int i = 0; i < 10000; i++)
         {
            sb.Append("01234567890012345678900123456789001234567890012345678900123456789001234567890012345678900123456789001234567890\r\n");
         }

         sb.Append(Environment.NewLine);

         socket.Send("MAIL FROM: test@test.com\r\n");
         socket.Receive();
         socket.Send("RCPT TO: test@test.com\r\n");
         socket.Receive();
         socket.Send("DATA\r\n");
         socket.Receive();

         for (int i = 0; i < 100; i++)
         {
            Shared.AssertLowMemoryUsage(memoryUsage + 30);

            try
            {
               socket.Send(sb.ToString());
            }
            catch (Exception)
            {
               return;
            }
         }

         socket.Send("\r\n.\r\n");
         string result = socket.Receive();
         Assert.IsTrue(result.StartsWith("250"));

         socket.Send("QUIT\r\n");
         socket.Receive();
         socket.Disconnect();

         Pop3ClientSimulator.AssertMessageCount(_domain.Accounts[0].Address, "test", 1);
      }


      /// <summary>
      /// What happens if we start a SMTP data session and send 1GB with newline...
      /// </summary>
      [Test]
      public void TestLongSMTPDataSessionWithoutNewline()
      {
         long memoryUsage = Shared.GetCurrentMemoryUsage();

         var socket = new TcpConnection();
         socket.Connect(25);
         socket.Receive();

         socket.Send("HELO test.com\r\n");
         socket.Receive();

         // Build a large string...
         var sb = new StringBuilder();
         sb.Append("A01");
         for (int i = 0; i < 10000; i++)
         {
            sb.Append("01234567890012345678900123456789001234567890012345678900123456789001234567890012345678900123456789001234567890");
         }

         socket.Send("MAIL FROM: test@test.com\r\n");
         socket.Receive();
         socket.Send("RCPT TO: test@test.com\r\n");
         socket.Receive();
         socket.Send("DATA\r\n");
         socket.Receive();

         for (int i = 1; i <= 1000; i++)
         {
            Shared.AssertLowMemoryUsage(memoryUsage + 30);

            try
            {
               socket.Send(sb.ToString());
            }
            catch (Exception)
            {
               return;
            }

            if ((i%10) == 0)
            {
               TestTracer.WriteTraceInfo("{0}/{1}", i, 1000);
            }
         }

         socket.Send("\r\n.\r\n");
         string result = socket.Receive();

         Assert.IsTrue(result.StartsWith("554 Too long line was received. Transmission aborted."));

         socket.Send("QUIT\r\n");
         socket.Receive();


         socket.Disconnect();
      }


      [Test]
      [Description("Send messages being scanned by SpamAssassin.")]
      public void TestSendViaSpamAssassin()
      {
         CustomAsserts.AssertSpamAssassinIsRunning();

         SingletonProvider<TestSetup>.Instance.GetApp().Settings.AntiSpam.SpamAssassinEnabled = true;

         string dataDir =
             Path.Combine(SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories.DataDirectory, "test.com");

         string accountDir = Path.Combine(dataDir, "test");

         int memoryUsage = Shared.GetCurrentMemoryUsage();
         int maxMemoryUsage = memoryUsage + 2;

         const int numberOfMessages = 100;

         var mail = new MailMessage();
         mail.From = new MailAddress("test@test.com");
         mail.To.Add("test@test.com");
         mail.Subject = "Automatic server test";
         mail.Body = "Automatic server test";
         mail.BodyEncoding = Encoding.GetEncoding(1252);
         mail.SubjectEncoding = Encoding.GetEncoding(1252);

         for (int i = 1; i <= numberOfMessages; i++)
         {

            var oClient = new SmtpClient("localhost", 25);
            oClient.Send(mail);

            if (i % 5 == 0)
            {
               Shared.AssertLowMemoryUsage(maxMemoryUsage);
               TestTracer.WriteTraceInfo("{0}/{1}", i, numberOfMessages);
            }
         }

         WaitForFilesInFolder(accountDir, numberOfMessages);
         _domain.Delete();
      }

      private void WaitForFilesInFolder(string folder, int expectedCount)
      {
         // Check number of delivered messages.
         RetryHelper.TryAction(() =>
            {
               int filesInFolder = GetNumberOfFilesInFolder(folder);
               Assert.AreEqual(expectedCount, filesInFolder);
            }, TimeSpan.FromSeconds(5), TimeSpan.FromMinutes(5));
      }

      private int GetNumberOfFilesInFolder(string folder)
      {
         if (!Directory.Exists(folder))
            return 0;

         string[] accountFolders = Directory.GetDirectories(folder);
         string[] files = Directory.GetFiles(folder);

         int totalCount = 0;

         foreach (string subFolder in accountFolders)
         {
            string[] filesInSubFolder = Directory.GetFiles(subFolder);
            totalCount += filesInSubFolder.Length;
         }

         totalCount += files.Length;
         return totalCount;
      }

      [Test]
      public void TestManyInvalidConnectDisconnect()
      {  
         var status = _application.Status;

         int before = status.get_SessionCount(eSessionType.eSTSMTP);

         int count = 1000;

         var connections = new List<TcpConnection>();
         for (int i = 0; i < count; i++)
         {
            var conn = new TcpConnection(false);
            conn.Connect(25);

            connections.Add(conn);
         }

         RetryHelper.TryAction(() =>
         {
            int connCount = status.get_SessionCount(eSessionType.eSTSMTP);
            Assert.GreaterOrEqual(connCount, count);
         }, TimeSpan.FromSeconds(10), TimeSpan.FromSeconds(1));

         foreach (var conn in connections)
            conn.Dispose();

         RetryHelper.TryAction(() =>
         {
            int after = status.get_SessionCount(eSessionType.eSTSMTP);
            Assert.GreaterOrEqual(before, after);

         }, TimeSpan.FromSeconds(30), TimeSpan.FromSeconds(1));

         
      }
   }
}

