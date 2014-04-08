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
    class NodeGroup : INode
    {
        private int _groupID = 0;
        private string _groupName = "";


        public NodeGroup(string groupName, int groupID)
        {
            _groupName = groupName;
            _groupID = groupID;
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
                return _groupName;
            }
            set
            {
                _groupName = value;
            }
        }

        public string Icon
        {
            get
            {
               return "chart_organisation.ico";
            }
        }

        public UserControl CreateControl()
        {
            return new ucGroup(_groupID);
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
           if (Utility.AskDeleteItem(_groupName))
           {
              hMailServer.Groups groups = APICreator.Groups;
              groups.DeleteByDBID(_groupID);
              Instances.MainForm.RefreshParentNode();

              Marshal.ReleaseComObject(groups);
           }
        }
    }
}
