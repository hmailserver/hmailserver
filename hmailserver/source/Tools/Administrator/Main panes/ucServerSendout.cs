// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucServerSendout : UserControl, ISettingsControl
    {
        public ucServerSendout()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

            EnableDisable();
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
            comboDomains.Items.Clear();

            hMailServer.Domains domains = APICreator.Application.Domains;
            string domainNames = domains.Names;
            string[] rows = Microsoft.VisualBasic.Strings.Split(domainNames, "\r\n", -1, Microsoft.VisualBasic.CompareMethod.Binary);

            List<ListViewItem> items = new List<ListViewItem>();

            foreach (string row in rows)
            {
                if (string.IsNullOrEmpty(row))
                    continue;

                string[] properties = Microsoft.VisualBasic.Strings.Split(row, "\t", -1, Microsoft.VisualBasic.CompareMethod.Text);

                int id = Convert.ToInt32(properties[0]);
                string name = properties[1];
                comboDomains.AddItem(name, null);
            }

            Marshal.ReleaseComObject(domains);
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

        private void buttonSend_Click(object sender, EventArgs e)
        {
            string wildcard = "";

            if (radioToAllAccounts.Checked)
                wildcard = "*";
            else if (radioSpecificDomain.Checked)
                wildcard = "*@" + comboDomains.Text;
            else if (radioMatchingWildcard.Checked)
                wildcard = textWildcard.Text;

            hMailServer.Utilities utilities = APICreator.Application.Utilities;
            utilities.EmailAllAccounts(wildcard, textFromAddress.Text, textFromName.Text, textSubject.Text, textBody.Text);
        }

        private void radioToAllAccounts_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void EnableDisable()
        {
            comboDomains.Enabled = radioSpecificDomain.Checked;
            textWildcard.Enabled = radioMatchingWildcard.Checked;
        }

        private void radioSpecificDomain_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void radioMatchingWildcard_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }
    }
}
