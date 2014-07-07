// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using VMwareIntegration.Common;

namespace VMwareIntegration.Console
{
   class Program
   {
      private static string _logFile;
      private static object _outputLogLock = new object();

      static int Main(string[] args)
      {
         var softwareUnderTest = args[0];
         _logFile = args[1];
         _logFile = _logFile.Replace("%TIMESTAMP%", DateTime.Now.ToString("yyyy-MM-dd HHmmss"));

         LogText("Loading test suite...");

         // Load static container of all tests.
         List<TestEnvironment> listEnvironments = new List<TestEnvironment>();
         TestEnvironments.AddAll(listEnvironments);

         int testIndex = 0;

         var options = new ParallelOptions()
            {
               MaxDegreeOfParallelism = 4
            };

         // We can run tests on XP and Vista/2003/2008 at the same time since it's separate VMware images.
         var environmentsGroupedByVmwareImage = listEnvironments.GroupBy(item => item.VMwarePath);

         Parallel.ForEach(environmentsGroupedByVmwareImage, options, environmentGroup =>
         {
            foreach (var environment in environmentGroup)
            {
               int localIndex = Interlocked.Increment(ref testIndex);

               string message = string.Format("{5} - Running test {3} / {4} - {0} on {1} (Snapshot: {2})",
                  environment.Description,
                  environment.OperatingSystem,
                  environment.SnapshotName,
                  localIndex,
                  listEnvironments.Count,
                  DateTime.Now);

               LogText(message);

               TestRunner runner = new TestRunner(true, localIndex, environment, true, softwareUnderTest);
               runner.TestCompleted += runner_TestCompleted;
               if (!runner.Run())
               {

                  throw new Exception("Unable to run test.");
               }
            }

         });

         return 0;
      }

      static void runner_TestCompleted(int testIndex, bool result, string message, string failureText)
      {
         if (result)
            LogText(string.Format("The test {0} completed successfully.", testIndex));
         else
         {
            LogText(string.Format("The test {0} failed.", testIndex));
            LogText(failureText);
         }
      }

      private static void LogText(string text)
      {
         lock (_outputLogLock)
         {
            System.Console.WriteLine(text);
            File.AppendAllText(_logFile, text + Environment.NewLine);
         }
      }
   }
}
