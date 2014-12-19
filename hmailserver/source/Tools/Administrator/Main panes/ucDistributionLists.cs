// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;

namespace hMailServer.Administrator
{
    public partial class ucDistributionLists : ucItemsView
    {
        private int _domainID;

        public ucDistributionLists(int domainID)
        {
            InitializeComponent();

            _domainID = domainID;
        }

        protected override void LoadList()
        {
            listDistributionLists.Items.Clear();

            hMailServer.Links links = APICreator.Links;
            hMailServer.Domain domain = links.get_Domain(_domainID);
            hMailServer.DistributionLists lists = domain.DistributionLists;

            for (int i = 0; i < lists.Count; i++)
            {
                hMailServer.DistributionList list = lists[i];

                ListViewItem item = listDistributionLists.Items.Add(list.Address);

                item.SubItems.Add(EnumStrings.GetYesNoString(list.Active));

                item.Tag = list.ID;

                Marshal.ReleaseComObject(list);
            }

            Marshal.ReleaseComObject(lists);
            Marshal.ReleaseComObject(domain);
            Marshal.ReleaseComObject(links);
        }


        protected override ListView GetListView()
        {
            return listDistributionLists;
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.Domain domain = APICreator.GetDomain(_domainID);
            hMailServer.DistributionLists lists = domain.DistributionLists;

            foreach (var item in items)
            {
                int listID = Convert.ToInt32(item.Tag);
                lists.DeleteByDBID(listID);
            }

            Marshal.ReleaseComObject(lists);
            Marshal.ReleaseComObject(domain);
            
        }

        protected override void AddItem()
        {
            IMainForm mainForm = Instances.MainForm;

            // Show the new account.
            NodeDistributionList newNode = new NodeDistributionList(_domainID, "", 0);
            mainForm.ShowItem(newNode);
        }


    }
}
