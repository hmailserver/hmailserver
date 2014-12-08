// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Shared;

namespace DataDirectorySynchronizer.Pages
{
   public partial class ucWelcome : UserControl, IWizardPage
   {
      public ucWelcome()
      {
         InitializeComponent();
      }


      public void OnShowPage(Dictionary<string, string> _state)
      {
         
      }

      public bool OnLeavePage(bool next)
      {

         return true;
      }

      public string Title
      {
         get { return "Welcome"; }
      }
   }
}
