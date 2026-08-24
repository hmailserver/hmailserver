using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace VMTestRunner.Console
{
   /// <summary>
   /// Estimates how long a run will take, based on the JSON result file of the previous run.
   /// </summary>
   public static class CompletionEstimator
   {
      /// <summary>
      /// Returns the estimated duration of the run, or null if the previous run is missing,
      /// did not pass completely or did not contain all the tests about to be run.
      /// </summary>
      public static TimeSpan? Estimate(IEnumerable<TestEnvironment> environments, int maxParallelism, out string previousRun)
      {
         previousRun = null;

         var previousResultFile = FindPreviousResultFile();

         if (previousResultFile == null)
            return null;

         var previousReport = TestResultWriter.TryRead(previousResultFile);
         var previousResults = previousReport?.Tests;

         if (previousResults == null || previousResults.Count == 0)
            return null;

         if (previousResults.Any(result => result.Status != TestStatus.Passed))
            return null;

         var durations = new Dictionary<string, double>();

         foreach (var result in previousResults)
            durations[result.Name] = result.DurationSeconds;

         // Every test must be known, otherwise the estimate would be too low.
         if (environments.Any(environment => !durations.ContainsKey(environment.Name)))
            return null;

         previousRun = Path.GetFileName(previousResultFile);

         return TimeSpan.FromSeconds(EstimateSeconds(environments, durations, maxParallelism));
      }

      /// <summary>
      /// Tests on the same VM run one after the other, while separate VMs run in parallel.
      /// The groups are handed out to whichever worker becomes free first.
      /// </summary>
      private static double EstimateSeconds(IEnumerable<TestEnvironment> environments,
         IDictionary<string, double> durations, int maxParallelism)
      {
         var groupDurations = environments
            .GroupBy(environment => environment.VMName)
            .Select(group => group.Sum(environment => durations[environment.Name]))
            .ToList();

         var workers = new double[Math.Max(1, maxParallelism)];

         foreach (var groupDuration in groupDurations)
         {
            var firstFree = Array.IndexOf(workers, workers.Min());
            workers[firstFree] += groupDuration;
         }

         return workers.Max();
      }

      private static string FindPreviousResultFile()
      {
         var currentFile = RunContext.GetResultSummaryFilePath();
         var directory = Path.GetDirectoryName(currentFile);

         return Directory.GetFiles(directory, "vmtestrunner-*_results.json")
            .Where(file => !string.Equals(file, currentFile, StringComparison.OrdinalIgnoreCase))
            .OrderByDescending(file => file, StringComparer.OrdinalIgnoreCase)
            .FirstOrDefault();
      }
   }
}
