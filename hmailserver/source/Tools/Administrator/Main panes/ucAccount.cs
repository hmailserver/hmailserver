// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Nodes;
using hMailServer.Administrator.Utilities.Localization;
using hMailServer.Administrator.Dialogs;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucAccount : UserControl, ISettingsControl
    {
        private int _domainID = 0;
        private hMailServer.Account _representedAccount = null;

        public ucAccount(int domainID, int accountID)
        {
            InitializeComponent();

            _domainID = domainID;

            hMailServer.Links links = APICreator.Links;

            hMailServer.Domain domain = links.get_Domain(_domainID);

            if (accountID > 0)
            {
                _representedAccount = links.get_Account(accountID);
            }

            textAddress.Text = "@" + domain.Name;

            Marshal.ReleaseComObject(domain);
            Marshal.ReleaseComObject(links);

            comboAdministrationLevel.AddItem(InternalNames.GetAdministrationLevelName(eAdminLevel.hAdminLevelNormal), eAdminLevel.hAdminLevelNormal);
            comboAdministrationLevel.AddItem(InternalNames.GetAdministrationLevelName(eAdminLevel.hAdminLevelDomainAdmin), eAdminLevel.hAdminLevelDomainAdmin);
            comboAdministrationLevel.AddItem(InternalNames.GetAdministrationLevelName(eAdminLevel.hAdminLevelServerAdmin), eAdminLevel.hAdminLevelServerAdmin);
            comboAdministrationLevel.SelectedIndex = 0;

            dateVacationMessageExpiresDate.Value = DateTime.Now;

            DirtyChecker.SubscribeToChange(this, OnContentChanged);
            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        public void OnLeavePage()
        {
            if (_representedAccount != null)
                Marshal.ReleaseComObject(_representedAccount);
        }

        public bool Dirty
        {
            get
            {
                return DirtyChecker.IsDirty(this) &&
                       textAddress.Text.Length > 0;
            }
        }

        private void EnableDisable()
        {
            textVacationMessageSubject.Enabled = checkVacationMessageEnable.Checked;
            textVacationMessageText.Enabled = checkVacationMessageEnable.Checked;
            checkVacationMessageExpires.Enabled = checkVacationMessageEnable.Checked;
            dateVacationMessageExpiresDate.Enabled = checkVacationMessageEnable.Checked;

            textForwardAddress.Enabled = checkForwardEnabled.Checked;
            checkForwardKeepOriginal.Enabled = checkForwardEnabled.Checked;

            textSignaturePlainText.Enabled = checkSignatureEnabled.Checked;
            textSignatureHTML.Enabled = checkSignatureEnabled.Checked;

            textADDomain.Enabled = checkAccountIsAD.Checked;
            textADUsername.Enabled = checkAccountIsAD.Checked;

            tabControl.TabPages[4].Enabled = IsPersistentAccount;
            tabControl.TabPages[5].Enabled = IsPersistentAccount;

            buttonEmptyAccount.Enabled = IsPersistentAccount;
            buttonEditFolders.Enabled = IsPersistentAccount;
            buttonUnlock.Enabled = IsPersistentAccount;

        }

        private bool IsPersistentAccount
        {
            get
            {
                return _representedAccount != null && _representedAccount.ID > 0;
            }
        }

        public void LoadData()
        {
            EnableDisable();

            if (_representedAccount == null)
            {
                return;
            }


            textAddress.Text = _representedAccount.Address;
            textMaxSize.Number = _representedAccount.MaxSize;
            checkEnabled.Checked = _representedAccount.Active;
            textSize.Text = Math.Round(_representedAccount.Size, 3).ToString();

            checkVacationMessageEnable.Checked = _representedAccount.VacationMessageIsOn;
            textVacationMessageSubject.Text = _representedAccount.VacationSubject;
            textVacationMessageText.Text = _representedAccount.VacationMessage;
            checkVacationMessageExpires.Checked = _representedAccount.VacationMessageExpires;
            dateVacationMessageExpiresDate.Value = Convert.ToDateTime(_representedAccount.VacationMessageExpiresDate);

            // If out-of-office is not currently enabled, change the date to today. This makes it easier
            // for users to configure their out-of-office.
            // http://www.hmailserver.com/forum/viewtopic.php?f=7&t=17353
            //
            if (!checkVacationMessageExpires.Checked)
            {
               dateVacationMessageExpiresDate.Value = DateTime.Now;
            }

            checkForwardEnabled.Checked = _representedAccount.ForwardEnabled;
            textForwardAddress.Text = _representedAccount.ForwardAddress;
            checkForwardKeepOriginal.Checked = _representedAccount.ForwardKeepOriginal;

            checkSignatureEnabled.Checked = _representedAccount.SignatureEnabled;
            textSignaturePlainText.Text = _representedAccount.SignaturePlainText;
            textSignatureHTML.Text = _representedAccount.SignatureHTML;

            checkAccountIsAD.Checked = _representedAccount.IsAD;
            textADDomain.Text = _representedAccount.ADDomain;
            textADUsername.Text = _representedAccount.ADUsername;

            textFirstName.Text = _representedAccount.PersonFirstName;
            textLastName.Text = _representedAccount.PersonLastName;

            textLastLogonTime.Text = _representedAccount.LastLogonTime.ToString();

            comboAdministrationLevel.SelectedValue = _representedAccount.AdminLevel;

            ListExternalAccounts();

            rules.LoadRules(_representedAccount.Rules);


            EnableDisable();
        }

        private bool ValidateForm()
        {
            if (textPassword.Dirty)
            {
                if (!APICreator.Application.Utilities.IsStrongPassword(textAddress.Text, textPassword.Password))
                {
                    if (MessageBox.Show(Strings.Localize("The password appears to be weak. Are you sure you want to use it?"), EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNo) != DialogResult.Yes)
                    {
                        return false;
                    }
                }
            }

            return true;
        }

        public bool SaveData()
        {
            if (!ValidateForm())
            {
                return false;
            }


            bool newAccount = false;

            if (_representedAccount == null)
            {
                hMailServer.Domain domain = APICreator.GetDomain(_domainID);

                hMailServer.Accounts accounts = domain.Accounts;
                _representedAccount = accounts.Add();
                
                Marshal.ReleaseComObject(accounts);
                Marshal.ReleaseComObject(domain);
            }

            if (_representedAccount.ID == 0)
                newAccount = true;

            _representedAccount.Address = textAddress.Text;
            _representedAccount.MaxSize = textMaxSize.Number;
            _representedAccount.Active = checkEnabled.Checked;

            _representedAccount.VacationMessageIsOn = checkVacationMessageEnable.Checked;
            _representedAccount.VacationSubject = textVacationMessageSubject.Text;
            _representedAccount.VacationMessage = textVacationMessageText.Text;
            _representedAccount.VacationMessageExpires = checkVacationMessageExpires.Checked;
            _representedAccount.VacationMessageExpiresDate = dateVacationMessageExpiresDate.FormattedValue;

            _representedAccount.ForwardEnabled = checkForwardEnabled.Checked;
            _representedAccount.ForwardAddress = textForwardAddress.Text;
            _representedAccount.ForwardKeepOriginal = checkForwardKeepOriginal.Checked;

            _representedAccount.SignatureEnabled = checkSignatureEnabled.Checked;
            _representedAccount.SignaturePlainText = textSignaturePlainText.Text;
            _representedAccount.SignatureHTML = textSignatureHTML.Text;

            _representedAccount.IsAD = checkAccountIsAD.Checked;
            _representedAccount.ADDomain = textADDomain.Text;
            _representedAccount.ADUsername = textADUsername.Text;

            _representedAccount.PersonFirstName = textFirstName.Text;
            _representedAccount.PersonLastName = textLastName.Text;
            _representedAccount.AdminLevel = (hMailServer.eAdminLevel)comboAdministrationLevel.SelectedValue;

            if (textPassword.Dirty)
                _representedAccount.Password = textPassword.Password;

            _representedAccount.Save();

            // Refresh the node in the tree if the name has changed.
            IMainForm mainForm = Instances.MainForm;
            mainForm.RefreshCurrentNode(_representedAccount.Address);

            // Set the object to clean.
            DirtyChecker.SetClean(this);

            if (newAccount)
            {
                SearchNodeText crit = new SearchNodeText(_representedAccount.Address);
                mainForm.SelectNode(crit);
            }
            else
                EnableDisable();

            return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }

        private void buttonEmptyAccount_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show(Strings.Localize("Are you sure you want to remove all messages in the account?"),
                                EnumStrings.hMailServerAdministrator,
                                MessageBoxButtons.YesNo,
                                MessageBoxIcon.Question) == DialogResult.Yes)
            {
               using (new WaitCursor())
               {
                  _representedAccount.DeleteMessages();
               }
            }

        }

        private void buttonUnlock_Click(object sender, EventArgs e)
        {
            if (_representedAccount == null)
            {
                MessageBox.Show("The account must be saved first.", EnumStrings.hMailServerAdministrator);
                return;
            }

            _representedAccount.UnlockMailbox();
        }

        private void OnContentChanged()
        {
            Instances.MainForm.OnContentChanged();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
            OnContentChanged();
        }

        private void buttonAddExternalAccount_Click(object sender, EventArgs e)
        {
            formExternalAccount externalAccountDialog = new formExternalAccount();

            if (externalAccountDialog.ShowDialog() == DialogResult.OK)
            {
                hMailServer.FetchAccounts fetchAccounts = _representedAccount.FetchAccounts;
                hMailServer.FetchAccount fetchAccount = _representedAccount.FetchAccounts.Add();
                Marshal.ReleaseComObject(fetchAccounts);

                externalAccountDialog.SaveAccountProperties(fetchAccount);
                fetchAccount.Save();

                Marshal.ReleaseComObject(fetchAccount);

                ListExternalAccounts();
            }


        }

        private void ListExternalAccounts()
        {
            listFetchAccounts.Items.Clear();

            hMailServer.FetchAccounts fetchAccounts = _representedAccount.FetchAccounts;

            for (int i = 0; i < fetchAccounts.Count; i++)
            {
                hMailServer.FetchAccount fetchAccount = fetchAccounts.get_Item(i);

                string name = fetchAccount.Name;
                string enabled = EnumStrings.GetYesNoString(fetchAccount.Enabled);

                ListViewItem item = listFetchAccounts.Items.Add(name);
                item.SubItems.Add(enabled);

                if (fetchAccount.Enabled)
                {
                    item.SubItems.Add(fetchAccount.NextDownloadTime);
                }

                item.Tag = fetchAccount.ID;

                Marshal.ReleaseComObject(fetchAccount);
            }

        }

        private void buttonDeleteExternalAccount_Click(object sender, EventArgs e)
        {
            hMailServer.FetchAccounts fetchAccounts = _representedAccount.FetchAccounts;

            foreach (ListViewItem item in listFetchAccounts.SelectedItems)
            {
                int id = Convert.ToInt32(item.Tag);

                fetchAccounts.DeleteByDBID(id);
            }

            Marshal.ReleaseComObject(fetchAccounts);

            ListExternalAccounts();
        }

        private void buttonEditExternalAccount_Click(object sender, EventArgs e)
        {
            EditSelectedExternalAccount();

        }

        private void EditSelectedExternalAccount()
        {
            if (listFetchAccounts.SelectedItems.Count < 1)
                return;

            int id = Convert.ToInt32(listFetchAccounts.SelectedItems[0].Tag);

            hMailServer.FetchAccounts fetchAccounts = _representedAccount.FetchAccounts;
            hMailServer.FetchAccount fetchAccount = fetchAccounts.get_ItemByDBID(id);

            formExternalAccount externalAccountDialog = new formExternalAccount();

            externalAccountDialog.LoadAccountProperties(fetchAccount);
            if (externalAccountDialog.ShowDialog() == DialogResult.OK)
            {
                externalAccountDialog.SaveAccountProperties(fetchAccount);
                fetchAccount.Save();
            }

            ListExternalAccounts();

            Marshal.ReleaseComObject(fetchAccounts);
            Marshal.ReleaseComObject(fetchAccount);
        }

        private void imapFolders_Load(object sender, EventArgs e)
        {

        }

        private void listFetchAccounts_SelectedIndexChanged(object sender, EventArgs e)
        {
            buttonEditExternalAccount.Enabled = listFetchAccounts.SelectedItems.Count == 1;
            buttonDeleteExternalAccount.Enabled = listFetchAccounts.SelectedItems.Count > 0;
        }

        private void listFetchAccounts_DoubleClick(object sender, EventArgs e)
        {
            EditSelectedExternalAccount();
        }

        private void checkVacationMessageEnable_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void checkAccountIsAD_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void checkForwardEnabled_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void checkSignatureEnabled_CheckedChanged(object sender, EventArgs e)
        {
            EnableDisable();
        }

        private void buttonEditFolders_Click(object sender, EventArgs e)
        {
            if (_representedAccount == null)
            {
                MessageBox.Show("The account must be saved first.", EnumStrings.hMailServerAdministrator);
                return;
            }

            formAccountFolders accountFoldersDlg = new formAccountFolders(_representedAccount.IMAPFolders, false);
            accountFoldersDlg.ShowDialog();
        }


    }
}
