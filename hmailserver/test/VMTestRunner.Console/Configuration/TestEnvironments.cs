using System.Collections.Generic;
using System.IO;
using System.Reflection;
using Newtonsoft.Json;

namespace VMTestRunner.Console
{
   public class TestEnvironments
   {
      private const string MySQLLibPlaceholder = "{{MySQLLib}}";

      public static void AddAll(List<TestEnvironment> listEnvironments)
      {
         string jsonPath = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "TestEnvironments.json");
         string json = File.ReadAllText(jsonPath);

         var items = JsonConvert.DeserializeObject<List<EnvironmentDto>>(json);

         foreach (var item in items)
         {
            if (item.GuestTransport == GuestTransport.Network && string.IsNullOrEmpty(item.GuestAddress))
               throw new System.Exception($"The environment '{item.Description}' uses the network transport, so it must specify guestAddress.");

            // The stress category only exists in the regression suite.
            if (item.TestSuite != TestSuite.RegressionTests && item.IncludeStressTests)
               throw new System.Exception($"The environment '{item.Description}' runs {item.TestSuite}, so it cannot set includeStressTests.");

            // The name is what --test selects on; it defaults to the operating system and the description.
            var name = string.IsNullOrWhiteSpace(item.Name) ? $"{item.OperatingSystem} - {item.Description}" : item.Name;

            var env = new TestEnvironment
            {
               BaseName = name,
               OperatingSystem = item.OperatingSystem,
               Description = item.Description,
               VMName = item.VmName,
               SnapshotName = item.SnapshotName,
               Enabled = item.Enabled,
               GuestTransport = item.GuestTransport,
               GuestAddress = item.GuestAddress,
               TestSuite = item.TestSuite,
               EnablePageHeap = item.EnablePageHeap,
               IncludeStressTests = item.IncludeStressTests,

               // Resolved here so a missing SDK is reported before any VM is started.
               GFlagsPath = item.EnablePageHeap ? GetGFlags() : null,
            };

            foreach (var cmd in item.PreInstallCommands)
               env.PreInstallCommands.Add(new InstallCommand(cmd.Executable, cmd.Parameters));

            foreach (var copy in item.PreInstallFileCopy)
               env.PreInstallFileCopy.Add(new FileCopyCommand(ResolvePath(copy.From), copy.To));

            foreach (var cmd in item.PostInstallCommands)
               env.PostInstallCommands.Add(new InstallCommand(cmd.Executable, cmd.Parameters));

            foreach (var copy in item.PostInstallFileCopy)
               env.PostInstallFileCopy.Add(new FileCopyCommand(ResolvePath(copy.From), copy.To));

            listEnvironments.Add(env);
         }
      }

      private static string ResolvePath(string path)
      {
         switch (path)
         {
            case MySQLLibPlaceholder:
               return GetMySQLLib();
            default:
               return path;
         }
      }

      private static string GetMySQLLib()
      {
         string currentDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
         var librariesDir = Path.Combine(currentDir, @"..\..\..\..\..\..\libraries");
         var libMySqlDir = Path.Combine(librariesDir, "libmysql-5.7.38");

         string name = Path.Combine(libMySqlDir, "libmySQL.dll");

         if (!File.Exists(name))
         {
            throw new System.Exception($"The file {name} could not be found.");
         }

         return name;
      }

      /// <summary>
      /// Locates gflags.exe, which comes with the Debugging Tools for Windows feature
      /// of the Windows SDK.
      /// </summary>
      private static string GetGFlags()
      {
         var candidates = new List<string>();

         var sdkDir = System.Environment.GetEnvironmentVariable("WindowsSdkDir");

         if (!string.IsNullOrEmpty(sdkDir))
            candidates.Add(Path.Combine(sdkDir, @"Debuggers\x64\gflags.exe"));

         foreach (var programFiles in new[] { "ProgramFiles(x86)", "ProgramFiles" })
         {
            var root = System.Environment.GetEnvironmentVariable(programFiles);

            if (!string.IsNullOrEmpty(root))
               candidates.Add(Path.Combine(root, @"Windows Kits\10\Debuggers\x64\gflags.exe"));
         }

         foreach (var candidate in candidates)
         {
            if (File.Exists(candidate))
               return candidate;
         }

         throw new System.Exception("gflags.exe could not be found, so page heap cannot be enabled. " +
            "Install the Debugging Tools for Windows feature of the Windows SDK. Looked in:" +
            System.Environment.NewLine + string.Join(System.Environment.NewLine, candidates));
      }

      private class EnvironmentDto
      {
         public string Name { get; set; }
         public string OperatingSystem { get; set; }
         public string Description { get; set; }
         public string VmName { get; set; }
         public string SnapshotName { get; set; }
         public List<CommandDto> PreInstallCommands { get; set; } = new List<CommandDto>();
         public List<FileCopyDto> PreInstallFileCopy { get; set; } = new List<FileCopyDto>();
         public List<CommandDto> PostInstallCommands { get; set; } = new List<CommandDto>();
         public List<FileCopyDto> PostInstallFileCopy { get; set; } = new List<FileCopyDto>();
         public bool IncludeStressTests { get; set; } = false;
         public bool Enabled { get; set; } = true;
         public TestSuite TestSuite { get; set; } = TestSuite.RegressionTests;
         public bool EnablePageHeap { get; set; } = false;
         public GuestTransport GuestTransport { get; set; } = GuestTransport.PowerShellDirect;
         public string GuestAddress { get; set; }
      }

      private class CommandDto
      {
         public string Executable { get; set; }
         public string Parameters { get; set; }
      }

      private class FileCopyDto
      {
         public string From { get; set; }
         public string To { get; set; }
      }
   }
}
