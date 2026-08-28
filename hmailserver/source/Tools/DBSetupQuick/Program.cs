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
      static int Main()
      {
         CommandLineParser.Parse();

         _application = new hMailServer.Application();

         if (_application.Database.DatabaseExists)
            return UpgradeDatabase();
         else
            return CreateDatabase();
      }

      /// <summary>
      /// Shows an error, unless we're running unattended. In that case there may be
      /// no desktop to show it on, so the failure is reported using the exit code instead.
      /// </summary>
      private static void ShowError(string text)
      {
         if (CommandLineParser.IsSilent())
            return;

         MessageBox.Show(text, "hMailServer", MessageBoxButtons.OK, MessageBoxIcon.Error);
      }

      private static int UpgradeDatabase()
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

            // The updater needs the administrator password as well, since it can't ask
            // for it when running unattended.
            if (CommandLineParser.ContainsArgument("password"))
               arguments += " password:" + CommandLineParser.GetArgument("password");

            upgradeProcess.Arguments = arguments;

            // Launch upgrader and wait for it to complete.
            Process p = Process.Start(upgradeProcess);
            p.WaitForExit();

            return p.ExitCode;
         }
         catch (Exception ex)
         {
            ShowError("Failed to start DBUpdater.exe" + Environment.NewLine + ex.Message);
            return ExitCodes.UnexpectedError;
         }
      }

      private static int CreateDatabase()
      {
         string adminPassword = string.Empty;

         if (CommandLineParser.ContainsArgument("password"))
            adminPassword = CommandLineParser.GetArgument("password");

         if (!Authenticator.AuthenticateUser(_application, adminPassword))
            return ExitCodes.AuthenticationFailed;

         if (_application.Database.DatabaseType == hMailServer.eDBtype.hDBTypeMSSQLCE ||
             _application.Database.DatabaseType == hMailServer.eDBtype.hDBTypeUnknown)
         {
            return InitializeInternalDatabase();
         }

         return ExitCodes.Success;
      }

      private static int InitializeInternalDatabase()
      {
          try
          {
              hMailServer.Database database = _application.Database;

              database.CreateInternalDatabase();

              // Database has been upgraded. Reinitialize the connections.
              _application.Reinitialize();

              // Re-initialize to connect to the newly created database.
              _application.Reinitialize();

              return ExitCodes.Success;
          }
          catch (Exception ex)
          {
              ShowError(ex.Message);
              return ExitCodes.UnexpectedError;
          }
      }

   }
}
