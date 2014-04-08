namespace hMailServer.Administrator

{
    partial class ucAccount
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
           this.tabControl = new System.Windows.Forms.TabControl();
           this.tabPageGeneral = new System.Windows.Forms.TabPage();
           this.textPassword = new hMailServer.Administrator.Controls.ucPassword();
           this.textAddress = new hMailServer.Administrator.Controls.ucEmailEdit();
           this.checkEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.textLastLogonTime = new hMailServer.Shared.ucText();
           this.labelLastLogonTime = new System.Windows.Forms.Label();
           this.labelAdministrationLevel = new System.Windows.Forms.Label();
           this.comboAdministrationLevel = new hMailServer.Administrator.Controls.ucComboBox();
           this.textMaxSize = new hMailServer.Shared.ucText();
           this.labelMaxSize = new System.Windows.Forms.Label();
           this.textSize = new hMailServer.Shared.ucText();
           this.labelSize = new System.Windows.Forms.Label();
           this.labelPassword = new System.Windows.Forms.Label();
           this.labelAddress = new System.Windows.Forms.Label();
           this.tabPageAutoReply = new System.Windows.Forms.TabPage();
           this.dateVacationMessageExpiresDate = new hMailServer.Administrator.Controls.ucDateTimePicker();
           this.checkVacationMessageExpires = new hMailServer.Administrator.Controls.ucCheckbox();
           this.labelAutoReplyText = new System.Windows.Forms.Label();
           this.labelAutoReplySubject = new System.Windows.Forms.Label();
           this.textVacationMessageText = new hMailServer.Shared.ucText();
           this.textVacationMessageSubject = new hMailServer.Shared.ucText();
           this.checkVacationMessageEnable = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabPageForwarding = new System.Windows.Forms.TabPage();
           this.checkForwardKeepOriginal = new hMailServer.Administrator.Controls.ucCheckbox();
           this.textForwardAddress = new hMailServer.Administrator.Controls.ucEmailEdit();
           this.labelForwardAddress = new System.Windows.Forms.Label();
           this.checkForwardEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabPageSignature = new System.Windows.Forms.TabPage();
           this.labelSignatureHTML = new System.Windows.Forms.Label();
           this.labelSignaturePlainText = new System.Windows.Forms.Label();
           this.textSignatureHTML = new hMailServer.Shared.ucText();
           this.textSignaturePlainText = new hMailServer.Shared.ucText();
           this.checkSignatureEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabPageExternalAccounts = new System.Windows.Forms.TabPage();
           this.buttonEditExternalAccount = new System.Windows.Forms.Button();
           this.buttonDeleteExternalAccount = new System.Windows.Forms.Button();
           this.buttonAddExternalAccount = new System.Windows.Forms.Button();
           this.listFetchAccounts = new hMailServer.Administrator.ucListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
           this.tabPageRules = new System.Windows.Forms.TabPage();
           this.rules = new hMailServer.Administrator.ucRules();
           this.tabPageActiveDirectory = new System.Windows.Forms.TabPage();
           this.labelAccountADInfo = new System.Windows.Forms.Label();
           this.labelUsername = new System.Windows.Forms.Label();
           this.labelADDomain = new System.Windows.Forms.Label();
           this.textADUsername = new hMailServer.Shared.ucText();
           this.textADDomain = new hMailServer.Shared.ucText();
           this.checkAccountIsAD = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabPageAdvanced = new System.Windows.Forms.TabPage();
           this.buttonEditFolders = new System.Windows.Forms.Button();
           this.buttonUnlock = new System.Windows.Forms.Button();
           this.buttonEmptyAccount = new System.Windows.Forms.Button();
           this.labelOther = new System.Windows.Forms.Label();
           this.labelLastName = new System.Windows.Forms.Label();
           this.labelFirstName = new System.Windows.Forms.Label();
           this.labelPersonalInformation = new System.Windows.Forms.Label();
           this.textLastName = new hMailServer.Shared.ucText();
           this.textFirstName = new hMailServer.Shared.ucText();
           this.tabControl.SuspendLayout();
           this.tabPageGeneral.SuspendLayout();
           this.tabPageAutoReply.SuspendLayout();
           this.tabPageForwarding.SuspendLayout();
           this.tabPageSignature.SuspendLayout();
           this.tabPageExternalAccounts.SuspendLayout();
           this.tabPageRules.SuspendLayout();
           this.tabPageActiveDirectory.SuspendLayout();
           this.tabPageAdvanced.SuspendLayout();
           this.SuspendLayout();
           // 
           // tabControl
           // 
           this.tabControl.Controls.Add(this.tabPageGeneral);
           this.tabControl.Controls.Add(this.tabPageAutoReply);
           this.tabControl.Controls.Add(this.tabPageForwarding);
           this.tabControl.Controls.Add(this.tabPageSignature);
           this.tabControl.Controls.Add(this.tabPageExternalAccounts);
           this.tabControl.Controls.Add(this.tabPageRules);
           this.tabControl.Controls.Add(this.tabPageActiveDirectory);
           this.tabControl.Controls.Add(this.tabPageAdvanced);
           this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
           this.tabControl.Location = new System.Drawing.Point(0, 0);
           this.tabControl.Name = "tabControl";
           this.tabControl.SelectedIndex = 0;
           this.tabControl.Size = new System.Drawing.Size(733, 395);
           this.tabControl.TabIndex = 0;
           // 
           // tabPageGeneral
           // 
           this.tabPageGeneral.Controls.Add(this.textPassword);
           this.tabPageGeneral.Controls.Add(this.textAddress);
           this.tabPageGeneral.Controls.Add(this.checkEnabled);
           this.tabPageGeneral.Controls.Add(this.textLastLogonTime);
           this.tabPageGeneral.Controls.Add(this.labelLastLogonTime);
           this.tabPageGeneral.Controls.Add(this.labelAdministrationLevel);
           this.tabPageGeneral.Controls.Add(this.comboAdministrationLevel);
           this.tabPageGeneral.Controls.Add(this.textMaxSize);
           this.tabPageGeneral.Controls.Add(this.labelMaxSize);
           this.tabPageGeneral.Controls.Add(this.textSize);
           this.tabPageGeneral.Controls.Add(this.labelSize);
           this.tabPageGeneral.Controls.Add(this.labelPassword);
           this.tabPageGeneral.Controls.Add(this.labelAddress);
           this.tabPageGeneral.Location = new System.Drawing.Point(4, 22);
           this.tabPageGeneral.Name = "tabPageGeneral";
           this.tabPageGeneral.Padding = new System.Windows.Forms.Padding(3);
           this.tabPageGeneral.Size = new System.Drawing.Size(725, 369);
           this.tabPageGeneral.TabIndex = 0;
           this.tabPageGeneral.Text = "General";
           this.tabPageGeneral.UseVisualStyleBackColor = true;
           // 
           // textPassword
           // 
           this.textPassword.Location = new System.Drawing.Point(8, 72);
           this.textPassword.Name = "textPassword";
           this.textPassword.Size = new System.Drawing.Size(208, 20);
           this.textPassword.TabIndex = 1;
           this.textPassword.Text = "<< Encrypted >>";
           // 
           // textAddress
           // 
           this.textAddress.Location = new System.Drawing.Point(8, 24);
           this.textAddress.Name = "textAddress";
           this.textAddress.ReadOnlyHost = true;
           this.textAddress.Size = new System.Drawing.Size(296, 24);
           this.textAddress.TabIndex = 0;
           // 
           // checkEnabled
           // 
           this.checkEnabled.AutoSize = true;
           this.checkEnabled.Checked = true;
           this.checkEnabled.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkEnabled.Location = new System.Drawing.Point(9, 246);
           this.checkEnabled.Name = "checkEnabled";
           this.checkEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkEnabled.TabIndex = 13;
           this.checkEnabled.Text = "Enabled";
           this.checkEnabled.UseVisualStyleBackColor = true;
           // 
           // textLastLogonTime
           // 
           this.textLastLogonTime.Location = new System.Drawing.Point(8, 216);
           this.textLastLogonTime.Name = "textLastLogonTime";
           this.textLastLogonTime.Number = 0;
           this.textLastLogonTime.Numeric = false;
           this.textLastLogonTime.ReadOnly = true;
           this.textLastLogonTime.Size = new System.Drawing.Size(212, 20);
           this.textLastLogonTime.TabIndex = 4;
           // 
           // labelLastLogonTime
           // 
           this.labelLastLogonTime.AutoSize = true;
           this.labelLastLogonTime.Location = new System.Drawing.Point(8, 200);
           this.labelLastLogonTime.Name = "labelLastLogonTime";
           this.labelLastLogonTime.Size = new System.Drawing.Size(78, 13);
           this.labelLastLogonTime.TabIndex = 11;
           this.labelLastLogonTime.Text = "Last logon time";
           // 
           // labelAdministrationLevel
           // 
           this.labelAdministrationLevel.AutoSize = true;
           this.labelAdministrationLevel.Location = new System.Drawing.Point(11, 152);
           this.labelAdministrationLevel.Name = "labelAdministrationLevel";
           this.labelAdministrationLevel.Size = new System.Drawing.Size(97, 13);
           this.labelAdministrationLevel.TabIndex = 10;
           this.labelAdministrationLevel.Text = "Administration level";
           // 
           // comboAdministrationLevel
           // 
           this.comboAdministrationLevel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
           this.comboAdministrationLevel.FormattingEnabled = true;
           this.comboAdministrationLevel.Location = new System.Drawing.Point(8, 168);
           this.comboAdministrationLevel.Name = "comboAdministrationLevel";
           this.comboAdministrationLevel.Size = new System.Drawing.Size(215, 21);
           this.comboAdministrationLevel.TabIndex = 3;
           // 
           // textMaxSize
           // 
           this.textMaxSize.Location = new System.Drawing.Point(120, 120);
           this.textMaxSize.Name = "textMaxSize";
           this.textMaxSize.Number = 0;
           this.textMaxSize.Numeric = true;
           this.textMaxSize.Size = new System.Drawing.Size(97, 20);
           this.textMaxSize.TabIndex = 2;
           this.textMaxSize.Text = "0";
           // 
           // labelMaxSize
           // 
           this.labelMaxSize.AutoSize = true;
           this.labelMaxSize.Location = new System.Drawing.Point(120, 104);
           this.labelMaxSize.Margin = new System.Windows.Forms.Padding(0);
           this.labelMaxSize.Name = "labelMaxSize";
           this.labelMaxSize.Size = new System.Drawing.Size(97, 13);
           this.labelMaxSize.TabIndex = 7;
           this.labelMaxSize.Text = "Maximum size (MB)";
           // 
           // textSize
           // 
           this.textSize.Location = new System.Drawing.Point(8, 120);
           this.textSize.Name = "textSize";
           this.textSize.Number = 0;
           this.textSize.Numeric = false;
           this.textSize.ReadOnly = true;
           this.textSize.Size = new System.Drawing.Size(97, 20);
           this.textSize.TabIndex = 6;
           // 
           // labelSize
           // 
           this.labelSize.AutoSize = true;
           this.labelSize.Location = new System.Drawing.Point(8, 104);
           this.labelSize.Name = "labelSize";
           this.labelSize.Size = new System.Drawing.Size(52, 13);
           this.labelSize.TabIndex = 5;
           this.labelSize.Text = "Size (MB)";
           // 
           // labelPassword
           // 
           this.labelPassword.AutoSize = true;
           this.labelPassword.Location = new System.Drawing.Point(8, 56);
           this.labelPassword.Name = "labelPassword";
           this.labelPassword.Size = new System.Drawing.Size(53, 13);
           this.labelPassword.TabIndex = 3;
           this.labelPassword.Text = "Password";
           // 
           // labelAddress
           // 
           this.labelAddress.AutoSize = true;
           this.labelAddress.Location = new System.Drawing.Point(8, 8);
           this.labelAddress.Name = "labelAddress";
           this.labelAddress.Size = new System.Drawing.Size(45, 13);
           this.labelAddress.TabIndex = 1;
           this.labelAddress.Text = "Address";
           // 
           // tabPageAutoReply
           // 
           this.tabPageAutoReply.Controls.Add(this.dateVacationMessageExpiresDate);
           this.tabPageAutoReply.Controls.Add(this.checkVacationMessageExpires);
           this.tabPageAutoReply.Controls.Add(this.labelAutoReplyText);
           this.tabPageAutoReply.Controls.Add(this.labelAutoReplySubject);
           this.tabPageAutoReply.Controls.Add(this.textVacationMessageText);
           this.tabPageAutoReply.Controls.Add(this.textVacationMessageSubject);
           this.tabPageAutoReply.Controls.Add(this.checkVacationMessageEnable);
           this.tabPageAutoReply.Location = new System.Drawing.Point(4, 22);
           this.tabPageAutoReply.Name = "tabPageAutoReply";
           this.tabPageAutoReply.Padding = new System.Windows.Forms.Padding(3);
           this.tabPageAutoReply.Size = new System.Drawing.Size(725, 369);
           this.tabPageAutoReply.TabIndex = 1;
           this.tabPageAutoReply.Text = "Auto-reply";
           this.tabPageAutoReply.UseVisualStyleBackColor = true;
           // 
           // dateVacationMessageExpiresDate
           // 
           this.dateVacationMessageExpiresDate.Location = new System.Drawing.Point(39, 245);
           this.dateVacationMessageExpiresDate.Name = "dateVacationMessageExpiresDate";
           this.dateVacationMessageExpiresDate.Size = new System.Drawing.Size(200, 20);
           this.dateVacationMessageExpiresDate.TabIndex = 20;
           this.dateVacationMessageExpiresDate.Value = new System.DateTime(2008, 2, 10, 10, 18, 34, 562);
           // 
           // checkVacationMessageExpires
           // 
           this.checkVacationMessageExpires.AutoSize = true;
           this.checkVacationMessageExpires.Location = new System.Drawing.Point(24, 222);
           this.checkVacationMessageExpires.Name = "checkVacationMessageExpires";
           this.checkVacationMessageExpires.Size = new System.Drawing.Size(119, 17);
           this.checkVacationMessageExpires.TabIndex = 19;
           this.checkVacationMessageExpires.Text = "Automatically expire";
           this.checkVacationMessageExpires.UseVisualStyleBackColor = true;
           // 
           // labelAutoReplyText
           // 
           this.labelAutoReplyText.AutoSize = true;
           this.labelAutoReplyText.Location = new System.Drawing.Point(24, 80);
           this.labelAutoReplyText.Name = "labelAutoReplyText";
           this.labelAutoReplyText.Size = new System.Drawing.Size(28, 13);
           this.labelAutoReplyText.TabIndex = 18;
           this.labelAutoReplyText.Text = "Text";
           // 
           // labelAutoReplySubject
           // 
           this.labelAutoReplySubject.AutoSize = true;
           this.labelAutoReplySubject.Location = new System.Drawing.Point(24, 32);
           this.labelAutoReplySubject.Margin = new System.Windows.Forms.Padding(0);
           this.labelAutoReplySubject.Name = "labelAutoReplySubject";
           this.labelAutoReplySubject.Size = new System.Drawing.Size(43, 13);
           this.labelAutoReplySubject.TabIndex = 16;
           this.labelAutoReplySubject.Text = "Subject";
           // 
           // textVacationMessageText
           // 
           this.textVacationMessageText.Location = new System.Drawing.Point(24, 96);
           this.textVacationMessageText.MaxLength = 1000;
           this.textVacationMessageText.Multiline = true;
           this.textVacationMessageText.Name = "textVacationMessageText";
           this.textVacationMessageText.Number = 0;
           this.textVacationMessageText.Numeric = false;
           this.textVacationMessageText.Size = new System.Drawing.Size(215, 120);
           this.textVacationMessageText.TabIndex = 17;
           // 
           // textVacationMessageSubject
           // 
           this.textVacationMessageSubject.Location = new System.Drawing.Point(24, 48);
           this.textVacationMessageSubject.Name = "textVacationMessageSubject";
           this.textVacationMessageSubject.Number = 0;
           this.textVacationMessageSubject.Numeric = false;
           this.textVacationMessageSubject.Size = new System.Drawing.Size(215, 20);
           this.textVacationMessageSubject.TabIndex = 15;
           // 
           // checkVacationMessageEnable
           // 
           this.checkVacationMessageEnable.AutoSize = true;
           this.checkVacationMessageEnable.Location = new System.Drawing.Point(8, 8);
           this.checkVacationMessageEnable.Name = "checkVacationMessageEnable";
           this.checkVacationMessageEnable.Size = new System.Drawing.Size(65, 17);
           this.checkVacationMessageEnable.TabIndex = 14;
           this.checkVacationMessageEnable.Text = "Enabled";
           this.checkVacationMessageEnable.UseVisualStyleBackColor = true;
           this.checkVacationMessageEnable.CheckedChanged += new System.EventHandler(this.checkVacationMessageEnable_CheckedChanged);
           // 
           // tabPageForwarding
           // 
           this.tabPageForwarding.Controls.Add(this.checkForwardKeepOriginal);
           this.tabPageForwarding.Controls.Add(this.textForwardAddress);
           this.tabPageForwarding.Controls.Add(this.labelForwardAddress);
           this.tabPageForwarding.Controls.Add(this.checkForwardEnabled);
           this.tabPageForwarding.Location = new System.Drawing.Point(4, 22);
           this.tabPageForwarding.Name = "tabPageForwarding";
           this.tabPageForwarding.Size = new System.Drawing.Size(725, 369);
           this.tabPageForwarding.TabIndex = 2;
           this.tabPageForwarding.Text = "Forwarding";
           this.tabPageForwarding.UseVisualStyleBackColor = true;
           // 
           // checkForwardKeepOriginal
           // 
           this.checkForwardKeepOriginal.AutoSize = true;
           this.checkForwardKeepOriginal.Location = new System.Drawing.Point(26, 80);
           this.checkForwardKeepOriginal.Name = "checkForwardKeepOriginal";
           this.checkForwardKeepOriginal.Size = new System.Drawing.Size(132, 17);
           this.checkForwardKeepOriginal.TabIndex = 18;
           this.checkForwardKeepOriginal.Text = "Keep original message";
           this.checkForwardKeepOriginal.UseVisualStyleBackColor = true;
           // 
           // textForwardAddress
           // 
           this.textForwardAddress.Location = new System.Drawing.Point(26, 49);
           this.textForwardAddress.Name = "textForwardAddress";
           this.textForwardAddress.ReadOnlyHost = false;
           this.textForwardAddress.Size = new System.Drawing.Size(282, 25);
           this.textForwardAddress.TabIndex = 17;
           // 
           // labelForwardAddress
           // 
           this.labelForwardAddress.AutoSize = true;
           this.labelForwardAddress.Location = new System.Drawing.Point(24, 32);
           this.labelForwardAddress.Margin = new System.Windows.Forms.Padding(0);
           this.labelForwardAddress.Name = "labelForwardAddress";
           this.labelForwardAddress.Size = new System.Drawing.Size(57, 13);
           this.labelForwardAddress.TabIndex = 16;
           this.labelForwardAddress.Text = "Forward to";
           // 
           // checkForwardEnabled
           // 
           this.checkForwardEnabled.AutoSize = true;
           this.checkForwardEnabled.Location = new System.Drawing.Point(8, 8);
           this.checkForwardEnabled.Name = "checkForwardEnabled";
           this.checkForwardEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkForwardEnabled.TabIndex = 15;
           this.checkForwardEnabled.Text = "Enabled";
           this.checkForwardEnabled.UseVisualStyleBackColor = true;
           this.checkForwardEnabled.CheckedChanged += new System.EventHandler(this.checkForwardEnabled_CheckedChanged);
           // 
           // tabPageSignature
           // 
           this.tabPageSignature.Controls.Add(this.labelSignatureHTML);
           this.tabPageSignature.Controls.Add(this.labelSignaturePlainText);
           this.tabPageSignature.Controls.Add(this.textSignatureHTML);
           this.tabPageSignature.Controls.Add(this.textSignaturePlainText);
           this.tabPageSignature.Controls.Add(this.checkSignatureEnabled);
           this.tabPageSignature.Location = new System.Drawing.Point(4, 22);
           this.tabPageSignature.Name = "tabPageSignature";
           this.tabPageSignature.Size = new System.Drawing.Size(725, 369);
           this.tabPageSignature.TabIndex = 3;
           this.tabPageSignature.Text = "Signature";
           this.tabPageSignature.UseVisualStyleBackColor = true;
           // 
           // labelSignatureHTML
           // 
           this.labelSignatureHTML.AutoSize = true;
           this.labelSignatureHTML.Location = new System.Drawing.Point(34, 166);
           this.labelSignatureHTML.Name = "labelSignatureHTML";
           this.labelSignatureHTML.Size = new System.Drawing.Size(83, 13);
           this.labelSignatureHTML.TabIndex = 21;
           this.labelSignatureHTML.Text = "HTML signature";
           // 
           // labelSignaturePlainText
           // 
           this.labelSignaturePlainText.AutoSize = true;
           this.labelSignaturePlainText.Location = new System.Drawing.Point(34, 38);
           this.labelSignaturePlainText.Name = "labelSignaturePlainText";
           this.labelSignaturePlainText.Size = new System.Drawing.Size(96, 13);
           this.labelSignaturePlainText.TabIndex = 19;
           this.labelSignaturePlainText.Text = "Plain text signature";
           // 
           // textSignatureHTML
           // 
           this.textSignatureHTML.Location = new System.Drawing.Point(35, 182);
           this.textSignatureHTML.Multiline = true;
           this.textSignatureHTML.Name = "textSignatureHTML";
           this.textSignatureHTML.Number = 0;
           this.textSignatureHTML.Numeric = false;
           this.textSignatureHTML.Size = new System.Drawing.Size(225, 100);
           this.textSignatureHTML.TabIndex = 20;
           // 
           // textSignaturePlainText
           // 
           this.textSignaturePlainText.Location = new System.Drawing.Point(35, 54);
           this.textSignaturePlainText.Multiline = true;
           this.textSignaturePlainText.Name = "textSignaturePlainText";
           this.textSignaturePlainText.Number = 0;
           this.textSignaturePlainText.Numeric = false;
           this.textSignaturePlainText.Size = new System.Drawing.Size(225, 100);
           this.textSignaturePlainText.TabIndex = 18;
           // 
           // checkSignatureEnabled
           // 
           this.checkSignatureEnabled.AutoSize = true;
           this.checkSignatureEnabled.Location = new System.Drawing.Point(12, 15);
           this.checkSignatureEnabled.Name = "checkSignatureEnabled";
           this.checkSignatureEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkSignatureEnabled.TabIndex = 15;
           this.checkSignatureEnabled.Text = "Enabled";
           this.checkSignatureEnabled.UseVisualStyleBackColor = true;
           this.checkSignatureEnabled.CheckedChanged += new System.EventHandler(this.checkSignatureEnabled_CheckedChanged);
           // 
           // tabPageExternalAccounts
           // 
           this.tabPageExternalAccounts.Controls.Add(this.buttonEditExternalAccount);
           this.tabPageExternalAccounts.Controls.Add(this.buttonDeleteExternalAccount);
           this.tabPageExternalAccounts.Controls.Add(this.buttonAddExternalAccount);
           this.tabPageExternalAccounts.Controls.Add(this.listFetchAccounts);
           this.tabPageExternalAccounts.Location = new System.Drawing.Point(4, 22);
           this.tabPageExternalAccounts.Name = "tabPageExternalAccounts";
           this.tabPageExternalAccounts.Size = new System.Drawing.Size(725, 369);
           this.tabPageExternalAccounts.TabIndex = 4;
           this.tabPageExternalAccounts.Text = "External accounts";
           this.tabPageExternalAccounts.UseVisualStyleBackColor = true;
           // 
           // buttonEditExternalAccount
           // 
           this.buttonEditExternalAccount.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonEditExternalAccount.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonEditExternalAccount.Location = new System.Drawing.Point(475, 42);
           this.buttonEditExternalAccount.Name = "buttonEditExternalAccount";
           this.buttonEditExternalAccount.Size = new System.Drawing.Size(100, 25);
           this.buttonEditExternalAccount.TabIndex = 46;
           this.buttonEditExternalAccount.Text = "&Edit...";
           this.buttonEditExternalAccount.UseVisualStyleBackColor = true;
           this.buttonEditExternalAccount.Click += new System.EventHandler(this.buttonEditExternalAccount_Click);
           // 
           // buttonDeleteExternalAccount
           // 
           this.buttonDeleteExternalAccount.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonDeleteExternalAccount.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonDeleteExternalAccount.Location = new System.Drawing.Point(475, 73);
           this.buttonDeleteExternalAccount.Name = "buttonDeleteExternalAccount";
           this.buttonDeleteExternalAccount.Size = new System.Drawing.Size(100, 25);
           this.buttonDeleteExternalAccount.TabIndex = 45;
           this.buttonDeleteExternalAccount.Text = "Remove";
           this.buttonDeleteExternalAccount.UseVisualStyleBackColor = true;
           this.buttonDeleteExternalAccount.Click += new System.EventHandler(this.buttonDeleteExternalAccount_Click);
           // 
           // buttonAddExternalAccount
           // 
           this.buttonAddExternalAccount.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonAddExternalAccount.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonAddExternalAccount.Location = new System.Drawing.Point(475, 11);
           this.buttonAddExternalAccount.Name = "buttonAddExternalAccount";
           this.buttonAddExternalAccount.Size = new System.Drawing.Size(100, 25);
           this.buttonAddExternalAccount.TabIndex = 44;
           this.buttonAddExternalAccount.Text = "&Add...";
           this.buttonAddExternalAccount.UseVisualStyleBackColor = true;
           this.buttonAddExternalAccount.Click += new System.EventHandler(this.buttonAddExternalAccount_Click);
           // 
           // listFetchAccounts
           // 
           this.listFetchAccounts.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listFetchAccounts.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3});
           this.listFetchAccounts.FullRowSelect = true;
           this.listFetchAccounts.Location = new System.Drawing.Point(12, 11);
           this.listFetchAccounts.Name = "listFetchAccounts";
           this.listFetchAccounts.Size = new System.Drawing.Size(457, 359);
           this.listFetchAccounts.TabIndex = 43;
           this.listFetchAccounts.UseCompatibleStateImageBehavior = false;
           this.listFetchAccounts.View = System.Windows.Forms.View.Details;
           this.listFetchAccounts.SelectedIndexChanged += new System.EventHandler(this.listFetchAccounts_SelectedIndexChanged);
           this.listFetchAccounts.DoubleClick += new System.EventHandler(this.listFetchAccounts_DoubleClick);
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "Name";
           this.columnHeader1.Width = 150;
           // 
           // columnHeader2
           // 
           this.columnHeader2.Text = "Enabled";
           this.columnHeader2.Width = 100;
           // 
           // columnHeader3
           // 
           this.columnHeader3.Text = "Next download";
           this.columnHeader3.Width = 120;
           // 
           // tabPageRules
           // 
           this.tabPageRules.Controls.Add(this.rules);
           this.tabPageRules.Location = new System.Drawing.Point(4, 22);
           this.tabPageRules.Name = "tabPageRules";
           this.tabPageRules.Size = new System.Drawing.Size(725, 369);
           this.tabPageRules.TabIndex = 5;
           this.tabPageRules.Text = "Rules";
           this.tabPageRules.UseVisualStyleBackColor = true;
           // 
           // rules
           // 
           this.rules.Dock = System.Windows.Forms.DockStyle.Fill;
           this.rules.Location = new System.Drawing.Point(0, 0);
           this.rules.Name = "rules";
           this.rules.Size = new System.Drawing.Size(725, 369);
           this.rules.TabIndex = 0;
           // 
           // tabPageActiveDirectory
           // 
           this.tabPageActiveDirectory.Controls.Add(this.labelAccountADInfo);
           this.tabPageActiveDirectory.Controls.Add(this.labelUsername);
           this.tabPageActiveDirectory.Controls.Add(this.labelADDomain);
           this.tabPageActiveDirectory.Controls.Add(this.textADUsername);
           this.tabPageActiveDirectory.Controls.Add(this.textADDomain);
           this.tabPageActiveDirectory.Controls.Add(this.checkAccountIsAD);
           this.tabPageActiveDirectory.Location = new System.Drawing.Point(4, 22);
           this.tabPageActiveDirectory.Name = "tabPageActiveDirectory";
           this.tabPageActiveDirectory.Size = new System.Drawing.Size(725, 369);
           this.tabPageActiveDirectory.TabIndex = 7;
           this.tabPageActiveDirectory.Text = "Active Directory";
           this.tabPageActiveDirectory.UseVisualStyleBackColor = true;
           // 
           // labelAccountADInfo
           // 
           this.labelAccountADInfo.Location = new System.Drawing.Point(23, 14);
           this.labelAccountADInfo.Name = "labelAccountADInfo";
           this.labelAccountADInfo.Size = new System.Drawing.Size(470, 50);
           this.labelAccountADInfo.TabIndex = 22;
           this.labelAccountADInfo.Text = "By entering the fields below, you can connect this account to an Active Directory" +
               ". When a user connects to the server, hMailServer will use the Active Directory " +
               "to validate the user\'s password.";
           // 
           // labelUsername
           // 
           this.labelUsername.AutoSize = true;
           this.labelUsername.Location = new System.Drawing.Point(46, 140);
           this.labelUsername.Name = "labelUsername";
           this.labelUsername.Size = new System.Drawing.Size(58, 13);
           this.labelUsername.TabIndex = 21;
           this.labelUsername.Text = "User name";
           // 
           // labelADDomain
           // 
           this.labelADDomain.AutoSize = true;
           this.labelADDomain.Location = new System.Drawing.Point(46, 93);
           this.labelADDomain.Name = "labelADDomain";
           this.labelADDomain.Size = new System.Drawing.Size(43, 13);
           this.labelADDomain.TabIndex = 19;
           this.labelADDomain.Text = "Domain";
           // 
           // textADUsername
           // 
           this.textADUsername.Location = new System.Drawing.Point(49, 156);
           this.textADUsername.Name = "textADUsername";
           this.textADUsername.Number = 0;
           this.textADUsername.Numeric = false;
           this.textADUsername.Size = new System.Drawing.Size(215, 20);
           this.textADUsername.TabIndex = 20;
           // 
           // textADDomain
           // 
           this.textADDomain.Location = new System.Drawing.Point(49, 109);
           this.textADDomain.Name = "textADDomain";
           this.textADDomain.Number = 0;
           this.textADDomain.Numeric = false;
           this.textADDomain.Size = new System.Drawing.Size(215, 20);
           this.textADDomain.TabIndex = 18;
           // 
           // checkAccountIsAD
           // 
           this.checkAccountIsAD.AutoSize = true;
           this.checkAccountIsAD.Location = new System.Drawing.Point(26, 67);
           this.checkAccountIsAD.Name = "checkAccountIsAD";
           this.checkAccountIsAD.Size = new System.Drawing.Size(143, 17);
           this.checkAccountIsAD.TabIndex = 17;
           this.checkAccountIsAD.Text = "Active Directory account";
           this.checkAccountIsAD.UseVisualStyleBackColor = true;
           this.checkAccountIsAD.CheckedChanged += new System.EventHandler(this.checkAccountIsAD_CheckedChanged);
           // 
           // tabPageAdvanced
           // 
           this.tabPageAdvanced.Controls.Add(this.buttonEditFolders);
           this.tabPageAdvanced.Controls.Add(this.buttonUnlock);
           this.tabPageAdvanced.Controls.Add(this.buttonEmptyAccount);
           this.tabPageAdvanced.Controls.Add(this.labelOther);
           this.tabPageAdvanced.Controls.Add(this.labelLastName);
           this.tabPageAdvanced.Controls.Add(this.labelFirstName);
           this.tabPageAdvanced.Controls.Add(this.labelPersonalInformation);
           this.tabPageAdvanced.Controls.Add(this.textLastName);
           this.tabPageAdvanced.Controls.Add(this.textFirstName);
           this.tabPageAdvanced.Location = new System.Drawing.Point(4, 22);
           this.tabPageAdvanced.Name = "tabPageAdvanced";
           this.tabPageAdvanced.Size = new System.Drawing.Size(725, 369);
           this.tabPageAdvanced.TabIndex = 8;
           this.tabPageAdvanced.Text = "Advanced";
           this.tabPageAdvanced.UseVisualStyleBackColor = true;
           // 
           // buttonEditFolders
           // 
           this.buttonEditFolders.Location = new System.Drawing.Point(32, 112);
           this.buttonEditFolders.Name = "buttonEditFolders";
           this.buttonEditFolders.Size = new System.Drawing.Size(100, 25);
           this.buttonEditFolders.TabIndex = 10;
           this.buttonEditFolders.Text = "Edit folders";
           this.buttonEditFolders.UseVisualStyleBackColor = true;
           this.buttonEditFolders.Click += new System.EventHandler(this.buttonEditFolders_Click);
           // 
           // buttonUnlock
           // 
           this.buttonUnlock.Location = new System.Drawing.Point(32, 178);
           this.buttonUnlock.Name = "buttonUnlock";
           this.buttonUnlock.Size = new System.Drawing.Size(100, 25);
           this.buttonUnlock.TabIndex = 9;
           this.buttonUnlock.Text = "Unlock";
           this.buttonUnlock.UseVisualStyleBackColor = true;
           this.buttonUnlock.Click += new System.EventHandler(this.buttonUnlock_Click);
           // 
           // buttonEmptyAccount
           // 
           this.buttonEmptyAccount.Location = new System.Drawing.Point(32, 144);
           this.buttonEmptyAccount.Name = "buttonEmptyAccount";
           this.buttonEmptyAccount.Size = new System.Drawing.Size(100, 25);
           this.buttonEmptyAccount.TabIndex = 8;
           this.buttonEmptyAccount.Text = "Empty account";
           this.buttonEmptyAccount.UseVisualStyleBackColor = true;
           this.buttonEmptyAccount.Click += new System.EventHandler(this.buttonEmptyAccount_Click);
           // 
           // labelOther
           // 
           this.labelOther.AutoSize = true;
           this.labelOther.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelOther.Location = new System.Drawing.Point(14, 91);
           this.labelOther.Name = "labelOther";
           this.labelOther.Size = new System.Drawing.Size(83, 13);
           this.labelOther.TabIndex = 7;
           this.labelOther.Text = "Other actions";
           // 
           // labelLastName
           // 
           this.labelLastName.AutoSize = true;
           this.labelLastName.Location = new System.Drawing.Point(195, 35);
           this.labelLastName.Name = "labelLastName";
           this.labelLastName.Size = new System.Drawing.Size(56, 13);
           this.labelLastName.TabIndex = 5;
           this.labelLastName.Text = "Last name";
           // 
           // labelFirstName
           // 
           this.labelFirstName.AutoSize = true;
           this.labelFirstName.Location = new System.Drawing.Point(30, 35);
           this.labelFirstName.Name = "labelFirstName";
           this.labelFirstName.Size = new System.Drawing.Size(55, 13);
           this.labelFirstName.TabIndex = 3;
           this.labelFirstName.Text = "First name";
           // 
           // labelPersonalInformation
           // 
           this.labelPersonalInformation.AutoSize = true;
           this.labelPersonalInformation.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelPersonalInformation.Location = new System.Drawing.Point(14, 10);
           this.labelPersonalInformation.Name = "labelPersonalInformation";
           this.labelPersonalInformation.Size = new System.Drawing.Size(122, 13);
           this.labelPersonalInformation.TabIndex = 2;
           this.labelPersonalInformation.Text = "Personal information";
           // 
           // textLastName
           // 
           this.textLastName.Location = new System.Drawing.Point(197, 54);
           this.textLastName.Name = "textLastName";
           this.textLastName.Number = 0;
           this.textLastName.Numeric = false;
           this.textLastName.Size = new System.Drawing.Size(159, 20);
           this.textLastName.TabIndex = 6;
           // 
           // textFirstName
           // 
           this.textFirstName.Location = new System.Drawing.Point(32, 54);
           this.textFirstName.Name = "textFirstName";
           this.textFirstName.Number = 0;
           this.textFirstName.Numeric = false;
           this.textFirstName.Size = new System.Drawing.Size(159, 20);
           this.textFirstName.TabIndex = 4;
           // 
           // ucAccount
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.tabControl);
           this.Name = "ucAccount";
           this.Size = new System.Drawing.Size(733, 395);
           this.tabControl.ResumeLayout(false);
           this.tabPageGeneral.ResumeLayout(false);
           this.tabPageGeneral.PerformLayout();
           this.tabPageAutoReply.ResumeLayout(false);
           this.tabPageAutoReply.PerformLayout();
           this.tabPageForwarding.ResumeLayout(false);
           this.tabPageForwarding.PerformLayout();
           this.tabPageSignature.ResumeLayout(false);
           this.tabPageSignature.PerformLayout();
           this.tabPageExternalAccounts.ResumeLayout(false);
           this.tabPageRules.ResumeLayout(false);
           this.tabPageActiveDirectory.ResumeLayout(false);
           this.tabPageActiveDirectory.PerformLayout();
           this.tabPageAdvanced.ResumeLayout(false);
           this.tabPageAdvanced.PerformLayout();
           this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage tabPageGeneral;
        private System.Windows.Forms.TabPage tabPageAutoReply;
        private System.Windows.Forms.TabPage tabPageForwarding;
        private System.Windows.Forms.TabPage tabPageSignature;
        private System.Windows.Forms.TabPage tabPageExternalAccounts;
       private System.Windows.Forms.TabPage tabPageRules;
        private System.Windows.Forms.TabPage tabPageActiveDirectory;
       private System.Windows.Forms.TabPage tabPageAdvanced;
       private System.Windows.Forms.Label labelAddress;
       private hMailServer.Shared.ucText textMaxSize;
       private System.Windows.Forms.Label labelMaxSize;
       private hMailServer.Shared.ucText textSize;
       private System.Windows.Forms.Label labelSize;
       private System.Windows.Forms.Label labelPassword;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnabled;
       private hMailServer.Shared.ucText textLastLogonTime;
       private System.Windows.Forms.Label labelLastLogonTime;
       private System.Windows.Forms.Label labelAdministrationLevel;
       private hMailServer.Administrator.Controls.ucComboBox comboAdministrationLevel;
       private System.Windows.Forms.Label labelAutoReplySubject;
       private hMailServer.Shared.ucText textVacationMessageSubject;
       private hMailServer.Administrator.Controls.ucCheckbox checkVacationMessageEnable;
       private System.Windows.Forms.Label labelAutoReplyText;
       private hMailServer.Shared.ucText textVacationMessageText;
       private hMailServer.Administrator.Controls.ucCheckbox checkVacationMessageExpires;
       private hMailServer.Administrator.Controls.ucCheckbox checkForwardEnabled;
       private hMailServer.Administrator.Controls.ucCheckbox checkForwardKeepOriginal;
       private hMailServer.Administrator.Controls.ucEmailEdit textForwardAddress;
       private System.Windows.Forms.Label labelForwardAddress;
       private System.Windows.Forms.Label labelSignatureHTML;
       private hMailServer.Shared.ucText textSignatureHTML;
       private System.Windows.Forms.Label labelSignaturePlainText;
       private hMailServer.Shared.ucText textSignaturePlainText;
       private hMailServer.Administrator.Controls.ucCheckbox checkSignatureEnabled;
       private System.Windows.Forms.Button buttonEditExternalAccount;
       private System.Windows.Forms.Button buttonDeleteExternalAccount;
       private System.Windows.Forms.Button buttonAddExternalAccount;
       private ucListView listFetchAccounts;
       private System.Windows.Forms.Label labelADDomain;
       private hMailServer.Shared.ucText textADDomain;
       private hMailServer.Administrator.Controls.ucCheckbox checkAccountIsAD;
       private System.Windows.Forms.Label labelAccountADInfo;
       private System.Windows.Forms.Label labelUsername;
       private hMailServer.Shared.ucText textADUsername;
       private System.Windows.Forms.Button buttonUnlock;
       private System.Windows.Forms.Button buttonEmptyAccount;
       private System.Windows.Forms.Label labelOther;
       private hMailServer.Shared.ucText textLastName;
       private System.Windows.Forms.Label labelLastName;
       private hMailServer.Shared.ucText textFirstName;
       private System.Windows.Forms.Label labelFirstName;
       private System.Windows.Forms.Label labelPersonalInformation;
       private hMailServer.Administrator.Controls.ucDateTimePicker dateVacationMessageExpiresDate;
       private hMailServer.Administrator.Controls.ucEmailEdit textAddress;
       private ucRules rules;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.ColumnHeader columnHeader2;
       private hMailServer.Administrator.Controls.ucPassword textPassword;
       private System.Windows.Forms.ColumnHeader columnHeader3;
       private System.Windows.Forms.Button buttonEditFolders;
    }
}
