// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace hMailServer.Administrator.Nodes
{
    class NodeUtilities : INode
    {
        public string Title
        {
            get
            {
                return "Utilities";
                
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
               return "multitool.ico";
            }
        }

        public UserControl CreateControl()
        {
            return new ucUtilities();
        }

        public List<INode> SubNodes
        {
            get
            {
                List<INode> subNodes = new List<INode>();
                subNodes.Add(new NodeBackup());
                subNodes.Add(new NodeMXQuery());
                subNodes.Add(new NodeServerSendout());
                subNodes.Add(new NodeDiagnostics());
                return subNodes;

            }
        }


       public ContextMenuStrip CreateContextMenu()
       {
          return null;
       }
    }
}
