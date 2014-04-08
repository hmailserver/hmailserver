// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace hMailServer.Administrator.Nodes
{
   class NodeAntiSpam : INode
   {
      public NodeAntiSpam()
      {
      
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
            return "Anti-spam";
         }
          set { }
      }

      public string Icon
      {
         get
         {
            return "spam_filter.ico";
         }
      }

      public UserControl CreateControl()
      {
         return new ucAntiSpam();
      }

      public List<INode> SubNodes
      {
         get
         {
            List<INode> subNodes = new List<INode>();
            subNodes.Add(new NodeDNSBlackLists());
            subNodes.Add(new NodeSURBLServers());
            subNodes.Add(new NodeGreyListing());
            subNodes.Add(new NodeWhiteListing());
            return subNodes;

         }
      }

      public ContextMenuStrip CreateContextMenu()
      {
         return null;
      }
   }
}
