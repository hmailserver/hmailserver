// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Drawing;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator.Dialogs
{
   public partial class formExternalAccount : Form
   {
      private hMailServer.FetchAccount _fetchAccount = null;
      private Timer _timer;
      private bool _isLoading = false;

      public formExternalAccount()
      {
         InitializeComponent();

         comboServerType.AddItem("POP3", 1);
         comboServerType.SelectedIndex = 0;

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
         Strings.Localize(this);

         textPort.Number = 110;
         textMIMERecipientHeaders.Text = "To,CC,X-RCPT-TO,X-Envelope-To";

         DirtyChecker.SubscribeToChange(this, OnContentChanged);
         
         buttonDownloadNow.Enabled = false;
         
         comboConnectionSecurity.AddItems(ConnectionSecurityTypes.Get(false));
         comboConnectionSecurity.SelectedIndex = 0;

         EnableDisable();
      }

      private void OnContentChanged(object sender, EventArgs e)
      {
         
         EnableDisable();
      }

      private void EnableDisable()
      {
         btnOK.Enabled = DirtyChecker.IsDirty(this) && textName.Text.Length > 0;
         checkProcessMIMERecipients.Enabled = textMIMERecipientHeaders.Text.Length > 0;
         checkEnableRouteRecipients.Enabled = checkProcessMIMERecipients.Enabled && checkProcessMIMERecipients.Checked;
      }


      public void LoadAccountProperties(hMailServer.FetchAccount fetchAccount)
      {
         _isLoading = true;

         _fetchAccount = fetchAccount;

         checkEnabled.Checked = fetchAccount.Enabled;
         textName.Text = fetchAccount.Name;
         textMinutesBetweenFetch.Number = fetchAccount.MinutesBetweenFetch;
         comboServerType.SelectedValue = fetchAccount.ServerType;
         textServer.Text = fetchAccount.ServerAddress;
         textPort.Number = fetchAccount.Port;
         textUsername.Text = fetchAccount.Username;
         comboConnectionSecurity.SelectedValue = fetchAccount.ConnectionSecurity;

         textMIMERecipientHeaders.Text = !string.IsNullOrEmpty(fetchAccount.MIMERecipientHeaders) ? fetchAccount.MIMERecipientHeaders : "To,CC,X-RCPT-TO,X-Envelope-To";
         checkProcessMIMERecipients.Checked = fetchAccount.ProcessMIMERecipients && !string.IsNullOrEmpty(fetchAccount.MIMERecipientHeaders);
         checkProcessMIMEDate.Checked = fetchAccount.ProcessMIMEDate;

         checkUseAntiSpam.Checked = fetchAccount.UseAntiSpam;
         checkUseAntiVirus.Checked = fetchAccount.UseAntiVirus;

         checkEnableRouteRecipients.Checked = fetchAccount.EnableRouteRecipients;

         if (fetchAccount.DaysToKeepMessages == -1)
            radioDeleteImmediately.Checked = true;
         else if (fetchAccount.DaysToKeepMessages == 0)
            radioNeverDeleteMessages.Checked = true;
         else
         {
            radioDeleteMessagesAfter.Checked = true;
            textDaysToKeepMessages.Number = fetchAccount.DaysToKeepMessages;
         }

         buttonDownloadNow.Enabled = true;

         _isLoading = false;

         EnableDisable();
      }

      public void SaveAccountProperties(hMailServer.FetchAccount fetchAccount)
      {
         fetchAccount.DaysToKeepMessages = SettingDaysToKeepMessages;
         fetchAccount.Enabled = checkEnabled.Checked;
         fetchAccount.MinutesBetweenFetch = textMinutesBetweenFetch.Number;
         fetchAccount.Name = textName.Text;
         fetchAccount.Port = textPort.Number;
         fetchAccount.MIMERecipientHeaders = !string.IsNullOrEmpty(textMIMERecipientHeaders.Text) ? textMIMERecipientHeaders.Text : "To,CC,X-RCPT-TO,X-Envelope-To";
         fetchAccount.ProcessMIMEDate = checkProcessMIMEDate.Checked;
         fetchAccount.ProcessMIMERecipients = checkProcessMIMERecipients.Checked && checkProcessMIMERecipients.Enabled;
         fetchAccount.ServerAddress = textServer.Text;
         fetchAccount.ServerType = (int) comboServerType.SelectedValue;
         fetchAccount.Username = textUsername.Text;
         fetchAccount.ConnectionSecurity = (eConnectionSecurity) comboConnectionSecurity.SelectedValue;
         fetchAccount.UseAntiSpam = checkUseAntiSpam.Checked;
         fetchAccount.UseAntiVirus = checkUseAntiVirus.Checked;
         fetchAccount.EnableRouteRecipients = checkEnableRouteRecipients.Checked && checkEnableRouteRecipients.Enabled;

         if (textPassword.Dirty)
            fetchAccount.Password = textPassword.Password;

      }

      private int SettingDaysToKeepMessages
      {
         get
         {
            if (radioDeleteImmediately.Checked)
               return -1;

            if (radioDeleteMessagesAfter.Checked)
               return textDaysToKeepMessages.Number;

            if (radioNeverDeleteMessages.Checked)
               return 0;

            return 0;
         }
      }

      private void buttonDownloadNow_Click(object sender, EventArgs e)
      {
          // save properties prior to downloading, so we have
          // the latest settings.
          if (_fetchAccount != null)
          {
              SaveAccountProperties(_fetchAccount);
              _fetchAccount.Save();
              _fetchAccount.DownloadNow();
              this.Close();
          }
      }

      private void checkProcessMIMEDate_CheckedChanged(object sender, EventArgs e)
      {

      }

      private void formExternalAccount_Load(object sender, EventArgs e)
      {

      }

      private void checkProcessMIMERecipients_CheckedChanged(object sender, EventArgs e)
      {
         EnableDisable();
      }

      private void textMIMERecipientHeaders_TextChanged(object sender, EventArgs e)
      {
         EnableDisable();
      }

      private void comboConnectionSecurity_SelectedIndexChanged(object sender, EventArgs e)
      {
          if (_isLoading)
              return;

          if ((eConnectionSecurity)comboConnectionSecurity.SelectedValue == eConnectionSecurity.eCSTLS)
              textPort.Number = 995;
          else
              textPort.Number = 110;

          textPort.Font = new Font(this.Font, FontStyle.Bold);

          if (_timer != null)
              _timer.Stop();

          _timer = new Timer();
          _timer.Interval = 3000;
          _timer.Tick += (s, ev) =>
          {
              textPort.Font = new Font(this.Font, FontStyle.Regular);
              _timer.Stop();
          };
          _timer.Start();
      }
   }
}