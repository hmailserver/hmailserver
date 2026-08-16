using System;

namespace VMTestRunner.Console
{
   /// <summary>
   /// Thrown when one or more NUnit tests fail. Carries the raw NUnit output as
   /// the exception message and, separately, a human readable summary of the
   /// failing tests so both can be written to the log.
   /// </summary>
   public class TestFailedException : Exception
   {
      public TestFailedException(string rawOutput, string failureSummary)
         : base(rawOutput)
      {
         FailureSummary = failureSummary;
      }

      /// <summary>
      /// A human readable summary of the failing tests and their messages, or
      /// <c>null</c> when no summary could be produced.
      /// </summary>
      public string FailureSummary { get; }
   }
}
