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
        private hMailServer.DistributionList _representedObject;
        private int _domainID;

        public ucDistributionList(int domainID, int listID)
        {
            InitializeComponent();

            _domainID = domainID;
            
            hMailServer.Links links = APICreator.Links;

            hMailServer.Domain domain = links.get_Domain(_domainID);

            if (listID > 0)
            {
                _representedObject = links.get_DistributionList(listID);
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
            if (_representedObject != null)
                Marshal.ReleaseComObject(_representedObject);
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
            tabControl.TabPages[1].Enabled = _representedObject != null;
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

            if (_representedObject == null)
                return;

            textAddress.Text = _representedObject.Address;
            checkEnabled.Checked = _representedObject.Active;

            radioModePublic.Checked = _representedObject.Mode == eDistributionListMode.eLMPublic;
            radioModeMembership.Checked = _representedObject.Mode == eDistributionListMode.eLMMembership;
            optModeAnnouncements.Checked = _representedObject.Mode == eDistributionListMode.eLMAnnouncement;

            textRequireAddress.Text = _representedObject.RequireSenderAddress;
            checkRequireSMTPAuthentication.Checked = _representedObject.RequireSMTPAuth;

            ListRecipients();
        }


        public bool SaveData()
        {
            bool newObject = false;
            if (_representedObject == null)
            {
                hMailServer.Domain domain = APICreator.GetDomain(_domainID);

                hMailServer.DistributionLists lists = domain.DistributionLists;
                _representedObject = lists.Add();
                newObject = true;

                Marshal.ReleaseComObject(lists);
                Marshal.ReleaseComObject(domain);
            }

            _representedObject.Address = textAddress.Text;
            _representedObject.Active = checkEnabled.Checked;

            if (radioModePublic.Checked)
                _representedObject.Mode = eDistributionListMode.eLMPublic;

            if (radioModeMembership.Checked)
                _representedObject.Mode = eDistributionListMode.eLMMembership;

            if (optModeAnnouncements.Checked)
                _representedObject.Mode = eDistributionListMode.eLMAnnouncement;

            _representedObject.RequireSenderAddress = textRequireAddress.Text;
            _representedObject.RequireSMTPAuth = checkRequireSMTPAuthentication.Checked;

            _representedObject.Save();

            // Refresh the node in the tree if the name has changed.
            IMainForm mainForm = Instances.MainForm;
            mainForm.RefreshCurrentNode(textAddress.Text);

            // Set the object to clean.
            DirtyChecker.SetClean(this);

            if (newObject)
            {
                SearchNodeText crit = new SearchNodeText(_representedObject.Address);
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
                hMailServer.DistributionListRecipients recipients = _representedObject.Recipients;
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
                hMailServer.DistributionListRecipients recipients = _representedObject.Recipients;

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
           using (new WaitCursor())
           {
              listRecipients.Items.Clear();

              var listViewItems = new List<ListViewItem>();

              hMailServer.DistributionListRecipients recipients = _representedObject.Recipients;

              for (int i = 0; i < recipients.Count; i++)
              {
                 hMailServer.DistributionListRecipient recipient = recipients[i];

                 var item = new ListViewItem(recipient.RecipientAddress) {Tag = recipient.ID};

                 listViewItems.Add(item);

                 Marshal.ReleaseComObject(recipient);
              }

              Marshal.ReleaseComObject(recipients);

              listRecipients.Items.AddRange(listViewItems.ToArray());
           }
        }

        private void buttonDeleteRecipient_Click(object sender, EventArgs e)
        {
            hMailServer.DistributionListRecipients recipients = _representedObject.Recipients;

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
            hMailServer.DistributionListRecipients recipients = _representedObject.Recipients;
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

        private void buttonImport_Click(object sender, EventArgs e)
        {
           var importMembers = new formImportMembers(_representedObject);
           importMembers.ShowDialog();

           ListRecipients();
        }

    }
}
