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
    class NodeIPRanges : INode
    {
        public string Title
        {
            get
            {
                return "IP Ranges";
            }
            set { }
        }

        public System.Drawing.Color ForeColor { get { return System.Drawing.SystemColors.WindowText; } set { } }

        public bool IsUserCreated
        {
            get { return false; }
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
            return new ucIPRanges();
        }

        public List<INode> SubNodes
        {
            get
            {
                List<INode> subNodes = new List<INode>();

                hMailServer.SecurityRanges securityRanges = APICreator.SecurityRanges;
                for (int i = 0; i < securityRanges.Count; i++)
                {
                    hMailServer.SecurityRange securityRange = securityRanges[i];
                    subNodes.Add(new NodeIPRange(securityRange.ID, securityRange.Name, securityRange.Expires));
                    Marshal.ReleaseComObject(securityRange);
                }

                Marshal.ReleaseComObject(securityRanges);

                return subNodes;

            }
        }


        public ContextMenuStrip CreateContextMenu()
        {
            ContextMenuStrip menu = new ContextMenuStrip();
            ToolStripItem itemAdd = menu.Items.Add(Strings.Localize("Add..."));
            itemAdd.Click += new EventHandler(OnAddIPRange);
            return menu;
        }

        internal void OnAddIPRange(object sender, EventArgs e)
        {
            IMainForm mainForm = Instances.MainForm;
            NodeIPRange newRouteNode = new NodeIPRange(0, "", false);
            mainForm.ShowItem(newRouteNode);
        }
    }
}
