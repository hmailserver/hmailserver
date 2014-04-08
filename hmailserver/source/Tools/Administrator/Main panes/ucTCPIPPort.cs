// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Utilities.Localization;
using Microsoft.VisualBasic;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucTCPIPPort : UserControl, ISettingsControl
    {
        private hMailServer.TCPIPPort _representedObject;

        public ucTCPIPPort(int portID)
        {
            InitializeComponent();

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            if (portID > 0)
            {
                hMailServer.TCPIPPorts ports = APICreator.TCPIPPortsSettings;
                _representedObject = ports.get_ItemByDBID(portID);
                Marshal.ReleaseComObject(ports);
            }

            comboProtocol.AddItem("SMTP", eSessionType.eSTSMTP);
            comboProtocol.AddItem("POP3", eSessionType.eSTPOP3);
            comboProtocol.AddItem("IMAP", eSessionType.eSTIMAP);

            ListSSLCertificates();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

            EnableDisable();
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
            if (_representedObject == null)
                return;

            textIPAddress.Text = _representedObject.Address;
            textTCPIPPort.Number = _representedObject.PortNumber;
            checkEnableSSL.Checked = _representedObject.UseSSL;
            comboProtocol.SelectedValue = _representedObject.Protocol;
            comboSSLCertificate.SelectedValue = _representedObject.SSLCertificateID;

            EnableDisable();
        }

        private void ListSSLCertificates()
        {
            comboSSLCertificate.Items.Clear();

            hMailServer.Settings settings = APICreator.Settings;
            hMailServer.SSLCertificates sslCertificates = settings.SSLCertificates;

            for (int i = 0; i < sslCertificates.Count; i++)
            {
                hMailServer.SSLCertificate certificate = sslCertificates[i];

                comboSSLCertificate.AddItem(certificate.Name, certificate.ID);

                Marshal.ReleaseComObject(certificate);
            }

            Marshal.ReleaseComObject(settings);
            Marshal.ReleaseComObject(sslCertificates);



        }

        public bool SaveData()
        {
            if (_representedObject == null)
            {
                hMailServer.TCPIPPorts tcpIPPorts = APICreator.TCPIPPortsSettings;
                _representedObject = tcpIPPorts.Add();
                Marshal.ReleaseComObject(tcpIPPorts);
            }

            _representedObject.Address = textIPAddress.Text;
            _representedObject.PortNumber = textTCPIPPort.Number;
            _representedObject.UseSSL = checkEnableSSL.Checked;
            _representedObject.Protocol = (hMailServer.eSessionType)comboProtocol.SelectedValue;

            if (comboSSLCertificate.SelectedValue == null)
                _representedObject.SSLCertificateID = 0;
            else
                _representedObject.SSLCertificateID = (int)comboSSLCertificate.SelectedValue;

            _representedObject.Save();

            DirtyChecker.SetClean(this);

            Utility.RefreshNode(InternalNames.GetPortName(_representedObject));

            if (MessageBox.Show(Strings.Localize("hMailServer needs to be restarted for the changes to take effect.") + Environment.NewLine +
                                Strings.Localize("Do you want to restart hMailServer now?"), EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNo,MessageBoxIcon.Question) == DialogResult.Yes)
            {
               using (new WaitCursor())
               {
                  hMailServer.Application application = APICreator.Application;
                  application.Stop();
                  application.Start();

                  MessageBox.Show(Strings.Localize("The hMailServer server has been restarted."), EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Information);
               }
            }

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

        private void checkEnableSSL_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void EnableDisable()
        {
            comboSSLCertificate.Enabled = checkEnableSSL.Checked;
        }

    }
}

