using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace hMailServer.Administrator.Nodes
{
    class NodeGlobalRules : INode
    {
        public string Title
        {
            get
            {
                return "Rules";
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
            return new ucGlobalRules();
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
          return null;
       }
    }
}
