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
    class NodeRoutes : INode
    {
        public string Title
        {
            get
            {
                return "Routes";
            }
            set
            {
            }
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
            return new ucRoutes();
        }

        public List<INode> SubNodes
        {
            get
            {
                List<INode> subNodes = new List<INode>();

                hMailServer.Routes routes = APICreator.Routes;
                for (int i = 0; i < routes.Count; i++)
                {
                    hMailServer.Route route = routes[i];
                    subNodes.Add(new NodeRoute(route.ID, route.DomainName));
                    Marshal.ReleaseComObject(route);
                }

                Marshal.ReleaseComObject(routes);

                return subNodes;

            }
        }

        public ContextMenuStrip CreateContextMenu()
        {
            ContextMenuStrip menu = new ContextMenuStrip();
            ToolStripItem itemAdd = menu.Items.Add(Strings.Localize("Add..."));
            itemAdd.Click += new EventHandler(OnAddRoute);
            return menu;
        }

        internal void OnAddRoute(object sender, EventArgs e)
        {
            IMainForm mainForm = Instances.MainForm;
            NodeRoute newRouteNode = new NodeRoute(0, "");
            mainForm.ShowItem(newRouteNode);
        }
    }
}
