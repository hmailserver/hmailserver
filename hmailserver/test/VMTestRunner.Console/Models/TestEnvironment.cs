using System.Collections.Generic;

namespace VMTestRunner.Console
{
   public class TestEnvironment
   {
      public TestEnvironment(string operatingSystem, string description, string vmName, string snapshotName,
         bool includeStressTests, GuestTransport guestTransport, string guestAddress)
      {
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
      public string Name => $"{OperatingSystem} - {Description}";

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
   }
}
