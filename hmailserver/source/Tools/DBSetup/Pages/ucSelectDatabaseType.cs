// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using hMailServer.Shared;

namespace DBSetup.Pages
{
   public partial class ucSelectDatabaseType : UserControl, IWizardPage
   {
      private Dictionary<string, string> _state;

      public ucSelectDatabaseType()
      {
         InitializeComponent();
      }

      public string Title
      {
         get
         {
            return "Select database server type";
         }
      }

      public void OnShowPage(Dictionary<string, string> state)
      {
         _state = state;

         if (_state.ContainsKey("ServerType"))
         {
            switch (state["ServerType"])
            {
               case "MSSQL":
                  radioMSSQL.Checked = true;
                  break;
               case "MySQL":
                  radioMySQL.Checked = true;
                  break;
               case "PGSQL":
                  radioPGSQL.Checked = true;
                  break;
            }
         }
      }

      public bool OnLeavePage(bool next)
      {
         if (radioMSSQL.Checked)
            _state["ServerType"] = "MSSQL";
         else if (radioMySQL.Checked)
            _state["ServerType"] = "MySQL";
         else if (radioPGSQL.Checked)
            _state["ServerType"] = "PGSQL";

         return true;
      }
   }
}
