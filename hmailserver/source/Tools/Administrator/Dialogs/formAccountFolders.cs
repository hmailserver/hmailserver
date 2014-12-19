// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Shared;

namespace hMailServer.Administrator.Dialogs
{
   public partial class formAccountFolders : Form
   {
      public formAccountFolders(hMailServer.IMAPFolders imapFolders, bool publicFolders)
      {
         InitializeComponent();

         ucIMAPFolders1.LoadProperties(imapFolders, publicFolders);

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

         Strings.Localize(this.buttonClose);
         this.Text = Strings.Localize(this.Text);
         ucIMAPFolders1.Localize();
      }

      private void buttonClose_Click(object sender, EventArgs e)
      {
         if (ucIMAPFolders1.SaveCurrentFolder())
         {
            this.Close();
         }
      }
   }
}