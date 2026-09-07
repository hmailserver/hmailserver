using CommandLine;

namespace VMTestRunner.Console
{
   class Options
   {
      [Option('i', "installer", Required = true, HelpText = "Path to the hMailServer installer executable.")]
      public string InstallerPath { get; set; }

      [Option('p', "parallelism", Required = false, Default = 1, HelpText = "Max degree of parallelism for VM test execution.")]
      public int MaxParallelism { get; set; }

      [Option('t', "test", Required = false, HelpText = "Run only the test with this description or name, for example \"Internal (SQL Compact): New installation\".")]
      public string Test { get; set; }

      [Option('c', "count", Required = false, Default = 1, HelpText = "Number of times to run each selected test.")]
      public int Count { get; set; }
   }
}
