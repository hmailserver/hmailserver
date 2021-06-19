namespace hMailServer.Administrator
{
    partial class ucDomain
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
           this.textDomainName = new hMailServer.Shared.ucText();
           this.checkEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.labelDomain = new System.Windows.Forms.Label();
           this.tabPageNames = new System.Windows.Forms.TabPage();
           this.buttonAdd = new System.Windows.Forms.Button();
           this.buttonEdit = new System.Windows.Forms.Button();
           this.listAliases = new hMailServer.Administrator.ucListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.buttonRemove = new System.Windows.Forms.Button();
           this.tabPageSignature = new System.Windows.Forms.TabPage();
           this.textHTMLSignature = new hMailServer.Shared.ucText();
           this.labelHTMLSignature = new System.Windows.Forms.Label();
           this.labelPlainTextSignature = new System.Windows.Forms.Label();
           this.textPlainTextSignature = new hMailServer.Shared.ucText();
           this.radioSMAppendToAccountSignature = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioSMOverwriteAccountSignature = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioSMSetIfNotSpecifiedInAccount = new hMailServer.Administrator.Controls.ucRadioButton();
           this.checkSignatureEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAddSignaturesToLocalMail = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAddSignaturesToReplies = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabPageLimits = new System.Windows.Forms.TabPage();
           this.textMaxNumberOfDistributionLists = new hMailServer.Shared.ucText();
           this.lblMaxNoOfDistributionLists = new System.Windows.Forms.Label();
           this.lblMaxNoOfAliases = new System.Windows.Forms.Label();
           this.lblMaxNoOfAccounts = new System.Windows.Forms.Label();
           this.lblMaxAccountSize = new System.Windows.Forms.Label();
           this.lblMaxMessageSize = new System.Windows.Forms.Label();
           this.lblAllocatedSize = new System.Windows.Forms.Label();
           this.labelMaxSize = new System.Windows.Forms.Label();
           this.textMaxNumberOfAliases = new hMailServer.Shared.ucText();
           this.textMaxNumberOfAccounts = new hMailServer.Shared.ucText();
           this.textMaxAccountSize = new hMailServer.Shared.ucText();
           this.textMaxMessageSize = new hMailServer.Shared.ucText();
           this.textAllocatedSize = new hMailServer.Shared.ucText();
           this.textMaxSize = new hMailServer.Shared.ucText();
           this.checkMaxNumberOfDistributionListsEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkMaxNumberOfAliasesEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkMaxNumberOfAccountsEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabPageDKIM = new System.Windows.Forms.TabPage();
           this.buttonSelectCertificate = new System.Windows.Forms.Button();
           this.panel3 = new System.Windows.Forms.Panel();
           this.radioDKIMSigningAlgorithmSHA256 = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioDKIMSigningAlgorithmSHA1 = new hMailServer.Administrator.Controls.ucRadioButton();
           this.panel2 = new System.Windows.Forms.Panel();
           this.radioDKIMBodyMethodRelaxed = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioDKIMBodyMethodSimple = new hMailServer.Administrator.Controls.ucRadioButton();
           this.panel1 = new System.Windows.Forms.Panel();
           this.radioDKIMHeaderMethodRelaxed = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioDKIMHeaderMethodSimple = new hMailServer.Administrator.Controls.ucRadioButton();
           this.labelDKIMSigningAlgorithm = new System.Windows.Forms.Label();
           this.labelDKIMBodyCanonicalizationMethod = new System.Windows.Forms.Label();
           this.labelDKIMHeaderCanonicalizationMethod = new System.Windows.Forms.Label();
           this.labelDKIMSelector = new System.Windows.Forms.Label();
           this.textDKIMSelector = new hMailServer.Shared.ucText();
           this.labelDKIMPrivateKeyFile = new System.Windows.Forms.Label();
           this.textDKIMPrivateKeyFile = new hMailServer.Shared.ucText();
           this.checkDKIMSignEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkDKIMSignAliasesEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabPageAdvanced = new System.Windows.Forms.TabPage();
           this.textCatchAllAddress = new hMailServer.Administrator.Controls.ucEmailEdit();
           this.labelCatchAll = new System.Windows.Forms.Label();
           this.comboPlusAddressingCharacter = new hMailServer.Administrator.Controls.ucComboBox();
           this.checkGreyListingEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.labelGrayListing = new System.Windows.Forms.Label();
           this.labelPlusAddressingCharacter = new System.Windows.Forms.Label();
           this.labelPlusAddressing = new System.Windows.Forms.Label();
           this.checkUsePlusAddressing = new hMailServer.Administrator.Controls.ucCheckbox();
           this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
           this.tabControl.SuspendLayout();
           this.tabPageGeneral.SuspendLayout();
           this.tabPageNames.SuspendLayout();
           this.tabPageSignature.SuspendLayout();
           this.tabPageLimits.SuspendLayout();
           this.tabPageDKIM.SuspendLayout();
           this.panel3.SuspendLayout();
           this.panel2.SuspendLayout();
           this.panel1.SuspendLayout();
           this.tabPageAdvanced.SuspendLayout();
           this.SuspendLayout();
           // 
           // tabControl
           // 
           this.tabControl.Controls.Add(this.tabPageGeneral);
           this.tabControl.Controls.Add(this.tabPageNames);
           this.tabControl.Controls.Add(this.tabPageSignature);
           this.tabControl.Controls.Add(this.tabPageLimits);
           this.tabControl.Controls.Add(this.tabPageDKIM);
           this.tabControl.Controls.Add(this.tabPageAdvanced);
           this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
           this.tabControl.Location = new System.Drawing.Point(0, 0);
           this.tabControl.Name = "tabControl";
           this.tabControl.SelectedIndex = 0;
           this.tabControl.Size = new System.Drawing.Size(562, 380);
           this.tabControl.TabIndex = 0;
           this.tabControl.Enter += new System.EventHandler(this.tabControl_Enter);
           this.tabControl.SelectedIndexChanged += new System.EventHandler(this.tabControl_SelectedIndexChanged);
           // 
           // tabPageGeneral
           // 
           this.tabPageGeneral.Controls.Add(this.textDomainName);
           this.tabPageGeneral.Controls.Add(this.checkEnabled);
           this.tabPageGeneral.Controls.Add(this.labelDomain);
           this.tabPageGeneral.Location = new System.Drawing.Point(4, 22);
           this.tabPageGeneral.Name = "tabPageGeneral";
           this.tabPageGeneral.Padding = new System.Windows.Forms.Padding(3);
           this.tabPageGeneral.Size = new System.Drawing.Size(554, 354);
           this.tabPageGeneral.TabIndex = 0;
           this.tabPageGeneral.Text = "General";
           this.tabPageGeneral.UseVisualStyleBackColor = true;
           // 
           // textDomainName
           // 
           this.textDomainName.Location = new System.Drawing.Point(8, 24);
           this.textDomainName.Name = "textDomainName";
           this.textDomainName.Number = 0;
           this.textDomainName.Numeric = false;
           this.textDomainName.Size = new System.Drawing.Size(282, 20);
           this.textDomainName.TabIndex = 0;
           // 
           // checkEnabled
           // 
           this.checkEnabled.AutoSize = true;
           this.checkEnabled.Checked = true;
           this.checkEnabled.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkEnabled.Location = new System.Drawing.Point(8, 56);
           this.checkEnabled.Name = "checkEnabled";
           this.checkEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkEnabled.TabIndex = 14;
           this.checkEnabled.Text = "Enabled";
           this.checkEnabled.UseVisualStyleBackColor = true;
           // 
           // labelDomain
           // 
           this.labelDomain.AutoSize = true;
           this.labelDomain.Location = new System.Drawing.Point(6, 8);
           this.labelDomain.Name = "labelDomain";
           this.labelDomain.Size = new System.Drawing.Size(43, 13);
           this.labelDomain.TabIndex = 0;
           this.labelDomain.Text = "Domain";
           // 
           // tabPageNames
           // 
           this.tabPageNames.Controls.Add(this.buttonAdd);
           this.tabPageNames.Controls.Add(this.buttonEdit);
           this.tabPageNames.Controls.Add(this.listAliases);
           this.tabPageNames.Controls.Add(this.buttonRemove);
           this.tabPageNames.Location = new System.Drawing.Point(4, 22);
           this.tabPageNames.Name = "tabPageNames";
           this.tabPageNames.Padding = new System.Windows.Forms.Padding(3);
           this.tabPageNames.Size = new System.Drawing.Size(554, 354);
           this.tabPageNames.TabIndex = 1;
           this.tabPageNames.Text = "Names";
           this.tabPageNames.UseVisualStyleBackColor = true;
           // 
           // buttonAdd
           // 
           this.buttonAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonAdd.Location = new System.Drawing.Point(448, 8);
           this.buttonAdd.Name = "buttonAdd";
           this.buttonAdd.Size = new System.Drawing.Size(100, 25);
           this.buttonAdd.TabIndex = 1;
           this.buttonAdd.Text = "Add...";
           this.buttonAdd.UseVisualStyleBackColor = true;
           this.buttonAdd.Click += new System.EventHandler(this.buttonAdd_Click);
           // 
           // buttonEdit
           // 
           this.buttonEdit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonEdit.Location = new System.Drawing.Point(448, 40);
           this.buttonEdit.Name = "buttonEdit";
           this.buttonEdit.Size = new System.Drawing.Size(100, 25);
           this.buttonEdit.TabIndex = 4;
           this.buttonEdit.Text = "&Edit...";
           this.buttonEdit.UseVisualStyleBackColor = true;
           this.buttonEdit.Click += new System.EventHandler(this.buttonEdit_Click);
           // 
           // listAliases
           // 
           this.listAliases.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listAliases.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
           this.listAliases.FullRowSelect = true;
           this.listAliases.Location = new System.Drawing.Point(8, 8);
           this.listAliases.Name = "listAliases";
           this.listAliases.Size = new System.Drawing.Size(432, 334);
           this.listAliases.TabIndex = 3;
           this.listAliases.UseCompatibleStateImageBehavior = false;
           this.listAliases.View = System.Windows.Forms.View.Details;
           this.listAliases.SelectedIndexChanged += new System.EventHandler(this.listAliases_SelectedIndexChanged);
           this.listAliases.DoubleClick += new System.EventHandler(this.listAliases_DoubleClick);
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "Name";
           this.columnHeader1.Width = 180;
           // 
           // buttonRemove
           // 
           this.buttonRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonRemove.Location = new System.Drawing.Point(448, 72);
           this.buttonRemove.Name = "buttonRemove";
           this.buttonRemove.Size = new System.Drawing.Size(100, 25);
           this.buttonRemove.TabIndex = 2;
           this.buttonRemove.Text = "Remove";
           this.buttonRemove.UseVisualStyleBackColor = true;
           this.buttonRemove.Click += new System.EventHandler(this.buttonRemove_Click);
           // 
           // tabPageSignature
           // 
           this.tabPageSignature.Controls.Add(this.textHTMLSignature);
           this.tabPageSignature.Controls.Add(this.labelHTMLSignature);
           this.tabPageSignature.Controls.Add(this.labelPlainTextSignature);
           this.tabPageSignature.Controls.Add(this.textPlainTextSignature);
           this.tabPageSignature.Controls.Add(this.radioSMAppendToAccountSignature);
           this.tabPageSignature.Controls.Add(this.radioSMOverwriteAccountSignature);
           this.tabPageSignature.Controls.Add(this.radioSMSetIfNotSpecifiedInAccount);
           this.tabPageSignature.Controls.Add(this.checkSignatureEnabled);
           this.tabPageSignature.Controls.Add(this.checkAddSignaturesToLocalMail);
           this.tabPageSignature.Controls.Add(this.checkAddSignaturesToReplies);
           this.tabPageSignature.Location = new System.Drawing.Point(4, 22);
           this.tabPageSignature.Name = "tabPageSignature";
           this.tabPageSignature.Size = new System.Drawing.Size(554, 354);
           this.tabPageSignature.TabIndex = 2;
           this.tabPageSignature.Text = "Signature";
           this.tabPageSignature.UseVisualStyleBackColor = true;
           // 
           // textHTMLSignature
           // 
           this.textHTMLSignature.Location = new System.Drawing.Point(16, 256);
           this.textHTMLSignature.Multiline = true;
           this.textHTMLSignature.Name = "textHTMLSignature";
           this.textHTMLSignature.Number = 0;
           this.textHTMLSignature.Numeric = false;
           this.textHTMLSignature.Size = new System.Drawing.Size(344, 60);
           this.textHTMLSignature.TabIndex = 12;
           // 
           // labelHTMLSignature
           // 
           this.labelHTMLSignature.AutoSize = true;
           this.labelHTMLSignature.Location = new System.Drawing.Point(13, 240);
           this.labelHTMLSignature.Name = "labelHTMLSignature";
           this.labelHTMLSignature.Size = new System.Drawing.Size(83, 13);
           this.labelHTMLSignature.TabIndex = 11;
           this.labelHTMLSignature.Text = "HTML signature";
           // 
           // labelPlainTextSignature
           // 
           this.labelPlainTextSignature.AutoSize = true;
           this.labelPlainTextSignature.Location = new System.Drawing.Point(16, 156);
           this.labelPlainTextSignature.Name = "labelPlainTextSignature";
           this.labelPlainTextSignature.Size = new System.Drawing.Size(96, 13);
           this.labelPlainTextSignature.TabIndex = 9;
           this.labelPlainTextSignature.Text = "Plain text signature";
           // 
           // textPlainTextSignature
           // 
           this.textPlainTextSignature.Location = new System.Drawing.Point(16, 172);
           this.textPlainTextSignature.Multiline = true;
           this.textPlainTextSignature.Name = "textPlainTextSignature";
           this.textPlainTextSignature.Number = 0;
           this.textPlainTextSignature.Numeric = false;
           this.textPlainTextSignature.Size = new System.Drawing.Size(344, 60);
           this.textPlainTextSignature.TabIndex = 10;
           // 
           // radioSMAppendToAccountSignature
           // 
           this.radioSMAppendToAccountSignature.AutoSize = true;
           this.radioSMAppendToAccountSignature.Location = new System.Drawing.Point(32, 80);
           this.radioSMAppendToAccountSignature.Name = "radioSMAppendToAccountSignature";
           this.radioSMAppendToAccountSignature.Size = new System.Drawing.Size(162, 17);
           this.radioSMAppendToAccountSignature.TabIndex = 8;
           this.radioSMAppendToAccountSignature.TabStop = true;
           this.radioSMAppendToAccountSignature.Text = "Append to account signature";
           this.radioSMAppendToAccountSignature.UseVisualStyleBackColor = true;
           // 
           // radioSMOverwriteAccountSignature
           // 
           this.radioSMOverwriteAccountSignature.AutoSize = true;
           this.radioSMOverwriteAccountSignature.Location = new System.Drawing.Point(32, 57);
           this.radioSMOverwriteAccountSignature.Name = "radioSMOverwriteAccountSignature";
           this.radioSMOverwriteAccountSignature.Size = new System.Drawing.Size(158, 17);
           this.radioSMOverwriteAccountSignature.TabIndex = 7;
           this.radioSMOverwriteAccountSignature.TabStop = true;
           this.radioSMOverwriteAccountSignature.Text = "Overwrite account signature";
           this.radioSMOverwriteAccountSignature.UseVisualStyleBackColor = true;
           // 
           // radioSMSetIfNotSpecifiedInAccount
           // 
           this.radioSMSetIfNotSpecifiedInAccount.AutoSize = true;
           this.radioSMSetIfNotSpecifiedInAccount.Location = new System.Drawing.Point(32, 34);
           this.radioSMSetIfNotSpecifiedInAccount.Name = "radioSMSetIfNotSpecifiedInAccount";
           this.radioSMSetIfNotSpecifiedInAccount.Size = new System.Drawing.Size(312, 17);
           this.radioSMSetIfNotSpecifiedInAccount.TabIndex = 6;
           this.radioSMSetIfNotSpecifiedInAccount.TabStop = true;
           this.radioSMSetIfNotSpecifiedInAccount.Text = "Use signature if none has been specified in sender\'s account";
           this.radioSMSetIfNotSpecifiedInAccount.UseVisualStyleBackColor = true;
           this.radioSMSetIfNotSpecifiedInAccount.CheckedChanged += new System.EventHandler(this.radioSMSetIfNotSpecifiedInAccount_CheckedChanged);
           // 
           // checkSignatureEnabled
           // 
           this.checkSignatureEnabled.AutoSize = true;
           this.checkSignatureEnabled.Location = new System.Drawing.Point(16, 8);
           this.checkSignatureEnabled.Name = "checkSignatureEnabled";
           this.checkSignatureEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkSignatureEnabled.TabIndex = 2;
           this.checkSignatureEnabled.Text = "Enabled";
           this.checkSignatureEnabled.UseVisualStyleBackColor = true;
           this.checkSignatureEnabled.CheckedChanged += new System.EventHandler(this.checkSignatureEnabled_CheckedChanged);
           // 
           // checkAddSignaturesToLocalMail
           // 
           this.checkAddSignaturesToLocalMail.AutoSize = true;
           this.checkAddSignaturesToLocalMail.Location = new System.Drawing.Point(16, 128);
           this.checkAddSignaturesToLocalMail.Name = "checkAddSignaturesToLocalMail";
           this.checkAddSignaturesToLocalMail.Size = new System.Drawing.Size(160, 17);
           this.checkAddSignaturesToLocalMail.TabIndex = 1;
           this.checkAddSignaturesToLocalMail.Text = "Add signatures to local email";
           this.checkAddSignaturesToLocalMail.UseVisualStyleBackColor = true;
           // 
           // checkAddSignaturesToReplies
           // 
           this.checkAddSignaturesToReplies.AutoSize = true;
           this.checkAddSignaturesToReplies.Location = new System.Drawing.Point(16, 104);
           this.checkAddSignaturesToReplies.Name = "checkAddSignaturesToReplies";
           this.checkAddSignaturesToReplies.Size = new System.Drawing.Size(141, 17);
           this.checkAddSignaturesToReplies.TabIndex = 0;
           this.checkAddSignaturesToReplies.Text = "Add signatures to replies";
           this.checkAddSignaturesToReplies.UseVisualStyleBackColor = true;
           // 
           // tabPageLimits
           // 
           this.tabPageLimits.Controls.Add(this.textMaxNumberOfDistributionLists);
           this.tabPageLimits.Controls.Add(this.lblMaxNoOfDistributionLists);
           this.tabPageLimits.Controls.Add(this.lblMaxNoOfAliases);
           this.tabPageLimits.Controls.Add(this.lblMaxNoOfAccounts);
           this.tabPageLimits.Controls.Add(this.lblMaxAccountSize);
           this.tabPageLimits.Controls.Add(this.lblMaxMessageSize);
           this.tabPageLimits.Controls.Add(this.lblAllocatedSize);
           this.tabPageLimits.Controls.Add(this.labelMaxSize);
           this.tabPageLimits.Controls.Add(this.textMaxNumberOfAliases);
           this.tabPageLimits.Controls.Add(this.textMaxNumberOfAccounts);
           this.tabPageLimits.Controls.Add(this.textMaxAccountSize);
           this.tabPageLimits.Controls.Add(this.textMaxMessageSize);
           this.tabPageLimits.Controls.Add(this.textAllocatedSize);
           this.tabPageLimits.Controls.Add(this.textMaxSize);
           this.tabPageLimits.Controls.Add(this.checkMaxNumberOfDistributionListsEnabled);
           this.tabPageLimits.Controls.Add(this.checkMaxNumberOfAliasesEnabled);
           this.tabPageLimits.Controls.Add(this.checkMaxNumberOfAccountsEnabled);
           this.tabPageLimits.Location = new System.Drawing.Point(4, 22);
           this.tabPageLimits.Name = "tabPageLimits";
           this.tabPageLimits.Size = new System.Drawing.Size(554, 354);
           this.tabPageLimits.TabIndex = 3;
           this.tabPageLimits.Text = "Limits";
           this.tabPageLimits.UseVisualStyleBackColor = true;
           // 
           // textMaxNumberOfDistributionLists
           // 
           this.textMaxNumberOfDistributionLists.Location = new System.Drawing.Point(32, 240);
           this.textMaxNumberOfDistributionLists.Name = "textMaxNumberOfDistributionLists";
           this.textMaxNumberOfDistributionLists.Number = 0;
           this.textMaxNumberOfDistributionLists.Numeric = true;
           this.textMaxNumberOfDistributionLists.Size = new System.Drawing.Size(73, 20);
           this.textMaxNumberOfDistributionLists.TabIndex = 17;
           // 
           // lblMaxNoOfDistributionLists
           // 
           this.lblMaxNoOfDistributionLists.AutoSize = true;
           this.lblMaxNoOfDistributionLists.Location = new System.Drawing.Point(8, 224);
           this.lblMaxNoOfDistributionLists.Name = "lblMaxNoOfDistributionLists";
           this.lblMaxNoOfDistributionLists.Size = new System.Drawing.Size(150, 13);
           this.lblMaxNoOfDistributionLists.TabIndex = 16;
           this.lblMaxNoOfDistributionLists.Text = "Max number of distribution lists";
           // 
           // lblMaxNoOfAliases
           // 
           this.lblMaxNoOfAliases.AutoSize = true;
           this.lblMaxNoOfAliases.Location = new System.Drawing.Point(8, 187);
           this.lblMaxNoOfAliases.Name = "lblMaxNoOfAliases";
           this.lblMaxNoOfAliases.Size = new System.Drawing.Size(112, 13);
           this.lblMaxNoOfAliases.TabIndex = 13;
           this.lblMaxNoOfAliases.Text = "Max number of aliases";
           // 
           // lblMaxNoOfAccounts
           // 
           this.lblMaxNoOfAccounts.AutoSize = true;
           this.lblMaxNoOfAccounts.Location = new System.Drawing.Point(8, 145);
           this.lblMaxNoOfAccounts.Name = "lblMaxNoOfAccounts";
           this.lblMaxNoOfAccounts.Size = new System.Drawing.Size(124, 13);
           this.lblMaxNoOfAccounts.TabIndex = 10;
           this.lblMaxNoOfAccounts.Text = "Max number of accounts";
           // 
           // lblMaxAccountSize
           // 
           this.lblMaxAccountSize.AutoSize = true;
           this.lblMaxAccountSize.Location = new System.Drawing.Point(5, 103);
           this.lblMaxAccountSize.Name = "lblMaxAccountSize";
           this.lblMaxAccountSize.Size = new System.Drawing.Size(132, 13);
           this.lblMaxAccountSize.TabIndex = 7;
           this.lblMaxAccountSize.Text = "Max size of accounts (MB)";
           // 
           // lblMaxMessageSize
           // 
           this.lblMaxMessageSize.AutoSize = true;
           this.lblMaxMessageSize.Location = new System.Drawing.Point(5, 55);
           this.lblMaxMessageSize.Name = "lblMaxMessageSize";
           this.lblMaxMessageSize.Size = new System.Drawing.Size(116, 13);
           this.lblMaxMessageSize.TabIndex = 5;
           this.lblMaxMessageSize.Text = "Max message size (KB)";
           // 
           // lblAllocatedSize
           // 
           this.lblAllocatedSize.AutoSize = true;
           this.lblAllocatedSize.Location = new System.Drawing.Point(144, 8);
           this.lblAllocatedSize.Name = "lblAllocatedSize";
           this.lblAllocatedSize.Size = new System.Drawing.Size(97, 13);
           this.lblAllocatedSize.TabIndex = 2;
           this.lblAllocatedSize.Text = "Allocated size (MB)";
           // 
           // labelMaxSize
           // 
           this.labelMaxSize.AutoSize = true;
           this.labelMaxSize.Location = new System.Drawing.Point(8, 8);
           this.labelMaxSize.Name = "labelMaxSize";
           this.labelMaxSize.Size = new System.Drawing.Size(97, 13);
           this.labelMaxSize.TabIndex = 1;
           this.labelMaxSize.Text = "Maximum size (MB)";
           // 
           // textMaxNumberOfAliases
           // 
           this.textMaxNumberOfAliases.Location = new System.Drawing.Point(32, 204);
           this.textMaxNumberOfAliases.Name = "textMaxNumberOfAliases";
           this.textMaxNumberOfAliases.Number = 0;
           this.textMaxNumberOfAliases.Numeric = true;
           this.textMaxNumberOfAliases.Size = new System.Drawing.Size(73, 20);
           this.textMaxNumberOfAliases.TabIndex = 14;
           // 
           // textMaxNumberOfAccounts
           // 
           this.textMaxNumberOfAccounts.Location = new System.Drawing.Point(32, 162);
           this.textMaxNumberOfAccounts.Name = "textMaxNumberOfAccounts";
           this.textMaxNumberOfAccounts.Number = 0;
           this.textMaxNumberOfAccounts.Numeric = true;
           this.textMaxNumberOfAccounts.Size = new System.Drawing.Size(73, 20);
           this.textMaxNumberOfAccounts.TabIndex = 11;
           // 
           // textMaxAccountSize
           // 
           this.textMaxAccountSize.Location = new System.Drawing.Point(8, 120);
           this.textMaxAccountSize.Name = "textMaxAccountSize";
           this.textMaxAccountSize.Number = 0;
           this.textMaxAccountSize.Numeric = true;
           this.textMaxAccountSize.Size = new System.Drawing.Size(126, 20);
           this.textMaxAccountSize.TabIndex = 8;
           // 
           // textMaxMessageSize
           // 
           this.textMaxMessageSize.Location = new System.Drawing.Point(8, 72);
           this.textMaxMessageSize.Name = "textMaxMessageSize";
           this.textMaxMessageSize.Number = 0;
           this.textMaxMessageSize.Numeric = true;
           this.textMaxMessageSize.Size = new System.Drawing.Size(126, 20);
           this.textMaxMessageSize.TabIndex = 6;
           // 
           // textAllocatedSize
           // 
           this.textAllocatedSize.Location = new System.Drawing.Point(144, 24);
           this.textAllocatedSize.Name = "textAllocatedSize";
           this.textAllocatedSize.Number = 0;
           this.textAllocatedSize.Numeric = true;
           this.textAllocatedSize.ReadOnly = true;
           this.textAllocatedSize.Size = new System.Drawing.Size(126, 20);
           this.textAllocatedSize.TabIndex = 4;
           // 
           // textMaxSize
           // 
           this.textMaxSize.Location = new System.Drawing.Point(8, 24);
           this.textMaxSize.Name = "textMaxSize";
           this.textMaxSize.Number = 0;
           this.textMaxSize.Numeric = true;
           this.textMaxSize.Size = new System.Drawing.Size(126, 20);
           this.textMaxSize.TabIndex = 3;
           // 
           // checkMaxNumberOfDistributionListsEnabled
           // 
           this.checkMaxNumberOfDistributionListsEnabled.AutoSize = true;
           this.checkMaxNumberOfDistributionListsEnabled.Location = new System.Drawing.Point(11, 242);
           this.checkMaxNumberOfDistributionListsEnabled.Name = "checkMaxNumberOfDistributionListsEnabled";
           this.checkMaxNumberOfDistributionListsEnabled.Size = new System.Drawing.Size(15, 14);
           this.checkMaxNumberOfDistributionListsEnabled.TabIndex = 15;
           this.checkMaxNumberOfDistributionListsEnabled.UseVisualStyleBackColor = true;
           this.checkMaxNumberOfDistributionListsEnabled.CheckedChanged += new System.EventHandler(this.checkMaxNumberOfDistributionListsEnabled_CheckedChanged);
           // 
           // checkMaxNumberOfAliasesEnabled
           // 
           this.checkMaxNumberOfAliasesEnabled.AutoSize = true;
           this.checkMaxNumberOfAliasesEnabled.Location = new System.Drawing.Point(11, 206);
           this.checkMaxNumberOfAliasesEnabled.Name = "checkMaxNumberOfAliasesEnabled";
           this.checkMaxNumberOfAliasesEnabled.Size = new System.Drawing.Size(15, 14);
           this.checkMaxNumberOfAliasesEnabled.TabIndex = 12;
           this.checkMaxNumberOfAliasesEnabled.UseVisualStyleBackColor = true;
           this.checkMaxNumberOfAliasesEnabled.CheckedChanged += new System.EventHandler(this.checkMaxNumberOfAliasesEnabled_CheckedChanged);
           // 
           // checkMaxNumberOfAccountsEnabled
           // 
           this.checkMaxNumberOfAccountsEnabled.AutoSize = true;
           this.checkMaxNumberOfAccountsEnabled.Location = new System.Drawing.Point(11, 164);
           this.checkMaxNumberOfAccountsEnabled.Name = "checkMaxNumberOfAccountsEnabled";
           this.checkMaxNumberOfAccountsEnabled.Size = new System.Drawing.Size(15, 14);
           this.checkMaxNumberOfAccountsEnabled.TabIndex = 9;
           this.checkMaxNumberOfAccountsEnabled.UseVisualStyleBackColor = true;
           this.checkMaxNumberOfAccountsEnabled.CheckedChanged += new System.EventHandler(this.checkMaxNumberOfAccountsEnabled_CheckedChanged);
           // 
           // tabPageDKIM
           // 
           this.tabPageDKIM.Controls.Add(this.buttonSelectCertificate);
           this.tabPageDKIM.Controls.Add(this.panel3);
           this.tabPageDKIM.Controls.Add(this.panel2);
           this.tabPageDKIM.Controls.Add(this.panel1);
           this.tabPageDKIM.Controls.Add(this.labelDKIMSigningAlgorithm);
           this.tabPageDKIM.Controls.Add(this.labelDKIMBodyCanonicalizationMethod);
           this.tabPageDKIM.Controls.Add(this.labelDKIMHeaderCanonicalizationMethod);
           this.tabPageDKIM.Controls.Add(this.labelDKIMSelector);
           this.tabPageDKIM.Controls.Add(this.textDKIMSelector);
           this.tabPageDKIM.Controls.Add(this.labelDKIMPrivateKeyFile);
           this.tabPageDKIM.Controls.Add(this.textDKIMPrivateKeyFile);
           this.tabPageDKIM.Controls.Add(this.checkDKIMSignEnabled);
           this.tabPageDKIM.Controls.Add(this.checkDKIMSignAliasesEnabled);
           this.tabPageDKIM.Location = new System.Drawing.Point(4, 22);
           this.tabPageDKIM.Name = "tabPageDKIM";
           this.tabPageDKIM.Size = new System.Drawing.Size(554, 354);
           this.tabPageDKIM.TabIndex = 5;
           this.tabPageDKIM.Text = "DKIM Signing";
           this.tabPageDKIM.UseVisualStyleBackColor = true;
           this.tabPageDKIM.Click += new System.EventHandler(this.tabPageDKIM_Click);
           // 
           // buttonSelectCertificate
           // 
           this.buttonSelectCertificate.Location = new System.Drawing.Point(225, 60);
           this.buttonSelectCertificate.Name = "buttonSelectCertificate";
           this.buttonSelectCertificate.Size = new System.Drawing.Size(47, 19);
           this.buttonSelectCertificate.TabIndex = 32;
           this.buttonSelectCertificate.Text = "...";
           this.buttonSelectCertificate.UseVisualStyleBackColor = true;
           this.buttonSelectCertificate.Click += new System.EventHandler(this.buttonSelectCertificate_Click);
           // 
           // panel3
           // 
           this.panel3.Controls.Add(this.radioDKIMSigningAlgorithmSHA256);
           this.panel3.Controls.Add(this.radioDKIMSigningAlgorithmSHA1);
           this.panel3.Location = new System.Drawing.Point(14, 266);
           this.panel3.Name = "panel3";
           this.panel3.Size = new System.Drawing.Size(170, 24);
           this.panel3.TabIndex = 31;
           // 
           // radioDKIMSigningAlgorithmSHA256
           // 
           this.radioDKIMSigningAlgorithmSHA256.AutoSize = true;
           this.radioDKIMSigningAlgorithmSHA256.Location = new System.Drawing.Point(82, 4);
           this.radioDKIMSigningAlgorithmSHA256.Name = "radioDKIMSigningAlgorithmSHA256";
           this.radioDKIMSigningAlgorithmSHA256.Size = new System.Drawing.Size(65, 17);
           this.radioDKIMSigningAlgorithmSHA256.TabIndex = 41;
           this.radioDKIMSigningAlgorithmSHA256.TabStop = true;
           this.radioDKIMSigningAlgorithmSHA256.Text = "SHA256";
           this.radioDKIMSigningAlgorithmSHA256.UseVisualStyleBackColor = true;
           // 
           // radioDKIMSigningAlgorithmSHA1
           // 
           this.radioDKIMSigningAlgorithmSHA1.AutoSize = true;
           this.radioDKIMSigningAlgorithmSHA1.Location = new System.Drawing.Point(3, 3);
           this.radioDKIMSigningAlgorithmSHA1.Name = "radioDKIMSigningAlgorithmSHA1";
           this.radioDKIMSigningAlgorithmSHA1.Size = new System.Drawing.Size(53, 17);
           this.radioDKIMSigningAlgorithmSHA1.TabIndex = 40;
           this.radioDKIMSigningAlgorithmSHA1.TabStop = true;
           this.radioDKIMSigningAlgorithmSHA1.Text = "SHA1";
           this.radioDKIMSigningAlgorithmSHA1.UseVisualStyleBackColor = true;
           // 
           // panel2
           // 
           this.panel2.Controls.Add(this.radioDKIMBodyMethodRelaxed);
           this.panel2.Controls.Add(this.radioDKIMBodyMethodSimple);
           this.panel2.Location = new System.Drawing.Point(14, 209);
           this.panel2.Name = "panel2";
           this.panel2.Size = new System.Drawing.Size(170, 24);
           this.panel2.TabIndex = 30;
           // 
           // radioDKIMBodyMethodRelaxed
           // 
           this.radioDKIMBodyMethodRelaxed.AutoSize = true;
           this.radioDKIMBodyMethodRelaxed.Location = new System.Drawing.Point(82, 3);
           this.radioDKIMBodyMethodRelaxed.Name = "radioDKIMBodyMethodRelaxed";
           this.radioDKIMBodyMethodRelaxed.Size = new System.Drawing.Size(64, 17);
           this.radioDKIMBodyMethodRelaxed.TabIndex = 37;
           this.radioDKIMBodyMethodRelaxed.TabStop = true;
           this.radioDKIMBodyMethodRelaxed.Text = "Relaxed";
           this.radioDKIMBodyMethodRelaxed.UseVisualStyleBackColor = true;
           // 
           // radioDKIMBodyMethodSimple
           // 
           this.radioDKIMBodyMethodSimple.AutoSize = true;
           this.radioDKIMBodyMethodSimple.Location = new System.Drawing.Point(3, 3);
           this.radioDKIMBodyMethodSimple.Name = "radioDKIMBodyMethodSimple";
           this.radioDKIMBodyMethodSimple.Size = new System.Drawing.Size(56, 17);
           this.radioDKIMBodyMethodSimple.TabIndex = 36;
           this.radioDKIMBodyMethodSimple.TabStop = true;
           this.radioDKIMBodyMethodSimple.Text = "Simple";
           this.radioDKIMBodyMethodSimple.UseVisualStyleBackColor = true;
           // 
           // panel1
           // 
           this.panel1.Controls.Add(this.radioDKIMHeaderMethodRelaxed);
           this.panel1.Controls.Add(this.radioDKIMHeaderMethodSimple);
           this.panel1.Location = new System.Drawing.Point(14, 154);
           this.panel1.Name = "panel1";
           this.panel1.Size = new System.Drawing.Size(170, 24);
           this.panel1.TabIndex = 29;
           // 
           // radioDKIMHeaderMethodRelaxed
           // 
           this.radioDKIMHeaderMethodRelaxed.AutoSize = true;
           this.radioDKIMHeaderMethodRelaxed.Location = new System.Drawing.Point(82, 4);
           this.radioDKIMHeaderMethodRelaxed.Name = "radioDKIMHeaderMethodRelaxed";
           this.radioDKIMHeaderMethodRelaxed.Size = new System.Drawing.Size(64, 17);
           this.radioDKIMHeaderMethodRelaxed.TabIndex = 35;
           this.radioDKIMHeaderMethodRelaxed.TabStop = true;
           this.radioDKIMHeaderMethodRelaxed.Text = "Relaxed";
           this.radioDKIMHeaderMethodRelaxed.UseVisualStyleBackColor = true;
           // 
           // radioDKIMHeaderMethodSimple
           // 
           this.radioDKIMHeaderMethodSimple.AutoSize = true;
           this.radioDKIMHeaderMethodSimple.Location = new System.Drawing.Point(3, 4);
           this.radioDKIMHeaderMethodSimple.Name = "radioDKIMHeaderMethodSimple";
           this.radioDKIMHeaderMethodSimple.Size = new System.Drawing.Size(56, 17);
           this.radioDKIMHeaderMethodSimple.TabIndex = 33;
           this.radioDKIMHeaderMethodSimple.TabStop = true;
           this.radioDKIMHeaderMethodSimple.Text = "Simple";
           this.radioDKIMHeaderMethodSimple.UseVisualStyleBackColor = true;
           // 
           // labelDKIMSigningAlgorithm
           // 
           this.labelDKIMSigningAlgorithm.AutoSize = true;
           this.labelDKIMSigningAlgorithm.Location = new System.Drawing.Point(11, 250);
           this.labelDKIMSigningAlgorithm.Name = "labelDKIMSigningAlgorithm";
           this.labelDKIMSigningAlgorithm.Size = new System.Drawing.Size(87, 13);
           this.labelDKIMSigningAlgorithm.TabIndex = 26;
           this.labelDKIMSigningAlgorithm.Text = "Signing algorithm";
           // 
           // labelDKIMBodyCanonicalizationMethod
           // 
           this.labelDKIMBodyCanonicalizationMethod.AutoSize = true;
           this.labelDKIMBodyCanonicalizationMethod.Location = new System.Drawing.Point(11, 193);
           this.labelDKIMBodyCanonicalizationMethod.Name = "labelDKIMBodyCanonicalizationMethod";
           this.labelDKIMBodyCanonicalizationMethod.Size = new System.Drawing.Size(69, 13);
           this.labelDKIMBodyCanonicalizationMethod.TabIndex = 23;
           this.labelDKIMBodyCanonicalizationMethod.Text = "Body method";
           // 
           // labelDKIMHeaderCanonicalizationMethod
           // 
           this.labelDKIMHeaderCanonicalizationMethod.AutoSize = true;
           this.labelDKIMHeaderCanonicalizationMethod.Location = new System.Drawing.Point(11, 138);
           this.labelDKIMHeaderCanonicalizationMethod.Name = "labelDKIMHeaderCanonicalizationMethod";
           this.labelDKIMHeaderCanonicalizationMethod.Size = new System.Drawing.Size(80, 13);
           this.labelDKIMHeaderCanonicalizationMethod.TabIndex = 21;
           this.labelDKIMHeaderCanonicalizationMethod.Text = "Header method";
           // 
           // labelDKIMSelector
           // 
           this.labelDKIMSelector.AutoSize = true;
           this.labelDKIMSelector.Location = new System.Drawing.Point(11, 89);
           this.labelDKIMSelector.Name = "labelDKIMSelector";
           this.labelDKIMSelector.Size = new System.Drawing.Size(46, 13);
           this.labelDKIMSelector.TabIndex = 20;
           this.labelDKIMSelector.Text = "Selector";
           // 
           // textDKIMSelector
           // 
           this.textDKIMSelector.Location = new System.Drawing.Point(14, 105);
           this.textDKIMSelector.Name = "textDKIMSelector";
           this.textDKIMSelector.Number = 0;
           this.textDKIMSelector.Numeric = false;
           this.textDKIMSelector.Size = new System.Drawing.Size(161, 20);
           this.textDKIMSelector.TabIndex = 19;
           // 
           // labelDKIMPrivateKeyFile
           // 
           this.labelDKIMPrivateKeyFile.AutoSize = true;
           this.labelDKIMPrivateKeyFile.Location = new System.Drawing.Point(11, 43);
           this.labelDKIMPrivateKeyFile.Name = "labelDKIMPrivateKeyFile";
           this.labelDKIMPrivateKeyFile.Size = new System.Drawing.Size(76, 13);
           this.labelDKIMPrivateKeyFile.TabIndex = 18;
           this.labelDKIMPrivateKeyFile.Text = "Private key file";
           // 
           // textDKIMPrivateKeyFile
           // 
           this.textDKIMPrivateKeyFile.Location = new System.Drawing.Point(14, 59);
           this.textDKIMPrivateKeyFile.Name = "textDKIMPrivateKeyFile";
           this.textDKIMPrivateKeyFile.Number = 0;
           this.textDKIMPrivateKeyFile.Numeric = false;
           this.textDKIMPrivateKeyFile.Size = new System.Drawing.Size(205, 20);
           this.textDKIMPrivateKeyFile.TabIndex = 17;
           // 
           // checkDKIMSignEnabled
           // 
           this.checkDKIMSignEnabled.AutoSize = true;
           this.checkDKIMSignEnabled.Location = new System.Drawing.Point(14, 14);
           this.checkDKIMSignEnabled.Name = "checkDKIMSignEnabled";
           this.checkDKIMSignEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkDKIMSignEnabled.TabIndex = 15;
           this.checkDKIMSignEnabled.Text = "Enabled";
           this.checkDKIMSignEnabled.UseVisualStyleBackColor = true;
            // 
            // checkDKIMSignAliasesEnabled
            // 
            this.checkDKIMSignAliasesEnabled.AutoSize = true;
            this.checkDKIMSignAliasesEnabled.Location = new System.Drawing.Point(90, 14);
            this.checkDKIMSignAliasesEnabled.Name = "checkDKIMSignAliasesEnabled";
            this.checkDKIMSignAliasesEnabled.Size = new System.Drawing.Size(90, 17);
            this.checkDKIMSignAliasesEnabled.TabIndex = 16;
            this.checkDKIMSignAliasesEnabled.Text = "Sign Aliases";
            this.checkDKIMSignAliasesEnabled.UseVisualStyleBackColor = true;
         // 
         // tabPageAdvanced
         // 
         this.tabPageAdvanced.Controls.Add(this.textCatchAllAddress);
           this.tabPageAdvanced.Controls.Add(this.labelCatchAll);
           this.tabPageAdvanced.Controls.Add(this.comboPlusAddressingCharacter);
           this.tabPageAdvanced.Controls.Add(this.checkGreyListingEnabled);
           this.tabPageAdvanced.Controls.Add(this.labelGrayListing);
           this.tabPageAdvanced.Controls.Add(this.labelPlusAddressingCharacter);
           this.tabPageAdvanced.Controls.Add(this.labelPlusAddressing);
           this.tabPageAdvanced.Controls.Add(this.checkUsePlusAddressing);
           this.tabPageAdvanced.Location = new System.Drawing.Point(4, 22);
           this.tabPageAdvanced.Name = "tabPageAdvanced";
           this.tabPageAdvanced.Size = new System.Drawing.Size(554, 354);
           this.tabPageAdvanced.TabIndex = 4;
           this.tabPageAdvanced.Text = "Advanced";
           this.tabPageAdvanced.UseVisualStyleBackColor = true;
           // 
           // textCatchAllAddress
           // 
           this.textCatchAllAddress.Location = new System.Drawing.Point(8, 24);
           this.textCatchAllAddress.Name = "textCatchAllAddress";
           this.textCatchAllAddress.ReadOnlyHost = false;
           this.textCatchAllAddress.Size = new System.Drawing.Size(296, 24);
           this.textCatchAllAddress.TabIndex = 8;
           // 
           // labelCatchAll
           // 
           this.labelCatchAll.AutoSize = true;
           this.labelCatchAll.Location = new System.Drawing.Point(6, 7);
           this.labelCatchAll.Name = "labelCatchAll";
           this.labelCatchAll.Size = new System.Drawing.Size(88, 13);
           this.labelCatchAll.TabIndex = 7;
           this.labelCatchAll.Text = "Catch-all address";
           // 
           // comboPlusAddressingCharacter
           // 
           this.comboPlusAddressingCharacter.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
           this.comboPlusAddressingCharacter.FormattingEnabled = true;
           this.comboPlusAddressingCharacter.Items.AddRange(new object[] {
            "+",
            "-",
            "_",
            "%"});
           this.comboPlusAddressingCharacter.Location = new System.Drawing.Point(24, 120);
           this.comboPlusAddressingCharacter.Name = "comboPlusAddressingCharacter";
           this.comboPlusAddressingCharacter.Size = new System.Drawing.Size(173, 21);
           this.comboPlusAddressingCharacter.TabIndex = 6;
           // 
           // checkGreyListingEnabled
           // 
           this.checkGreyListingEnabled.AutoSize = true;
           this.checkGreyListingEnabled.Location = new System.Drawing.Point(23, 176);
           this.checkGreyListingEnabled.Name = "checkGreyListingEnabled";
           this.checkGreyListingEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkGreyListingEnabled.TabIndex = 5;
           this.checkGreyListingEnabled.Text = "Enabled";
           this.checkGreyListingEnabled.UseVisualStyleBackColor = true;
           // 
           // labelGrayListing
           // 
           this.labelGrayListing.AutoSize = true;
           this.labelGrayListing.Location = new System.Drawing.Point(8, 152);
           this.labelGrayListing.Name = "labelGrayListing";
           this.labelGrayListing.Size = new System.Drawing.Size(55, 13);
           this.labelGrayListing.TabIndex = 4;
           this.labelGrayListing.Text = "Greylisting";
           // 
           // labelPlusAddressingCharacter
           // 
           this.labelPlusAddressingCharacter.AutoSize = true;
           this.labelPlusAddressingCharacter.Location = new System.Drawing.Point(20, 103);
           this.labelPlusAddressingCharacter.Name = "labelPlusAddressingCharacter";
           this.labelPlusAddressingCharacter.Size = new System.Drawing.Size(53, 13);
           this.labelPlusAddressingCharacter.TabIndex = 2;
           this.labelPlusAddressingCharacter.Text = "Character";
           // 
           // labelPlusAddressing
           // 
           this.labelPlusAddressing.AutoSize = true;
           this.labelPlusAddressing.Location = new System.Drawing.Point(8, 56);
           this.labelPlusAddressing.Name = "labelPlusAddressing";
           this.labelPlusAddressing.Size = new System.Drawing.Size(81, 13);
           this.labelPlusAddressing.TabIndex = 0;
           this.labelPlusAddressing.Text = "Plus addressing";
           // 
           // checkUsePlusAddressing
           // 
           this.checkUsePlusAddressing.AutoSize = true;
           this.checkUsePlusAddressing.Location = new System.Drawing.Point(24, 80);
           this.checkUsePlusAddressing.Name = "checkUsePlusAddressing";
           this.checkUsePlusAddressing.Size = new System.Drawing.Size(65, 17);
           this.checkUsePlusAddressing.TabIndex = 1;
           this.checkUsePlusAddressing.Text = "Enabled";
           this.checkUsePlusAddressing.UseVisualStyleBackColor = true;
           // 
           // openFileDialog
           // 
           this.openFileDialog.FileName = "openFileDialog1";
           // 
           // ucDomain
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.tabControl);
           this.Name = "ucDomain";
           this.Size = new System.Drawing.Size(562, 380);
           this.tabControl.ResumeLayout(false);
           this.tabPageGeneral.ResumeLayout(false);
           this.tabPageGeneral.PerformLayout();
           this.tabPageNames.ResumeLayout(false);
           this.tabPageSignature.ResumeLayout(false);
           this.tabPageSignature.PerformLayout();
           this.tabPageLimits.ResumeLayout(false);
           this.tabPageLimits.PerformLayout();
           this.tabPageDKIM.ResumeLayout(false);
           this.tabPageDKIM.PerformLayout();
           this.panel3.ResumeLayout(false);
           this.panel3.PerformLayout();
           this.panel2.ResumeLayout(false);
           this.panel2.PerformLayout();
           this.panel1.ResumeLayout(false);
           this.panel1.PerformLayout();
           this.tabPageAdvanced.ResumeLayout(false);
           this.tabPageAdvanced.PerformLayout();
           this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage tabPageGeneral;
        private System.Windows.Forms.TabPage tabPageNames;
        private System.Windows.Forms.TabPage tabPageSignature;
        private System.Windows.Forms.TabPage tabPageLimits;
        private System.Windows.Forms.TabPage tabPageAdvanced;
        private hMailServer.Shared.ucText textDomainName;
       private System.Windows.Forms.Label labelDomain;
       private System.Windows.Forms.Button buttonAdd;
       private System.Windows.Forms.Button buttonRemove;
       private hMailServer.Administrator.Controls.ucRadioButton radioSMAppendToAccountSignature;
       private hMailServer.Administrator.Controls.ucRadioButton radioSMOverwriteAccountSignature;
       private hMailServer.Administrator.Controls.ucRadioButton radioSMSetIfNotSpecifiedInAccount;
       private hMailServer.Administrator.Controls.ucCheckbox checkSignatureEnabled;
       private hMailServer.Administrator.Controls.ucCheckbox checkAddSignaturesToLocalMail;
       private hMailServer.Administrator.Controls.ucCheckbox checkAddSignaturesToReplies;
       private System.Windows.Forms.Label labelPlainTextSignature;
       private hMailServer.Shared.ucText textHTMLSignature;
       private System.Windows.Forms.Label labelHTMLSignature;
       private hMailServer.Shared.ucText textPlainTextSignature;
       private hMailServer.Shared.ucText textMaxAccountSize;
       private System.Windows.Forms.Label lblMaxAccountSize;
       private hMailServer.Shared.ucText textMaxMessageSize;
       private System.Windows.Forms.Label lblMaxMessageSize;
       private hMailServer.Shared.ucText textAllocatedSize;
       private hMailServer.Shared.ucText textMaxSize;
       private System.Windows.Forms.Label lblAllocatedSize;
       private System.Windows.Forms.Label labelMaxSize;
       private hMailServer.Shared.ucText textMaxNumberOfAliases;
       private System.Windows.Forms.Label lblMaxNoOfAliases;
       private hMailServer.Administrator.Controls.ucCheckbox checkMaxNumberOfAliasesEnabled;
       private hMailServer.Shared.ucText textMaxNumberOfAccounts;
       private System.Windows.Forms.Label lblMaxNoOfAccounts;
       private hMailServer.Administrator.Controls.ucCheckbox checkMaxNumberOfAccountsEnabled;
       private hMailServer.Shared.ucText textMaxNumberOfDistributionLists;
       private System.Windows.Forms.Label lblMaxNoOfDistributionLists;
       private hMailServer.Administrator.Controls.ucCheckbox checkMaxNumberOfDistributionListsEnabled;
        private hMailServer.Administrator.Controls.ucCheckbox checkGreyListingEnabled;
       private System.Windows.Forms.Label labelPlusAddressingCharacter;
       private hMailServer.Administrator.Controls.ucCheckbox checkUsePlusAddressing;
       private System.Windows.Forms.Label labelPlusAddressing;
       private hMailServer.Administrator.Controls.ucComboBox comboPlusAddressingCharacter;
       private ucListView listAliases;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.Button buttonEdit;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnabled;
       private hMailServer.Administrator.Controls.ucEmailEdit textCatchAllAddress;
        private System.Windows.Forms.Label labelCatchAll;
        private System.Windows.Forms.Label labelGrayListing;
        private System.Windows.Forms.TabPage tabPageDKIM;
        private System.Windows.Forms.Label labelDKIMSelector;
        private hMailServer.Shared.ucText textDKIMSelector;
        private System.Windows.Forms.Label labelDKIMPrivateKeyFile;
        private hMailServer.Shared.ucText textDKIMPrivateKeyFile;
        private hMailServer.Administrator.Controls.ucCheckbox checkDKIMSignEnabled;
        private hMailServer.Administrator.Controls.ucCheckbox checkDKIMSignAliasesEnabled;
        private System.Windows.Forms.Label labelDKIMSigningAlgorithm;
        private System.Windows.Forms.Label labelDKIMBodyCanonicalizationMethod;
        private System.Windows.Forms.Label labelDKIMHeaderCanonicalizationMethod;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel1;
        private hMailServer.Administrator.Controls.ucRadioButton radioDKIMBodyMethodRelaxed;
        private hMailServer.Administrator.Controls.ucRadioButton radioDKIMBodyMethodSimple;
        private hMailServer.Administrator.Controls.ucRadioButton radioDKIMHeaderMethodRelaxed;
        private hMailServer.Administrator.Controls.ucRadioButton radioDKIMHeaderMethodSimple;
        private hMailServer.Administrator.Controls.ucRadioButton radioDKIMSigningAlgorithmSHA256;
        private hMailServer.Administrator.Controls.ucRadioButton radioDKIMSigningAlgorithmSHA1;
        private System.Windows.Forms.Button buttonSelectCertificate;
        private System.Windows.Forms.OpenFileDialog openFileDialog;
    }
}
