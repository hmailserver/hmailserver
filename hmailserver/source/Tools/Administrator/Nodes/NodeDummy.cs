// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace hMailServer.Administrator.Nodes
{
    class NodeDummy : INode
    {

        public string Title
        {
            get { return "Dummy";  }
            set { }
        }

        public System.Drawing.Color ForeColor { get { return System.Drawing.SystemColors.WindowText; } set { } }

        public string Icon
        {
            get { return null;  }
        }

        public System.Windows.Forms.UserControl CreateControl()
        {
            return null;
        }

        public List<INode> SubNodes
        {
            get { return null;  }
        }

        public bool IsUserCreated
        {
            get { return false;  }
        }

        public System.Windows.Forms.ContextMenuStrip CreateContextMenu()
        {
            return null;
        }
    }
}
