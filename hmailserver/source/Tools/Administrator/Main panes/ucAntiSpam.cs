// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using System.IO;
using hMailServer.Administrator.Dialogs;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucAntiSpam : UserControl, ISettingsControl
    {
        public ucAntiSpam()
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
            hMailServer.Application app = APICreator.Application;

            hMailServer.Settings settings = app.Settings;
            hMailServer.AntiSpam antiSpam = settings.AntiSpam;

            textSpamMarkThreshold.Number = antiSpam.SpamMarkThreshold;
            textSpamDeleteThreshold.Number = antiSpam.SpamDeleteThreshold;
            checkAddHeaderSpam.Checked = antiSpam.AddHeaderSpam;
            checkAddHeaderReason.Checked = antiSpam.AddHeaderReason;
            checkPrependSubject.Checked = antiSpam.PrependSubject;
            textPrependSubjectText.Text = antiSpam.PrependSubjectText;

            chkUseSPF.Checked = antiSpam.UseSPF;
            textUseSPFScore.Number = antiSpam.UseSPFScore;
            chkCheckHostInHelo.Checked = antiSpam.CheckHostInHelo;
            textCheckHostInHeloScore.Number = antiSpam.CheckHostInHeloScore;
            chkCheckPTR.Checked = antiSpam.CheckPTR;
            textCheckPTRScore.Number = antiSpam.CheckPTRScore;
            chkUseMXChecks.Checked = antiSpam.UseMXChecks;
            textUseMXChecksScore.Number = antiSpam.UseMXChecksScore;

            checkSpamAssassinEnabled.Checked = antiSpam.SpamAssassinEnabled;
            textSpamAssassinHost.Text = antiSpam.SpamAssassinHost;
            textSpamAssassinPort.Number = antiSpam.SpamAssassinPort;
            checkSpamAssassinMergeScore.Checked = antiSpam.SpamAssassinMergeScore;
            textSpamAssassinScore.Number = antiSpam.SpamAssassinScore;

            textMaximumMessageSize.Number = antiSpam.MaximumMessageSize;

            checkDKIMVerificationEnabled.Checked = antiSpam.DKIMVerificationEnabled;
            textDKIMVerificationFailureScore.Number = antiSpam.DKIMVerificationFailureScore;

            EnableDisable();

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(antiSpam);
        }

        public bool SaveData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;
            hMailServer.AntiSpam antiSpam = settings.AntiSpam;

            antiSpam.SpamMarkThreshold = textSpamMarkThreshold.Number;
            antiSpam.SpamDeleteThreshold = textSpamDeleteThreshold.Number;
            antiSpam.AddHeaderSpam = checkAddHeaderSpam.Checked;
            antiSpam.AddHeaderReason = checkAddHeaderReason.Checked;
            antiSpam.PrependSubject = checkPrependSubject.Checked;
            antiSpam.PrependSubjectText = textPrependSubjectText.Text;

            antiSpam.UseSPF = chkUseSPF.Checked;
            antiSpam.UseSPFScore = textUseSPFScore.Number;
            antiSpam.CheckHostInHelo = chkCheckHostInHelo.Checked;
            antiSpam.CheckHostInHeloScore = textCheckHostInHeloScore.Number;
            antiSpam.CheckPTR = chkCheckPTR.Checked;
            antiSpam.CheckPTRScore = textCheckPTRScore.Number;
            antiSpam.UseMXChecks = chkUseMXChecks.Checked;
            antiSpam.UseMXChecksScore = textUseMXChecksScore.Number;

            antiSpam.SpamAssassinEnabled = checkSpamAssassinEnabled.Checked;
            antiSpam.SpamAssassinHost = textSpamAssassinHost.Text;
            antiSpam.SpamAssassinPort = textSpamAssassinPort.Number;
            antiSpam.SpamAssassinMergeScore = checkSpamAssassinMergeScore.Checked;
            antiSpam.SpamAssassinScore = textSpamAssassinScore.Number;

            antiSpam.DKIMVerificationEnabled = checkDKIMVerificationEnabled.Checked;
            antiSpam.DKIMVerificationFailureScore = textDKIMVerificationFailureScore.Number;

            antiSpam.MaximumMessageSize = textMaximumMessageSize.Number;

            DirtyChecker.SetClean(this);

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(antiSpam);

            return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }

        private void label2_Click(object sender, EventArgs e)
        {

        }

        private void ucText2_TextChanged(object sender, EventArgs e)
        {

        }

        private void checkPrependSubject_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void EnableDisable()
        {
            textPrependSubjectText.Enabled = checkPrependSubject.Checked;

            textSpamAssassinHost.Enabled = checkSpamAssassinEnabled.Checked;
            textSpamAssassinPort.Enabled = checkSpamAssassinEnabled.Checked;
            checkSpamAssassinMergeScore.Enabled = checkSpamAssassinEnabled.Checked;
            textSpamAssassinScore.Enabled = checkSpamAssassinEnabled.Checked && !checkSpamAssassinMergeScore.Checked;
            buttonTest.Enabled = checkSpamAssassinEnabled.Checked;

            textUseSPFScore.Enabled = chkUseSPF.Checked;
            textCheckHostInHeloScore.Enabled = chkCheckHostInHelo.Checked;
            textCheckPTRScore.Enabled = chkCheckPTR.Checked;
            textUseMXChecksScore.Enabled = chkUseMXChecks.Checked;

            textDKIMVerificationFailureScore.Enabled = checkDKIMVerificationEnabled.Checked;

        }

        private void checkSpamAssassinMergeScore_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();


        }

        private void checkSpamAssassinEnabled_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void tabPage2_Click(object sender, EventArgs e)
        {

        }

        private void chkUseSPF_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void chkCheckHostInHelo_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();

        }

        private void chkCheckPTR_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();

        }

        private void chkUseMXChecks_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void textSpamAssassinScore_TextChanged(object sender, EventArgs e)
        {

        }

        private void checkDKIMVerificationEnabled_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void buttonTest_Click(object sender, EventArgs e)
        {
           hMailServer.AntiSpam antiSpam = APICreator.AntiSpamSettings;

           string messageText = "";
           bool testPass = antiSpam.TestSpamAssassinConnection(textSpamAssassinHost.Text, textSpamAssassinPort.Number, out messageText);

           Marshal.ReleaseComObject(antiSpam);

           if (testPass)
           {
              string tempFile = Path.GetTempFileName();
              File.WriteAllText(tempFile, messageText);
              formMessageViewer viewer = new formMessageViewer(tempFile);
              viewer.ShowDialog();
              File.Delete(tempFile);
           }
           else
           {
              MessageBox.Show(messageText, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Error);
           }

           
        }
    }
}
