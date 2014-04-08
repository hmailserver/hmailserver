// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using MercuryMigrate.Pages;

namespace MercuryMigrate
{
   public partial class formMain : Form
   {
      public formMain()
      {
         InitializeComponent();

         wizard.AddPage(new ucWelcome());
         wizard.AddPage(new ucSelectPath());
         wizard.AddPage(new ucSelectDomains());
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
         this.Text = "Mercury to hMailServer Migration - Step " + currentPage.ToString() + " of " + lastPage.ToString();
      }
   }
}