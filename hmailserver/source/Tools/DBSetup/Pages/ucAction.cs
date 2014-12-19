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
   public partial class ucAction : UserControl, IWizardPage
   {
      private Dictionary<string, string> _state;

      public ucAction()
      {
         InitializeComponent();
      }

      public string Title
      {
         get
         {
            return "Select option";
         }
      }


      public void OnShowPage(Dictionary<string, string> state)
      {
         _state = state;

         if (_state.ContainsKey("CreateNew"))
         {
            if (state["CreateNew"] == "Yes")
               radioCreateNew.Checked = true;
            else
               radioSelectNew.Checked = true;
         }
      }

      public bool OnLeavePage(bool next)
      {
         if (radioCreateNew.Checked)
            _state["CreateNew"] = "Yes";
         else
            _state["CreateNew"] = "No";

         return true;
      }
   }
}
