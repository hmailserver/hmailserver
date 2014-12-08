// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucSURBLServers : ucItemsView
    {
        public ucSURBLServers()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);


        }

        protected override void LoadList()
        {
            listObjects.Items.Clear();

            hMailServer.SURBLServers surblServers = GetSurblServers();

            for (int i = 0; i < surblServers.Count; i++)
            {
                hMailServer.SURBLServer surblServer = surblServers[i];

                ListViewItem item = listObjects.Items.Add(surblServer.DNSHost);
                item.SubItems.Add(EnumStrings.GetYesNoString(surblServer.Active));
                item.Tag = surblServer.ID;

                Marshal.ReleaseComObject(surblServer);
            }

            Marshal.ReleaseComObject(surblServers);
        }

        private hMailServer.SURBLServers GetSurblServers()
        {
            hMailServer.AntiSpam antiSpam = APICreator.AntiSpamSettings;
            hMailServer.SURBLServers surblServers = antiSpam.SURBLServers;

            Marshal.ReleaseComObject(antiSpam);

            return surblServers;
        }

        protected override ListView GetListView()
        {
            return listObjects;
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.SURBLServers surblServers = GetSurblServers();

            foreach (var item in items)
            {
                int id = Convert.ToInt32(item.Tag);
                surblServers.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(surblServers);
        }

        protected override void AddItem()
        {
            IMainForm mainForm = Instances.MainForm;

            // Show the new account.
            NodeSURBLServer newNode = new NodeSURBLServer(0, "");
            mainForm.ShowItem(newNode);
        }
    }
}
