using System.IO;
using System.Reflection;

namespace VMTestRunner.Console
{
   public class TestSettings
   {
      /// <summary>
      /// Get test folder.
      /// </summary>
      /// <returns></returns>
      public static string GetTestFolder()
      {
         string currentDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
         string fixturePath = Path.Combine(currentDir, @"..\..\..\..\");

         return Path.GetFullPath(fixturePath);
      }
   }
}
