// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities.Localization;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;

namespace hMailServer.Administrator.Nodes
{
   class NodeTCPIPPort : INode
   {
       private int _portID = 0;
       private string _portName = "";

      public NodeTCPIPPort(int portID, string portName)
      {
          _portID = portID;
          _portName = portName;
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
             return _portName;
         }
         set
         {
             _portName = value;
         }
      }

      public string Icon
      {
         get
         {
            return "connect.ico";
         }
      }

      public UserControl CreateControl()
      {
         return new ucTCPIPPort(_portID);
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
         ContextMenuStrip menu = new ContextMenuStrip();
         ToolStripItem itemAdd = menu.Items.Add(Strings.Localize("Remove"));
         itemAdd.Click += OnDeleteObject;

         return menu;
      }

      public void OnDeleteObject(object sender, EventArgs args)
      {
         if (!Utility.AskDeleteItem(_portName))
             return;

          hMailServer.TCPIPPorts ports = APICreator.TCPIPPortsSettings;
          ports.DeleteByDBID(_portID);
          Marshal.ReleaseComObject(ports);
          Instances.MainForm.RefreshParentNode();
      }
   }
}
