// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucServerMessages : UserControl, ISettingsControl
    {
        public ucServerMessages()
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
           listObjects.Items.Clear();

           hMailServer.Application app = APICreator.Application;
           hMailServer.Settings settings = app.Settings;
           hMailServer.ServerMessages serverMessages = settings.ServerMessages;

           for (int i = 0; i < serverMessages.Count; i++)
           {
              hMailServer.ServerMessage serverMessage = serverMessages[i];

              ListViewItem item = listObjects.Items.Add(serverMessage.Name);
              item.Tag = serverMessage.ID;
           }

           Marshal.ReleaseComObject(settings);
           Marshal.ReleaseComObject(serverMessages);
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

       private void listObjects_DoubleClick(object sender, EventArgs e)
       {
          if (listObjects.SelectedItems.Count == 0)
             return;

          string name = listObjects.SelectedItems[0].Text;
          IMainForm mainForm = Instances.MainForm;
          SearchNodeText crit = new SearchNodeText(name);
          mainForm.SelectNode(crit);
       }
    }
}
