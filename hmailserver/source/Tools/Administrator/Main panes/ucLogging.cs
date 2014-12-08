// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucLogging : UserControl, ISettingsControl
    {
        public ucLogging()
        {
            InitializeComponent();

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        public void OnLeavePage()
        {

        }

        private void OnContentChanged()
        {
            Instances.MainForm.OnContentChanged();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
            OnContentChanged();
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
            hMailServer.Settings settings = APICreator.Application.Settings;
            hMailServer.Logging logging = settings.Logging;

            checkEnableLogging.Checked = logging.Enabled;
            chkLogApplication.Checked = logging.LogApplication;
            checkLogSMTP.Checked = logging.LogSMTP;
            checkLogPOP3.Checked = logging.LogPOP3;
            checkLogIMAP.Checked = logging.LogIMAP;
            checkLogTCPIP.Checked = logging.LogTCPIP;
            checkLogDebug.Checked = logging.LogDebug;
            checkLogAWStats.Checked = logging.AWStatsEnabled;
            checkKeepFilesOpen.Checked = logging.KeepFilesOpen;

            EnableDisable();

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(logging);
        }

        private void EnableDisable()
        {
            chkLogApplication.Enabled = checkEnableLogging.Checked;
            checkLogSMTP.Enabled = checkEnableLogging.Checked;
            checkLogPOP3.Enabled = checkEnableLogging.Checked;
            checkLogIMAP.Enabled = checkEnableLogging.Checked;
            checkLogTCPIP.Enabled = checkEnableLogging.Checked;
            checkLogDebug.Enabled = checkEnableLogging.Checked;
            checkLogAWStats.Enabled = checkEnableLogging.Checked;
            checkKeepFilesOpen.Enabled = checkEnableLogging.Checked;
        }

        public bool SaveData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;
            hMailServer.Logging logging = settings.Logging;

            logging.Enabled = checkEnableLogging.Checked;
            logging.LogApplication = chkLogApplication.Checked;
            logging.LogSMTP = checkLogSMTP.Checked;
            logging.LogPOP3 = checkLogPOP3.Checked;
            logging.LogIMAP = checkLogIMAP.Checked;
            logging.LogTCPIP = checkLogTCPIP.Checked;
            logging.LogDebug = checkLogDebug.Checked;
            logging.AWStatsEnabled = checkLogAWStats.Checked;
            logging.KeepFilesOpen = checkKeepFilesOpen.Checked;
            DirtyChecker.SetClean(this);

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(logging);

            return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }

        private void buttonShowLogs_Click(object sender, EventArgs e)
        {
            hMailServer.Settings settings = APICreator.Application.Settings;

            System.Diagnostics.ProcessStartInfo explorer = new System.Diagnostics.ProcessStartInfo();
            explorer.FileName = "explorer.exe";
            explorer.Arguments = settings.Logging.Directory;
            System.Diagnostics.Process.Start(explorer);

            Marshal.ReleaseComObject(settings);
        }

        private void checkEnableLogging_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }
    }
}
