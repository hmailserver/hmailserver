// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Nodes;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucWelcome : UserControl, ISettingsControl
    {
        public ucWelcome()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }


        public void OnLeavePage()
        {

        }

        public bool Dirty
        {
            get { return false;  }
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

       private void buttonAddDomain_Click(object sender, EventArgs e)
       {
          IMainForm mainForm = Instances.MainForm;

          // Jump to the domains node.
          SearchNodeType crit = new SearchNodeType(typeof(NodeDomains));
          mainForm.SelectNode(crit);
         
          // Show the new domain.
          NodeDomain domain = new NodeDomain(mainForm, 0, null);
          mainForm.ShowItem(domain);
           
       }

    }
}
