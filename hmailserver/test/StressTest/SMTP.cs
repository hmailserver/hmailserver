// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using RegressionTests;
using System.IO;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Net.Mail;
using RegressionTests.Shared;

namespace StressTest
{
    [TestFixture]
    public class SMTP : TestFixtureBase
    {
        [Test]
        [Description("Test to send to distribution list with 1000 members.")]
        public void TestLargeDistributionList()
        {
            // Add distribution list
            hMailServer.Accounts accounts = _domain.Accounts;

            List<string> recipients = new List<string>();
            for (int i = 0; i < 1000; i++)
            {
                hMailServer.Account account =
                    SingletonProvider<TestSetup>.Instance.AddAccount(accounts, string.Format("recipient{0}@test.com", i), "test");

                recipients.Add(account.Address);

                Marshal.ReleaseComObject(account);
            }

            Marshal.ReleaseComObject(accounts);

            SingletonProvider<TestSetup>.Instance.AddDistributionList(_domain, "list@test.com", recipients);

            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
            sw.Start();
            SMTPClientSimulator.StaticSend("test@test.com", "list@test.com", "Test", "Test message");
            sw.Stop();

            Console.WriteLine("Elapsed time: " + sw.Elapsed.TotalSeconds.ToString());

            string dataDir =
                Path.Combine(SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories.DataDirectory, "test.com");

            // Check number of accounts.
            int secondsToWait = 60;
            while (secondsToWait > 0)
            {
                try
                {
                    if (IsFolderCountReached(dataDir, 1000))
                        break;
                }
                catch (Exception)
                {

                }

                System.Threading.Thread.Sleep(10000);

                secondsToWait -= 5;
            }

            Assert.IsTrue(IsFolderCountReached(dataDir, 1000));

            _domain.Delete();

            Marshal.ReleaseComObject(_domain);
        }

       private bool IsFolderCountReached(string folder, int expectedCount)
       {
          string[] accountFolders = Directory.GetDirectories(folder);

          if (accountFolders.Length == expectedCount)
             return true;
          else
             return false;
       }

        [Test]
        [Description("Test to send to 1000 members.")]
        public void TestSendToManyRecipients()
        {
            // Add distribution list
            hMailServer.Accounts accounts = _domain.Accounts;

            List<string> recipients = new List<string>();
            for (int i = 0; i < 1000; i++)
            {
                hMailServer.Account account =
                    SingletonProvider<TestSetup>.Instance.AddAccount(accounts, string.Format("recipient{0}@test.com", i), "test");

                recipients.Add(account.Address);

                Marshal.ReleaseComObject(account);
            }

            Marshal.ReleaseComObject(accounts);

            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
            sw.Start();
            SMTPClientSimulator.StaticSend("test@test.com", recipients, "Test", "Test message");
            sw.Stop();

            Console.WriteLine("Elapsed time: " + sw.Elapsed.TotalSeconds.ToString());

            string dataDir =
                Path.Combine(SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories.DataDirectory, "test.com");

            // Check number of accounts.
            int secondsToWait = 120;

            while (secondsToWait > 0)
            {
                try
                {
                   if (IsFolderCountReached(dataDir, 1000))
                      break;
                }
                catch (Exception)
                {

                }

                // wait a minute.
                System.Threading.Thread.Sleep(3000);
                secondsToWait = secondsToWait - 3;
            }

            Assert.IsTrue(IsFolderCountReached(dataDir, 1000));

            _domain.Delete();
        }

        [Test]
        [Description("Try to send to 51 000 recipients and confirm that you're stopped after 50 000 recipients.")]
        [Ignore]
        public void TestSendToTooManyRecipients()
        {
           hMailServer.Accounts accounts = _domain.Accounts;
           hMailServer.Account senderAccount =
                    SingletonProvider<TestSetup>.Instance.AddAccount(accounts, string.Format("sender@test.com"), "test");

           List<string> recipients = new List<string>();


           TcpSocket sock = new TcpSocket();
           sock.Connect(25);
           string result = sock.ReadUntil("\r\n");
           sock.Send("EHLO test.com\r\n");
           result = sock.ReadUntil("\r\n");
           Assert.IsTrue(result.StartsWith("250"));
           sock.Send("MAIL FROM: sender@test.com\r\n");
           result = sock.ReadUntil("\r\n");
           Assert.IsTrue(result.StartsWith("250"));

           Stopwatch watch = new Stopwatch();
           watch.Start();

           for (int i = 0; i < 51000; i++)
           {
              string address = string.Format("test{0}@gmail.com", i);

              sock.Send(string.Format("RCPT TO: <{0}>\r\n", address));

              result = sock.ReadUntil("\r\n");
              if (i < 50000)
              {
                 Assert.IsTrue(result.StartsWith("250"));
              }
              else
              {
                 long perfor = watch.ElapsedMilliseconds;
                 Assert.IsFalse(result.StartsWith("250"));
              }
           }

        }

