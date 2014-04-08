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
    class NodeSURBLServers : INode
    {
        public string Title
        {
            get
            {
                return "SURBL servers";
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
            return new ucSURBLServers();
        }

        public List<INode> SubNodes
        {
            get
            {
               List<INode> subNodes = new List<INode>();

               hMailServer.AntiSpam antiSpam = APICreator.AntiSpamSettings;
               hMailServer.SURBLServers surblServers = antiSpam.SURBLServers;
               for (int i = 0; i < surblServers.Count; i++)
               {
                  hMailServer.SURBLServer surblServer = surblServers[i];
                  subNodes.Add(new NodeSURBLServer(surblServer.ID, surblServer.DNSHost));
                  Marshal.ReleaseComObject(surblServer);
               }
               Marshal.ReleaseComObject(antiSpam);
               Marshal.ReleaseComObject(surblServers);

               return subNodes;

            }
        }


        public ContextMenuStrip CreateContextMenu()
        {
           ContextMenuStrip menu = new ContextMenuStrip();
           ToolStripItem itemAdd = menu.Items.Add(Strings.Localize("Add..."));
           itemAdd.Click += new EventHandler(OnAddSURBLServer);
           return menu;
        }

        internal void OnAddSURBLServer(object sender, EventArgs e)
        {
           IMainForm mainForm = Instances.MainForm;
           NodeSURBLServer newSURBLServerNode = new NodeSURBLServer(0, "");
           mainForm.ShowItem(newSURBLServerNode);
        }
    }
}
