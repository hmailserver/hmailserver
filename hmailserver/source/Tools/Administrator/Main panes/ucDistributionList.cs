// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Dialogs;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucDistributionList : UserControl, ISettingsControl
    {
        private hMailServer.DistributionList representedObject;
        private int _domainID;

        public ucDistributionList(int domainID, int listID)
        {
            InitializeComponent();

            _domainID = domainID;
            
            hMailServer.Links links = APICreator.Links;

            hMailServer.Domain domain = links.get_Domain(_domainID);

            if (listID > 0)
            {
                representedObject = links.get_DistributionList(listID);
                Marshal.ReleaseComObject(links);
            }

            textAddress.Text = "@" + domain.Name;

            Marshal.ReleaseComObject(domain);

            EnableDisable();

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        public void OnLeavePage()
        {
            if (representedObject != null)
                Marshal.ReleaseComObject(representedObject);
        }

        public bool Dirty
        {
            get
            {
                return DirtyChecker.IsDirty(this) &&
                       textAddress.Text.Length > 0;
            }
        }

        private void EnableDisableTabs()
        {
            tabControl.TabPages[1].Enabled = representedObject != null;
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
            EnableDisableTabs();

            if (representedObject == null)
                return;

            textAddress.Text = representedObject.Address;
            checkEnabled.Checked = representedObject.Active;

            radioModePublic.Checked = representedObject.Mode == eDistributionListMode.eLMPublic;
            radioModeMembership.Checked = representedObject.Mode == eDistributionListMode.eLMMembership;
            optModeAnnouncements.Checked = representedObject.Mode == eDistributionListMode.eLMAnnouncement;

            textRequireAddress.Text = representedObject.RequireSenderAddress;
            checkRequireSMTPAuthentication.Checked = representedObject.RequireSMTPAuth;

            ListRecipients();
        }


        public bool SaveData()
        {
            bool newObject = false;
            if (representedObject == null)
            {
                hMailServer.Domain domain = APICreator.GetDomain(_domainID);

                hMailServer.DistributionLists lists = domain.DistributionLists;
                representedObject = lists.Add();
                newObject = true;

                Marshal.ReleaseComObject(lists);
                Marshal.ReleaseComObject(domain);
            }

            representedObject.Address = textAddress.Text;
            representedObject.Active = checkEnabled.Checked;

            if (radioModePublic.Checked)
                representedObject.Mode = eDistributionListMode.eLMPublic;

            if (radioModeMembership.Checked)
                representedObject.Mode = eDistributionListMode.eLMMembership;

            if (optModeAnnouncements.Checked)
                representedObject.Mode = eDistributionListMode.eLMAnnouncement;

            representedObject.RequireSenderAddress = textRequireAddress.Text;
            representedObject.RequireSMTPAuth = checkRequireSMTPAuthentication.Checked;

            representedObject.Save();

            // Refresh the node in the tree if the name has changed.
            IMainForm mainForm = Instances.MainForm;
            mainForm.RefreshCurrentNode(textAddress.Text);

            // Set the object to clean.
            DirtyChecker.SetClean(this);

            if (newObject)
            {
                SearchNodeText crit = new SearchNodeText(representedObject.Address);
                mainForm.SelectNode(crit);
            }

            EnableDisableTabs();

            return true;
        }

        public void LoadResources()
        {

        }

        private void buttonAddRecipient_Click(object sender, EventArgs e)
        {
            formInputDialog inputDialog = new formInputDialog();

            inputDialog.Title = "Address";
            inputDialog.Text = "Enter email address";

            if (inputDialog.ShowDialog() == DialogResult.OK)
            {
                hMailServer.DistributionListRecipients recipients = representedObject.Recipients;
                hMailServer.DistributionListRecipient recipient = recipients.Add();

                recipient.RecipientAddress = inputDialog.Value;
                recipient.Save();

                Marshal.ReleaseComObject(recipients);
                Marshal.ReleaseComObject(recipient);
            }

            ListRecipients();
        }



        private void buttonSelectRecipients_Click(object sender, EventArgs e)
        {
            formSelectUsers selectUsers = new formSelectUsers(true, 0);

            if (selectUsers.ShowDialog() == DialogResult.OK)
            {
                hMailServer.DistributionListRecipients recipients = representedObject.Recipients;

                List<string> listUsers = selectUsers.GetSelectedTexts();

                foreach (string address in listUsers)
                {
                    hMailServer.DistributionListRecipient recipient = recipients.Add();
                    recipient.RecipientAddress = address;
                    recipient.Save();

                    Marshal.ReleaseComObject(recipient);
                }

                Marshal.ReleaseComObject(recipients);

                ListRecipients();
            }
        }

        private void ListRecipients()
        {
            listRecipients.Items.Clear();

            hMailServer.DistributionListRecipients recipients = representedObject.Recipients;

            for (int i = 0; i < recipients.Count; i++)
            {
                hMailServer.DistributionListRecipient recipient = recipients[i];

                ListViewItem item = listRecipients.Items.Add(recipient.RecipientAddress);
                item.Tag = recipient.ID;

                Marshal.ReleaseComObject(recipient);
            }

            Marshal.ReleaseComObject(recipients);
        }

        private void buttonDeleteRecipient_Click(object sender, EventArgs e)
        {
            hMailServer.DistributionListRecipients recipients = representedObject.Recipients;

            foreach (ListViewItem item in listRecipients.SelectedItems)
            {
                int id = Convert.ToInt32(item.Tag);

                recipients.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(recipients);

            ListRecipients();
        }

        private void buttonEdit_Click(object sender, EventArgs e)
        {
            EditSelectedItem();
        }

        private void EditSelectedItem()
        {
            if (listRecipients.SelectedItems.Count != 1)
                return;

            formInputDialog inputDialog = new formInputDialog();

            int id = Convert.ToInt32(listRecipients.SelectedItems[0].Tag);
            hMailServer.DistributionListRecipients recipients = representedObject.Recipients;
            hMailServer.DistributionListRecipient recipient = recipients.get_ItemByDBID(id);

            inputDialog.Title = "Address";
            inputDialog.Text = "Enter email address";
            inputDialog.Value = recipient.RecipientAddress;

            if (inputDialog.ShowDialog() == DialogResult.OK)
            {
                recipient.RecipientAddress = inputDialog.Value;
                recipient.Save();
            }

            Marshal.ReleaseComObject(recipients);
            Marshal.ReleaseComObject(recipient);

            ListRecipients();
        }

        private void EnableDisable()
        {
            buttonEdit.Enabled = listRecipients.SelectedItems.Count == 1;
            buttonDeleteRecipient.Enabled = listRecipients.SelectedItems.Count > 0;

            textRequireAddress.Enabled = optModeAnnouncements.Checked;
        }

        private void listRecipients_SelectedIndexChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void optModeAnnouncements_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void listRecipients_DoubleClick(object sender, EventArgs e)
        {
            EditSelectedItem();
        }

    }
}
