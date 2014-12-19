// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;

using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DataDirectorySynchronizer.Pages;

namespace DataDirectorySynchronizer
{
   public partial class formMain : Form
   {
      public formMain()
      {
         InitializeComponent();

         wizard.AddPage(new ucWelcome());
         wizard.AddPage(new ucSynchMode());
         wizard.AddPage(new ucSelectDomain());
         wizard.AddPage(new ucProgress());
      }

      private void formMain_Shown(object sender, EventArgs e)
      {
         wizard.Start();
      }

      private void wizard_Cancel(object sender, EventArgs e)
      {
         this.Close();
      }
   }
}