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
    public partial class ucRoutes : ucItemsView
    {
        public ucRoutes()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        protected override void LoadList()
        {
            listObjects.Items.Clear();

            hMailServer.Application app = APICreator.Application;
            hMailServer.Settings settings = app.Settings;
            hMailServer.Routes routes = settings.Routes;
            for (int i = 0; i < routes.Count; i++)
            {
                hMailServer.Route route = routes[i];

                ListViewItem item = listObjects.Items.Add(route.DomainName);
                item.Tag = route.ID;

                Marshal.ReleaseComObject(route);
            }

            Marshal.ReleaseComObject(routes);
            Marshal.ReleaseComObject(settings);
        }

        protected override ListView GetListView()
        {
            return listObjects;
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.Routes routes = APICreator.Routes;

            foreach (ListViewItem item in items)
            {
                int id = Convert.ToInt32(item.Tag);
                routes.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(routes);
        }

        protected override void AddItem()
        {
            IMainForm mainForm = Instances.MainForm;

            // Show the new account.
            NodeRoute newNode = new NodeRoute(0, "");
            mainForm.ShowItem(newNode);
        }

    }
}
