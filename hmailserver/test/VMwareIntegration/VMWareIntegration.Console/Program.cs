// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using VMwareIntegration.Common;

namespace VMwareIntegration.Console
{
   class Program
   {
      private static string _logFile;

      static int Main(string[] args)
      {
         var softwareUnderTest = args[0];
         _logFile = args[1];

         System.Console.WriteLine("Loading test suite...");

         // Load static container of all tests.
         List<TestEnvironment> listEnvironments = new List<TestEnvironment>();
         TestEnvironments.AddAll(listEnvironments);

         int testIndex = 1;
         foreach (TestEnvironment environment in listEnvironments)
         {
            string message = string.Format("{5} - Running test {3} / {4} - {0} on {1} (Snapshot: {2})",
               environment.Description,
               environment.OperatingSystem,
               environment.SnapshotName,
               testIndex,
               listEnvironments.Count,
               DateTime.Now);

            System.Console.WriteLine(message);

            TestRunner runner = new TestRunner(true, environment, true, softwareUnderTest);
            runner.TestCompleted += runner_TestCompleted;
            if (!runner.Run())
               return -1;

            testIndex++;
         }

         return 0;
      }

      static void runner_TestCompleted(bool result, string message, string failureText)
      {
         if (result)
            LogText("The test completed successfully.");
         else
         {
            LogText("The test failed.");
            LogText(failureText);
         }
      }

      private static void LogText(string text)
      {
         System.Console.WriteLine(text);
         File.AppendAllText(_logFile, text);
      }
   }
}
