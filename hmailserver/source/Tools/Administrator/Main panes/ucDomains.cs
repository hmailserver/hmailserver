// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucDomains : ucItemsView
    {
        [DllImport("user32.dll", EntryPoint = "LockWindowUpdate", SetLastError = true,
            ExactSpelling = true, CharSet = CharSet.Auto,
            CallingConvention = CallingConvention.StdCall)]
        private static extern long LockWindowUpdate(long Handle);

        public ucDomains()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        public override void OnLeavePage()
        {
            //if (_domains != null)
            //    Marshal.ReleaseComObject(_domains);

            base.OnLeavePage();
        }

        protected override ListView GetListView()
        {
            return listDomains;
        }

        protected override void LoadList()
        {
            listDomains.Items.Clear();

            hMailServer.Application app = APICreator.Application;
            hMailServer.Domains domains = app.Domains;

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
                bool enabled = properties[2] == "1";

                ListViewItem item = new ListViewItem();

                item.Text = name;
                item.SubItems.Add(EnumStrings.GetYesNoString(enabled));
                item.Tag = id;

                items.Add(item);
            }

            listDomains.Items.AddRange(items.ToArray());

            Marshal.ReleaseComObject(domains);
        }

        protected override void DeleteItems(List<ListViewItem> items)
        {
            hMailServer.Domains domains = APICreator.Application.Domains;

            foreach (var item in items)
            {
                int domainID = Convert.ToInt32(item.Tag);
                hMailServer.Domain domain = domains.get_ItemByDBID(domainID);
                domain.Delete();
                Marshal.ReleaseComObject(domain);
            }
            
            Marshal.ReleaseComObject(domains);
        }

        protected override void AddItem()
        {
            IMainForm mainForm = Instances.MainForm;

            // Show the new domain.
            NodeDomain domain = new NodeDomain(mainForm, 0, "");
            mainForm.ShowItem(domain);
        }


    }
}
