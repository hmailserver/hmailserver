// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Dialogs;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class ucProtocolIMAP : UserControl, ISettingsControl
   {
      public ucProtocolIMAP()
      {
         InitializeComponent();

         DirtyChecker.SubscribeToChange(this, OnContentChanged);

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
      }

      public void OnLeavePage()
      {

      }

      public bool Dirty
      {
         get 
         {
            return DirtyChecker.IsDirty(this);
         }
      }

      private void OnContentChanged()
      {
         Instances.MainForm.OnContentChanged();
      }

      private void OnContentChanged(object sender, EventArgs e)
      {
         OnContentChanged();
      }

      public void LoadData()
      {
         hMailServer.Settings settings = APICreator.Application.Settings;

         textMaxIMAPConnections.Number = settings.MaxIMAPConnections;
         textWelcomeMessage.Text = settings.WelcomeIMAP;

         checkEnableIMAPSort.Checked = settings.IMAPSortEnabled;
         checkEnableIMAPQuota.Checked = settings.IMAPQuotaEnabled;
         checkEnableIMAPIdle.Checked = settings.IMAPIdleEnabled;
         checkEnableIMAPACL.Checked = settings.IMAPACLEnabled;
         checkEnableIMAPPlain.Checked = settings.IMAPSASLPlainEnabled;
         checkEnableIMAPInitialResponse.Checked = settings.IMAPSASLInitialResponseEnabled;
         textIMAPMasterUser.Text = settings.IMAPMasterUser;

         textIMAPPublicFolderName.Text = settings.IMAPPublicFolderName;
         comboIMAPHierarchyDelimiter.Text = settings.IMAPHierarchyDelimiter;

         Marshal.ReleaseComObject(settings);

      }

      public bool SaveData()
      {
         hMailServer.Settings settings = APICreator.Application.Settings;

         settings.MaxIMAPConnections = textMaxIMAPConnections.Number;
         settings.WelcomeIMAP = textWelcomeMessage.Text;

         settings.IMAPSortEnabled = checkEnableIMAPSort.Checked;
         settings.IMAPQuotaEnabled = checkEnableIMAPQuota.Checked;
         settings.IMAPIdleEnabled = checkEnableIMAPIdle.Checked;
         settings.IMAPACLEnabled = checkEnableIMAPACL.Checked;
         settings.IMAPPublicFolderName = textIMAPPublicFolderName.Text;

         settings.IMAPSASLPlainEnabled = checkEnableIMAPPlain.Checked;
         settings.IMAPSASLInitialResponseEnabled = checkEnableIMAPInitialResponse.Checked;
         settings.IMAPMasterUser = textIMAPMasterUser.Text;
         
         bool setClean = true;
         try
         {
            settings.IMAPHierarchyDelimiter = comboIMAPHierarchyDelimiter.Text;
         }
         catch (Exception ex)
         {
            setClean = false;
            MessageBox.Show(ex.Message, EnumStrings.hMailServerAdministrator);
         }

         Marshal.ReleaseComObject(settings);

         if (setClean)
            DirtyChecker.SetClean(this);

         return true;
      }

      public void LoadResources()
      {
         // load the translated resources
      }

      private void buttonEditFolders_Click(object sender, EventArgs e)
      {
         hMailServer.Settings settings = APICreator.Application.Settings;
         hMailServer.IMAPFolders publicFolders = settings.PublicFolders;
         
         formAccountFolders accountFoldersDlg = new formAccountFolders(publicFolders, true);
         accountFoldersDlg.ShowDialog();

         Marshal.ReleaseComObject(settings);
         Marshal.ReleaseComObject(publicFolders);

      }
   }
}
