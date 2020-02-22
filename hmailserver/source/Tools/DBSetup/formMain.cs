// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DBSetup.Pages;

namespace DBSetup
{
   public partial class formMain : Form
   {
      public formMain()
      {
         InitializeComponent();

         this.Cursor = Cursors.Default;

         wizard.AddPage(new ucWelcome());
         wizard.AddPage(new ucAction());
         wizard.AddPage(new ucSelectDatabaseType());
         wizard.AddPage(new ucDBConnectionInfo());
         wizard.AddPage(new ucServiceDependency());
         wizard.AddPage(new ucPerformTask());
         wizard.AddPage(new ucCompleted());
      }

      private void wizard_OnCancel(object sender, EventArgs e)
      {
         this.Close();
      }

      private void formMain_Shown(object sender, EventArgs e)
      {
         wizard.Start();
      }

      private void wizard_PageChanged(int currentPage, int lastPage)
      {
         this.Text = "hMailServer Database Setup - Step " + currentPage.ToString() + " of " + lastPage.ToString();
      }
   }
}