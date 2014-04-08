using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;

namespace hMailServer.Administrator.Nodes
{
    class NodeIncomingRelay : INode
    {
        private int _incomingRelayID = 0;
        private string _incomingRelayTitle = "";

        public NodeIncomingRelay(int incomingRelayID, string incomingRelayTitle)
        {
            _incomingRelayID = incomingRelayID;
            _incomingRelayTitle = incomingRelayTitle;
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
                return _incomingRelayTitle;
            }
            set
            {
                _incomingRelayTitle = value;
            }
        }

        public string Icon
        {
            get
            {
               return "arrow_in.ico";
            }
        }

        public UserControl CreateControl()
        {
            return new ucIncomingRelay(_incomingRelayID);
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
            if (!Utility.AskDeleteItem(_incomingRelayTitle))
               return;

            hMailServer.Settings settings = APICreator.Settings;
            hMailServer.IncomingRelays incomingRelays = settings.IncomingRelays;
            
            incomingRelays.DeleteByDBID(_incomingRelayID);
            Marshal.ReleaseComObject(incomingRelays);
            Marshal.ReleaseComObject(settings);

            Instances.MainForm.RefreshParentNode();
        }
    }
}
