
using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

namespace VMwareIntegration.Common
{
   public class TestEnvironments
   {
      private static string WindowsServer2019Path = @"C:\Dev\Build\VMWareX64\Windows Server 2008 x64\Windows Server 2008 x64.vmx";

      public static void AddAll(List<TestEnvironment> listEnvironments)
      {
         AddBasicTestEnvironments(listEnvironments);
      }

      static void AddBasicTestEnvironments(List<TestEnvironment> listEnvironments)
      {
         listEnvironments.Add(new TestEnvironment("Windows Server 2008 SP2", "New installation", "Internal", WindowsServer2019Path, "VMware Tools installed"));
      }

      private static string GetTestDataDir()
      {
          string currentDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
          string testDataDir = Path.Combine(currentDir, @"..\..\..\..\TestData");
          string shortendName = Path.GetFullPath(testDataDir);
          return shortendName;
      }

      private static string GetMySQLLib()
      {
          string name = Path.Combine(GetTestDataDir(), "MySQLLib\\libmySQL.dll");

          if (!File.Exists(name))
          {
              throw new Exception("The file " + name + " could not be found.");
          }

          return name;
      }
   }
}
