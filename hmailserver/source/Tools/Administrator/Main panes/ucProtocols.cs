// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucProtocols : UserControl, ISettingsControl
    {
        public ucProtocols()
        {
            InitializeComponent();

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        public void OnLeavePage()
        {

        }

        public bool Dirty
        {
            get
            {
                return DirtyChecker.IsDirty(this);
            }
        }

        private void OnContentChanged()
        {
            Instances.MainForm.OnContentChanged();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
            OnContentChanged();
        }

        public void LoadData()
        {
            hMailServer.Application app = APICreator.Application;

            hMailServer.Settings settings = app.Settings;
            checkSMTP.Checked = settings.ServiceSMTP;
            checkPOP3.Checked = settings.ServicePOP3;
            checkIMAP.Checked = settings.ServiceIMAP;

            Marshal.ReleaseComObject(settings);
        }

        public bool SaveData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;
            settings.ServiceSMTP = checkSMTP.Checked;
            settings.ServicePOP3 = checkPOP3.Checked;
            settings.ServiceIMAP = checkIMAP.Checked;

            DirtyChecker.SetClean(this);

            Marshal.ReleaseComObject(settings);

            return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }
    }
}
