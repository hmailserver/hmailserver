// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Microsoft.Win32;
using System.IO;
using System.Runtime.InteropServices;

namespace PasswordDisplayer
{
   public partial class formDatabasePassword : Form
   {
      [DllImport("kernel32")]
      private static extern int GetPrivateProfileString(string section,
               string key, string def, StringBuilder retVal,
          int size, string filePath);

      public formDatabasePassword()
      {
         InitializeComponent();
      }

      public string IniReadValue(string fileName, string Section, string Key)
      {
         StringBuilder temp = new StringBuilder(255);
         int i = GetPrivateProfileString(Section, Key, "", temp, 255, fileName);
         return temp.ToString();

      }


      private void formDatabasePassword_Load(object sender, EventArgs e)
      {
         // Locate hMailServer.ini
         RegistryKey key = Registry.LocalMachine.OpenSubKey(@"SOFTWARE\hMailServer");
         string installDir = key.GetValue( "InstallLocation" ) as string;
         string binDir = Path.Combine(installDir, "Bin");
         string iniFile = Path.Combine(binDir, "hMailServer.ini");

         // Read the database password.
         string encryptedPassword = IniReadValue(iniFile, "Database", "password");

         hMailServer.Application app = GetApp();

         string decryptedPassword = app.Utilities.BlowfishDecrypt(encryptedPassword);

         textPassword.Text = decryptedPassword;
      }

      private hMailServer.Application GetApp()
      {
         hMailServer.Application application = new hMailServer.Application();
         hMailServer.Account account = application.Authenticate("Administrator", "");
         
         if (account != null)
            return application;

         account = application.Authenticate("Administrator", "testar");

         if (account != null)
            return application;

         MessageBox.Show("Authentication failed", "Database password");

         return null;
      }

      private void buttonCopy_Click(object sender, EventArgs e)
      {
         
      }

      private void buttonCopyAndClose_Click(object sender, EventArgs e)
      {
         Clipboard.SetText(textPassword.Text);
         this.Close();
      }
   }
}
