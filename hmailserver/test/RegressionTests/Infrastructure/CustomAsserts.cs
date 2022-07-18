using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.ServiceProcess;
using System.Text;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure
{
   public static class CustomAsserts
   {
      public delegate void AssertionBody();

      private static Application GetApp()
      {
         return SingletonProvider<TestSetup>.Instance.GetApp();
      }


      public static void AssertSessionCount(eSessionType sessionType, int expectedCount)
      {
         Application application = GetApp();

         RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
         {
            int count = application.Status.get_SessionCount(sessionType);

            if (count != expectedCount)
               return;

            Assert.AreEqual(expectedCount, count);

         });

      }

      public static void AssertBounceMessageExistsInQueue(string bounceTo)
      {
         var application = GetApp();
         Status status = application.Status;
         for (int i = 0; i < 100; i++)
         {
            if (status.UndeliveredMessages.Length == 0 || status.UndeliveredMessages.Contains("\t" + bounceTo))
               return;

            Thread.Sleep(100);
         }

         Assert.Fail("Delivery queue not empty");
      }

      public static string AssertLiveLogContents()
      {
         var application = GetApp();
         var settings = application.Settings;
         Logging logging = settings.Logging;
         for (int i = 0; i < 40; i++)
         {
            string contents = logging.LiveLog;
            if (contents.Length > 0)
               return contents;

            Thread.Sleep(100);
         }

         return "";
      }

      public static void AssertSpamAssassinIsRunning()
      {
         Process[] processlist = Process.GetProcesses();

         foreach (Process theprocess in processlist)
         {
            if (theprocess.ProcessName == "spamd")
               return;
         }

         // Check if we can launch it...
         try
         {
            var serviceController = new ServiceController("SpamAssassinJAM");
            serviceController.Start();
         }
         catch (Exception)
         {
            Assert.Inconclusive("Unable to start SpamAssassin process. Is SpamAssassin installed?");
         }
      }

      public static void AssertDeleteFile(string file)
      {
         for (int i = 0; i <= 400; i++)
         {
            if (!File.Exists(file))
               return;

            try
            {
               File.Delete(file);
               return;
            }
            catch (Exception e)
            {
               if (i == 400)
                  throw e;
            }
            Thread.Sleep(25);
         }
      }

      public static void AssertRecipientsInDeliveryQueue(int count)
      {
         AssertRecipientsInDeliveryQueue(count, true);
      }

      public static void AssertRecipientsInDeliveryQueue(int count, bool forceSend)
      {
         if (forceSend)
            TestSetup.SendMessagesInQueue();

         DateTime timeoutTime = DateTime.UtcNow.AddSeconds(20);

         while (DateTime.UtcNow < timeoutTime)
         {
            if (TestSetup.GetNumberOfMessagesInDeliveryQueue() == count)
               return;

            TestSetup.SendMessagesInQueue();

            Thread.Sleep(TimeSpan.FromMilliseconds(100));
         }

         int currentCount = TestSetup.GetNumberOfMessagesInDeliveryQueue();
         if (currentCount == count)
            return;

         TestSetup.DeleteMessagesInQueue();

         string message = string.Format(
            "Message queue does not contain correct number of messages. Actual: {0}, Expected: {1}",
            currentCount, count);
         Assert.Fail(message);
      }

      public static void AssertClamDRunning()
      {
         Process[] processlist = Process.GetProcesses();

         foreach (Process theprocess in processlist)
         {
            if (theprocess.ProcessName == "clamd")
               return;
         }

         // Check if we can launch it...
         var startInfo = new ProcessStartInfo();
         startInfo.FileName = @"C:\clamav\clamd.exe";
         startInfo.WorkingDirectory = @"C:\Clamav";
         startInfo.Arguments = "";

         try
         {
            Process.Start(startInfo);

            // Wait for clamav to start up.
            for (int i = 0; i < 10; i++)
            {
               var sock = new TcpConnection();
               if (sock.Connect(3310))
                  return;
               Thread.Sleep(1000);
            }

            Assert.Fail("ClamD process not starting up.");
         }
         catch (Exception)
         {
            Assert.Inconclusive("Unable to start ClamD process. Is ClamAV installed?");
         }
      }

      public static Message AssertGetFirstMessage(Account account, string folderName)
      {
         IMAPFolder folder = account.IMAPFolders.get_ItemByName(folderName);

         // Wait for message to appear.
         AssertFolderMessageCount(folder, 1);

         // return the message.
         return folder.Messages[0];
      }

      public static void AssertFolderMessageCount(IMAPFolder folder, int expectedCount)
      {
         if (expectedCount == 0)
         {
            // just in case.
            AssertRecipientsInDeliveryQueue(0);
         }

         int currentCount = 0;
         int timeout = 100;
         while (timeout > 0)
         {
            currentCount = folder.Messages.Count;

            if (currentCount == expectedCount)
               return;

            timeout--;
            Thread.Sleep(100);
         }

         string error = "Wrong number of messages in mailbox " + folder.Name;
         Assert.Fail(error);
      }

      public static Message AssertRetrieveFirstMessage(IMAPFolder folder)
      {
         int timeout = 100;
         while (timeout > 0)
         {
            if (folder.Messages.Count > 0)
            {
               return folder.Messages[0];
            }

            timeout--;
            Thread.Sleep(100);
         }

         string error = "Could not retrieve message from folder";
         Assert.Fail(error);

         return null;
      }

      public static IMAPFolder AssertFolderExists(IMAPFolders folders, string folderName)
      {
         int timeout = 100;
         while (timeout > 0)
         {
            try
            {
               return folders.get_ItemByName(folderName);
            }
            catch (Exception)
            {
            }

            timeout--;
            Thread.Sleep(100);
         }

         string error = "Folder could not be found " + folderName;
         Assert.Fail(error);
         return null;
      }

      public static void AssertFileExists(string file, bool delete)
      {
         int timeout = 100;
         while (timeout > 0)
         {
            try
            {
               if (File.Exists(file))
               {
                  if (delete)
                  {
                     TestTracer.WriteTraceInfo("Deleting file {0}...", file);
                     File.Delete(file);
                  }

                  return;
               }
            }
            catch (Exception)
            {
            }

            timeout--;
            Thread.Sleep(100);
         }

         Assert.Fail("Expected file does not exist:" + file);
      }

      public static void AssertNoReportedError()
      {
         if (File.Exists(LogHandler.GetErrorLogFileName()))
         {
            string contents = LogHandler.ReadErrorLog();
            Assert.Fail(contents);
         }

      }

      public static void AssertReportedError(string firstContent, params string[] contents)
      {
         var allExpectedContent = new List<string>();
         allExpectedContent.Add(firstContent);
         allExpectedContent.AddRange(contents);

         try
         {
            RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
            {
               string errorLog = LogHandler.ReadErrorLog();

               foreach (var content in allExpectedContent)
               {
                  StringAssert.Contains(content, errorLog);
               }
            });
         }
         finally
         {
            LogHandler.DeleteErrorLog();
         }
      }

      public static void AssertFilesInUserDirectory(Account account, int expectedFileCount)
      {
         var app = GetApp();
         var settings = app.Settings;

         string domain = account.Address.Substring(account.Address.IndexOf("@") + 1);
         string mailbox = account.Address.Substring(0, account.Address.IndexOf("@"));

         string domainDir = Path.Combine(settings.Directories.DataDirectory, domain);
         string userDir = Path.Combine(domainDir, mailbox);

         AssertFilesInDirectory(userDir, expectedFileCount);
      }

      public static void AssertFilesInDirectory(string directory, int expectedFileCount)
      {
         int count = 0;

         if (Directory.Exists(directory))
         {
            string[] dirs = Directory.GetDirectories(directory);

            foreach (string dir in dirs)
            {
               string[] files = Directory.GetFiles(dir);
               count += files.Length;
            }
         }

         Assert.AreEqual(expectedFileCount, count);
      }

      public static void Throws<T>(AssertionBody func) where T : Exception
      {
         var exceptionThrown = false;
         try
         {
            func.Invoke();
         }
         catch (T)
         {
            exceptionThrown = true;
         }

         if (!exceptionThrown)
         {
            throw new InvalidOperationException(
                String.Format("An exception of type {0} was expected, but not thrown", typeof(T))
                );
         }
      }
   }
}
