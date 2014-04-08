// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace hMailServer.Administrator.Nodes
{
    class NodeWelcome : INode
    {
        public string Title
        {
            get 
            {
                return "Welcome";
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
               return "star.ico";
            }
        }

        public UserControl CreateControl()
        {
            return new ucWelcome();
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
