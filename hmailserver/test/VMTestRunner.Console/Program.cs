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
      private static readonly NLog.Logger Logger = NLog.LogManager.GetCurrentClassLogger();

      static int Main(string[] args)
      {
         NLog.Targets.Target.Register<TestBoardConsoleTarget>("TestBoardConsole");

         // Makes the log file and the NUnit result files share the same timestamp.
         NLog.GlobalDiagnosticsContext.Set(RunContext.TimestampVariableName, RunContext.Timestamp);

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

         var report = new TestRunReport
         {
            SoftwareUnderTest = Path.GetFullPath(softwareUnderTest),
            SoftwareVersion = GetFileVersion(softwareUnderTest),
         };

         WriteHeader(report);

         Logger.Info("Loading test suite...");

         // Load static container of all tests.
         var listEnvironments = new List<TestEnvironment>();
         TestEnvironments.AddAll(listEnvironments);

         // The one-based position in this list identifies the test, both in the log
         // and in the row it occupies on the status board.
         var results = listEnvironments
            .Select(environment => new TestRunResult
            {
               Name = environment.Name,
               OperatingSystem = environment.OperatingSystem,
               Description = environment.Description,
               Status = TestStatus.Pending,
            })
            .ToList();

         report.Tests = results;

         var tests = listEnvironments
            .Select((environment, index) => new { environment, testIndex = index + 1 })
            .ToList();

         WriteEstimatedCompletion(listEnvironments, options.MaxParallelism);

         TestStatusBoard.Instance.Initialize(results.Select(result => result.Name));

         var parallelOptions = new ParallelOptions { MaxDegreeOfParallelism = options.MaxParallelism };

         // We can run tests on separate Hyper-V VMs at the same time since they are separate images.
         var environmentsGroupedByVmName = tests.GroupBy(item => item.environment.VMName).ToList();

         var partitioner = Partitioner.Create(environmentsGroupedByVmName, EnumerablePartitionerOptions.NoBuffering);

         Parallel.ForEach(partitioner, parallelOptions, environmentGroup =>
         {
            foreach (var test in environmentGroup)
            {
               var environment = test.environment;
               var testIndex = test.testIndex;
               var result = results[testIndex - 1];

               Logger.Info($"{testIndex}/{listEnvironments.Count} - Test: {environment.Description} on {environment.OperatingSystem}. VM: {environment.VMName} (Snapshot: {environment.SnapshotName}), Include stress tests: {environment.IncludeStressTests}");

               result.StartedUtc = DateTime.UtcNow;
               TestStatusBoard.Instance.SetRunning(testIndex, "Starting");

               var runner = new TestRunner(environment, softwareUnderTest, testIndex);

               try
               {
                  runner.Run();

                  Complete(result, testIndex, TestStatus.Passed);
               }
               catch (Exception ex)
               {
                  var testFailure = ex as TestFailedException;

                  result.FailureSummary = testFailure?.FailureSummary ?? ex.Message;

                  Complete(result, testIndex, TestStatus.Failed);

                  // The raw NUnit output is only useful in the log file - it would flood the console.
                  var details = new NLog.LogEventInfo(NLog.LogLevel.Error, Logger.Name, $"Test {testIndex} failed: {ex}");
                  details.Properties[TestBoardConsoleTarget.FileOnlyProperty] = true;
                  Logger.Log(details);

                  Logger.Error($"Test {testIndex} - {result.Name} - failed:{Environment.NewLine}{result.FailureSummary}");
               }
            }
         });

         TestStatusBoard.Instance.Complete();

         var resultSummaryFile = RunContext.GetResultSummaryFilePath();
         TestResultWriter.Write(resultSummaryFile, report);

         var failedCount = results.Count(result => result.Status != TestStatus.Passed);

         Logger.Info($"All tests completed for {report.SoftwareUnderTest}. {results.Count - failedCount} passed, {failedCount} failed. Result summary saved to {resultSummaryFile}");

         // Info messages are not printed to the console, so the summary is written directly.
         TestStatusBoard.Instance.WriteLine(
            $"{Environment.NewLine}{results.Count - failedCount} of {results.Count} tests passed for {Path.GetFileName(report.SoftwareUnderTest)}." +
            $"{Environment.NewLine}Results: {resultSummaryFile}",
            failedCount == 0 ? ConsoleColor.Green : ConsoleColor.Red);

         if (Debugger.IsAttached)
         {
            Logger.Info("Press Enter to exit.");
            System.Console.ReadLine();
         }

         return failedCount == 0 ? 0 : 1;
      }

      /// <summary>
      /// Tells the user which installer is being tested.
      /// </summary>
      private static void WriteHeader(TestRunReport report)
      {
         var version = report.SoftwareVersion == null ? string.Empty : $" (version {report.SoftwareVersion})";
         var message = $"Testing {report.SoftwareUnderTest}{version}";

         Logger.Info(message);

         // Info messages are not printed to the console, so the header is written directly.
         TestStatusBoard.Instance.WriteLine(message, ConsoleColor.Cyan);
      }

      private static string GetFileVersion(string path)
      {
         try
         {
            // The version resource of the installer is padded with spaces.
            var version = FileVersionInfo.GetVersionInfo(path).FileVersion?.Trim();

            return string.IsNullOrEmpty(version) ? null : version;
         }
         catch (Exception)
         {
            return null;
         }
      }

      /// <summary>
      /// Tells the user when the run is expected to finish. Only a previous run where every
      /// test passed can be used, since a failed test may have been cut short.
      /// </summary>
      private static void WriteEstimatedCompletion(List<TestEnvironment> environments, int maxParallelism)
      {
         var estimate = CompletionEstimator.Estimate(environments, maxParallelism, out var previousRun);

         var message = estimate == null
            ? "Estimated completion time could not be determined."
            : $"Estimated completion: {DateTime.Now.Add(estimate.Value):yyyy-MM-dd HH:mm} " +
              $"({(int)estimate.Value.TotalHours}h {estimate.Value.Minutes:D2}m), based on {previousRun}";

         Logger.Info(message);

         // Info messages are not printed to the console, so the estimate is written directly.
         TestStatusBoard.Instance.WriteLine(message, ConsoleColor.Cyan);
      }

      private static void Complete(TestRunResult result, int testIndex, TestStatus status)
      {
         result.CompletedUtc = DateTime.UtcNow;
         result.Status = status;

         var duration = result.CompletedUtc - result.StartedUtc;
         result.Duration = duration.ToString("c");
         result.DurationSeconds = Math.Round(duration.TotalSeconds, 3);

         TestStatusBoard.Instance.SetCompleted(testIndex, status, duration);
      }
   }
}
