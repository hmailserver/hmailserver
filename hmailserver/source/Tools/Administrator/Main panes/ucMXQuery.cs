// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;


namespace hMailServer.Administrator
{
    public partial class ucMXQuery : UserControl, ISettingsControl
    {
        public ucMXQuery()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        public void OnLeavePage()
        {

        }


        public bool Dirty
        {
            get { return false; }
        }

        public void LoadData()
        {
            // nothing to load
        }

        public bool SaveData()
        {
            // nothing to save
           return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }

       private void buttonResolve_Click(object sender, EventArgs e)
       {
          listMailServers.Items.Clear();

          if (textEmailAddress.Text.Length == 0)
             return;

          using (WaitCursor cursor = new WaitCursor())
          {
             string result = APICreator.Application.Utilities.GetMailServer(textEmailAddress.Text);

             string[] servers = result.Split(",".ToCharArray());

             foreach (string server in servers)
             {
                listMailServers.Items.Add(server);
             }
          }

       }

       private void copyToClipboard_Click(object sender, EventArgs e)
       {
          StringBuilder sbAllServers = new StringBuilder();
          foreach (ListViewItem item in listMailServers.SelectedItems)
          {
             if (sbAllServers.Length > 0)
                sbAllServers.AppendLine();
             
             sbAllServers.Append(item.Text);

          }

          if (sbAllServers.Length > 0)
            Clipboard.SetText(sbAllServers.ToString());
       }
    }
}
