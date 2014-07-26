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
      private static string WindowsXPPath = @"C:\Build\VMware\Windows XP Professional\Windows XP Professional.vmx";
      private static string WindowsXPPostgreSQL831Path = @"C:\Build\VMWare\Windows XP - PostgreSQL 8.3.1 (Link)\Windows XP - PostgreSQL 8.3.1 (Link).vmx";
      private static string WindowsXPMySQL5_0_51Path = @"C:\Build\VMWare\Windows XP - MySQL 5.0.51 (Link)\Windows XP - MySQL 5.0.51 (Link).vmx";
      private static string WindowsXPSQLServer2005Path = @"C:\Build\VMWare\Windows XP - SQL Server 2005\Windows XP - SQL Server 2005.vmx";
      private static string WindowsXPSQLServer2000Path = @"C:\Build\VMWare\Windows XP - SQL Server 2000 (Link)\Windows XP - SQL Server 2000 (Link).vmx";
      private static string WindowsXPHmail441SQLServerPath = @"C:\Build\VMWare\Windows XP - hMailServer 4.4.1 (SQL Server) (Link)\Windows XP - hMailServer 4.4.1 (SQL Server) (Link).vmx";
      private static string WindowsXPHmail50PostgreSQL8_3_1 = @"C:\Build\VMWare\Windows XP - hMailServer 5.0 B289 (PostgreSQL 8.3.1) (Link)\Windows XP - hMailServer 5.0 B289 (PostgreSQL 8.3.1) (Link).vmx";
                                                               
      private static string WindowsXPHmail441InternalMySQL = @"C:\Build\VMWare\Windows XP - hMailServer 4.4.1 (Internal MySQL) (Link)\Windows XP - hMailServer 4.4.1 (Internal MySQL) (Link).vmx";
      private static string WindowsXPHmail50InternalMSSQLCE = @"C:\Build\VMWare\Windows XP - hMailServer 5.0 (B289 Internal SQL CE)\Windows XP - hMailServer 5.0 (B289 Internal SQL CE).vmx";
      private static string WindowsXPHmail33SQL2000 = @"C:\Build\VMWare\Windows XP - hMailServer 3.3 (B57, SQL Server 2000)\Windows XP - hMailServer 3.3 (B57, SQL Server 2000).vmx";

      private static string Windows2000Path = @"C:\Build\VMware\Windows 2000\Windows 2000 Professional.vmx";
      private static string Windows2003Path = @"C:\Build\VMware\Windows Server 2003 Enterprise Edition\Windows Server 2003 Enterprise Edition.vmx";
      private static string Windows2008Path = @"C:\Build\VMware\Windows Server 2008\Windows Server 2008 (experimental).vmx";
      private static string WindowsVistaPath = @"C:\Build\VMware\Windows Vista\Windows Vista.vmx";


      static public void AddAll(List<TestEnvironment> listEnvironments)
      {
         AddBasicTestEnvironments(listEnvironments);
         AddUpgradeTests(listEnvironments);
      }

      static void AddBasicTestEnvironments(List<TestEnvironment> listEnvironments)
      {
         TestWindows2000(listEnvironments);
         TestXPSP2(listEnvironments);
         TestWindows2003(listEnvironments);
         TestWindows2008(listEnvironments);
         TestWindowsVista(listEnvironments);
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

      private static void TestWindows2000(List<TestEnvironment> listEnvironments)
      {
         // Two basics using internal database
         listEnvironments.Add(new TestEnvironment("Windows 2000", "New installation", "Internal", Windows2000Path, "Service Pack 4"));
      }

      private static void TestXPSP2(List<TestEnvironment> listEnvironments)
      {
         // Test using PostgreSQL...
         TestEnvironment tempEnv2 = new TestEnvironment("Windows XP SP2", "New installation", "PostgreSQL 8.3.1", WindowsXPPostgreSQL831Path, "Initial");
         tempEnv2.PostInstallCommands.Add(new PostInstallCommand(@"C:\Program Files\hMailServer\Bin\DBSetup.exe", "/silent ServerType:PGSQL ServerAddress:localhost DatabaseName:hMailServer Authentication:Server CreateNew:Yes Username:postgres Password:build"));
         listEnvironments.Add(tempEnv2);

         // One more complex using MySQL.
         TestEnvironment tempEnv1 = new TestEnvironment("Windows XP SP2", "New installation", "MySQL 5.0.51", WindowsXPMySQL5_0_51Path, "Initial");
         tempEnv1.PostInstallCommands.Add(new PostInstallCommand(@"C:\Program Files\hMailServer\Bin\DBSetup.exe", "/silent ServerType:MySQL ServerAddress:localhost DatabaseName:hMailServer Authentication:Server CreateNew:Yes Username:root Password:build"));
         tempEnv1.PostInstallFileCopy.Add(new PostInstallFileCopy(Path.Combine(GetTestDataDir(), GetMySQLLib()), @"C:\Program Files\hMailServer\Bin\libmySQL.dll"));
         listEnvironments.Add(tempEnv1);

         // One basic using internal database
         listEnvironments.Add(new TestEnvironment("Windows XP SP2", "New installation", "Internal", WindowsXPPath, "Windows XP SP2, .NET Framework 2.0"));

         // One more complex using SQL Server 2005.
         TestEnvironment tempEnv = new TestEnvironment("Windows XP SP2", "New installation", "SQL Server 2005", WindowsXPSQLServer2005Path, "Initial");
         tempEnv.PostInstallCommands.Add(new PostInstallCommand(@"C:\Program Files\hMailServer\Bin\DBSetup.exe", "/silent ServerType:MSSQL ServerAddress:HI DatabaseName:hMailServer Authentication:Windows CreateNew:Yes"));
         listEnvironments.Add(tempEnv);

         // Test one using SQL Server 2000.
         tempEnv = new TestEnvironment("Windows XP SP2", "New installation", "SQL Server 2000", WindowsXPSQLServer2000Path, "Initial");
         tempEnv.PostInstallCommands.Add(new PostInstallCommand(@"C:\Program Files\hMailServer\Bin\DBSetup.exe", "/silent ServerType:MSSQL ServerAddress:. DatabaseName:hMailServer Authentication:Windows CreateNew:Yes"));
         listEnvironments.Add(tempEnv);
      }

      private static void TestWindows2008(List<TestEnvironment> listEnvironments)
      {
         // Two basics using internal database
         listEnvironments.Add(new TestEnvironment("Windows 2008", "New installation", "Internal", Windows2008Path, "Windows Server 2008"));
      }

      private static void TestWindows2003(List<TestEnvironment> listEnvironments)
      {
         // Two basics using internal database
         listEnvironments.Add(new TestEnvironment("Windows 2003", "New installation", "Internal", Windows2003Path, ".NET framework 2.0 installed"));
      }


      private static void TestWindowsVista(List<TestEnvironment> listEnvironments)
      {
         // Two basics using internal database
         listEnvironments.Add(new TestEnvironment("Windows Vista", "New installation", "Internal", WindowsVistaPath, "Windows Vista SP2"));
      }

      static void AddUpgradeTests(List<TestEnvironment> listEnvironments)
      {
         listEnvironments.Add(new TestEnvironment("Windows XP SP2", "Upgrade", "PostgreSQL 8.3.1", WindowsXPHmail50PostgreSQL8_3_1, "Initial"));
         listEnvironments.Add(new TestEnvironment("Windows XP SP2", "Upgrade", "Internal", WindowsXPHmail441InternalMySQL, "Initial"));
         listEnvironments.Add(new TestEnvironment("Windows XP SP2", "Upgrade", "SQL Server", WindowsXPHmail441SQLServerPath, "Initial"));
         listEnvironments.Add(new TestEnvironment("Windows XP SP2", "Upgrade", "Internal", WindowsXPHmail50InternalMSSQLCE, "Initial"));
         listEnvironments.Add(new TestEnvironment("Windows XP SP2", "Upgrade", "SQL Server 2000", WindowsXPHmail33SQL2000, "Initial"));

      }
   }
}
