// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Shared;

namespace DBUpdater
{
   static class Program
   {
      /// <summary>
      /// The main entry point for the application.
      /// </summary>
      [STAThread]
      static void Main()
      {
         Application.EnableVisualStyles();
         Application.SetCompatibleTextRenderingDefault(false);

         string databaseOldErrorMessage = "The database is too old for this version of hMailServer.";

         try
         {
            CommandLineParser.Parse();

            hMailServer.Application application = new hMailServer.Application();

             try
             {
                 application.Connect();
             }
             catch (Exception ex)
             {
                 if (!ex.Message.Contains(databaseOldErrorMessage))
                     throw ex;

             }
            

            int from = application.Database.CurrentVersion;
            int to = application.Database.RequiredVersion;

            if (from == to)
            {
               if (!CommandLineParser.ContainsArgument("/SilentIfOk") && !CommandLineParser.IsSilent())
                  MessageBox.Show("Your hMailServer database is already up to date.", "hMailServer Administrator");

               return;
            }

            if (!Authenticator.AuthenticateUser(application))
               return;

            formMain main = new formMain(application);

            if (!main.LoadSettings())
               return;

            if (!main.CreateUpgradePath())
               return;

            if (CommandLineParser.IsSilent())
            {
               // Silently perform the upgrade
               main.DoUpgrade();
               return;
            }

            // Do it the default way.
            Application.Run(main);
         }
         catch (Exception ex)
         {
             MessageBox.Show(ex.Message + Environment.NewLine + Environment.NewLine + "Please check the hMailServer error log for further details.", "hMailServer Administrator", MessageBoxButtons.OK, MessageBoxIcon.Error);
         }
      }
   }
}