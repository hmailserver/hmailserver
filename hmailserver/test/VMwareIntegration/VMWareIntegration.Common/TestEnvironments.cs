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
      
      private const string Windows10Path = 
         VirtualMachineBasePath + @"\Windows 10\Windows 10.vmx";

      private const string Windows10_4_4_4_InternalMySQL = 
         VirtualMachineBasePath + @"\Windows 10 - 4.4.4 - Internal MySQL\4.4.4 - Internal MySQL.vmx";

      private const string Windows10_5_0_0_InternalSQLCompact =
         VirtualMachineBasePath + @"\Windows 10 - 5.0.0 - Internal SQL Compact\Windows 10 - 5.0.0 - Internal SQL Compact.vmx";

      private const string Windows10_PostgreSQL_9_6_24 =
         VirtualMachineBasePath + @"\Windows 10 - PostgreSQL 9.6.24\Windows 10 - PostgreSQL 9.6.24.vmx";

      private const string Windows10_SQL_Server_2019 =
         VirtualMachineBasePath + @"\Windows 10 - SQL Server 2019\Windows 10 - SQL Server 2019.vmx";

      private const string Windows10_MySQL_8_0_29 =
         VirtualMachineBasePath + @"\Windows 10 - MySQL 8.0.29\Windows 10 - MySQL 8.0.29.vmx";

      private const string Windows10_4_4_4_SQL_Server_2019 =
         VirtualMachineBasePath + @"\Windows 10 - 4.4.4 - SQL Server 2019\Windows 10 - 4.4.4 - SQL Server 2019.vmx";

      private const string Windows10_5_0_0_PostgreSQL_9_6_24 =
         VirtualMachineBasePath + @"\Windows 10 - 5.0.0 - PostgreSQL 9.6.24\Windows 10 - 5.0.0 - PostgreSQL 9.6.24.vmx";

      private const string Windows10_4_4_4_MySQL_8_0_29 =
         VirtualMachineBasePath + @"\Windows 10 - 4.4.4 - MySQL 8.0.29\Windows 10 - 4.4.4 - MySQL 8.0.29.vmx";



      static public void AddAll(List<TestEnvironment> listEnvironments)
      {
         // New installation
         listEnvironments.AddRange(new List<TestEnvironment>()
         {
            new TestEnvironment("Windows 10", "New installation", "Internal (SQL Compact)",  Windows10Path, "Booted"),
            new TestEnvironment("Windows 10", "New installation", "PostgreSQL 9.6.24",  Windows10_PostgreSQL_9_6_24, "Booted")
            {
               PostInstallCommands = { new PostInstallCommand(@"C:\Program Files (x86)\hMailServer\Bin\DBSetup.exe", "/silent ServerType:PGSQL ServerAddress:localhost DatabaseName:hMailServer Authentication:Server CreateNew:Yes Username:postgres Password:Secret123") }
            },
            new TestEnvironment("Windows 10", "New installation9", "SQL Server 2019",  Windows10_SQL_Server_2019, "Booted")
            {
               PostInstallCommands = { new PostInstallCommand(@"C:\Program Files (x86)\hMailServer\Bin\DBSetup.exe", "/silent ServerType:MSSQL ServerAddress:.\\SQLEXPRESS DatabaseName:hMailServer Authentication:SServer CreateNew:Yes Username:sa Password:Secret123") }
            },
            new TestEnvironment("Windows 10", "New installation9", "MySQL 8.0.29",  Windows10_MySQL_8_0_29, "Booted")
            {
               PostInstallCommands = { new PostInstallCommand(@"C:\Program Files (x86)\hMailServer\Bin\DBSetup.exe", "/silent ServerType:MySQL ServerAddress:localhost DatabaseName:hMailServer Authentication:Server CreateNew:Yes Username:root Password:Secret123") },
               PostInstallFileCopy = { new PostInstallFileCopy(Path.Combine(GetMySQLLib()), @"C:\Program Files (x86)\hMailServer\Bin\libmySQL.dll") }
            }
         });

         // Upgrade tests
         listEnvironments.AddRange(new List<TestEnvironment>()
         {
            new TestEnvironment("Windows 10", "Upgrade, 4.1.1", "Internal",  Windows10_4_4_4_InternalMySQL, "Booted"),
            new TestEnvironment("Windows 10", "Upgrade, 5.0.0", "Internal",  Windows10_5_0_0_InternalSQLCompact, "Booted"),
            new TestEnvironment("Windows 10", "Upgrade, 4.4.4", "SQL Server 2019",  Windows10_4_4_4_SQL_Server_2019, "Booted"),
            new TestEnvironment("Windows 10", "Upgrade, 5.0.0", "PostgreSQL 9.6.24", Windows10_5_0_0_PostgreSQL_9_6_24, "Booted"),
            new TestEnvironment("Windows 10", "Upgrade, 4.4.4", "MySQL 8.0.29", Windows10_4_4_4_MySQL_8_0_29, "Booted")
         });
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
