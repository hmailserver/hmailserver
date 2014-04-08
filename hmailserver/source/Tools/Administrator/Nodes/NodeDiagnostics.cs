// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;

namespace hMailServer.Administrator.Nodes
{
   class NodeDiagnostics : INode
   {
      #region INode Members

      public string Title
      {
         get
         {
            return "Diagnostics";
         }
         set { }
      }

      public System.Windows.Forms.UserControl CreateControl()
      {
         return new ucDiagnostics();
      }

      public List<INode> SubNodes
      {
         get
         {
            List<INode> subNodes = new List<INode>();
            return subNodes;

         }
      }

      public bool IsUserCreated
      {
         get { return false; }
      }

      public string Icon
      {
         get
         {
            return "heart.ico";
         }
      }

      public System.Drawing.Color ForeColor { get { return System.Drawing.SystemColors.WindowText; } set { } }

      public System.Windows.Forms.ContextMenuStrip CreateContextMenu()
      {
         return null;
      }

      #endregion
   }
}
