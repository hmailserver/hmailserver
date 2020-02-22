// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Nodes;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucIPRanges : ucItemsView
    {
        public ucIPRanges()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        protected override void LoadList()
        {
            listObjects.Items.Clear();

            hMailServer.Application app = APICreator.Application;

            hMailServer.SecurityRanges securityRanges = APICreator.SecurityRanges;

            for (int i = 0; i < securityRanges.Count; i++)
            {
                hMailServer.SecurityRange securityRange = securityRanges[i];

                ListViewItem item = listObjects.Items.Add(securityRange.Name);
                item.SubItems.Add(securityRange.LowerIP);
                item.SubItems.Add(securityRange.UpperIP);
                item.SubItems.Add(securityRange.Priority.ToString());

                if (securityRange.Expires)
                {
                   item.SubItems.Add(securityRange.ExpiresTime.ToString());
                   item.ForeColor = Color.Red;
                }
                else
                   item.SubItems.Add("");
               
                  

                item.Tag = securityRange.ID;


                Marshal.ReleaseComObject(securityRange);
            }

            Marshal.ReleaseComObject(securityRanges);
        }

        protected override ListView GetListView()
        {
            return listObjects;
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.SecurityRanges securityRanges = APICreator.SecurityRanges;

            foreach (var item in items)
            {
                int id = Convert.ToInt32(item.Tag);
                securityRanges.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(securityRanges);
        }

        protected override void AddItem()
        {
            IMainForm mainForm = Instances.MainForm;

            // Show the new account.
            NodeIPRange newNode = new NodeIPRange(0, "", false);
            mainForm.ShowItem(newNode);
        }

        private void buttonDefault_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show(Strings.Localize("This operation will change the configuration of the IP ranges back to their default values. Are you sure you want to do this?"), EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                hMailServer.Application app = APICreator.Application;

                hMailServer.Settings settings = app.Settings;
                hMailServer.SecurityRanges securityRanges = settings.SecurityRanges;

                securityRanges.SetDefault();

                Marshal.ReleaseComObject(settings);
                Marshal.ReleaseComObject(securityRanges);

                LoadList();

                Instances.MainForm.RefreshCurrentNode(null);
            }
        }

        private void ucIPRanges_Load(object sender, EventArgs e)
        {

        }
    }
}
