// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucProtocolSMTP : UserControl, ISettingsControl
    {
        public ucProtocolSMTP()
        {
            InitializeComponent();

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

            comboConnectionSecurity.AddItems(ConnectionSecurityTypes.Get(true));
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
            hMailServer.Settings settings = APICreator.Application.Settings;

            textMaxSMTPConnections.Number = settings.MaxSMTPConnections;
            textWelcomeMessage.Text = settings.WelcomeSMTP;
            textMaxMessageSize.Number = settings.MaxMessageSize;

            textNoOfRetries.Number = settings.SMTPNoOfTries;
            textMinutesBetween.Number = settings.SMTPMinutesBetweenTry;
            chkSmtpDeliveryConnectionSecurity.Checked = settings.SMTPConnectionSecurity ==
                                                       eConnectionSecurity.eCSSTARTTLSOptional;

            textHostName.Text = settings.HostName;
            textSMTPRelayer.Text = settings.SMTPRelayer;
            textSMTPRelayerPort.Number = settings.SMTPRelayerPort;
            chkSMTPRelayerRequiresAuth.Checked = settings.SMTPRelayerRequiresAuthentication;
            textSMTPRelayerUsername.Text = settings.SMTPRelayerUsername;
            comboConnectionSecurity.SelectedValue = settings.SMTPRelayerConnectionSecurity;

            checkAllowPlainTextAuthentication.Checked = settings.AllowSMTPAuthPlain;
            checkAllowMailFromNull.Checked = !settings.DenyMailFromNull;
            checkAllowIncorrectLineEndings.Checked = settings.AllowIncorrectLineEndings;
            checkDisconnectInvalidClients.Checked = settings.DisconnectInvalidClients;
            textMaxNumberOfInvalidCommands.Number = settings.MaxNumberOfInvalidCommands;

            textBindToLocalIP.Text = settings.SMTPDeliveryBindToIP;
            textMaxSMTPRecipientsInBatch.Number = settings.MaxSMTPRecipientsInBatch;
            checkAddDeliveredToHeader.Checked = settings.AddDeliveredToHeader;
            textRuleLoopLimit.Number = settings.RuleLoopLimit;
            textMaxNumberOfMXHosts.Number = settings.MaxNumberOfMXHosts;

            EnableDisable();

            Marshal.ReleaseComObject(settings);

        }

        public bool SaveData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;

            settings.MaxSMTPConnections = textMaxSMTPConnections.Number;
            settings.WelcomeSMTP = textWelcomeMessage.Text;
            settings.MaxMessageSize = textMaxMessageSize.Number;

            settings.SMTPNoOfTries = textNoOfRetries.Number;
            settings.SMTPMinutesBetweenTry = textMinutesBetween.Number;
            settings.SMTPConnectionSecurity =  chkSmtpDeliveryConnectionSecurity.Checked ? eConnectionSecurity.eCSSTARTTLSOptional : eConnectionSecurity.eCSNone;
           
            settings.HostName = textHostName.Text;
            settings.SMTPRelayer = textSMTPRelayer.Text;
            settings.SMTPRelayerPort = textSMTPRelayerPort.Number;
            settings.SMTPRelayerRequiresAuthentication = chkSMTPRelayerRequiresAuth.Checked;
            settings.SMTPRelayerUsername = textSMTPRelayerUsername.Text;
            settings.SMTPRelayerConnectionSecurity = (eConnectionSecurity)comboConnectionSecurity.SelectedValue;

            if (textSMTPRelayerPassword.Dirty)
                settings.SetSMTPRelayerPassword(textSMTPRelayerPassword.Password);

            settings.AllowSMTPAuthPlain = checkAllowPlainTextAuthentication.Checked;
            settings.DenyMailFromNull = !checkAllowMailFromNull.Checked;
            settings.AllowIncorrectLineEndings = checkAllowIncorrectLineEndings.Checked;
            settings.DisconnectInvalidClients = checkDisconnectInvalidClients.Checked;
            settings.MaxNumberOfInvalidCommands = textMaxNumberOfInvalidCommands.Number;

            settings.SMTPDeliveryBindToIP = textBindToLocalIP.Text;
            settings.MaxSMTPRecipientsInBatch = textMaxSMTPRecipientsInBatch.Number;
            settings.AddDeliveredToHeader = checkAddDeliveredToHeader.Checked;
            settings.RuleLoopLimit = textRuleLoopLimit.Number;
            settings.MaxNumberOfMXHosts = textMaxNumberOfMXHosts.Number;

            DirtyChecker.SetClean(this);

            Marshal.ReleaseComObject(settings);

            return true;
        }

        private void EnableDisable()
        {
            textSMTPRelayerUsername.Enabled = chkSMTPRelayerRequiresAuth.Checked;
            textSMTPRelayerPassword.Enabled = chkSMTPRelayerRequiresAuth.Checked;

            textMaxNumberOfInvalidCommands.Enabled = checkDisconnectInvalidClients.Checked;
        }

        public void LoadResources()
        {
            // load the translated resources
        }

        private void chkSMTPRelayerRequiresAuth_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void checkDisconnectInvalidClients_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }
    }
}
