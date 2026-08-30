using System;
using System.Diagnostics;

namespace RegressionTests.Shared
{
   /// <summary>
   ///    Detects if the hMailServer service has been restarted.
   ///    This is often an indication of a bug.
   /// </summary>
   public class ServiceRestartDetector
   {
      public static int? ExpectedProcessId;
      private static readonly object LockObj = new object();

      public static void ValidateProcessId()
      {
         lock (LockObj)
         {
            // Looking up a process by name reads the name of every process on the
            // machine. Once the process is known, checking that the same process id
            // is still alive is enough.
            if (ExpectedProcessId.HasValue && IsRunning(ExpectedProcessId.Value))
               return;

            var matchingProcesses = Process.GetProcessesByName("hmailserver");

            try
            {
               if (matchingProcesses.Length > 1)
                  throw new Exception("Multiple hMailServer.exe processes are running");
               if (matchingProcesses.Length == 0)
                  throw new Exception("No hMailServer.exe processes are running");

               var currentProcessId = matchingProcesses[0].Id;

               if (ExpectedProcessId.HasValue)
               {
                  // Validate that it has not changed
                  if (currentProcessId != ExpectedProcessId.Value)
                     throw new Exception(string.Format(
                        "hMailServer.exe has restarted. Old process id: {0}, New process id: {1}",
                        ExpectedProcessId.Value, currentProcessId));
               }
               else
               {
                  ExpectedProcessId = currentProcessId;
               }
            }
            finally
            {
               foreach (var process in matchingProcesses)
                  process.Dispose();
            }
         }
      }

      private static bool IsRunning(int processId)
      {
         try
         {
            using (Process.GetProcessById(processId))
               return true;
         }
         catch (ArgumentException)
         {
            return false;
         }
      }
   }
}