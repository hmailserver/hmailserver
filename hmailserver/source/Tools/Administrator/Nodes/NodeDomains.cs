// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace hMailServer.Administrator.Nodes
{
    class NodeDomains : INode
    {
        private IMainForm _mainForm;

        public NodeDomains(IMainForm mainForm)
        {
           _mainForm = mainForm;
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
                return "Domains";
            }
            set { }
        }

        public string Icon
        {
            get
            {
               return "world.ico";
            }
        }

        public UserControl CreateControl()
        {
            return new ucDomains();
        }

        public List<INode> SubNodes
        {
            get
            {
                List<INode> subNodes = new List<INode>();

                hMailServer.Domains domains = APICreator.Application.Domains;
                string domainNames = domains.Names;

                string [] rows = Microsoft.VisualBasic.Strings.Split(domainNames, "\r\n", -1, Microsoft.VisualBasic.CompareMethod.Binary);

                foreach (string row in rows)
                {
                    if (string.IsNullOrEmpty(row))
                        continue;

                    string [] properties = Microsoft.VisualBasic.Strings.Split(row, "\t", -1, Microsoft.VisualBasic.CompareMethod.Text);

                    int id = Convert.ToInt32(properties[0]);
                    string name = properties[1];


                    subNodes.Add(new NodeDomain(_mainForm, id, name));
                }

                Marshal.ReleaseComObject(domains);

                return subNodes;
            }
        }


       public ContextMenuStrip CreateContextMenu()
       {
          ContextMenuStrip menu = new ContextMenuStrip();
          ToolStripItem itemAdd = menu.Items.Add(Strings.Localize("Add..."));
          itemAdd.Click += new EventHandler(OnAddDomain);
          return menu;
       }

       internal void OnAddDomain(object sender, EventArgs e)
       {
          NodeDomain domain = new NodeDomain(_mainForm, 0, null);
          _mainForm.ShowItem(domain);
       }
    }
}
