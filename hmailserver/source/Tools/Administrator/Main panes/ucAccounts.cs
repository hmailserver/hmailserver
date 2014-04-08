// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Utilities;

namespace hMailServer.Administrator
{
   public partial class ucAccounts : ucItemsView
   {
      private hMailServer.Domain representedDomain;

      public ucAccounts(hMailServer.Domain domain)
      {
         InitializeComponent();

         representedDomain = domain;

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
      }

      protected override void LoadList()
      {
         listAccounts.Items.Clear();

         hMailServer.Accounts accounts = representedDomain.Accounts;

         for (int i = 0; i < accounts.Count; i++)
         {
            hMailServer.Account account = accounts[i];

            ListViewItem item = listAccounts.Items.Add(account.Address);

            item.SubItems.Add(EnumStrings.GetYesNoString(account.Active));

            item.Tag = account;
         }
      }

      protected override ListView GetListView()
      {
         return listAccounts;
      }

      protected override void DeleteItem(ListViewItem item)
      {
         hMailServer.Account account = item.Tag as hMailServer.Account;
         account.Delete();
      }
      
      protected override void AddItem()
      {
         IMainForm mainForm = Instances.MainForm;

         // Show the new account.
         NodeAccount account = new NodeAccount(mainForm, representedDomain, null);
         mainForm.ShowItem(account);
      }



   }
}
