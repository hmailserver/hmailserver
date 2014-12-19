// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucGreyListing : UserControl, ISettingsControl
    {
        public ucGreyListing()
        {
            InitializeComponent();

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

        }

        public void OnLeavePage()
        {

        }

        public bool Dirty
        {
            get
            {
                return DirtyChecker.IsDirty(this);
            }
        }

        public void LoadData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;
            hMailServer.AntiSpam antiSpamSettings = settings.AntiSpam;

            checkEnable.Checked = antiSpamSettings.GreyListingEnabled;
            textGreyListingInitialDelay.Number = antiSpamSettings.GreyListingInitialDelay;
            textGreyListingInitialDelete.Number = antiSpamSettings.GreyListingInitialDelete / 24;
            textGreyListingFinalDelete.Number = antiSpamSettings.GreyListingFinalDelete / 24;

            checkBypassGreylistingOnSPFSuccess.Checked = antiSpamSettings.BypassGreylistingOnSPFSuccess;
            checkBypassGreyListingOnMailFromMX.Checked = antiSpamSettings.BypassGreylistingOnMailFromMX;

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(antiSpamSettings);

            ListWhiteListAddresses();

            EnableDisable();

        }

        private void EnableDisable()
        {
            textGreyListingInitialDelay.Enabled = checkEnable.Checked;
            textGreyListingInitialDelete.Enabled = checkEnable.Checked;
            textGreyListingFinalDelete.Enabled = checkEnable.Checked;
            checkBypassGreylistingOnSPFSuccess.Enabled = checkEnable.Checked;
            checkBypassGreyListingOnMailFromMX.Enabled = checkEnable.Checked;
        }

        private void ListWhiteListAddresses()
        {
            listWhitelistingRecords.Items.Clear();

            hMailServer.GreyListingWhiteAddresses whiteAddresses = APICreator.GreylistingWhiteAddresses;

            for (int i = 0; i < whiteAddresses.Count; i++)
            {
                hMailServer.GreyListingWhiteAddress address = whiteAddresses[i];

                ListViewItem item = listWhitelistingRecords.Items.Add(address.IPAddress);
                item.SubItems.Add(address.Description);
                item.Tag = address.ID;

                Marshal.ReleaseComObject(address);
            }

            Marshal.ReleaseComObject(whiteAddresses);
        }

        public bool SaveData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;
            hMailServer.AntiSpam antiSpamSettings = settings.AntiSpam;

            antiSpamSettings.GreyListingEnabled = checkEnable.Checked;
            antiSpamSettings.GreyListingInitialDelay = textGreyListingInitialDelay.Number;
            antiSpamSettings.GreyListingInitialDelete = textGreyListingInitialDelete.Number * 24;
            antiSpamSettings.GreyListingFinalDelete = textGreyListingFinalDelete.Number * 24;
            antiSpamSettings.BypassGreylistingOnMailFromMX = checkBypassGreyListingOnMailFromMX.Checked;
            antiSpamSettings.BypassGreylistingOnSPFSuccess = checkBypassGreylistingOnSPFSuccess.Checked;

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(antiSpamSettings);

            DirtyChecker.SetClean(this);

            return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }

        private void OnContentChanged()
        {
            Instances.MainForm.OnContentChanged();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
            OnContentChanged();
        }

        private void buttonAddWhiteList_Click(object sender, EventArgs e)
        {
            formGreyListingWhiteAddress whiteDlg = new formGreyListingWhiteAddress();

            if (whiteDlg.ShowDialog() == DialogResult.OK)
            {
                hMailServer.Application app = APICreator.Application;
                hMailServer.Settings settings = app.Settings;
                hMailServer.AntiSpam antiSpamSettings = settings.AntiSpam;
                hMailServer.GreyListingWhiteAddresses greyListingWhiteAddresses = antiSpamSettings.GreyListingWhiteAddresses;
                hMailServer.GreyListingWhiteAddress whiteAddress = greyListingWhiteAddresses.Add();

                whiteDlg.SaveProperties(whiteAddress);
                whiteAddress.Save();

                Marshal.ReleaseComObject(settings);
                Marshal.ReleaseComObject(antiSpamSettings);
                Marshal.ReleaseComObject(greyListingWhiteAddresses);
                Marshal.ReleaseComObject(whiteAddress);

                ListWhiteListAddresses();
            }
        }

        private void buttonEditWhiteList_Click(object sender, EventArgs e)
        {
            if (listWhitelistingRecords.SelectedItems.Count != 1)
                return;

            int id = Convert.ToInt32(listWhitelistingRecords.SelectedItems[0].Tag);
            GreyListingWhiteAddresses addresses = APICreator.GreylistingWhiteAddresses;

            hMailServer.GreyListingWhiteAddress whiteAddress =
               addresses.get_ItemByDBID(id);

            formGreyListingWhiteAddress whiteDlg = new formGreyListingWhiteAddress();
            whiteDlg.LoadProperties(whiteAddress);

            if (whiteDlg.ShowDialog() == DialogResult.OK)
            {
                whiteDlg.SaveProperties(whiteAddress);

                whiteAddress.Save();

                ListWhiteListAddresses();
            }

            Marshal.ReleaseComObject(whiteAddress);
            Marshal.ReleaseComObject(addresses);
        }

        private void buttonDeleteWhiteList_Click(object sender, EventArgs e)
        {
            hMailServer.GreyListingWhiteAddresses whiteAddresses = APICreator.GreylistingWhiteAddresses;

            foreach (ListViewItem item in listWhitelistingRecords.SelectedItems)
            {
                int id = Convert.ToInt32(item.Tag);
                whiteAddresses.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(whiteAddresses);

            ListWhiteListAddresses();
        }

        private void checkEnable_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }



    }
}
