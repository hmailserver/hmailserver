using System.Collections.Generic;

namespace VMTestRunner.Console
{
   /// <summary>
   /// The JSON result file of a single run.
   /// </summary>
   public class TestRunReport
   {
      /// <summary>
      /// The installer which was tested.
      /// </summary>
      public string SoftwareUnderTest { get; set; }

      public string SoftwareVersion { get; set; }

      public List<TestRunResult> Tests { get; set; } = new List<TestRunResult>();
   }
}
