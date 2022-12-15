using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure
{
   public class LogHandler
   {
      public static void DeleteEventLog()
      {
         CustomAsserts.AssertDeleteFile(GetEventLogFileName());
      }

      public static void DeleteErrorLog()
      {
         var errorLog = GetErrorLogFileName();

         if (File.Exists(errorLog))
         {
            File.Delete(errorLog);
         }

      }

      public static string ReadAndDeleteErrorLog()
      {
         string contents = ReadErrorLog();

         DeleteErrorLog();

         return contents;
      }

      public static string ReadErrorLog()
      {
         string file = GetErrorLogFileName();
         CustomAsserts.AssertFileExists(file, false);

         // Read the file without taking a lock.
         // If a lock is taken, hMailServer will not be able to write to the file, while 
         // this code is reading from it.
         using (var fileStream = new FileStream(file, FileMode.Open, FileAccess.Read, FileShare.ReadWrite))
         using (var textReader = new StreamReader(fileStream))
         {
            return textReader.ReadToEnd();
         }
      }


      public static string GetErrorLogFileName()
      {
         return SingletonProvider<TestSetup>.Instance.GetApp().Settings.Logging.CurrentErrorLog;
      }

      public static string GetDefaultLogFileName()
      {
         return SingletonProvider<TestSetup>.Instance.GetApp().Settings.Logging.CurrentDefaultLog;
      }

      public static void DeleteCurrentDefaultLog()
      {
         for (int i = 0; i < 50; i++)
         {
            try
            {
               string filename = GetDefaultLogFileName();
               if (File.Exists(filename))
                  File.Delete(filename);

               return;
            }
            catch (Exception)
            {
               Thread.Sleep(100);
            }
         }

         throw new Exception("Failed to delete default log file.");
      }

      public static string ReadCurrentDefaultLog()
      {
         string filename = GetDefaultLogFileName();
         string content = string.Empty;
         if (File.Exists(filename))
            return TestSetup.ReadExistingTextFile(filename);

         return content;
      }

      public static bool DefaultLogContains(string data)
      {
         string filename = GetDefaultLogFileName();

         for (int i = 0; i < 40; i++)
         {
            if (File.Exists(filename))
            {
               string content = TestSetup.ReadExistingTextFile(filename);
               if (content.Contains(data))
                  return true;
            }

            Thread.Sleep(250);
         }

         return false;
      }

      public static string GetEventLogFileName()
      {
         return SingletonProvider<TestSetup>.Instance.GetApp().Settings.Logging.CurrentEventLog;
      }


   }
}
