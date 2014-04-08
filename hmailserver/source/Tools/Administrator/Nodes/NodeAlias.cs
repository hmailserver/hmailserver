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
    class NodeAlias : INode
    {
        private int _domainID;
        private string _aliasName = "";
        private int _aliasID = 0;

        public NodeAlias(int domainID, string aliasName, int aliasID)
        {
            _domainID = domainID;
            _aliasName = aliasName;
            _aliasID = aliasID;
        }

        public System.Drawing.Color ForeColor { get { return System.Drawing.SystemColors.WindowText; } set { } }

        public bool IsUserCreated
        {
            get { return true; }
        }

        public string Title
        {
            get
            {
                return _aliasName;
            }
            set 
            {
                _aliasName = value;
            }
        }

        public string Icon
        {
            get
            {
               return "arrow_switch.ico";
            }
        }

        public UserControl CreateControl()
        {
            return new ucAlias(_domainID, _aliasID);
        }

        public List<INode> SubNodes
        {
            get
            {
                List<INode> subNodes = new List<INode>();
                return subNodes;
            }
        }

        public ContextMenuStrip CreateContextMenu()
        {
            ContextMenuStrip menu = new ContextMenuStrip();
            ToolStripItem itemAdd = menu.Items.Add(Strings.Localize("Remove"));
            itemAdd.Click += OnDeleteObject;

            return menu;
        }

        public void OnDeleteObject(object sender, EventArgs args)
        {
            hMailServer.Links links = APICreator.Links;
            hMailServer.Alias alias = links.get_Alias(_aliasID);

            if (Utility.AskDeleteItem(alias.Name))
            {
               alias.Delete();

               Marshal.ReleaseComObject(links);
               Marshal.ReleaseComObject(alias);

               Instances.MainForm.RefreshParentNode();
            }
        }
    }
}
