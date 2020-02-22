// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using hMailServer.Administrator.Nodes;
using hMailServer.Shared;
using Microsoft.Win32;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;

namespace hMailServer.Administrator
{
   class Utility
   {
      [DllImport("kernel32")]
      private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

      public static void RefreshNode(string name)
      {
         // Refresh the node in the tree if the name has changed.
         IMainForm mainForm = Instances.MainForm;
         mainForm.RefreshCurrentNode(name);

         SearchNodeText crit = new SearchNodeText(name);
         mainForm.SelectNode(crit);

      }

      public static bool DetectClamWinPaths(out string executablePath, out string databasePath)
      {
         executablePath = "";
         databasePath = "";

         RegistryKey subKey = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\ClamWin");
         if (subKey == null)
            return false;

         object pathValue = subKey.GetValue("Path");
         if (pathValue == null)
            return false;

         string executableDirectory = pathValue.ToString();
         if (string.IsNullOrEmpty(executableDirectory))
            return false;

         string configFile = Path.Combine(executableDirectory, "ClamWin.conf");
         string executable = IniReadValue(configFile, "ClamAV", "clamscan");
         string database = IniReadValue(configFile, "ClamAV", "database");

         if (string.IsNullOrEmpty(executable) || string.IsNullOrEmpty(database))
            return false;

         executablePath = executable;
         databasePath = database;

         return true;
         
      }

      public static string IniReadValue(string file, string section, string key)
      {
         StringBuilder temp = new StringBuilder(255);
         int i = GetPrivateProfileString(section, key, "", temp,
                                         255, file);
         return temp.ToString();

      }

      static internal bool AskDeleteItem(string objectName)
      {
         string question = Strings.Localize("Are you sure you want to delete %s?");
         question = question.Replace("%s", objectName);

         if (MessageBox.Show(question, EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNo) == DialogResult.Yes)
            return true;
         else
            return false;
      }


      static internal bool AskDeleteItems()
      {
         string question = Strings.Localize("Are you sure you want to delete the selected items?");

         if (MessageBox.Show(question, EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNo) == DialogResult.Yes)
            return true;
         else
            return false;
      }

      static internal void AskRestartServer()
      {
         if (MessageBox.Show(Strings.Localize("hMailServer needs to be restarted for the changes to take effect.") + Environment.NewLine +
                    Strings.Localize("Do you want to restart hMailServer now?"), EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
         {
            using (new WaitCursor())
            {
               hMailServer.Application application = APICreator.Application;
               application.Stop();
               application.Start();

               MessageBox.Show(Strings.Localize("The hMailServer server has been restarted."), EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
         }
      }
   }
}
