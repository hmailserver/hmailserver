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
    public partial class ucAliases : ucItemsView
    {
        private int _domainID;

        public ucAliases(int domainID)
        {
            InitializeComponent();

            _domainID = domainID;

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        protected override void LoadList()
        {
            listAliases.Items.Clear();

            hMailServer.Links links = APICreator.Links;
            hMailServer.Domain domain = links.get_Domain(_domainID);
            Marshal.ReleaseComObject(links);
            
            hMailServer.Aliases aliases = domain.Aliases;
            Marshal.ReleaseComObject(domain);

            for (int i = 0; i < aliases.Count; i++)
            {
                hMailServer.Alias alias = aliases[i];

                ListViewItem item = listAliases.Items.Add(alias.Name);
                item.SubItems.Add(alias.Value);
                item.SubItems.Add(EnumStrings.GetYesNoString(alias.Active));

                item.Tag = alias.ID;

                Marshal.ReleaseComObject(alias);
            }

            
            Marshal.ReleaseComObject(aliases);
        }


        protected override ListView GetListView()
        {
            return listAliases;
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.Links links = APICreator.Links;
            hMailServer.Domain domain = links.get_Domain(_domainID);
            Marshal.ReleaseComObject(links);

            hMailServer.Aliases aliases = domain.Aliases;
            foreach (ListViewItem item in items)
            {
                int aliasID = Convert.ToInt32(item.Tag);
                aliases.DeleteByDBID(aliasID);
            }

            Marshal.ReleaseComObject(domain);
            Marshal.ReleaseComObject(aliases);
        }

        protected override void AddItem()
        {
            IMainForm mainForm = Instances.MainForm;

            // Show the new account.
            NodeAlias aliasNode = new NodeAlias(_domainID, "", 0);
            mainForm.ShowItem(aliasNode);
        }




    }
}
