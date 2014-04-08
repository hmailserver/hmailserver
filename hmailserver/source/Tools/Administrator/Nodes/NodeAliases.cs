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
    class NodeAliases : INode
    {
        private int _domainID;

        public NodeAliases(int domainID)
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
                return "Aliases";
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
            return new ucAliases(_domainID);
        }

        public List<INode> SubNodes
        {
            get
            {
                List<INode> subNodes = new List<INode>();

                hMailServer.Domain domain = APICreator.GetDomain(_domainID);
                hMailServer.Aliases aliases = domain.Aliases;

                for (int i = 0; i < aliases.Count; i++)
                {
                    hMailServer.Alias alias = aliases[i];

                    subNodes.Add(new NodeAlias(domain.ID, alias.Name, alias.ID));

                    Marshal.ReleaseComObject(alias);
                }

                Marshal.ReleaseComObject(domain);
                Marshal.ReleaseComObject(aliases);

                return subNodes;

            }
        }

        public ContextMenuStrip CreateContextMenu()
        {
            ContextMenuStrip menu = new ContextMenuStrip();
            ToolStripItem itemAdd = menu.Items.Add(Strings.Localize("Add..."));
            itemAdd.Click += new EventHandler(OnAddAlias);
            return menu;
        }

        internal void OnAddAlias(object sender, EventArgs e)
        {
            NodeAlias newAliasNode = new NodeAlias(_domainID, "", 0);
            Instances.MainForm.ShowItem(newAliasNode);
        }
    }
}
