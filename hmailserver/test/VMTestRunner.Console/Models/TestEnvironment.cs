using System.Collections.Generic;

namespace VMTestRunner.Console
{
   public class TestEnvironment
   {
      public TestEnvironment(string operatingSystem, string description, string vmName, string snapshotName, bool includeStressTests)
      {
         VMName = vmName;
         SnapshotName = snapshotName;
         OperatingSystem = operatingSystem;
         Description = description;
         IncludeStressTests = includeStressTests;
      }

      public string OperatingSystem { get; }

      public string Description { get; }

      public string SnapshotName { get; }

      public string VMName { get; }

      public bool IncludeStressTests;

      public List<InstallCommand> PostInstallCommands { get; } = new List<InstallCommand>();

      public List<FileCopyCommand> PostInstallFileCopy { get; } = new List<FileCopyCommand>();

      public List<FileCopyCommand> PreInstallFileCopy { get; } = new List<FileCopyCommand>();

      public List<InstallCommand> PreInstallCommands { get; } = new List<InstallCommand>();
   }
}
