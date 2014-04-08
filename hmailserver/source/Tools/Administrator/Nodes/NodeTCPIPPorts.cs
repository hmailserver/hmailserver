// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Utilities.Localization;

namespace hMailServer.Administrator.Nodes
{
    class NodeTCPIPPorts : INode
    {
        public string Title
        {
            get
            {
                return "TCP/IP ports";
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
            return new ucTCPIPPorts();
        }

        public List<INode> SubNodes
        {
            get
            {
               List<INode> subNodes = new List<INode>();

               hMailServer.TCPIPPorts tcpIPPorts = APICreator.TCPIPPortsSettings;
               for (int i = 0; i < tcpIPPorts.Count; i++)
               {
                  hMailServer.TCPIPPort tcpIPPort = tcpIPPorts[i];
                  subNodes.Add(new NodeTCPIPPort(tcpIPPort.ID, InternalNames.GetPortName(tcpIPPort)));
                  Marshal.ReleaseComObject(tcpIPPort);
               }
               Marshal.ReleaseComObject(tcpIPPorts);

               return subNodes;

            }
        }


        public ContextMenuStrip CreateContextMenu()
        {
           ContextMenuStrip menu = new ContextMenuStrip();
           ToolStripItem itemAdd = menu.Items.Add(Strings.Localize("Add..."));
           itemAdd.Click += new EventHandler(OnAddTCPIPPort);
           return menu;
        }

        internal void OnAddTCPIPPort(object sender, EventArgs e)
        {
           IMainForm mainForm = Instances.MainForm;
           NodeTCPIPPort newRouteNode = new NodeTCPIPPort(0, "");
           mainForm.ShowItem(newRouteNode);
        }
    }
}
