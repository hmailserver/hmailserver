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
            var env = new TestEnvironment(item.OperatingSystem, item.Description, item.VmName, item.SnapshotName,
               item.IncludeStressTests, item.GuestTransport, item.GuestAddress);

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

      private class EnvironmentDto
      {
         public string OperatingSystem { get; set; }
         public string Description { get; set; }
         public string VmName { get; set; }
         public string SnapshotName { get; set; }
         public List<CommandDto> PreInstallCommands { get; set; } = new List<CommandDto>();
         public List<FileCopyDto> PreInstallFileCopy { get; set; } = new List<FileCopyDto>();
         public List<CommandDto> PostInstallCommands { get; set; } = new List<CommandDto>();
         public List<FileCopyDto> PostInstallFileCopy { get; set; } = new List<FileCopyDto>();
         public bool IncludeStressTests = false;
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
