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

      private static object _lockCounterTest = new object();

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
               MaxDegreeOfParallelism = 6
            };

         // We can run tests on XP and Vista/2003/2008 at the same time since it's separate VMware images.
         var environmentsGroupedByVmwareImage = listEnvironments.GroupBy(item => item.VMwarePath).ToList();

         Parallel.ForEach(environmentsGroupedByVmwareImage, options, environmentGroup =>
         {
            foreach (var environment in environmentGroup)
            {
               int localIndex;

               lock (_lockCounterTest)
               {
                  localIndex = ++testIndex;

                  string message =
                     string.Format("{0}: {1}/{2} - Test: {3} on {4} with db {5}. Image: {6} (Snapshot: {7})",
                        DateTime.Now,
                        localIndex,
                        listEnvironments.Count,
                        environment.Description,
                        environment.OperatingSystem,
                        environment.DatabaseType,
                        Path.GetFileName(environment.VMwarePath),
                        environment.SnapshotName);

                  LogText(message);
               }

               TestRunner runner = new TestRunner(true, localIndex, environment, false, softwareUnderTest);
               runner.TestCompleted += runner_TestCompleted;
               if (!runner.Run())
               {
                  throw new Exception("Unable to run test.");
               }
            }

         });

         System.Console.WriteLine("All tests completed. Press Enter to exit.");

         System.Console.ReadLine();
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
