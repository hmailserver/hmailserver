// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace DataDirectorySynchronizer.Pages
{
   public partial class ucSelectDomain : UserControl, IWizardPage
   {
      public ucSelectDomain()
      {
         InitializeComponent();

         hMailServer.Application application = Globals.GetApp();
         hMailServer.Domains domains = application.Domains;

         for (int i = 0; i < domains.Count; i++)
         {
            hMailServer.Domain domain = domains[i];

            listViewDomains.Items.Add(domain.Name);

            Marshal.ReleaseComObject(domain);
         }

         Marshal.ReleaseComObject(domains);

         AddPublicFolders();
      }

      private void AddPublicFolders()
      {
         hMailServer.Application application = Globals.GetApp();
         hMailServer.Settings settings = application.Settings;
         hMailServer.IMAPFolders publicFolders = settings.PublicFolders;

         AddPublicFolders(publicFolders, "", settings.IMAPHierarchyDelimiter);

         Marshal.ReleaseComObject(publicFolders);
         Marshal.ReleaseComObject(settings);
      }

      private void AddPublicFolders(hMailServer.IMAPFolders folders, string parentPath, string hierarchyDelimiter)
      {
         for (int i = 0; i < folders.Count; i++)
         {
            hMailServer.IMAPFolder folder = folders[i];

            string path = parentPath.Length == 0
                             ? folder.Name
                             : parentPath + hierarchyDelimiter + folder.Name;

            comboPublicFolderImportPath.Items.Add(path);

            hMailServer.IMAPFolders subFolders = folder.SubFolders;

            AddPublicFolders(subFolders, path, hierarchyDelimiter);

            Marshal.ReleaseComObject(subFolders);
            Marshal.ReleaseComObject(folder);
         }
      }

      public void OnShowPage(Dictionary<string, string> _state)
      {
         foreach (ListViewItem item in listViewDomains.Items)
         {
            if (Globals.SelectedDomains.Contains(item.Text))
               item.Checked = true;
            else
               item.Checked = false;
         }

         checkPublicFolders.Checked = Globals.SynchronizePublicFolders;
         comboPublicFolderImportPath.Text = Globals.PublicFolderImportPath;

         // The public IMAP folder is only used when messages are imported. When messages are
         // deleted, the folder they belong to doesn't matter.
         bool importingMessages = Globals.Mode == Globals.ModeType.Import;

         labelPublicFolderImportPath.Visible = importingMessages;
         comboPublicFolderImportPath.Visible = importingMessages;

         EnableDisable();
      }

      public bool OnLeavePage(bool next)
      {
         if (next && checkPublicFolders.Checked && Globals.Mode == Globals.ModeType.Import &&
             comboPublicFolderImportPath.Text.Trim().Length == 0)
         {
            // Messages in public folders can't be imported unless we know which public IMAP
            // folder to place them in. The folder isn't part of the path on disk.
            MessageBox.Show("Please specify the public IMAP folder in which messages found in the public folder should be placed.",
                            "hMailServer", MessageBoxButtons.OK, MessageBoxIcon.Information);

            comboPublicFolderImportPath.Focus();
            return false;
         }

         Globals.SelectedDomains.Clear();

         foreach (ListViewItem item in listViewDomains.Items)
         {
            if (item.Checked)
               Globals.SelectedDomains.Add(item.Text);
         }

         Globals.SynchronizePublicFolders = checkPublicFolders.Checked;
         Globals.PublicFolderImportPath = comboPublicFolderImportPath.Text.Trim();

         return true;
      }

      public string Title
      {
         get { return "Select what to synchronize"; }
      }

      private void EnableDisable()
      {
         labelPublicFolderImportPath.Enabled = checkPublicFolders.Checked;
         comboPublicFolderImportPath.Enabled = checkPublicFolders.Checked;
      }

      private void buttonSelectAll_Click(object sender, EventArgs e)
      {
         foreach (ListViewItem item in listViewDomains.Items)
            item.Checked = true;

         checkPublicFolders.Checked = true;
      }

      private void buttonSelectNone_Click(object sender, EventArgs e)
      {
         foreach (ListViewItem item in listViewDomains.Items)
            item.Checked = false;

         checkPublicFolders.Checked = false;
      }

      private void checkPublicFolders_CheckedChanged(object sender, EventArgs e)
      {
         EnableDisable();
      }
   }
}
