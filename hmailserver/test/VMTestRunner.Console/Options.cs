using CommandLine;

namespace VMTestRunner.Console
{
   class Options
   {
      [Option('i', "installer", Required = true, HelpText = "Path to the hMailServer installer executable.")]
      public string InstallerPath { get; set; }

      [Option('p', "parallelism", Required = false, Default = 1, HelpText = "Max degree of parallelism for VM test execution.")]
      public int MaxParallelism { get; set; }

      [Option('t', "test", Required = false, HelpText = "Run only the test with this name, for example \"Windows 10 - Internal (SQL Compact): New installation\". Tests that are disabled in TestEnvironments.json only run when named here.")]
      public string Test { get; set; }

      [Option('c', "count", Required = false, Default = 1, HelpText = "Number of times to run each selected test.")]
      public int Count { get; set; }
   }
}
