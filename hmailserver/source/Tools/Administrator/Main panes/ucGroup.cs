// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Dialogs;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucGroup : UserControl, ISettingsControl
    {
        private hMailServer.Group representedObject;

        public ucGroup(int groupID)
        {
            InitializeComponent();

            if (groupID > 0)
            {
                hMailServer.Groups groups = APICreator.Groups;
                representedObject = groups.get_ItemByDBID(groupID);
                Marshal.ReleaseComObject(groups);
            }

            DirtyChecker.SubscribeToChange(this, OnContentChanged);
            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
            EnableDisable();
        }

        public void OnLeavePage()
        {

        }

        public bool Dirty
        {
            get
            {
                return DirtyChecker.IsDirty(this) &&
                       textName.Text.Length > 0;
            }
        }

        private void OnContentChanged()
        {
            Instances.MainForm.OnContentChanged();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
            OnContentChanged();
        }

        public void LoadData()
        {
            if (representedObject == null)
                return;

            textName.Text = representedObject.Name;

            ListGroupMembers();
        }

        private void ListGroupMembers()
        {
            listMembers.Items.Clear();

            hMailServer.GroupMembers members = representedObject.Members;

            for (int i = 0; i < members.Count; i++)
            {
                hMailServer.GroupMember member = members[i];

                try
                {
                    hMailServer.Account account = member.Account;
                    ListViewItem item = listMembers.Items.Add(account.Address);
                    item.Tag = member.ID;

                    Marshal.ReleaseComObject(account);

                }
                catch (Exception)
                {
                    ListViewItem item = listMembers.Items.Add("Unknown");
                    item.Tag = member.ID;
                }

                Marshal.ReleaseComObject(member);

            }

            Marshal.ReleaseComObject(members);
        }

        public bool SaveData()
        {
            bool newObject = false;
            if (representedObject == null)
            {
                hMailServer.Settings settings = APICreator.Application.Settings;
                hMailServer.Groups groups = settings.Groups;
                representedObject = groups.Add();

                Marshal.ReleaseComObject(settings);
                Marshal.ReleaseComObject(groups);

                newObject = true;
            }

            representedObject.Name = textName.Text;

            representedObject.Save();

            // Refresh the node in the tree if the name has changed.
            IMainForm mainForm = Instances.MainForm;
            mainForm.RefreshCurrentNode(textName.Text);

            // Set the object to clean.
            DirtyChecker.SetClean(this);

            if (newObject)
            {
                SearchNodeText crit = new SearchNodeText(representedObject.Name);
                mainForm.SelectNode(crit);
            }

            EnableDisable();

            return true;
        }

        public void LoadResources()
        {

        }

        private void buttonSelect_Click(object sender, EventArgs e)
        {
            formSelectUsers selectUsers = new formSelectUsers(true, 0);

            if (selectUsers.ShowDialog() == DialogResult.OK)
            {
                hMailServer.GroupMembers members = representedObject.Members;

                List<int> listUsers = selectUsers.GetSelectedIDs();

                foreach (int userID in listUsers)
                {
                    hMailServer.GroupMember member = members.Add();
                    member.AccountID = userID;
                    member.Save();

                    Marshal.ReleaseComObject(member);
                }

                Marshal.ReleaseComObject(members);

                ListGroupMembers();
            }



        }

        private void buttonDelete_Click(object sender, EventArgs e)
        {
            hMailServer.GroupMembers members = representedObject.Members;

            foreach (ListViewItem item in listMembers.SelectedItems)
            {
                int id = Convert.ToInt32(item.Tag);
                members.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(members);

            ListGroupMembers();
        }

        private void EnableDisable()
        {
            buttonDelete.Enabled = listMembers.SelectedItems.Count > 0;
            buttonSelect.Enabled = representedObject != null;
        }

        private void listMembers_SelectedIndexChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void textName_Load(object sender, EventArgs e)
        {

        }

    }
}
