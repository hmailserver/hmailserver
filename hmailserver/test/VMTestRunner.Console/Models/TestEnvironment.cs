using System.Collections.Generic;

namespace VMTestRunner.Console
{
   public class TestEnvironment
   {
      public string OperatingSystem { get; set; }

      /// <summary>
      /// Identifies the test in the status board and in the JSON result file.
      /// </summary>
      public string Name => RunNumber == 0 ? BaseName : $"{BaseName} (run {RunNumber})";

      /// <summary>
      /// The name of the test as configured, without the run number. This is the name
      /// the --test command line parameter selects on.
      /// </summary>
      public string BaseName { get; set; }

      /// <summary>
      /// One-based position when the same test is run several times, otherwise zero.
      /// </summary>
      public int RunNumber { get; private set; }

      public string Description { get; set; }

      public string SnapshotName { get; set; }

      public string VMName { get; set; }

      /// <summary>
      /// False for tests that are too slow to belong in a normal run. They only run
      /// when selected by name with --test.
      /// </summary>
      public bool Enabled { get; set; } = true;

      /// <summary>
      /// How the runner reaches the inside of the guest.
      /// </summary>
      public GuestTransport GuestTransport { get; set; }

      /// <summary>
      /// Host name or IP address of the guest. Only used by the network transport,
      /// which looks the address up when this isn't set.
      /// </summary>
      public string GuestAddress { get; set; }

      /// <summary>
      /// Which test assembly to run in the guest.
      /// </summary>
      public TestSuite TestSuite { get; set; } = TestSuite.RegressionTests;

      /// <summary>
      /// Turns on page heap for hMailServer.exe before the tests run, to catch heap
      /// corruption. Makes the run considerably slower.
      /// </summary>
      public bool EnablePageHeap { get; set; }

      /// <summary>
      /// Host path of the gflags.exe used to turn page heap on. Only set when
      /// <see cref="EnablePageHeap"/> is true.
      /// </summary>
      public string GFlagsPath { get; set; }

      public bool IncludeStressTests { get; set; }

      public List<InstallCommand> PostInstallCommands { get; } = new List<InstallCommand>();

      public List<FileCopyCommand> PostInstallFileCopy { get; } = new List<FileCopyCommand>();

      public List<FileCopyCommand> PreInstallFileCopy { get; } = new List<FileCopyCommand>();

      public List<InstallCommand> PreInstallCommands { get; } = new List<InstallCommand>();

      /// <summary>
      /// Returns a copy of this environment, marked as the given run in a repeated run.
      /// </summary>
      public TestEnvironment CopyForRun(int runNumber)
      {
         var copy = new TestEnvironment
         {
            BaseName = BaseName,
            OperatingSystem = OperatingSystem,
            Description = Description,
            VMName = VMName,
            SnapshotName = SnapshotName,
            Enabled = Enabled,
            GuestTransport = GuestTransport,
            GuestAddress = GuestAddress,
            TestSuite = TestSuite,
            EnablePageHeap = EnablePageHeap,
            GFlagsPath = GFlagsPath,
            IncludeStressTests = IncludeStressTests,
            RunNumber = runNumber,
         };

         copy.PreInstallCommands.AddRange(PreInstallCommands);
         copy.PreInstallFileCopy.AddRange(PreInstallFileCopy);
         copy.PostInstallCommands.AddRange(PostInstallCommands);
         copy.PostInstallFileCopy.AddRange(PostInstallFileCopy);

         return copy;
      }
   }
}
