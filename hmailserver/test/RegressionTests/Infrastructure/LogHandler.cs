using System;
using System.IO;
using System.Threading;
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

         if (File.Exists(errorLog)) File.Delete(errorLog);
      }

      public static string ReadAndDeleteErrorLog()
      {
         var contents = ReadErrorLog();

         DeleteErrorLog();

         return contents;
      }

      public static string ReadErrorLog()
      {
         var file = GetErrorLogFileName();
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
         for (var i = 0; i < 50; i++)
            try
            {
               var filename = GetDefaultLogFileName();
               if (File.Exists(filename))
                  File.Delete(filename);

               return;
            }
            catch (Exception)
            {
               Thread.Sleep(100);
            }

         throw new Exception("Failed to delete default log file.");
      }

      public static string ReadCurrentDefaultLog()
      {
         var filename = GetDefaultLogFileName();
         var content = string.Empty;
         if (File.Exists(filename))
            return TestSetup.ReadExistingTextFile(filename);

         return content;
      }

      /// <summary>
      ///    Waits for the text to appear in the default log.
      /// </summary>
      public static bool DefaultLogContains(string data)
      {
         return DefaultLogContains(data, TimeSpan.FromSeconds(10));
      }

      /// <summary>
      ///    Checks that the text is not written to the default log. Asserting on
      ///    DefaultLogContains instead always waits the full timeout, since the text
      ///    never appears. The callers have already waited for the operation to
      ///    complete, so a short grace period is enough here.
      /// </summary>
      public static bool DefaultLogDoesNotContain(string data)
      {
         return !DefaultLogContains(data, TimeSpan.FromSeconds(2));
      }

      private static bool DefaultLogContains(string data, TimeSpan timeout)
      {
         var filename = GetDefaultLogFileName();

         return Poll.Until(timeout, () =>
         {
            if (!File.Exists(filename))
               return false;

            return TestSetup.ReadExistingTextFile(filename).Contains(data);
         });
      }

      public static string GetEventLogFileName()
      {
         return SingletonProvider<TestSetup>.Instance.GetApp().Settings.Logging.CurrentEventLog;
      }
   }
}