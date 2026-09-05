using System;
using System.Diagnostics;
using System.IO;
using Microsoft.Win32;

namespace RegressionTests.Shared
{
   /// <summary>
   /// Locates hMailServer.ini the same way the server does. New installations keep
   /// the file in ProgramData, older ones in the Bin directory.
   /// </summary>
   public static class IniFileLocator
   {
      public static string GetIniFileName()
      {
         var programDataFile = Path.Combine(GetProgramDataDirectory(), "hMailServer.ini");

         if (File.Exists(programDataFile))
            return programDataFile;

         return Path.Combine(GetBinDirectory(), "hMailServer.ini");
      }

      public static string GetProgramDataDirectory()
      {
         return Path.Combine(
            Environment.GetFolderPath(Environment.SpecialFolder.CommonApplicationData),
            "hMailServer");
      }

      private static string GetBinDirectory()
      {
         // The installer writes the install location to the 32-bit registry view, so
         // read that view explicitly regardless of the bitness of the test process.
         using (var baseKey = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry32))
         using (var key = baseKey.OpenSubKey(@"SOFTWARE\hMailServer"))
         {
            var installLocation = key?.GetValue("InstallLocation") as string;

            if (!string.IsNullOrEmpty(installLocation))
               return Path.Combine(installLocation, "Bin");
         }

         var processes = Process.GetProcessesByName("hmailserver");

         if (processes.Length != 1)
            throw new InvalidOperationException("Expected exactly one running hMailServer.exe.");

         return Path.GetDirectoryName(processes[0].MainModule.FileName);
      }
   }
}
