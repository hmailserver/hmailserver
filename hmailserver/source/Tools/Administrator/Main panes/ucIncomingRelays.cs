// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Nodes;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucIncomingRelays : ucItemsView
    {
        public ucIncomingRelays()
        {
            InitializeComponent();
            
            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        protected override void LoadList()
        {
            listObjects.Items.Clear();

            hMailServer.Application app = APICreator.Application;
            hMailServer.Settings settings = APICreator.Settings;

            hMailServer.IncomingRelays IncomingRelays = settings.IncomingRelays;

            for (int i = 0; i < IncomingRelays.Count; i++)
            {
                hMailServer.IncomingRelay IncomingRelay = IncomingRelays[i];

                ListViewItem item = listObjects.Items.Add(IncomingRelay.Name);
                item.SubItems.Add(IncomingRelay.LowerIP);
                item.SubItems.Add(IncomingRelay.UpperIP);
                item.Tag = IncomingRelay.ID;


                Marshal.ReleaseComObject(IncomingRelay);
            }

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(IncomingRelays);
        }

        protected override ListView GetListView()
        {
            return listObjects;
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.Settings settings = APICreator.Settings;
            hMailServer.IncomingRelays IncomingRelays = settings.IncomingRelays;

            foreach (var item in items)
            {
                int id = Convert.ToInt32(item.Tag);
                IncomingRelays.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(IncomingRelays);
        }

        protected override void AddItem()
        {
            IMainForm mainForm = Instances.MainForm;

            // Show the new account.
            NodeIncomingRelay newNode = new NodeIncomingRelay(0, "");
            mainForm.ShowItem(newNode);
        }

    }
}
