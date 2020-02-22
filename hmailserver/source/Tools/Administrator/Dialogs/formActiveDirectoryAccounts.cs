// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.DirectoryServices;
using System.DirectoryServices.ActiveDirectory;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator.Dialogs
{
   public partial class formActiveDirectoryAccounts : Form
   {
      private string _domainName = null;
      private List<string> _accountNames = null;

      public formActiveDirectoryAccounts()
      {
         InitializeComponent();

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
         Strings.Localize(this);
      }

      public string DomainName
      {
         get
         {
            return _domainName;
         }
      }

      public List<string> AccountNames
      {
         get
         {
            return _accountNames;
         }

      }

      private void ListDomains()
      {
         try
         {
            Forest currentForest = Forest.GetCurrentForest();
            DomainCollection domains = currentForest.Domains;

            foreach (System.DirectoryServices.ActiveDirectory.Domain domain in domains)
            {
               comboDomain.Items.Add(domain.Name);
            }
         }
         catch (Exception e)
         {
            MessageBox.Show("Retrieving of domains failed:" + Environment.NewLine + e.Message, EnumStrings.hMailServerAdministrator);
         }
      }

      private void formActiveDirectoryAccounts_Shown(object sender, EventArgs e)
      {
         this.Cursor = Cursors.WaitCursor;
         ListDomains();
         this.Cursor = Cursors.Default;
      }

      private void comboDomain_SelectedValueChanged(object sender, EventArgs e)
      {
         ListAccountsInDomain();
      }

      private DirectoryEntry GetDirectoryEntry()
      {
         System.DirectoryServices.ActiveDirectory.Domain domain = GetSelectedDomain();
         DirectoryEntry directoryEntry = new DirectoryEntry("LDAP://" + domain.Name);
         
         return directoryEntry;
      }

      private SearchResultCollection RetrieveAllUsers()
      {
         DirectoryEntry directoryEntry = GetDirectoryEntry();
         DirectorySearcher searcher = new DirectorySearcher();

         searcher.SearchRoot = directoryEntry;
         searcher.Filter = "(&(objectClass=user))";
         searcher.SearchScope = SearchScope.Subtree;
         searcher.ServerTimeLimit = new TimeSpan(0, 10, 0, 0);
         searcher.SizeLimit = 100000;
         searcher.PageSize = 500;
         searcher.PropertiesToLoad.Add("samaccountname");

         SearchResultCollection results = searcher.FindAll();

         return results;

      }

      private void ListAccountsInDomain()
      {
         using (new WaitCursor())
         {
            SearchResultCollection users = RetrieveAllUsers();

            List<ListViewItem> listViewItems = new List<ListViewItem>();

            foreach (SearchResult result in users)
            {
               string name = "";

               ResultPropertyValueCollection nameProperties = null;

               if (result.Properties.Contains("samaccountname"))
                  nameProperties = result.Properties["samaccountname"];

               if (nameProperties != null && nameProperties.Count > 0)
                  name = nameProperties[0].ToString();

               ListViewItem item = new ListViewItem(name);
               item.Tag = name;

               listViewItems.Add(item);
            }

            listAccounts.Items.AddRange(listViewItems.ToArray());
         }
      }

      private System.DirectoryServices.ActiveDirectory.Domain GetSelectedDomain()
      {
         Forest currentForest = Forest.GetCurrentForest();
         DomainCollection domains = currentForest.Domains;

         foreach (System.DirectoryServices.ActiveDirectory.Domain domain in domains)
         {

            if (domain.Name == comboDomain.Text)
               return domain;
         }
         return null;
      }

      private void buttonOK_Click(object sender, EventArgs e)
      {
         _domainName = comboDomain.Text;
         _accountNames = new List<string>();

         foreach (ListViewItem item in listAccounts.SelectedItems)
         {
            _accountNames.Add(item.Tag as string);
         }
      }
   }
}