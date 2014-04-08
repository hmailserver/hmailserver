// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using System.Drawing;

namespace hMailServer.Administrator.Nodes
{
    class NodeIPRange : INode
    {
        private int _securityRangeID = 0;
        private string _securityRangeTitle = "";
        private bool _expires;

        public NodeIPRange(int securityRangeID, string securityRangeTitle, bool expires)
        {
            _securityRangeID = securityRangeID;
            _securityRangeTitle = securityRangeTitle;
            _expires = expires;
        }

        public System.Drawing.Color ForeColor 
        { 
           
           get 
           {
              System.Drawing.Color color = _expires ? Color.Red : System.Drawing.SystemColors.WindowText; 
              return color; 
           }   
           set
           {
              if (value == Color.Red)
                 _expires = true;
              else
                 _expires = false;
           }
 
        }

        public bool IsUserCreated
        {
            get { return true; }
        }

        public string Title
        {
            get
            {
                return _securityRangeTitle;
            }
            set 
            {
                _securityRangeTitle = value;
            }
        }

        public string Icon
        {
            get
            {
               return "construction.ico";
            }
        }

        public UserControl CreateControl()
        {
            return new ucIPRange(_securityRangeID);
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
           if (Utility.AskDeleteItem(_securityRangeTitle))
           {
              hMailServer.SecurityRanges securityRanges = APICreator.SecurityRanges;
              securityRanges.DeleteByDBID(_securityRangeID);
              Marshal.ReleaseComObject(securityRanges);

              Instances.MainForm.RefreshParentNode();
           }
        }
    }
}
