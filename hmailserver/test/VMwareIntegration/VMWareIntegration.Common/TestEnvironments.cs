// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Reflection;

namespace VMwareIntegration.Common
{
   public class TestEnvironments
   {
      private const string VirtualMachineBasePath = @"C:\Dev\Build\VMWare";
      private const string Windows10Path = VirtualMachineBasePath + @"\Windows 10\Windows 10.vmx";

      static public void AddAll(List<TestEnvironment> listEnvironments)
      {
         listEnvironments.AddRange(new List<TestEnvironment>()
         {
            new TestEnvironment("Windows 10", "New installation, built in SQL Compact", "Internal",  Windows10Path, "Booted")
         });

         //AddBasicTestEnvironments(listEnvironments);
         //AddUpgradeTests(listEnvironments);
      }

      //static void AddBasicTestEnvironments(List<TestEnvironment> listEnvironments)
      //{
      //   TestXPSP3(listEnvironments);
      //   TestWindows2003(listEnvironments);
      //   TestWindows2008(listEnvironments);
      //   TestWindowsVista(listEnvironments);
      //}

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

      //private static void TestXPSP3(List<TestEnvironment> listEnvironments)
      //{
      //   // Internal database
      //   listEnvironments.Add(new TestEnvironment("Windows XP SP3", "New installation", "Internal", WindowsXPPath, "Windows XP SP3, .NET Framework 2.0"));

      //   // Test using PostgreSQL...
      //   TestEnvironment tempEnv2 = new TestEnvironment("Windows XP SP3", "New installation", "PostgreSQL 8.3.1", WindowsXPPostgreSQL831Path, "Initial");
      //   tempEnv2.PostInstallCommands.Add(new PostInstallCommand(@"C:\Program Files\hMailServer\Bin\DBSetup.exe", "/silent ServerType:PGSQL ServerAddress:localhost DatabaseName:hMailServer Authentication:Server CreateNew:Yes Username:postgres Password:build"));
      //   listEnvironments.Add(tempEnv2);

      //   // Test using MySQL 5.0.51.
      //   TestEnvironment tempEnv1 = new TestEnvironment("Windows XP SP3", "New installation", "MySQL 5.0.51", WindowsXPMySQL5_0_51Path, "Initial");
      //   tempEnv1.PostInstallCommands.Add(new PostInstallCommand(@"C:\Program Files\hMailServer\Bin\DBSetup.exe", "/silent ServerType:MySQL ServerAddress:localhost DatabaseName:hMailServer Authentication:Server CreateNew:Yes Username:root Password:build"));
      //   tempEnv1.PostInstallFileCopy.Add(new PostInstallFileCopy(Path.Combine(GetTestDataDir(), GetMySQLLib()), @"C:\Program Files\hMailServer\Bin\libmySQL.dll"));
      //   listEnvironments.Add(tempEnv1);

      //   // Test using MySQL 5.6.20 (with proper UTF8-support).
      //   TestEnvironment tempEnv3 = new TestEnvironment("Windows XP SP3", "New installation", "MySQL 5.6.20", WindowsXPMySQL5_6_20Path, "Initial");
      //   tempEnv3.PostInstallCommands.Add(new PostInstallCommand(@"C:\Program Files\hMailServer\Bin\DBSetup.exe", "/silent ServerType:MySQL ServerAddress:localhost DatabaseName:hMailServer Authentication:Server CreateNew:Yes Username:root Password:build"));
      //   tempEnv3.PostInstallFileCopy.Add(new PostInstallFileCopy(Path.Combine(GetTestDataDir(), GetMySQLLib()), @"C:\Program Files\hMailServer\Bin\libmySQL.dll"));
      //   listEnvironments.Add(tempEnv3);

      //   // Test using SQL Server 2000.
      //   var tempEnv4 = new TestEnvironment("Windows XP SP3", "New installation", "SQL Server 2000", WindowsXPSQLServer2000Path, "Initial");
      //   tempEnv4.PostInstallCommands.Add(new PostInstallCommand(@"C:\Program Files\hMailServer\Bin\DBSetup.exe", "/silent ServerType:MSSQL ServerAddress:. DatabaseName:hMailServer Authentication:Windows CreateNew:Yes"));
      //   listEnvironments.Add(tempEnv4);

      //   // Test using SQL Server 2005.
      //   TestEnvironment tempEnv5 = new TestEnvironment("Windows XP SP3", "New installation", "SQL Server 2005", WindowsXPSQLServer2005Path, "Initial");
      //   tempEnv5.PostInstallCommands.Add(new PostInstallCommand(@"C:\Program Files\hMailServer\Bin\DBSetup.exe", "/silent ServerType:MSSQL ServerAddress:HI DatabaseName:hMailServer Authentication:Windows CreateNew:Yes"));
      //   listEnvironments.Add(tempEnv5);
      //}

      //private static void TestWindows2008(List<TestEnvironment> listEnvironments)
      //{
      //   // Two basics using internal database
      //   listEnvironments.Add(new TestEnvironment("Windows 2008", "New installation", "Internal", Windows2008Path, "Windows Server 2008"));
      //}

      //private static void TestWindows2003(List<TestEnvironment> listEnvironments)
      //{
      //   // Two basics using internal database
      //   listEnvironments.Add(new TestEnvironment("Windows 2003", "New installation", "Internal", Windows2003Path, ".NET framework 2.0 installed"));
      //}


      //private static void TestWindowsVista(List<TestEnvironment> listEnvironments)
      //{
      //   // Two basics using internal database
      //   listEnvironments.Add(new TestEnvironment("Windows Vista", "New installation", "Internal", WindowsVistaPath, "Windows Vista SP2"));
      //}

      //static void AddUpgradeTests(List<TestEnvironment> listEnvironments)
      //{
      //   listEnvironments.Add(new TestEnvironment("Windows XP SP3", "Upgrade", "PostgreSQL 8.3.1", WindowsXPHmail50PostgreSQL8_3_1, "Initial"));
      //   listEnvironments.Add(new TestEnvironment("Windows XP SP3", "Upgrade", "Internal", WindowsXPHmail441InternalMySQL, "Initial"));
      //   listEnvironments.Add(new TestEnvironment("Windows XP SP3", "Upgrade", "SQL Server", WindowsXPHmail441SQLServerPath, "Initial"));
      //   listEnvironments.Add(new TestEnvironment("Windows XP SP3", "Upgrade", "Internal", WindowsXPHmail50InternalMSSQLCE, "Initial"));
      //   listEnvironments.Add(new TestEnvironment("Windows XP SP3", "Upgrade", "SQL Server 2000", WindowsXPHmail33SQL2000, "Initial"));

      //}
   }
}
