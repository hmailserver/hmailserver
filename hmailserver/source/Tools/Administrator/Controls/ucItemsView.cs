// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Administrator.Nodes;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucItemsView : UserControl, ISettingsControl
    {
        public ucItemsView()
        {
            InitializeComponent();

            EnableDisable();
        }

        public bool Dirty
        {
            get { return false; }
        }

        public void LoadData()
        {

            ListView listView = GetListView();
            if (listView != null)
            {
                listView.SelectedIndexChanged += new System.EventHandler(this.OnSelectedIndexChanged);
                listView.DoubleClick += new System.EventHandler(this.OnDoubleClick);
            }

            LoadList();
        }

        public bool SaveData()
        {
            // nothing to save
           return true;
        }

        public void LoadResources()
        {

        }

        public virtual void OnLeavePage()
        {

        }

        protected virtual ListView GetListView()
        {


            // to be derived
            return null;
        }

        protected virtual void LoadList()
        {
            MessageBox.Show("Not implemented");
            // to be derived
        }

        protected virtual void DeleteItems(List<ListViewItem> items)
        {
            MessageBox.Show("Not implemented");
            // to be derived
        }

        protected virtual void AddItem()
        {
            MessageBox.Show("Not implemented");
            // to be derived
        }

        private void EditSelectedItem()
        {
            ListView listView = GetListView();
            if (listView == null)
                return;

            if (listView.SelectedItems.Count == 0)
                return;

            string text = listView.SelectedItems[0].Text;

            // Jump to the sub node.
            IMainForm mainForm = Instances.MainForm;
            SearchNodeText crit = new SearchNodeText(text);
            mainForm.SelectNode(crit);
        }

        private void EnableDisable()
        {
            ListView list = GetListView();

            if (list == null)
                return;

            buttonEdit.Enabled = list.SelectedItems.Count == 1;
            buttonRemove.Enabled = list.SelectedItems.Count > 0;
        }

        private void OnSelectedIndexChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void OnDoubleClick(object sender, EventArgs e)
        {
            EditSelectedItem();
        }

        private void buttonEdit_Click(object sender, EventArgs e)
        {
            EditSelectedItem();
        }

        private void buttonRemove_Click(object sender, EventArgs e)
        {
            if (!Utility.AskDeleteItems())
               return;

            using (new WaitCursor())
            {
                ListView list = GetListView();

                List<ListViewItem> itemsToRemove = new List<ListViewItem>();

                // Remove the BO's.
                foreach (ListViewItem item in list.SelectedItems)
                {
                    itemsToRemove.Add(item);
                }

                DeleteItems(itemsToRemove);

                // Remove the items from the list view
                foreach (ListViewItem item in itemsToRemove)
                {
                    list.Items.Remove(item);
                }

                // Refresh the node in the tree
                IMainForm mainForm = Instances.MainForm;
                mainForm.RefreshCurrentNode(null);
            }

        }

        private void buttonAdd_Click(object sender, EventArgs e)
        {
            AddItem();
        }

    }
}
