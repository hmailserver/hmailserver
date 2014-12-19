// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Utilities.Localization;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucTCPIPPorts : ucItemsView
    {
        public ucTCPIPPorts()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }


        protected override void LoadList()
        {
            listObjects.Items.Clear();

            hMailServer.TCPIPPorts tcpIPPorts = APICreator.TCPIPPortsSettings;
            for (int i = 0; i < tcpIPPorts.Count; i++)
            {
                hMailServer.TCPIPPort tcpIPPort = tcpIPPorts[i];
                ListViewItem item = listObjects.Items.Add(InternalNames.GetPortName(tcpIPPort));
                item.Tag = tcpIPPort.ID;

                Marshal.ReleaseComObject(tcpIPPort);
            }

            Marshal.ReleaseComObject(tcpIPPorts);
        }

        protected override ListView GetListView()
        {
            return listObjects;
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.TCPIPPorts tcpIPPorts = APICreator.TCPIPPortsSettings;
            
            foreach (ListViewItem item in items)
            {
                int id = Convert.ToInt32(item.Tag);
                tcpIPPorts.DeleteByDBID(id);
            }
           
            Marshal.ReleaseComObject(tcpIPPorts);
        }

        protected override void AddItem()
        {
            IMainForm mainForm = Instances.MainForm;

            // Show the new account.
            NodeTCPIPPort newNode = new NodeTCPIPPort(0, "");
            mainForm.ShowItem(newNode);
        }

        private void buttonDefault_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show(Strings.Localize("This operation will change the configuration of the TCP/IP ports to their default values. Are you sure you want to do this?"), EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                hMailServer.Application app = APICreator.Application;

                hMailServer.TCPIPPorts tcpIPPorts = APICreator.TCPIPPortsSettings;
                tcpIPPorts.SetDefault();
                Marshal.ReleaseComObject(tcpIPPorts);

                LoadList();

                Instances.MainForm.RefreshCurrentNode(null);
            }
        }
    }
}
