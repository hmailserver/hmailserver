// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Nodes;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucDomain : UserControl, ISettingsControl
    {
        private hMailServer.Domain _domain;
        private hMailServer.Application _application;

        public ucDomain(hMailServer.Domain domain)
        {
            InitializeComponent();

            _domain = domain;

            EnableDisableTabs();

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

            EnableDisable();
        }

        public void OnLeavePage()
        {
            if (_domain != null)
                Marshal.ReleaseComObject(_domain);
        }

        public bool Dirty
        {
            get
            {
                bool dkimSettingsAreOK = !checkDKIMSignEnabled.Checked || (textDKIMPrivateKeyFile.Text.Length > 0 && textDKIMSelector.Text.Length > 0);

                return DirtyChecker.IsDirty(this) &&
                       textDomainName.Text.Length > 0 &&
                       dkimSettingsAreOK;
            }
        }

        private void EnableDisableTabs()
        {
            if (_domain == null)
                tabControl.TabPages[1].Enabled = false;
            else
                tabControl.TabPages[1].Enabled = true;

        }

        public void LoadData()
        {
            hMailServer.Application app = APICreator.Application;

            _application = app;

            if (_domain == null)
                return;

            textDomainName.Text = _domain.Name;
            textCatchAllAddress.Text = _domain.Postmaster;
            checkAddSignaturesToReplies.Checked = _domain.AddSignaturesToReplies;
            checkAddSignaturesToLocalMail.Checked = _domain.AddSignaturesToLocalMail;
            checkSignatureEnabled.Checked = _domain.SignatureEnabled;
            checkEnabled.Checked = _domain.Active;

            switch (_domain.SignatureMethod)
            {
                case eDomainSignatureMethod.eSMAppendToAccountSignature:
                    radioSMAppendToAccountSignature.Checked = true;
                    break;
                case eDomainSignatureMethod.eSMOverwriteAccountSignature:
                    radioSMOverwriteAccountSignature.Checked = true;
                    break;
                case eDomainSignatureMethod.eSMSetIfNotSpecifiedInAccount:
                    radioSMSetIfNotSpecifiedInAccount.Checked = true;
                    break;
            }

            textMaxSize.Number = _domain.MaxSize;
            textAllocatedSize.Number64 = _domain.AllocatedSize;
            textMaxMessageSize.Number = _domain.MaxMessageSize;
            textMaxAccountSize.Number = _domain.MaxAccountSize;
            textMaxNumberOfAccounts.Number = _domain.MaxNumberOfAccounts;
            textMaxNumberOfAliases.Number = _domain.MaxNumberOfAliases;
            textMaxNumberOfDistributionLists.Number = _domain.MaxNumberOfDistributionLists;
            checkMaxNumberOfAccountsEnabled.Checked = _domain.MaxNumberOfAccountsEnabled;
            checkMaxNumberOfAliasesEnabled.Checked = _domain.MaxNumberOfAliasesEnabled;
            checkMaxNumberOfDistributionListsEnabled.Checked = _domain.MaxNumberOfDistributionListsEnabled;

            textPlainTextSignature.Text = _domain.SignaturePlainText;
            textHTMLSignature.Text = _domain.SignatureHTML;

            checkUsePlusAddressing.Checked = _domain.PlusAddressingEnabled;
            comboPlusAddressingCharacter.Text = _domain.PlusAddressingCharacter;
            checkGreyListingEnabled.Checked = _domain.AntiSpamEnableGreylisting;

            checkDKIMSignEnabled.Checked = _domain.DKIMSignEnabled;
            checkDKIMSignAliasesEnabled.Checked = _domain.DKIMSignAliasesEnabled;
            textDKIMPrivateKeyFile.Text = _domain.DKIMPrivateKeyFile;
            textDKIMSelector.Text = _domain.DKIMSelector;

            switch (_domain.DKIMHeaderCanonicalizationMethod)
            {
                case eDKIMCanonicalizationMethod.eCanonicalizationRelaxed:
                    radioDKIMHeaderMethodRelaxed.Checked = true;
                    break;
                case eDKIMCanonicalizationMethod.eCanonicalizationSimple:
                    radioDKIMHeaderMethodSimple.Checked = true;
                    break;
            }

            switch (_domain.DKIMBodyCanonicalizationMethod)
            {
                case eDKIMCanonicalizationMethod.eCanonicalizationRelaxed:
                    radioDKIMBodyMethodRelaxed.Checked = true;
                    break;
                case eDKIMCanonicalizationMethod.eCanonicalizationSimple:
                    radioDKIMBodyMethodSimple.Checked = true;
                    break;
            }

            switch (_domain.DKIMSigningAlgorithm)
            {
                case eDKIMAlgorithm.eSHA1:
                    radioDKIMSigningAlgorithmSHA1.Checked = true;
                    break;
                case eDKIMAlgorithm.eSHA256:
                    radioDKIMSigningAlgorithmSHA256.Checked = true;
                    break;
            }

            ListDomainAliases();
        }

        public bool SaveData()
        {
            if (_domain == null)
            {
                hMailServer.Domains domains = _application.Domains;
                _domain = domains.Add();
                Marshal.ReleaseComObject(domains);
            }

            _domain.Name = textDomainName.Text;
            _domain.Postmaster = textCatchAllAddress.Text;
            _domain.AddSignaturesToReplies = checkAddSignaturesToReplies.Checked;
            _domain.AddSignaturesToLocalMail = checkAddSignaturesToLocalMail.Checked;
            _domain.SignatureEnabled = checkSignatureEnabled.Checked;
            _domain.Active = checkEnabled.Checked;

            if (radioSMAppendToAccountSignature.Checked == true)
                _domain.SignatureMethod = eDomainSignatureMethod.eSMAppendToAccountSignature;
            else if (radioSMOverwriteAccountSignature.Checked == true)
                _domain.SignatureMethod = eDomainSignatureMethod.eSMOverwriteAccountSignature;
            else if (radioSMSetIfNotSpecifiedInAccount.Checked == true)
                _domain.SignatureMethod = eDomainSignatureMethod.eSMSetIfNotSpecifiedInAccount;

            _domain.MaxSize = textMaxSize.Number;
            _domain.MaxMessageSize = textMaxMessageSize.Number;
            _domain.MaxAccountSize = textMaxAccountSize.Number;
            _domain.MaxNumberOfAccounts = textMaxNumberOfAccounts.Number;
            _domain.MaxNumberOfAliases = textMaxNumberOfAliases.Number;
            _domain.MaxNumberOfDistributionLists = textMaxNumberOfDistributionLists.Number;

            _domain.MaxNumberOfAccountsEnabled = checkMaxNumberOfAccountsEnabled.Checked;
            _domain.MaxNumberOfAliasesEnabled = checkMaxNumberOfAliasesEnabled.Checked;
            _domain.MaxNumberOfDistributionListsEnabled = checkMaxNumberOfDistributionListsEnabled.Checked;

            _domain.SignaturePlainText = textPlainTextSignature.Text;
            _domain.SignatureHTML = textHTMLSignature.Text;

            _domain.PlusAddressingEnabled = checkUsePlusAddressing.Checked;
            _domain.PlusAddressingCharacter = comboPlusAddressingCharacter.Text;
            _domain.AntiSpamEnableGreylisting = checkGreyListingEnabled.Checked;

            _domain.DKIMSignEnabled = checkDKIMSignEnabled.Checked;
            _domain.DKIMSignAliasesEnabled = checkDKIMSignAliasesEnabled.Checked;
            _domain.DKIMPrivateKeyFile = textDKIMPrivateKeyFile.Text;
            _domain.DKIMSelector = textDKIMSelector.Text;

            if (radioDKIMHeaderMethodSimple.Checked)
                _domain.DKIMHeaderCanonicalizationMethod = eDKIMCanonicalizationMethod.eCanonicalizationSimple;
            else if (radioDKIMHeaderMethodRelaxed.Checked)
                _domain.DKIMHeaderCanonicalizationMethod = eDKIMCanonicalizationMethod.eCanonicalizationRelaxed;

            if (radioDKIMBodyMethodSimple.Checked)
                _domain.DKIMBodyCanonicalizationMethod = eDKIMCanonicalizationMethod.eCanonicalizationSimple;
            else if (radioDKIMBodyMethodRelaxed.Checked)
                _domain.DKIMBodyCanonicalizationMethod = eDKIMCanonicalizationMethod.eCanonicalizationRelaxed;

            if (radioDKIMSigningAlgorithmSHA1.Checked)
                _domain.DKIMSigningAlgorithm = eDKIMAlgorithm.eSHA1;
            else if (radioDKIMSigningAlgorithmSHA256.Checked)
                _domain.DKIMSigningAlgorithm = eDKIMAlgorithm.eSHA256;

                

            try
            {
                _domain.Save();
            }
            catch (Exception ex)
            {
                MessageBox.Show(Strings.Localize(ex.Message), EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            IMainForm mainForm = Instances.MainForm;

            // Refresh the node in the tree
            mainForm.RefreshCurrentNode(_domain.Name);

            // Set the object to clean.
            DirtyChecker.SetClean(this);

            SearchNodeText crit = new SearchNodeText(_domain.Name);
            mainForm.SelectNode(crit);

            EnableDisableTabs();

            return true;
        }

        public void LoadResources()
        {

        }

        private void tabControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            FocusChanger.SetFocus(tabControl.SelectedTab.Controls);
        }

        private void tabControl_Enter(object sender, EventArgs e)
        {
            FocusChanger.SetFocus(tabControl.SelectedTab.Controls);
        }

        private void OnContentChanged()
        {
            Instances.MainForm.OnContentChanged();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
            OnContentChanged();
        }

        private void buttonAdd_Click(object sender, EventArgs e)
        {
            formInputDialog inputDialog = new formInputDialog();
            inputDialog.Title = "Alias name";
            inputDialog.Text = "Enter domain alias name";

            if (inputDialog.ShowDialog() == DialogResult.OK)
            {
                // Add it
                string value = inputDialog.Value;

                hMailServer.DomainAliases domainAliases = _domain.DomainAliases;

                DomainAlias domainAlias = domainAliases.Add();
                domainAlias.AliasName = value;
                try
                {
                    domainAlias.Save();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, EnumStrings.hMailServerAdministrator);
                    return;
                }

                Marshal.ReleaseComObject(domainAliases);
                Marshal.ReleaseComObject(domainAlias);

                ListDomainAliases();
            }
        }

        private void ListDomainAliases()
        {
            listAliases.Items.Clear();

            DomainAliases domainAliases = _domain.DomainAliases;
            for (int i = 0; i < domainAliases.Count; i++)
            {
                DomainAlias domainAlias = domainAliases[i];

                ListViewItem item = listAliases.Items.Add(domainAlias.AliasName);
                item.Tag = domainAlias.ID;

                Marshal.ReleaseComObject(domainAlias);
            }

            Marshal.ReleaseComObject(domainAliases);
        }

        private void buttonRemove_Click(object sender, EventArgs e)
        {
            DomainAliases domainAliases = _domain.DomainAliases;
            foreach (ListViewItem item in listAliases.SelectedItems)
            {
                int id = Convert.ToInt32(item.Tag);
                domainAliases.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(domainAliases);
            ListDomainAliases();
        }

        private void buttonEdit_Click(object sender, EventArgs e)
        {
           EditSelectedName();
        }

        private void EditSelectedName()
        {
           if (listAliases.SelectedItems.Count != 1)
              return;

           int id = Convert.ToInt32(listAliases.SelectedItems[0].Tag);
           DomainAliases domainAliases = _domain.DomainAliases;
           hMailServer.DomainAlias da = domainAliases.get_ItemByDBID(id);

           formInputDialog inputDialog = new formInputDialog();
           inputDialog.Title = "Alias name";
           inputDialog.Text = "Enter domain alias name";
           inputDialog.Value = da.AliasName;

           if (inputDialog.ShowDialog() == DialogResult.OK)
           {
              // Add it
              string value = inputDialog.Value;
              da.AliasName = value;
              
              try
              {
                 da.Save();
              }
              catch (Exception ex)
              {
                  MessageBox.Show(Strings.Localize(ex.Message), EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Error);                                   
              }

              ListDomainAliases();
              
           }

           Marshal.ReleaseComObject(domainAliases);
           Marshal.ReleaseComObject(da);
        }

        private void listAliases_SelectedIndexChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void EnableDisable()
        {
            buttonEdit.Enabled = listAliases.SelectedItems.Count == 1;
            buttonRemove.Enabled = listAliases.SelectedItems.Count > 0;

            textMaxNumberOfAccounts.Enabled = checkMaxNumberOfAccountsEnabled.Checked;
            textMaxNumberOfAliases.Enabled = checkMaxNumberOfAliasesEnabled.Checked;
            textMaxNumberOfDistributionLists.Enabled = checkMaxNumberOfDistributionListsEnabled.Checked;

            radioSMSetIfNotSpecifiedInAccount.Enabled = checkSignatureEnabled.Checked;
            radioSMOverwriteAccountSignature.Enabled = checkSignatureEnabled.Checked;
            radioSMAppendToAccountSignature.Enabled = checkSignatureEnabled.Checked;
        }

        private void checkSignatureEnabled_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void checkMaxNumberOfAccountsEnabled_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void checkMaxNumberOfAliasesEnabled_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void checkMaxNumberOfDistributionListsEnabled_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void radioSMSetIfNotSpecifiedInAccount_CheckedChanged(object sender, EventArgs e)
        {

        }

        private void buttonSelectCertificate_Click(object sender, EventArgs e)
        {
            openFileDialog.Title = Strings.Localize("Private key file");
            openFileDialog.Filter = "*.*|*.*";
            openFileDialog.FileName = "";

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                textDKIMPrivateKeyFile.Text = openFileDialog.FileName;
            }
        }

        private void tabPageDKIM_Click(object sender, EventArgs e)
        {

        }

        private void listAliases_DoubleClick(object sender, EventArgs e)
        {
           EditSelectedName();
        }

    }
}
