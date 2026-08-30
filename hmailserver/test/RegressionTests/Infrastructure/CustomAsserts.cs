using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.ServiceProcess;
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
         var application = GetApp();

         RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
         {
            var count = application.Status.get_SessionCount(sessionType);

            if (count != expectedCount)
               return;

            RetryableAssert.AreEqual(expectedCount, count);
         });
      }

      public static void AssertBounceMessageExistsInQueue(string bounceTo)
      {
         var application = GetApp();
         var status = application.Status;
         for (var i = 0; i < 100; i++)
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
         var logging = settings.Logging;
         for (var i = 0; i < 40; i++)
         {
            var contents = logging.LiveLog;
            if (contents.Length > 0)
               return contents;

            Thread.Sleep(100);
         }

         return "";
      }

      public static void AssertSpamAssassinIsRunning()
      {
         var processlist = Process.GetProcesses();

         foreach (var theprocess in processlist)
            if (theprocess.ProcessName == "spamd")
               return;

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
         for (var i = 0; i <= 400; i++)
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
         // The queue is normally already in the expected state, so check that before
         // paying for a delivery run.
         if (TestSetup.GetNumberOfMessagesInDeliveryQueue() == count)
            return;

         if (forceSend)
            TestSetup.SendMessagesInQueue();

         if (Poll.Until(TimeSpan.FromSeconds(20), () =>
         {
            if (TestSetup.GetNumberOfMessagesInDeliveryQueue() == count)
               return true;

            TestSetup.SendMessagesInQueue();
            return false;
         }))
            return;

         var currentCount = TestSetup.GetNumberOfMessagesInDeliveryQueue();

         TestSetup.DeleteMessagesInQueue();

         var message = string.Format(
            "Message queue does not contain correct number of messages. Actual: {0}, Expected: {1}",
            currentCount, count);
         Assert.Fail(message);
      }

      public static void AssertClamDRunning()
      {
         var processlist = Process.GetProcesses();

         foreach (var theprocess in processlist)
            if (theprocess.ProcessName == "clamd")
               return;

         // Check if we can launch it...
         var startInfo = new ProcessStartInfo();
         startInfo.FileName = @"C:\clamav\clamd.exe";
         startInfo.WorkingDirectory = @"C:\Clamav";

         try
         {
            Process.Start(startInfo);

            // Wait for clamav to start up.
            for (var i = 0; i < 10; i++)
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
         var folder = account.IMAPFolders.get_ItemByName(folderName);

         // Wait for message to appear.
         AssertFolderMessageCount(folder, 1);

         // return the message.
         return folder.Messages[0];
      }

      public static void AssertFolderMessageCount(IMAPFolder folder, int expectedCount)
      {
         if (expectedCount == 0)
            // just in case.
            AssertRecipientsInDeliveryQueue(0);

         if (Poll.Until(TimeSpan.FromSeconds(10), () => folder.Messages.Count == expectedCount))
            return;

         var error = "Wrong number of messages in mailbox " + folder.Name;
         Assert.Fail(error);
      }

      public static Message AssertRetrieveFirstMessage(IMAPFolder folder)
      {
         if (Poll.Until(TimeSpan.FromSeconds(10), () => folder.Messages.Count > 0))
            return folder.Messages[0];

         var error = "Could not retrieve message from folder";
         Assert.Fail(error);

         return null;
      }

      public static IMAPFolder AssertFolderExists(IMAPFolders folders, string folderName)
      {
         IMAPFolder folder = null;

         if (Poll.Until(TimeSpan.FromSeconds(10), () =>
         {
            try
            {
               folder = folders.get_ItemByName(folderName);
               return true;
            }
            catch (Exception)
            {
               return false;
            }
         }))
            return folder;

         var error = "Folder could not be found " + folderName;
         Assert.Fail(error);
         return null;
      }

      public static void AssertFileExists(string file, bool delete)
      {
         var timeout = 100;
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
            var contents = LogHandler.ReadErrorLog();
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
               var errorLog = LogHandler.ReadErrorLog();

               foreach (var content in allExpectedContent) RetryableAssert.StringContains(content, errorLog);
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

         var domain = account.Address.Substring(account.Address.IndexOf("@") + 1);
         var mailbox = account.Address.Substring(0, account.Address.IndexOf("@"));

         var domainDir = Path.Combine(settings.Directories.DataDirectory, domain);
         var userDir = Path.Combine(domainDir, mailbox);

         AssertFilesInDirectory(userDir, expectedFileCount);
      }

      public static void AssertFilesInDirectory(string directory, int expectedFileCount)
      {
         var count = 0;

         if (Directory.Exists(directory))
         {
            var dirs = Directory.GetDirectories(directory);

            foreach (var dir in dirs)
            {
               var files = Directory.GetFiles(dir);
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
            throw new InvalidOperationException(
               string.Format("An exception of type {0} was expected, but not thrown", typeof(T))
            );
      }
   }
}