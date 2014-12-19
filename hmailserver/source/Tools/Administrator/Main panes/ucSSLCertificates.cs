// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucSSLCertificates : ucItemsView
    {
        public ucSSLCertificates()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }


        protected override void LoadList()
        {
            listObjects.Items.Clear();

            hMailServer.Application app = APICreator.Application;
            hMailServer.Settings settings = app.Settings;
            hMailServer.SSLCertificates sslCertificates = settings.SSLCertificates;

            for (int i = 0; i < sslCertificates.Count; i++)
            {
                hMailServer.SSLCertificate sslCertificate = sslCertificates[i];
                ListViewItem item = listObjects.Items.Add(sslCertificate.Name);
                item.Tag = sslCertificate.ID;
                Marshal.ReleaseComObject(sslCertificate);
            }

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(sslCertificates);
        }

        protected override ListView GetListView()
        {
            return listObjects;
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.Settings settings = APICreator.Settings;
            hMailServer.SSLCertificates sslCertificates = settings.SSLCertificates;

            foreach (ListViewItem item in items)
            {
                int id = Convert.ToInt32(item.Tag);
                sslCertificates.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(sslCertificates);
        }

        protected override void AddItem()
        {
            IMainForm mainForm = Instances.MainForm;

            // Show the new account.
            NodeSSLCertificate newNode = new NodeSSLCertificate(0, "");
            mainForm.ShowItem(newNode);
        }
    }
}
