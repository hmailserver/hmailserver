// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;


namespace hMailServer.Administrator
{
    public partial class ucWhiteListing : UserControl, ISettingsControl
    {
        public ucWhiteListing()
        {
            InitializeComponent();
            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

        }

        public void OnLeavePage()
        {

        }


        public bool Dirty
        {
            get { return false; }
        }

        public void LoadData()
        {
            ListItems();
        }

        private void ListItems()
        {
            listWhiteAddresses.Items.Clear();

            hMailServer.WhiteListAddresses addresses = GetWhiteListAddresses();

            for (int i = 0; i < addresses.Count; i++)
            {
                hMailServer.WhiteListAddress address = addresses[i];

                ListViewItem item = listWhiteAddresses.Items.Add(address.Description);
                item.SubItems.Add(address.LowerIPAddress);
                item.SubItems.Add(address.UpperIPAddress);
                item.SubItems.Add(address.EmailAddress);

                item.Tag = address.ID;
            }

            Marshal.ReleaseComObject(addresses);
        }

        private hMailServer.WhiteListAddresses GetWhiteListAddresses()
        {
            hMailServer.AntiSpam antiSpam = APICreator.AntiSpamSettings;
            hMailServer.WhiteListAddresses addresses = antiSpam.WhiteListAddresses;

            Marshal.ReleaseComObject(antiSpam);

            return addresses;
        }

        public bool SaveData()
        {
            // nothing to save
           return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }

        private void buttonAddWhiteAddress_Click(object sender, EventArgs e)
        {
            formWhiteListAddress whiteDlg = new formWhiteListAddress();

            if (whiteDlg.ShowDialog() == DialogResult.OK)
            {
                hMailServer.AntiSpam antiSpam = APICreator.AntiSpamSettings;
                hMailServer.WhiteListAddresses addresses = antiSpam.WhiteListAddresses;
                hMailServer.WhiteListAddress address = addresses.Add();

                whiteDlg.SaveProperties(address);

                address.Save();

                Marshal.ReleaseComObject(antiSpam);
                Marshal.ReleaseComObject(addresses);
                Marshal.ReleaseComObject(address);

                ListItems();
            }


        }

        private void buttonEditWhiteAddress_Click(object sender, EventArgs e)
        {
            EditSelectedItem();


        }

        private void EditSelectedItem()
        {
            if (listWhiteAddresses.SelectedItems.Count != 1)
                return;

            int id = Convert.ToInt32(listWhiteAddresses.SelectedItems[0].Tag);
            hMailServer.WhiteListAddresses addresses = GetWhiteListAddresses();
            hMailServer.WhiteListAddress address = addresses.get_ItemByDBID(id);

            formWhiteListAddress whiteDlg = new formWhiteListAddress();

            whiteDlg.LoadProperties(address);

            if (whiteDlg.ShowDialog() == DialogResult.OK)
            {
                whiteDlg.SaveProperties(address);

                address.Save();

                ListItems();
            }

            Marshal.ReleaseComObject(address);
            Marshal.ReleaseComObject(addresses);
        }

        private void buttonDeleteWhiteAddress_Click(object sender, EventArgs e)
        {
           if (!Utility.AskDeleteItems())
              return;

            hMailServer.WhiteListAddresses addresses = GetWhiteListAddresses();

            foreach (ListViewItem item in listWhiteAddresses.SelectedItems)
            {
                int id = Convert.ToInt32(item.Tag);
                addresses.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(addresses);

            ListItems();
        }

        private void listWhiteAddresses_DoubleClick(object sender, EventArgs e)
        {
            EditSelectedItem();
        }
    }
}
