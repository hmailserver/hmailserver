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
      }

      public bool OnLeavePage(bool next)
      {
         Globals.SelectedDomains.Clear();

         foreach (ListViewItem item in listViewDomains.Items)
         {
            if (item.Checked)
               Globals.SelectedDomains.Add(item.Text);
         }

         return true;
      }

      public string Title
      {
         get { return "Select domain"; }
      }

      private void buttonSelectAll_Click(object sender, EventArgs e)
      {
         foreach (ListViewItem item in listViewDomains.Items)
            item.Checked = true;
      }

      private void buttonSelectNone_Click(object sender, EventArgs e)
      {
         foreach (ListViewItem item in listViewDomains.Items)
            item.Checked = false;
      }
   }
}
