// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Shared;

namespace DataDirectorySynchronizer.Pages
{
    public partial class ucSynchMode : UserControl, IWizardPage
    {
        public ucSynchMode()
        {
            InitializeComponent();
        }


        public void OnShowPage(Dictionary<string, string> _state)
        {

        }

        public bool OnLeavePage(bool next)
        {
           if (radioImport.Checked)
              Globals.Mode = Globals.ModeType.Import;
           else
              Globals.Mode = Globals.ModeType.Delete;

           return true;
        }

        public string Title
        {
           get { return "Select mode"; }
        }
    }
}
