// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucRoute : UserControl, ISettingsControl
    {
        private hMailServer.Route _representedObject;

        public ucRoute(int routeID)
        {
            InitializeComponent();

            if (routeID > 0)
            {
                hMailServer.Routes routes = APICreator.Routes;
                _representedObject = routes.get_ItemByDBID(routeID);
                Marshal.ReleaseComObject(routes);
            }

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

            comboConnectionSecurity.AddItems(ConnectionSecurityTypes.Get(true));
            comboConnectionSecurity.SelectedIndex = 0;

            EnableDisable();
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
                       textDomainName.Text.Length > 0 &&
                       textTargetSMTPHost.Text.Length > 0;
            }
        }

        public void LoadData()
        {
            if (_representedObject == null)
                return;

            textDomainName.Text = _representedObject.DomainName;
            textDescription.Text = _representedObject.Description;
            textTargetSMTPHost.Text = _representedObject.TargetSMTPHost;
            textTargetSMTPPort.Number = _representedObject.TargetSMTPPort;

            comboConnectionSecurity.SelectedValue = _representedObject.ConnectionSecurity;

            radioTreatSenderAsLocalDomain.Checked = _representedObject.TreatSenderAsLocalDomain;
            radioTreatSenderAsExternalDomain.Checked = !radioTreatSenderAsLocalDomain.Checked;

            radioTreatRecipientAsLocalDomain.Checked = _representedObject.TreatRecipientAsLocalDomain;
            radioTreatRecipientAsExternalDomain.Checked = !radioTreatRecipientAsLocalDomain.Checked;

            textNumberOfTries.Number = _representedObject.NumberOfTries;
            textNumberOfMinutesBetween.Number = _representedObject.MinutesBetweenTry;

            checkServerRequiresAuth.Checked = _representedObject.RelayerRequiresAuth;
            textUsername.Text = _representedObject.RelayerAuthUsername;

            radioRouteForAll.Checked = _representedObject.AllAddresses;
            radioRouteForAddresses.Checked = !_representedObject.AllAddresses;

            ListRecipients();

            EnableDisable();

        }

        public bool SaveData()
        {
            if (_representedObject == null)
            {
                hMailServer.Settings settings = APICreator.Application.Settings;
                hMailServer.Routes routes = settings.Routes;
                _representedObject = routes.Add();

                Marshal.ReleaseComObject(settings);
                Marshal.ReleaseComObject(routes);

            }

            _representedObject.DomainName = textDomainName.Text;
            _representedObject.Description = textDescription.Text;
            _representedObject.TargetSMTPHost = textTargetSMTPHost.Text;
            _representedObject.TargetSMTPPort = textTargetSMTPPort.Number;

            _representedObject.ConnectionSecurity = (eConnectionSecurity) comboConnectionSecurity.SelectedValue;

            _representedObject.TreatSenderAsLocalDomain = radioTreatSenderAsLocalDomain.Checked;
            _representedObject.TreatRecipientAsLocalDomain = radioTreatRecipientAsLocalDomain.Checked;

            _representedObject.NumberOfTries = textNumberOfTries.Number;
            _representedObject.MinutesBetweenTry = textNumberOfMinutesBetween.Number;

            _representedObject.RelayerRequiresAuth = checkServerRequiresAuth.Checked;
            _representedObject.RelayerAuthUsername = textUsername.Text;

           
            if (textPassword.Dirty)
                _representedObject.SetRelayerAuthPassword(textPassword.Password);

            _representedObject.AllAddresses = radioRouteForAll.Checked;

            _representedObject.Save();

            // Set the object to clean.
            DirtyChecker.SetClean(this);

            Utility.RefreshNode(_representedObject.DomainName);

            return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }

        private void buttonAddRecipient_Click(object sender, EventArgs e)
        {
            formInputDialog inputDialog = new formInputDialog();

            inputDialog.Title = "Address";
            inputDialog.Text = "Enter email address";

            if (inputDialog.ShowDialog() == DialogResult.OK)
            {

                hMailServer.RouteAddresses routeAddresses = _representedObject.Addresses;
                hMailServer.RouteAddress routeAddress = routeAddresses.Add();

                routeAddress.Address = inputDialog.Value;
                routeAddress.Save();

                Marshal.ReleaseComObject(routeAddress);
                Marshal.ReleaseComObject(routeAddresses);
            }

            ListRecipients();
        }

        private void ListRecipients()
        {
            listAddresses.Items.Clear();

            hMailServer.RouteAddresses routeAddresses = _representedObject.Addresses;

            for (int i = 0; i < routeAddresses.Count; i++)
            {
                hMailServer.RouteAddress address = routeAddresses[i];

                ListViewItem item = listAddresses.Items.Add(address.Address);
                item.Tag = address;
            }
        }

        private void buttonDeleteWhiteList_Click(object sender, EventArgs e)
        {
            foreach (ListViewItem item in listAddresses.SelectedItems)
            {
                hMailServer.RouteAddress address = item.Tag as hMailServer.RouteAddress;
                address.Delete();
            }

            ListRecipients();
        }

        private void EnableDisable()
        {
            listAddresses.Enabled = _representedObject != null && radioRouteForAddresses.Checked;
            buttonAddRecipient.Enabled = _representedObject != null && radioRouteForAddresses.Checked;
            buttonDeleteWhiteList.Enabled = _representedObject != null && radioRouteForAddresses.Checked;

            textUsername.Enabled = checkServerRequiresAuth.Checked;
            textPassword.Enabled = checkServerRequiresAuth.Checked;

        }

        private void checkServerRequiresAuth_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void radioRouteForAddresses_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void tabPage1_Click(object sender, EventArgs e)
        {

        }


    }
}
