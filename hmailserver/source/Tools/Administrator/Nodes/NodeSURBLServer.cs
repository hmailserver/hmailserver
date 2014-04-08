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
    class NodeSURBLServer : INode
    {
        private int _serverID;
        private string _serverName;

        public NodeSURBLServer(int serverID, string serverName)
        {
            _serverID = serverID;
            _serverName = serverName;
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
                return _serverName;
            }
            set
            {
                _serverName = value;
            }
        }

        public string Icon
        {
            get
            {
               return "email_delete.ico";
            }
        }

        public UserControl CreateControl()
        {
            return new ucSURBLServer(_serverID);
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
           if (Utility.AskDeleteItem(_serverName))
           {
              hMailServer.SURBLServers surblServers = APICreator.SURBLServers;
              surblServers.DeleteByDBID(_serverID);
              Marshal.ReleaseComObject(surblServers);

              Instances.MainForm.RefreshParentNode();
           }
        }
    }
}
