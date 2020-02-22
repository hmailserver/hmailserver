// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucSSLTLS : UserControl, ISettingsControl
    {
       public ucSSLTLS()
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

        public void LoadData()
        {
           hMailServer.Application app = APICreator.Application;

           hMailServer.Settings settings = app.Settings;

           checkVerifyRemoteServerSslCertificate.Checked = settings.VerifyRemoteSslCertificate;
           textSslCipherList.Text = settings.SslCipherList;
           checkTlsVersion10.Checked = settings.TlsVersion10Enabled;
           checkTlsVersion11.Checked = settings.TlsVersion11Enabled;
           checkTlsVersion12.Checked = settings.TlsVersion12Enabled;
           checkTlsVersion13.Checked = settings.TlsVersion13Enabled;

           Marshal.ReleaseComObject(settings);
        }

        public bool SaveData()
        {
           hMailServer.Application app = APICreator.Application;

           hMailServer.Settings settings = app.Settings;

           bool restartRequired = textSslCipherList.Dirty || checkTlsVersion10.Dirty || checkTlsVersion11.Dirty || checkTlsVersion12.Dirty || checkTlsVersion13.Dirty;

           settings.VerifyRemoteSslCertificate = checkVerifyRemoteServerSslCertificate.Checked;
           settings.SslCipherList = textSslCipherList.Text;

           settings.TlsVersion10Enabled = checkTlsVersion10.Checked;
           settings.TlsVersion11Enabled = checkTlsVersion11.Checked;
           settings.TlsVersion12Enabled = checkTlsVersion12.Checked;
           settings.TlsVersion13Enabled = checkTlsVersion13.Checked;

           Marshal.ReleaseComObject(settings);

           if (restartRequired)
              Utility.AskRestartServer();

           DirtyChecker.SetClean(this);
           return true;

       }

        public void LoadResources()
        {
            // load the translated resources
        }


        private void OnContentChanged()
        {
           Instances.MainForm.OnContentChanged();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
           OnContentChanged();
        }
    }
}
