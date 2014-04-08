// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
namespace hMailServer.Administrator.Nodes
{
    class NodeDistributionList : INode
    {
        private int _domainID;

        private string _listName;
        private int _listID;

        public NodeDistributionList(int domainID, string listName, int listID)
        {
            _domainID = domainID;
            _listName = listName;
            _listID = listID;
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
                return _listName;
            }
            set 
            {
                _listName = value;
            }

        }

        public string Icon
        {
            get
            {
               return "arrow_out.ico";
            }
        }

        public UserControl CreateControl()
        {
            return new ucDistributionList(_domainID, _listID);
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
            hMailServer.DistributionList list = links.get_DistributionList(_listID);

            if (Utility.AskDeleteItem(list.Address))
            {
               list.Delete();

               Marshal.ReleaseComObject(links);
               Marshal.ReleaseComObject(list);

               Instances.MainForm.RefreshParentNode();
            }
        }
    }
}