        [Test]
        [Description("Test to send 200 000 messages and confirm that the memory usage does not increase over 3MB.")]
        [Ignore]
        public void TestSendLargeBatch()
        {

           hMailServer.Accounts accounts = _domain.Accounts;
           hMailServer.Account account =
                    SingletonProvider<TestSetup>.Instance.AddAccount(accounts, string.Format("test@test.com"), "test");

           string dataDir =
               Path.Combine(SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories.DataDirectory, "test.com");

           string accountDir = Path.Combine(dataDir, "test");

           int memoryUsage = Shared.GetCurrentMemoryUsage();
           int maxMemoryUsage = memoryUsage + 5;

           const int numberOfMessages = 200000;

           for (int i = 0; i < numberOfMessages; i++)
           {
               Assert.IsTrue(SMTPClientSimulator.StaticSend("test@test.com", "test@test.com", "Test", "Test message"));

              if (i % 100 == 0)
              {
                 Shared.AssertLowMemoryUsage(maxMemoryUsage);
              }
           }

           // Check number of delivered messages.
           while (true)
           {
              if (GetNumberOfFilesInFolder(accountDir) == numberOfMessages)
                 break;

              Shared.AssertLowMemoryUsage(maxMemoryUsage);
              
              System.Threading.Thread.Sleep(60 * 1000);
           }

           _domain.Delete();

        }

        [Test]
        [Description("Test 100 messages with a 5MB attachment and check that memory does not increase too much.")]
        public void TestSend100MessagesWithAttachment()
        {
           hMailServer.Accounts accounts = _domain.Accounts;
           hMailServer.Account account =
                    SingletonProvider<TestSetup>.Instance.AddAccount(accounts, string.Format("test@test.com"), "test");

           string dataDir =
               Path.Combine(SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories.DataDirectory, "test.com");

           string accountDir = Path.Combine(dataDir, "test");

           int memoryUsage = Shared.GetCurrentMemoryUsage();

           int maxMemoryUsage = memoryUsage + 2;
           const int numberOfMessages = 100;

           string executableName = Shared.GetExecutableName();

           MailMessage mail = new MailMessage();
           mail.From = new System.Net.Mail.MailAddress("test@test.com");
           mail.To.Add("test@test.com");
           mail.Subject = "Automatiskt servertest";
           mail.Body = "Detta är ett automatiskt test av epostservern.";
           mail.BodyEncoding = System.Text.Encoding.GetEncoding(1252);
           mail.SubjectEncoding = System.Text.Encoding.GetEncoding(1252);
           mail.Attachments.Add(new Attachment(executableName));

           for (int i = 0; i < numberOfMessages; i++)
           {

              SmtpClient oClient = new SmtpClient("localhost", 25);
              oClient.Send(mail);

              if (i % 100 == 0)
              {
                 Shared.AssertLowMemoryUsage(maxMemoryUsage);
              }
           }

           WaitForFilesInFolder(accountDir, numberOfMessages, maxMemoryUsage);

           _domain.Delete();

        }

        [Test]
        [Description("Send messages being scanned by SpamAssassin.")]
        public void TestSendViaSpamAssassin()
        {
            TestSetup.AssertSpamAssassinIsRunning();

           SingletonProvider<TestSetup>.Instance.GetApp().Settings.AntiSpam.SpamAssassinEnabled = true;

           hMailServer.Accounts accounts = _domain.Accounts;
           hMailServer.Account account =
                    SingletonProvider<TestSetup>.Instance.AddAccount(accounts, string.Format("test@test.com"), "test");

           string dataDir =
               Path.Combine(SingletonProvider<TestSetup>.Instance.GetApp().Settings.Directories.DataDirectory, "test.com");

           string accountDir = Path.Combine(dataDir, "test");

           int memoryUsage = Shared.GetCurrentMemoryUsage();
           int maxMemoryUsage = memoryUsage + 2;

           const int numberOfMessages = 100;

           string executableName = Shared.GetExecutableName();

           MailMessage mail = new MailMessage();
           mail.From = new System.Net.Mail.MailAddress("test@test.com");
           mail.To.Add("test@test.com");
           mail.Subject = "Automatiskt servertest";
           mail.Body = "Detta är ett automatiskt test av epostservern.";
           mail.BodyEncoding = System.Text.Encoding.GetEncoding(1252);
           mail.SubjectEncoding = System.Text.Encoding.GetEncoding(1252);

           for (int i = 0; i < numberOfMessages; i++)
           {

              SmtpClient oClient = new SmtpClient("localhost", 25);
              oClient.Send(mail);

              if (i % 5 == 0)
              {
                 Shared.AssertLowMemoryUsage(maxMemoryUsage);
              }
           }

           WaitForFilesInFolder(accountDir, numberOfMessages, maxMemoryUsage);
           _domain.Delete();
        }

        private void WaitForFilesInFolder(string folder, int count, int maxMemoryUsage)
        {
           // Check number of delivered messages.
           while (true)
           {
              if (GetNumberOfFilesInFolder(folder) == count)
                 break;

              if (maxMemoryUsage > 0)
                 Shared.AssertLowMemoryUsage(maxMemoryUsage);

              System.Threading.Thread.Sleep(3000);
           }
        }

        private int GetNumberOfFilesInFolder(string folder)
        {
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
    }
}

