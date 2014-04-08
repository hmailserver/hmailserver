// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace hMailServer.Administrator
{
    public interface INode
    {
        string Title
        {
            get;
            set;
        }

        string Icon
        {
            get;
        }

        UserControl CreateControl();

        List<INode> SubNodes
        {
            get;
        }

       bool IsUserCreated
       {
          get;
       }

       System.Drawing.Color ForeColor
       {
          get;
          set;
       }

       ContextMenuStrip CreateContextMenu();
    }
}
