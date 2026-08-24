using System;
using System.IO;
using System.Linq;
using System.Reflection;

namespace VMTestRunner.Console
{
   /// <summary>
   /// Holds information shared by all tests in a single run, such as the timestamp
   /// used in the names of the log file and the NUnit result files.
   /// </summary>
   public static class RunContext
   {
      public const string TimestampVariableName = "runtimestamp";

      public static string Timestamp { get; } = DateTime.Now.ToString("yyyy-MM-dd_HHmmss");

      /// <summary>
      /// Returns the path of the file where the NUnit result of the given environment is stored.
      /// </summary>
      public static string GetResultFilePath(TestEnvironment environment)
      {
         var baseDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
         var name = Sanitize($"{environment.OperatingSystem} {environment.Description}");

         return Path.Combine(baseDirectory, $"vmtestrunner-{Timestamp}_{name}.xml");
      }

      private static string Sanitize(string name)
      {
         var characters = name.Select(c => char.IsLetterOrDigit(c) ? c : '-').ToArray();

         // Collapse runs of separators, so that for example "Internal: Upgrade, 4.1.1" becomes "Internal-Upgrade-4-1-1".
         var result = new string(characters);

         while (result.Contains("--"))
            result = result.Replace("--", "-");

         return result.Trim('-');
      }
   }
}
