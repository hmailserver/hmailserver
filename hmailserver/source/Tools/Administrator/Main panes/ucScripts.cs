// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class ucScripts : UserControl, ISettingsControl
   {
      private hMailServer.Scripting _scriptingSettings;

      public ucScripts()
      {
         InitializeComponent();

         DirtyChecker.SubscribeToChange(this, OnContentChanged);

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

         comboLanguage.AddItem("VBScript", "VBScript");
         comboLanguage.AddItem("JScript", "JScript");

         hMailServer.Application app = APICreator.Application;
         hMailServer.Settings settings = app.Settings;
         _scriptingSettings = app.Settings.Scripting;

         Marshal.ReleaseComObject(settings);
      }

      public void OnLeavePage()
      {
          if (_scriptingSettings != null)
              Marshal.ReleaseComObject(_scriptingSettings);
      }

      private void OnContentChanged()
      {
         Instances.MainForm.OnContentChanged();
      }

      private void OnContentChanged(object sender, EventArgs e)
      {
         OnContentChanged();
      }

      public bool Dirty
      {
         get
         {
            return DirtyChecker.IsDirty(this);
         }
      }

      public void LoadData()
      {
         checkEnabled.Checked = _scriptingSettings.Enabled;
         comboLanguage.SelectedValue = _scriptingSettings.Language;

         EnableDisable();
      }

      public bool SaveData()
      {

         _scriptingSettings.Enabled = checkEnabled.Checked;
         _scriptingSettings.Language = (string)comboLanguage.SelectedValue;

         DirtyChecker.SetClean(this);

         return true;
      }

      private void EnableDisable()
      {
         comboLanguage.Enabled = checkEnabled.Checked;

         buttonShowScripts.Enabled = checkEnabled.Checked;
         buttonCheckSyntax.Enabled = checkEnabled.Checked;
         buttonReloadScripts.Enabled = checkEnabled.Checked;
      }

      public void LoadResources()
      {
         // load the translated resources
      }

      private void checkEnabled_CheckedChanged(object sender, EventArgs e)
      {
         EnableDisable();
      }

      private void buttonShowScripts_Click(object sender, EventArgs e)
      {

         System.Diagnostics.ProcessStartInfo explorer = new System.Diagnostics.ProcessStartInfo();
         explorer.FileName = "explorer.exe";
         explorer.Arguments = _scriptingSettings.Directory;
         System.Diagnostics.Process.Start(explorer);
      }

      private void buttonCheckSyntax_Click(object sender, EventArgs e)
      {
         string result = _scriptingSettings.CheckSyntax();

         MessageBoxIcon icon = MessageBoxIcon.Error;
         if (result.Length == 0)
         {
            result = "The syntax is correct.";
            icon = MessageBoxIcon.Information;
         }

         MessageBox.Show(result, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, icon);
      }

      private void buttonReloadScripts_Click(object sender, EventArgs e)
      {
         _scriptingSettings.Reload();
      }
   }
}
