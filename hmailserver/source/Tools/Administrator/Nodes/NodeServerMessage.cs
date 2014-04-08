// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace hMailServer.Administrator.Nodes
{ 
   class NodeServerMessage : INode
   {
       private int _id;
       private string _name;

      public NodeServerMessage(int id, string name)
      {
          _id = id;
          _name = name;
      }

      public System.Drawing.Color ForeColor { get { return System.Drawing.SystemColors.WindowText; } set { } }

      public bool IsUserCreated
      {
         get { return true; }
      }

      public string Title
      {
         get
         {
             return _name;
         }
         set 
         {
             _name = value;
         }
      }

      public string Icon
      {
         get
         {
            return "text_align_justity.ico";
         }
      }

      public UserControl CreateControl()
      {
         return new ucServerMessage(_id);
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
