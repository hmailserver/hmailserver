using System.Collections.Generic;

namespace VMTestRunner.Console
{
   public class TestEnvironment
   {
      public TestEnvironment(string name, string operatingSystem, string description, string vmName, string snapshotName,
         bool includeStressTests, GuestTransport guestTransport, string guestAddress)
      {
         BaseName = name;
         VMName = vmName;
         SnapshotName = snapshotName;
         OperatingSystem = operatingSystem;
         Description = description;
         IncludeStressTests = includeStressTests;
         GuestTransport = guestTransport;
         GuestAddress = guestAddress;
      }

      public string OperatingSystem { get; }

      /// <summary>
      /// Identifies the test in the status board and in the JSON result file.
      /// </summary>
      public string Name => RunNumber == 0 ? BaseName : $"{BaseName} (run {RunNumber})";

      /// <summary>
      /// The name of the test as configured, without the run number. This is the name
      /// the --test command line parameter selects on.
      /// </summary>
      public string BaseName { get; }

      /// <summary>
      /// One-based position when the same test is run several times, otherwise zero.
      /// </summary>
      public int RunNumber { get; private set; }

      public string Description { get; }

      public string SnapshotName { get; }

      public string VMName { get; }

      /// <summary>
      /// How the runner reaches the inside of the guest.
      /// </summary>
      public GuestTransport GuestTransport { get; }

      /// <summary>
      /// Host name or IP address of the guest. Only used by the network transport,
      /// which looks the address up when this isn't set.
      /// </summary>
      public string GuestAddress { get; }

      public bool IncludeStressTests;

      public List<InstallCommand> PostInstallCommands { get; } = new List<InstallCommand>();

      public List<FileCopyCommand> PostInstallFileCopy { get; } = new List<FileCopyCommand>();

      public List<FileCopyCommand> PreInstallFileCopy { get; } = new List<FileCopyCommand>();

      public List<InstallCommand> PreInstallCommands { get; } = new List<InstallCommand>();

      /// <summary>
      /// Returns a copy of this environment, marked as the given run in a repeated run.
      /// </summary>
      public TestEnvironment CopyForRun(int runNumber)
      {
         var copy = new TestEnvironment(BaseName, OperatingSystem, Description, VMName, SnapshotName,
            IncludeStressTests, GuestTransport, GuestAddress) { RunNumber = runNumber };

         copy.PreInstallCommands.AddRange(PreInstallCommands);
         copy.PreInstallFileCopy.AddRange(PreInstallFileCopy);
         copy.PostInstallCommands.AddRange(PostInstallCommands);
         copy.PostInstallFileCopy.AddRange(PostInstallFileCopy);

         return copy;
      }
   }
}
