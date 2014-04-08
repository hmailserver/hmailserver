// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;


namespace hMailServer.Administrator.Nodes
{
   class NodeDNSBlackLists : INode
   {

      public NodeDNSBlackLists()
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
            return "DNS blacklists";
         }
          set { }
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
         return new ucDNSBlackLists();
      }

      public List<INode> SubNodes
      {
         get
         {
            List<INode> subNodes = new List<INode>();

            hMailServer.DNSBlackLists dnsBlackLists = APICreator.DNSBlackLists;
            for (int i = 0; i < dnsBlackLists.Count; i++)
            {
               hMailServer.DNSBlackList dnsBlackList = dnsBlackLists[i];
               subNodes.Add(new NodeDNSBlackList(dnsBlackList.DNSHost, dnsBlackList.ID));
               Marshal.ReleaseComObject(dnsBlackList);
            }
            Marshal.ReleaseComObject(dnsBlackLists);

            return subNodes;

         }
      }

      public ContextMenuStrip CreateContextMenu()
      {
         ContextMenuStrip menu = new ContextMenuStrip();
         ToolStripItem itemAdd = menu.Items.Add(Strings.Localize("Add..."));
         itemAdd.Click += new EventHandler(OnAddDNSBlackList);
         return menu;
      }

      internal void OnAddDNSBlackList(object sender, EventArgs e)
      {
         IMainForm mainForm = Instances.MainForm;
         NodeDNSBlackList newDNSBlackListNode = new NodeDNSBlackList("", 0);
         mainForm.ShowItem(newDNSBlackListNode);
      }

   }
}
