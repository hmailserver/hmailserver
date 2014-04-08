// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using hMailServer.Administrator.Utilities;

namespace hMailServer.Administrator.Nodes
{
    class NodeDistributionLists : INode
    {
        private int _domainID;

        public NodeDistributionLists(int domainID)
        {
            _domainID = domainID;
        }

        public System.Drawing.Color ForeColor { get { return System.Drawing.SystemColors.WindowText; } set { } }

        public bool IsUserCreated
        {
            get { return false; }
        }

        public string Title
        {
            get
            {
                return "Distribution lists";
            }
            set { }
        }

        public string Icon
        {
            get
            {
                return "folder.ico";
            }
        }

        public UserControl CreateControl()
        {
            return new ucDistributionLists(_domainID);
        }


        public List<INode> SubNodes
        {
            get
            {
                

                List<INode> subNodes = new List<INode>();

                hMailServer.Domain domain = APICreator.GetDomain(_domainID);

                hMailServer.DistributionLists lists = domain.DistributionLists;

                for (int i = 0; i < lists.Count; i++)
                {
                    hMailServer.DistributionList list = lists[i];

                    subNodes.Add(new NodeDistributionList(domain.ID, list.Address, list.ID));

                    Marshal.ReleaseComObject(list);
                }

                Marshal.ReleaseComObject(domain);
                Marshal.ReleaseComObject(lists);

                return subNodes;

            }
        }

        public ContextMenuStrip CreateContextMenu()
        {
            ContextMenuStrip menu = new ContextMenuStrip();
            ToolStripItem itemAdd = menu.Items.Add(Strings.Localize("Add..."));
            itemAdd.Click += new EventHandler(OnAddDistributionList);
            return menu;
        }

        internal void OnAddDistributionList(object sender, EventArgs e)
        {
            NodeDistributionList newDistributionList = new NodeDistributionList(_domainID, "", 0);
            Instances.MainForm.ShowItem(newDistributionList);
        }
    }
}
