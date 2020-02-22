// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Dialogs;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucAntiVirus : UserControl, ISettingsControl
    {
        public ucAntiVirus()
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
                return DirtyChecker.IsDirty(this) &&
                      (!checkClamAVEnabled.Checked || (textClamAVHostName.Text.Length > 0 && textClamAVPort.Number > 0));
            }
        }

        public void LoadData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;
            hMailServer.AntiVirus antiVirusSettings = settings.AntiVirus;

            radioDeleteEmail.Checked = antiVirusSettings.Action == eAntivirusAction.hDeleteEmail;
            radioDeleteAttachments.Checked = antiVirusSettings.Action == eAntivirusAction.hDeleteAttachments;
            checkNotifySender.Checked = antiVirusSettings.NotifySender;
            checkNotifyReceiver.Checked = antiVirusSettings.NotifyReceiver;
            textVirusScanMaxSize.Number = antiVirusSettings.MaximumMessageSize;

            checkUseClamWin.Checked = antiVirusSettings.ClamWinEnabled;
            textClamScanExecutable.Text = antiVirusSettings.ClamWinExecutable;
            textClamScanDatabase.Text = antiVirusSettings.ClamWinDBFolder;

            checkUseCustomScanner.Checked = antiVirusSettings.CustomScannerEnabled;
            textCustomScannerExecutable.Text = antiVirusSettings.CustomScannerExecutable;
            textCustomScannerReturnValue.Number = antiVirusSettings.CustomScannerReturnValue;

            checkBlockAttachmentsEnabled.Checked = antiVirusSettings.EnableAttachmentBlocking;

            textClamAVHostName.Text = antiVirusSettings.ClamAVHost;
            textClamAVPort.Number = antiVirusSettings.ClamAVPort;
            checkClamAVEnabled.Checked = antiVirusSettings.ClamAVEnabled;

            ListBlockedAttachments();

            EnableDisable();

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(antiVirusSettings);

        }

        private void ListBlockedAttachments()
        {
            listBlockedAttachments.Items.Clear();

            hMailServer.Settings settings = APICreator.Application.Settings;

            hMailServer.AntiVirus antiVirusSettings = settings.AntiVirus;
            hMailServer.BlockedAttachments attachments = antiVirusSettings.BlockedAttachments;

            for (int i = 0; i < attachments.Count; i++)
            {
                hMailServer.BlockedAttachment attachment = attachments[i];

                ListViewItem item = listBlockedAttachments.Items.Add(attachment.Wildcard);
                item.SubItems.Add(attachment.Description);
                item.Tag = attachment.ID;

                Marshal.ReleaseComObject(attachment);
            }

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(antiVirusSettings);
            Marshal.ReleaseComObject(attachments);
        }


        public bool SaveData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;
            hMailServer.AntiVirus antiVirusSettings = settings.AntiVirus;

            antiVirusSettings.Action = radioDeleteEmail.Checked ? eAntivirusAction.hDeleteEmail : eAntivirusAction.hDeleteAttachments;

            antiVirusSettings.NotifySender = checkNotifySender.Checked;
            antiVirusSettings.NotifyReceiver = checkNotifyReceiver.Checked;
            antiVirusSettings.MaximumMessageSize = textVirusScanMaxSize.Number;

            antiVirusSettings.ClamWinEnabled = checkUseClamWin.Checked;
            antiVirusSettings.ClamWinExecutable = textClamScanExecutable.Text;
            antiVirusSettings.ClamWinDBFolder = textClamScanDatabase.Text;

            antiVirusSettings.CustomScannerEnabled = checkUseCustomScanner.Checked;
            antiVirusSettings.CustomScannerExecutable = textCustomScannerExecutable.Text;
            antiVirusSettings.CustomScannerReturnValue = textCustomScannerReturnValue.Number;

            antiVirusSettings.EnableAttachmentBlocking = checkBlockAttachmentsEnabled.Checked;

            antiVirusSettings.ClamAVHost = textClamAVHostName.Text;
            antiVirusSettings.ClamAVPort = textClamAVPort.Number;
            antiVirusSettings.ClamAVEnabled = checkClamAVEnabled.Checked;

            DirtyChecker.SetClean(this);

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(antiVirusSettings);

            return true;

        }

        public void LoadResources()
        {
            // load the translated resources
        }

        private void buttonAddBlockedAttachment_Click(object sender, EventArgs e)
        {
            formBlockedAttachment blockedDlg = new formBlockedAttachment();

            if (blockedDlg.ShowDialog() == DialogResult.OK)
            {
                hMailServer.Settings settings = APICreator.Application.Settings;
                hMailServer.AntiVirus antiVirusSettings = settings.AntiVirus;
                hMailServer.BlockedAttachments attachments = antiVirusSettings.BlockedAttachments;

                hMailServer.BlockedAttachment ba = attachments.Add();
                blockedDlg.SaveProperties(ba);
                ba.Save();

                Marshal.ReleaseComObject(ba);
                Marshal.ReleaseComObject(settings);
                Marshal.ReleaseComObject(antiVirusSettings);
                Marshal.ReleaseComObject(attachments);
            }

            ListBlockedAttachments();
        }

        private void buttonEditBlockedAttachment_Click(object sender, EventArgs e)
        {
            EditSelectedBlockedAttachment();
        }

        private void EditSelectedBlockedAttachment()
        {
            if (listBlockedAttachments.SelectedItems.Count < 0)
                return;

            int id = Convert.ToInt32(listBlockedAttachments.SelectedItems[0].Tag);

            hMailServer.Settings settings = APICreator.Settings;
            hMailServer.AntiVirus antiVirusSettings = settings.AntiVirus;
            hMailServer.BlockedAttachments attachments = antiVirusSettings.BlockedAttachments;
            hMailServer.BlockedAttachment ba = attachments.get_ItemByDBID(id);

            formBlockedAttachment blockedDlg = new formBlockedAttachment();

            blockedDlg.LoadProperties(ba);

            if (blockedDlg.ShowDialog() == DialogResult.OK)
            {
                blockedDlg.SaveProperties(ba);
                ba.Save();
            }

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(antiVirusSettings);
            Marshal.ReleaseComObject(attachments);
            Marshal.ReleaseComObject(ba);

            ListBlockedAttachments();
        }

        private void buttonDeleteBlockedAttachment_Click(object sender, EventArgs e)
        {
           if (!Utility.AskDeleteItems())
              return;
 
           hMailServer.Settings settings = APICreator.Settings;
            hMailServer.AntiVirus antiVirusSettings = settings.AntiVirus;
            hMailServer.BlockedAttachments attachments = antiVirusSettings.BlockedAttachments;

            List<ListViewItem> itemsToRemove = new List<ListViewItem>();

            foreach (ListViewItem item in listBlockedAttachments.SelectedItems)
            {
                int id = Convert.ToInt32(item.Tag);
                attachments.DeleteByDBID(id);
                itemsToRemove.Add(item);
            }

            foreach (ListViewItem item in itemsToRemove)
            {
                listBlockedAttachments.Items.Remove(item);
            }

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(antiVirusSettings);
            Marshal.ReleaseComObject(attachments);
        }

        private void radioDeleteEmail_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void EnableDisable()
        {
            checkNotifySender.Enabled = radioDeleteEmail.Checked;
            checkNotifyReceiver.Enabled = radioDeleteEmail.Checked;

            textClamScanExecutable.Enabled = checkUseClamWin.Checked;
            textClamScanDatabase.Enabled = checkUseClamWin.Checked;
            buttonAutoDetect.Enabled = checkUseClamWin.Checked;
            buttonClamWinTest.Enabled = checkUseClamWin.Checked;

            textCustomScannerExecutable.Enabled = checkUseCustomScanner.Checked;
            textCustomScannerReturnValue.Enabled = checkUseCustomScanner.Checked;
            buttonCustomScannerTest.Enabled = checkUseCustomScanner.Checked;

            textClamAVHostName.Enabled = checkClamAVEnabled.Checked;
            textClamAVPort.Enabled = checkClamAVEnabled.Checked;
            buttonClamAVTest.Enabled = checkClamAVEnabled.Checked;

            buttonEditBlockedAttachment.Enabled = listBlockedAttachments.SelectedItems.Count == 1;
            buttonDeleteBlockedAttachment.Enabled = listBlockedAttachments.SelectedItems.Count > 0;
        }

        private void buttonAutoDetect_Click(object sender, EventArgs e)
        {
            string executable;
            string database;
            if (Utility.DetectClamWinPaths(out executable, out database))
            {
                textClamScanDatabase.Text = database;
                textClamScanExecutable.Text = executable;

                string warning = "hMailServer Administrator was able to auto-detect the location of ClamScan and the database.\r\nIt is still recommended you verify that the path to the executable and the database is correct.";
                MessageBox.Show(warning, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                string warning = "hMailServer Administrator was unable to auto-detect the location of ClamWin.";
                MessageBox.Show(warning, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }


        }

        private void checkUseClamWin_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void checkUseCustomScanner_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void listBlockedAttachments_SelectedIndexChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void listBlockedAttachments_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            EditSelectedBlockedAttachment();
        }

        private void buttonClamAVTest_Click(object sender, EventArgs e)
        {
            using (new WaitCursor())
            {
                hMailServer.Settings settings = APICreator.Application.Settings;
                hMailServer.AntiVirus antiVirusSettings = settings.AntiVirus;

                string messageText = "";
                bool testPass = antiVirusSettings.TestClamAVScanner(textClamAVHostName.Text, textClamAVPort.Number, out messageText);

                Marshal.ReleaseComObject(antiVirusSettings);
                Marshal.ReleaseComObject(settings);

                ProcessVirusTestResult(testPass, messageText);
            }
        }

        private void ProcessVirusTestResult(bool found, string messageText)
        {
            if (found)
            {
                string message = Strings.Localize("Test virus was detected successfully. Virus name: ");
                messageText = message + messageText;
                MessageBox.Show(messageText, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            else
            {
                string message = Strings.Localize("Virus detection failed. Reason: ");
                messageText = message + messageText;
                MessageBox.Show(messageText, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        
        private void buttonClamWinTest_Click(object sender, EventArgs e)
        {
            using (new WaitCursor())
            {
                hMailServer.Settings settings = APICreator.Application.Settings;
                hMailServer.AntiVirus antiVirusSettings = settings.AntiVirus;

                string messageText = "";
                bool testPass = antiVirusSettings.TestClamWinScanner(textClamScanExecutable.Text, textClamScanDatabase.Text, out messageText);

                Marshal.ReleaseComObject(antiVirusSettings);
                Marshal.ReleaseComObject(settings);

                ProcessVirusTestResult(testPass, messageText);
            }
        }

        private void buttonCustomScannerTest_Click(object sender, EventArgs e)
        {
            using (new WaitCursor())
            {
                hMailServer.Settings settings = APICreator.Application.Settings;
                hMailServer.AntiVirus antiVirusSettings = settings.AntiVirus;

                string messageText = "";
                bool testPass = antiVirusSettings.TestCustomerScanner(textCustomScannerExecutable.Text, textCustomScannerReturnValue.Number, out messageText);

                Marshal.ReleaseComObject(antiVirusSettings);
                Marshal.ReleaseComObject(settings);

                ProcessVirusTestResult(testPass, messageText);
            }
        }

        private void checkClamAVEnabled_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }
    }
}
