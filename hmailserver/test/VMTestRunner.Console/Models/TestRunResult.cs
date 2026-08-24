using System;

namespace VMTestRunner.Console
{
   /// <summary>
   /// The outcome of a single test environment, serialized to the JSON result file.
   /// </summary>
   public class TestRunResult
   {
      public string Name { get; set; }

      public string OperatingSystem { get; set; }

      public string Description { get; set; }

      public TestStatus Status { get; set; }

      public DateTime StartedUtc { get; set; }

      public DateTime CompletedUtc { get; set; }

      /// <summary>
      /// Execution time as hh:mm:ss.fffffff.
      /// </summary>
      public string Duration { get; set; }

      public double DurationSeconds { get; set; }

      public string FailureSummary { get; set; }
   }
}
