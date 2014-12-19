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
    public partial class ucAccounts : ucItemsView
    {
        private int _domainID;

        public ucAccounts(int domainID)
        {
            InitializeComponent();

            _domainID = domainID;

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        protected override void LoadList()
        {
            listAccounts.Items.Clear();

            hMailServer.Domain domain = APICreator.GetDomain(_domainID);

            hMailServer.Accounts accounts = domain.Accounts;

            for (int i = 0; i < accounts.Count; i++)
            {
                hMailServer.Account account = accounts[i];
                ListViewItem item = listAccounts.Items.Add(account.Address);
                item.SubItems.Add(EnumStrings.GetYesNoString(account.Active));
                item.Tag = account.ID;

                Marshal.ReleaseComObject(account);
            }

            Marshal.ReleaseComObject(domain);
            Marshal.ReleaseComObject(accounts);
        }

        protected override ListView GetListView()
        {
            return listAccounts;
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.Domain domain = APICreator.GetDomain(_domainID);
            hMailServer.Accounts accounts = domain.Accounts;

            foreach (ListViewItem item in items)
            {
                int accountID = Convert.ToInt32(item.Tag);
                accounts.DeleteByDBID(accountID);
            }

            Marshal.ReleaseComObject(domain);
            Marshal.ReleaseComObject(accounts);
            
        }

        protected override void AddItem()
        {
            IMainForm mainForm = Instances.MainForm;

            // Show the new account.
            NodeAccount account = new NodeAccount(_domainID, "", 0);
            mainForm.ShowItem(account);
        }



    }
}
