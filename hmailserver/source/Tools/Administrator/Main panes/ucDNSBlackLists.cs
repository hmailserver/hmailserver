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
    public partial class ucDNSBlackLists : ucItemsView
    {
        public ucDNSBlackLists()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        protected override void LoadList()
        {
           listObjects.Items.Clear();

           hMailServer.DNSBlackLists dnsBlackLists = GetDNSBlackLists();
           for (int i = 0; i < dnsBlackLists.Count; i++)
           {
              hMailServer.DNSBlackList dnsBlackList = dnsBlackLists[i];

              ListViewItem item = listObjects.Items.Add(dnsBlackList.DNSHost);
              item.SubItems.Add(EnumStrings.GetYesNoString(dnsBlackList.Active));

              item.Tag = dnsBlackList.ID;

              Marshal.ReleaseComObject(dnsBlackList);
           }

           Marshal.ReleaseComObject(dnsBlackLists);
        }

        private hMailServer.DNSBlackLists GetDNSBlackLists()
        {
            hMailServer.Application app = APICreator.Application;
            hMailServer.Settings settings = app.Settings;
            hMailServer.AntiSpam antiSpam = settings.AntiSpam;
            hMailServer.DNSBlackLists dnsBlackLists = antiSpam.DNSBlackLists;

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(antiSpam);

            return dnsBlackLists;
        }


        protected override ListView GetListView()
        {
           return listObjects;
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.DNSBlackLists lists = GetDNSBlackLists();

            foreach (var item in items)
            {
                lists.DeleteByDBID(Convert.ToInt32(item.Tag));
            }
            
            Marshal.ReleaseComObject(lists);
        }

        protected override void AddItem()
        {
           IMainForm mainForm = Instances.MainForm;

           // Show the new account.
           NodeDNSBlackList newNode = new NodeDNSBlackList("", 0);
           mainForm.ShowItem(newNode);
        }

       private void listObjects_DoubleClick(object sender, EventArgs e)
       {

       }
    }
}
