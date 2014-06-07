// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Diagnostics;
using hMailServer.Shared;

namespace DBSetupQuick
{
   static class Program
   {
      private static hMailServer.Application _application;

      [STAThread]
      static void Main()
      {
         CommandLineParser.Parse();

         _application = new hMailServer.Application();

         if (_application.Database.DatabaseExists)
            UpgradeDatabase();
         else
            CreateDatabase();
      }

      private static void UpgradeDatabase()
      {
         try
         {
            // Database upgrader
            System.Diagnostics.ProcessStartInfo upgradeProcess = new System.Diagnostics.ProcessStartInfo();
            upgradeProcess.FileName = "DBUpdater.exe";

            // Means that it should automatically exit if already up to date. This is always
            // the case when we launch it via 'quick'.
            string arguments = "/SilentIfOk";

            // If the /silent param has been supplied to this process, we should forward it to the updater
            if (CommandLineParser.IsSilent())
               arguments += " /silent";

            upgradeProcess.Arguments = arguments;

            // Launch upgrader and wait for it to complete.
            Process p = Process.Start(upgradeProcess);
            p.WaitForExit();
         }
         catch (Exception ex)
         {
            MessageBox.Show("Failed to start DBUpdater.exe" + Environment.NewLine + ex.Message, "hMailServer");
         }
      }

      private static void CreateDatabase()
      {
         string adminPassword = string.Empty;

         if (CommandLineParser.ContainsArgument("password"))
            adminPassword = CommandLineParser.GetArgument("password");

         if (!Authenticator.AuthenticateUser(_application, adminPassword))
            return;

         if (_application.Database.DatabaseType == hMailServer.eDBtype.hDBTypeMSSQLCE ||
             _application.Database.DatabaseType == hMailServer.eDBtype.hDBTypeUnknown)
         {
            InitializeInternalDatabase();
         }
      }

      private static void InitializeInternalDatabase()
      {
          try
          {
              hMailServer.Database database = _application.Database;

              database.CreateInternalDatabase();

              // Database has been upgraded. Reinitialize the connections.
              _application.Reinitialize();

              // Re-initialize to connect to the newly created database.
              _application.Reinitialize();
          }
          catch (Exception ex)
          {
              MessageBox.Show(ex.Message, "hMailServer", MessageBoxButtons.OK, MessageBoxIcon.Error);
          }
      }

   }
}