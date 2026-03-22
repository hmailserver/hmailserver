using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using CommandLine;

namespace VMTestRunner.Console
{
   class Program
   {
      private static readonly object _lockCounterTest = new object();

      private static readonly NLog.Logger Logger = NLog.LogManager.GetCurrentClassLogger();

      static int Main(string[] args)
      {
         NLog.Targets.Target.Register<StatusLineConsoleTarget>("StatusLineConsole");
         NLog.LogManager.Configuration = new NLog.Config.XmlLoggingConfiguration(
            Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "NLog.config"));

         return Parser.Default.ParseArguments<Options>(args)
            .MapResult(
               options => RunTests(options),
               errors => -1);
      }

      private static int RunTests(Options options)
      {
         var softwareUnderTest = options.InstallerPath;

         if (!File.Exists(softwareUnderTest))
         {
            Logger.Error($"The file {softwareUnderTest} does not exist.");
            return -1;
         }

         Logger.Info("Loading test suite...");

         // Load static container of all tests.
         var listEnvironments = new List<TestEnvironment>();
         TestEnvironments.AddAll(listEnvironments);

         int testIndex = 0;

         var parallelOptions = new ParallelOptions { MaxDegreeOfParallelism = options.MaxParallelism };

         // We can run tests on separate Hyper-V VMs at the same time since they are separate images.
         var environmentsGroupedByVmName = listEnvironments.GroupBy(item => item.VMName).ToList();

         var partitioner = Partitioner.Create(environmentsGroupedByVmName, EnumerablePartitionerOptions.NoBuffering);

         Parallel.ForEach(partitioner, parallelOptions, environmentGroup =>
         {
            foreach (var environment in environmentGroup)
            {
               int localIndex;

               lock (_lockCounterTest)
               {
                  localIndex = ++testIndex;

                  Logger.Info($"{localIndex}/{listEnvironments.Count} - Test: {environment.Description} on {environment.OperatingSystem}. VM: {environment.VMName} (Snapshot: {environment.SnapshotName})");
               }

               var runner = new TestRunner(environment, softwareUnderTest, localIndex);

               try
               {
                  runner.Run();
                  var successEvent = new NLog.LogEventInfo(NLog.LogLevel.Info, Logger.Name, $"Test {localIndex} completed successfully.");
                  successEvent.Properties["success"] = true;
                  Logger.Log(successEvent);
               }
               catch (Exception ex)
               {
                  Logger.Error($"Test {localIndex} failed.");
                  var exLines = ex.ToString().Split(new[] { "\r\n", "\n" }, StringSplitOptions.None);
                  var truncated = exLines.Length > 100 ? exLines.Take(100).Append($"... ({exLines.Length - 100} more lines truncated)") : exLines;
                  Logger.Error(string.Join(Environment.NewLine, truncated));
               }
            }
         });

         Logger.Info("All tests completed.");

         if (Debugger.IsAttached)
         {
            Logger.Info("Press Enter to exit.");
            System.Console.ReadLine();
         }

         return 0;
      }

   }
}
