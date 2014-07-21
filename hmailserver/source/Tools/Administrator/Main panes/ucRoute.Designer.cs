namespace hMailServer.Administrator
{
    partial class ucRoute
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
           this.tabPage1 = new System.Windows.Forms.TabPage();
           this.panel2 = new System.Windows.Forms.Panel();
           this.radioTreatRecipientAsExternalDomain = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioTreatRecipientAsLocalDomain = new hMailServer.Administrator.Controls.ucRadioButton();
           this.labelTreatSenderAsLocalDomain = new System.Windows.Forms.Label();
           this.panel1 = new System.Windows.Forms.Panel();
           this.radioTreatSenderAsExternalDomain = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioTreatSenderAsLocalDomain = new hMailServer.Administrator.Controls.ucRadioButton();
           this.labelTreatThisRouteAs = new System.Windows.Forms.Label();
           this.textTargetSMTPPort = new hMailServer.Shared.ucText();
           this.labelTargetSMTPPort = new System.Windows.Forms.Label();
           this.textTargetSMTPHost = new hMailServer.Shared.ucText();
           this.labelTargetSMTPServer = new System.Windows.Forms.Label();
           this.textDescription = new hMailServer.Shared.ucText();
           this.labelDescription = new System.Windows.Forms.Label();
           this.textDomainName = new hMailServer.Shared.ucText();
           this.labelDomain = new System.Windows.Forms.Label();
           this.tabPage2 = new System.Windows.Forms.TabPage();
           this.buttonDeleteWhiteList = new System.Windows.Forms.Button();
           this.buttonAddRecipient = new System.Windows.Forms.Button();
           this.listAddresses = new hMailServer.Administrator.ucListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.radioRouteForAddresses = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioRouteForAll = new hMailServer.Administrator.Controls.ucRadioButton();
           this.tabPage3 = new System.Windows.Forms.TabPage();
           this.textPassword = new hMailServer.Administrator.Controls.ucPassword();
           this.labelPassword = new System.Windows.Forms.Label();
           this.label1 = new System.Windows.Forms.Label();
           this.labelNoOfMinutesBetween = new System.Windows.Forms.Label();
           this.labelNoOfTries = new System.Windows.Forms.Label();
           this.textUsername = new hMailServer.Shared.ucText();
           this.checkServerRequiresAuth = new hMailServer.Administrator.Controls.ucCheckbox();
           this.textNumberOfMinutesBetween = new hMailServer.Shared.ucText();
           this.textNumberOfTries = new hMailServer.Shared.ucText();
           this.label2 = new System.Windows.Forms.Label();
           this.label3 = new System.Windows.Forms.Label();
           this.label4 = new System.Windows.Forms.Label();
           this.label5 = new System.Windows.Forms.Label();
           this.label6 = new System.Windows.Forms.Label();
           this.ucRadioButton1 = new hMailServer.Administrator.Controls.ucRadioButton();
           this.ucRadioButton2 = new hMailServer.Administrator.Controls.ucRadioButton();
           this.ucCheckbox1 = new hMailServer.Administrator.Controls.ucCheckbox();
           this.ucText1 = new hMailServer.Shared.ucText();
           this.ucText2 = new hMailServer.Shared.ucText();
           this.ucText3 = new hMailServer.Shared.ucText();
           this.ucText4 = new hMailServer.Shared.ucText();
           this.labelConnectionSecurity = new System.Windows.Forms.Label();
           this.comboConnectionSecurity = new hMailServer.Administrator.Controls.ucComboBox();
           this.tabControl.SuspendLayout();
           this.tabPage1.SuspendLayout();
           this.panel2.SuspendLayout();
           this.panel1.SuspendLayout();
           this.tabPage2.SuspendLayout();
           this.tabPage3.SuspendLayout();
           this.SuspendLayout();
           // 
           // tabControl
           // 
           this.tabControl.Controls.Add(this.tabPage1);
           this.tabControl.Controls.Add(this.tabPage2);
           this.tabControl.Controls.Add(this.tabPage3);
           this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
           this.tabControl.Location = new System.Drawing.Point(0, 0);
           this.tabControl.Name = "tabControl";
           this.tabControl.SelectedIndex = 0;
           this.tabControl.Size = new System.Drawing.Size(747, 481);
           this.tabControl.TabIndex = 0;
           // 
           // tabPage1
           // 
           this.tabPage1.Controls.Add(this.labelConnectionSecurity);
           this.tabPage1.Controls.Add(this.comboConnectionSecurity);
           this.tabPage1.Controls.Add(this.panel2);
           this.tabPage1.Controls.Add(this.panel1);
           this.tabPage1.Controls.Add(this.textTargetSMTPPort);
           this.tabPage1.Controls.Add(this.labelTargetSMTPPort);
           this.tabPage1.Controls.Add(this.textTargetSMTPHost);
           this.tabPage1.Controls.Add(this.labelTargetSMTPServer);
           this.tabPage1.Controls.Add(this.textDescription);
           this.tabPage1.Controls.Add(this.labelDescription);
           this.tabPage1.Controls.Add(this.textDomainName);
           this.tabPage1.Controls.Add(this.labelDomain);
           this.tabPage1.Location = new System.Drawing.Point(4, 22);
           this.tabPage1.Name = "tabPage1";
           this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
           this.tabPage1.Size = new System.Drawing.Size(739, 455);
           this.tabPage1.TabIndex = 0;
           this.tabPage1.Text = "General";
           this.tabPage1.UseVisualStyleBackColor = true;
           this.tabPage1.Click += new System.EventHandler(this.tabPage1_Click);
           // 
           // panel2
           // 
           this.panel2.Controls.Add(this.radioTreatRecipientAsExternalDomain);
           this.panel2.Controls.Add(this.radioTreatRecipientAsLocalDomain);
           this.panel2.Controls.Add(this.labelTreatSenderAsLocalDomain);
           this.panel2.Location = new System.Drawing.Point(8, 335);
           this.panel2.Name = "panel2";
           this.panel2.Size = new System.Drawing.Size(523, 51);
           this.panel2.TabIndex = 17;
           // 
           // radioTreatRecipientAsExternalDomain
           // 
           this.radioTreatRecipientAsExternalDomain.AutoSize = true;
           this.radioTreatRecipientAsExternalDomain.Location = new System.Drawing.Point(223, 31);
           this.radioTreatRecipientAsExternalDomain.Name = "radioTreatRecipientAsExternalDomain";
           this.radioTreatRecipientAsExternalDomain.Size = new System.Drawing.Size(134, 17);
           this.radioTreatRecipientAsExternalDomain.TabIndex = 18;
           this.radioTreatRecipientAsExternalDomain.Text = "A remote email address";
           this.radioTreatRecipientAsExternalDomain.UseVisualStyleBackColor = true;
           // 
           // radioTreatRecipientAsLocalDomain
           // 
           this.radioTreatRecipientAsLocalDomain.AutoSize = true;
           this.radioTreatRecipientAsLocalDomain.Checked = true;
           this.radioTreatRecipientAsLocalDomain.Location = new System.Drawing.Point(34, 31);
           this.radioTreatRecipientAsLocalDomain.Name = "radioTreatRecipientAsLocalDomain";
           this.radioTreatRecipientAsLocalDomain.Size = new System.Drawing.Size(124, 17);
           this.radioTreatRecipientAsLocalDomain.TabIndex = 17;
           this.radioTreatRecipientAsLocalDomain.TabStop = true;
           this.radioTreatRecipientAsLocalDomain.Text = "A local email address";
           this.radioTreatRecipientAsLocalDomain.UseVisualStyleBackColor = true;
           // 
           // labelTreatSenderAsLocalDomain
           // 
           this.labelTreatSenderAsLocalDomain.AutoSize = true;
           this.labelTreatSenderAsLocalDomain.Location = new System.Drawing.Point(4, 9);
           this.labelTreatSenderAsLocalDomain.Name = "labelTreatSenderAsLocalDomain";
           this.labelTreatSenderAsLocalDomain.Size = new System.Drawing.Size(233, 13);
           this.labelTreatSenderAsLocalDomain.TabIndex = 16;
           this.labelTreatSenderAsLocalDomain.Text = "When recipient matches route, treat recipient as";
           // 
           // panel1
           // 
           this.panel1.Controls.Add(this.radioTreatSenderAsExternalDomain);
           this.panel1.Controls.Add(this.radioTreatSenderAsLocalDomain);
           this.panel1.Controls.Add(this.labelTreatThisRouteAs);
           this.panel1.Location = new System.Drawing.Point(9, 278);
           this.panel1.Name = "panel1";
           this.panel1.Size = new System.Drawing.Size(523, 51);
           this.panel1.TabIndex = 16;
           // 
           // radioTreatSenderAsExternalDomain
           // 
           this.radioTreatSenderAsExternalDomain.AutoSize = true;
           this.radioTreatSenderAsExternalDomain.Location = new System.Drawing.Point(222, 31);
           this.radioTreatSenderAsExternalDomain.Name = "radioTreatSenderAsExternalDomain";
           this.radioTreatSenderAsExternalDomain.Size = new System.Drawing.Size(134, 17);
           this.radioTreatSenderAsExternalDomain.TabIndex = 15;
           this.radioTreatSenderAsExternalDomain.Text = "A remote email address";
           this.radioTreatSenderAsExternalDomain.UseVisualStyleBackColor = true;
           // 
           // radioTreatSenderAsLocalDomain
           // 
           this.radioTreatSenderAsLocalDomain.AutoSize = true;
           this.radioTreatSenderAsLocalDomain.Checked = true;
           this.radioTreatSenderAsLocalDomain.Location = new System.Drawing.Point(33, 31);
           this.radioTreatSenderAsLocalDomain.Name = "radioTreatSenderAsLocalDomain";
           this.radioTreatSenderAsLocalDomain.Size = new System.Drawing.Size(124, 17);
           this.radioTreatSenderAsLocalDomain.TabIndex = 14;
           this.radioTreatSenderAsLocalDomain.TabStop = true;
           this.radioTreatSenderAsLocalDomain.Text = "A local email address";
           this.radioTreatSenderAsLocalDomain.UseVisualStyleBackColor = true;
           // 
           // labelTreatThisRouteAs
           // 
           this.labelTreatThisRouteAs.AutoSize = true;
           this.labelTreatThisRouteAs.Location = new System.Drawing.Point(3, 9);
           this.labelTreatThisRouteAs.Name = "labelTreatThisRouteAs";
           this.labelTreatThisRouteAs.Size = new System.Drawing.Size(217, 13);
           this.labelTreatThisRouteAs.TabIndex = 13;
           this.labelTreatThisRouteAs.Text = "When sender matches route, treat sender as";
           // 
           // textTargetSMTPPort
           // 
           this.textTargetSMTPPort.Location = new System.Drawing.Point(204, 190);
           this.textTargetSMTPPort.Name = "textTargetSMTPPort";
           this.textTargetSMTPPort.Number = 0;
           this.textTargetSMTPPort.Number64 = ((long)(0));
           this.textTargetSMTPPort.Numeric = true;
           this.textTargetSMTPPort.Size = new System.Drawing.Size(85, 20);
           this.textTargetSMTPPort.TabIndex = 8;
           // 
           // labelTargetSMTPPort
           // 
           this.labelTargetSMTPPort.AutoSize = true;
           this.labelTargetSMTPPort.Location = new System.Drawing.Point(201, 171);
           this.labelTargetSMTPPort.Name = "labelTargetSMTPPort";
           this.labelTargetSMTPPort.Size = new System.Drawing.Size(64, 13);
           this.labelTargetSMTPPort.TabIndex = 7;
           this.labelTargetSMTPPort.Text = "TCP/IP port";
           // 
           // textTargetSMTPHost
           // 
           this.textTargetSMTPHost.Location = new System.Drawing.Point(8, 190);
           this.textTargetSMTPHost.Name = "textTargetSMTPHost";
           this.textTargetSMTPHost.Number = 0;
           this.textTargetSMTPHost.Number64 = ((long)(0));
           this.textTargetSMTPHost.Numeric = false;
           this.textTargetSMTPHost.Size = new System.Drawing.Size(190, 20);
           this.textTargetSMTPHost.TabIndex = 6;
           // 
           // labelTargetSMTPServer
           // 
           this.labelTargetSMTPServer.AutoSize = true;
           this.labelTargetSMTPServer.Location = new System.Drawing.Point(6, 171);
           this.labelTargetSMTPServer.Name = "labelTargetSMTPServer";
           this.labelTargetSMTPServer.Size = new System.Drawing.Size(94, 13);
           this.labelTargetSMTPServer.TabIndex = 5;
           this.labelTargetSMTPServer.Text = "Target SMTP host";
           // 
           // textDescription
           // 
           this.textDescription.Location = new System.Drawing.Point(7, 75);
           this.textDescription.Multiline = true;
           this.textDescription.Name = "textDescription";
           this.textDescription.Number = 0;
           this.textDescription.Number64 = ((long)(0));
           this.textDescription.Numeric = false;
           this.textDescription.Size = new System.Drawing.Size(282, 91);
           this.textDescription.TabIndex = 4;
           // 
           // labelDescription
           // 
           this.labelDescription.AutoSize = true;
           this.labelDescription.Location = new System.Drawing.Point(5, 56);
           this.labelDescription.Name = "labelDescription";
           this.labelDescription.Size = new System.Drawing.Size(60, 13);
           this.labelDescription.TabIndex = 3;
           this.labelDescription.Text = "Description";
           // 
           // textDomainName
           // 
           this.textDomainName.Location = new System.Drawing.Point(6, 26);
           this.textDomainName.Name = "textDomainName";
           this.textDomainName.Number = 0;
           this.textDomainName.Number64 = ((long)(0));
           this.textDomainName.Numeric = false;
           this.textDomainName.Size = new System.Drawing.Size(187, 20);
           this.textDomainName.TabIndex = 2;
           // 
           // labelDomain
           // 
           this.labelDomain.AutoSize = true;
           this.labelDomain.Location = new System.Drawing.Point(4, 7);
           this.labelDomain.Name = "labelDomain";
           this.labelDomain.Size = new System.Drawing.Size(43, 13);
           this.labelDomain.TabIndex = 1;
           this.labelDomain.Text = "Domain";
           // 
           // tabPage2
           // 
           this.tabPage2.Controls.Add(this.buttonDeleteWhiteList);
           this.tabPage2.Controls.Add(this.buttonAddRecipient);
           this.tabPage2.Controls.Add(this.listAddresses);
           this.tabPage2.Controls.Add(this.radioRouteForAddresses);
           this.tabPage2.Controls.Add(this.radioRouteForAll);
           this.tabPage2.Location = new System.Drawing.Point(4, 22);
           this.tabPage2.Name = "tabPage2";
           this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
           this.tabPage2.Size = new System.Drawing.Size(739, 455);
           this.tabPage2.TabIndex = 1;
           this.tabPage2.Text = "Addresses";
           this.tabPage2.UseVisualStyleBackColor = true;
           // 
           // buttonDeleteWhiteList
           // 
           this.buttonDeleteWhiteList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonDeleteWhiteList.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonDeleteWhiteList.Location = new System.Drawing.Point(632, 88);
           this.buttonDeleteWhiteList.Name = "buttonDeleteWhiteList";
           this.buttonDeleteWhiteList.Size = new System.Drawing.Size(100, 25);
           this.buttonDeleteWhiteList.TabIndex = 44;
           this.buttonDeleteWhiteList.Text = "Remove";
           this.buttonDeleteWhiteList.UseVisualStyleBackColor = true;
           this.buttonDeleteWhiteList.Click += new System.EventHandler(this.buttonDeleteWhiteList_Click);
           // 
           // buttonAddRecipient
           // 
           this.buttonAddRecipient.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonAddRecipient.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonAddRecipient.Location = new System.Drawing.Point(632, 56);
           this.buttonAddRecipient.Name = "buttonAddRecipient";
           this.buttonAddRecipient.Size = new System.Drawing.Size(100, 25);
           this.buttonAddRecipient.TabIndex = 43;
           this.buttonAddRecipient.Text = "&Add...";
           this.buttonAddRecipient.UseVisualStyleBackColor = true;
           this.buttonAddRecipient.Click += new System.EventHandler(this.buttonAddRecipient_Click);
           // 
           // listAddresses
           // 
           this.listAddresses.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listAddresses.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
           this.listAddresses.FullRowSelect = true;
           this.listAddresses.Location = new System.Drawing.Point(8, 56);
           this.listAddresses.Name = "listAddresses";
           this.listAddresses.Size = new System.Drawing.Size(616, 391);
           this.listAddresses.TabIndex = 15;
           this.listAddresses.UseCompatibleStateImageBehavior = false;
           this.listAddresses.View = System.Windows.Forms.View.Details;
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "Address";
           this.columnHeader1.Width = 150;
           // 
           // radioRouteForAddresses
           // 
           this.radioRouteForAddresses.AutoSize = true;
           this.radioRouteForAddresses.Location = new System.Drawing.Point(8, 31);
           this.radioRouteForAddresses.Name = "radioRouteForAddresses";
           this.radioRouteForAddresses.Size = new System.Drawing.Size(155, 17);
           this.radioRouteForAddresses.TabIndex = 14;
           this.radioRouteForAddresses.Text = "Deliver to addresses below:";
           this.radioRouteForAddresses.UseVisualStyleBackColor = true;
           this.radioRouteForAddresses.CheckedChanged += new System.EventHandler(this.radioRouteForAddresses_CheckedChanged);
           // 
           // radioRouteForAll
           // 
           this.radioRouteForAll.AutoSize = true;
           this.radioRouteForAll.Checked = true;
           this.radioRouteForAll.Location = new System.Drawing.Point(8, 8);
           this.radioRouteForAll.Name = "radioRouteForAll";
           this.radioRouteForAll.Size = new System.Drawing.Size(134, 17);
           this.radioRouteForAll.TabIndex = 13;
           this.radioRouteForAll.TabStop = true;
           this.radioRouteForAll.Text = "Deliver to all addresses";
           this.radioRouteForAll.UseVisualStyleBackColor = true;
           // 
           // tabPage3
           // 
           this.tabPage3.Controls.Add(this.textPassword);
           this.tabPage3.Controls.Add(this.labelPassword);
           this.tabPage3.Controls.Add(this.label1);
           this.tabPage3.Controls.Add(this.labelNoOfMinutesBetween);
           this.tabPage3.Controls.Add(this.labelNoOfTries);
           this.tabPage3.Controls.Add(this.textUsername);
           this.tabPage3.Controls.Add(this.checkServerRequiresAuth);
           this.tabPage3.Controls.Add(this.textNumberOfMinutesBetween);
           this.tabPage3.Controls.Add(this.textNumberOfTries);
           this.tabPage3.Location = new System.Drawing.Point(4, 22);
           this.tabPage3.Name = "tabPage3";
           this.tabPage3.Size = new System.Drawing.Size(739, 455);
           this.tabPage3.TabIndex = 2;
           this.tabPage3.Text = "Delivery";
           this.tabPage3.UseVisualStyleBackColor = true;
           // 
           // textPassword
           // 
           this.textPassword.Location = new System.Drawing.Point(33, 199);
           this.textPassword.Name = "textPassword";
           this.textPassword.Size = new System.Drawing.Size(192, 20);
           this.textPassword.TabIndex = 17;
           this.textPassword.Text = "<< Encrypted >>";
           // 
           // labelPassword
           // 
           this.labelPassword.AutoSize = true;
           this.labelPassword.Location = new System.Drawing.Point(31, 183);
           this.labelPassword.Name = "labelPassword";
           this.labelPassword.Size = new System.Drawing.Size(53, 13);
           this.labelPassword.TabIndex = 16;
           this.labelPassword.Text = "Password";
           // 
           // label1
           // 
           this.label1.AutoSize = true;
           this.label1.Location = new System.Drawing.Point(31, 135);
           this.label1.Name = "label1";
           this.label1.Size = new System.Drawing.Size(58, 13);
           this.label1.TabIndex = 14;
           this.label1.Text = "User name";
           // 
           // labelNoOfMinutesBetween
           // 
           this.labelNoOfMinutesBetween.AutoSize = true;
           this.labelNoOfMinutesBetween.Location = new System.Drawing.Point(8, 56);
           this.labelNoOfMinutesBetween.Name = "labelNoOfMinutesBetween";
           this.labelNoOfMinutesBetween.Size = new System.Drawing.Size(140, 13);
           this.labelNoOfMinutesBetween.TabIndex = 11;
           this.labelNoOfMinutesBetween.Text = "Minutes between every retry";
           // 
           // labelNoOfTries
           // 
           this.labelNoOfTries.AutoSize = true;
           this.labelNoOfTries.Location = new System.Drawing.Point(8, 8);
           this.labelNoOfTries.Name = "labelNoOfTries";
           this.labelNoOfTries.Size = new System.Drawing.Size(78, 13);
           this.labelNoOfTries.TabIndex = 9;
           this.labelNoOfTries.Text = "Number of tries";
           // 
           // textUsername
           // 
           this.textUsername.Location = new System.Drawing.Point(33, 154);
           this.textUsername.Name = "textUsername";
           this.textUsername.Number = 0;
           this.textUsername.Number64 = ((long)(0));
           this.textUsername.Numeric = false;
           this.textUsername.Size = new System.Drawing.Size(187, 20);
           this.textUsername.TabIndex = 15;
           // 
           // checkServerRequiresAuth
           // 
           this.checkServerRequiresAuth.AutoSize = true;
           this.checkServerRequiresAuth.Location = new System.Drawing.Point(11, 112);
           this.checkServerRequiresAuth.Name = "checkServerRequiresAuth";
           this.checkServerRequiresAuth.Size = new System.Drawing.Size(167, 17);
           this.checkServerRequiresAuth.TabIndex = 13;
           this.checkServerRequiresAuth.Text = "Server requires authentication";
           this.checkServerRequiresAuth.UseVisualStyleBackColor = true;
           this.checkServerRequiresAuth.CheckedChanged += new System.EventHandler(this.checkServerRequiresAuth_CheckedChanged);
           // 
           // textNumberOfMinutesBetween
           // 
           this.textNumberOfMinutesBetween.Location = new System.Drawing.Point(11, 75);
           this.textNumberOfMinutesBetween.Name = "textNumberOfMinutesBetween";
           this.textNumberOfMinutesBetween.Number = 60;
           this.textNumberOfMinutesBetween.Number64 = ((long)(60));
           this.textNumberOfMinutesBetween.Numeric = true;
           this.textNumberOfMinutesBetween.Size = new System.Drawing.Size(85, 20);
           this.textNumberOfMinutesBetween.TabIndex = 12;
           this.textNumberOfMinutesBetween.Text = "60";
           // 
           // textNumberOfTries
           // 
           this.textNumberOfTries.Location = new System.Drawing.Point(11, 27);
           this.textNumberOfTries.Name = "textNumberOfTries";
           this.textNumberOfTries.Number = 4;
           this.textNumberOfTries.Number64 = ((long)(4));
           this.textNumberOfTries.Numeric = true;
           this.textNumberOfTries.Size = new System.Drawing.Size(85, 20);
           this.textNumberOfTries.TabIndex = 10;
           this.textNumberOfTries.Text = "4";
           // 
           // label2
           // 
           this.label2.AutoSize = true;
           this.label2.Location = new System.Drawing.Point(6, 251);
           this.label2.Name = "label2";
           this.label2.Size = new System.Drawing.Size(101, 13);
           this.label2.TabIndex = 10;
           this.label2.Text = "Treat this route as a";
           // 
           // label3
           // 
           this.label3.AutoSize = true;
           this.label3.Location = new System.Drawing.Point(201, 171);
           this.label3.Name = "label3";
           this.label3.Size = new System.Drawing.Size(26, 13);
           this.label3.TabIndex = 7;
           this.label3.Text = "Port";
           // 
           // label4
           // 
           this.label4.AutoSize = true;
           this.label4.Location = new System.Drawing.Point(6, 171);
           this.label4.Name = "label4";
           this.label4.Size = new System.Drawing.Size(103, 13);
           this.label4.TabIndex = 5;
           this.label4.Text = "Target SMTP server";
           // 
           // label5
           // 
           this.label5.AutoSize = true;
           this.label5.Location = new System.Drawing.Point(5, 56);
           this.label5.Name = "label5";
           this.label5.Size = new System.Drawing.Size(60, 13);
           this.label5.TabIndex = 3;
           this.label5.Text = "Description";
           // 
           // label6
           // 
           this.label6.AutoSize = true;
           this.label6.Location = new System.Drawing.Point(4, 7);
           this.label6.Name = "label6";
           this.label6.Size = new System.Drawing.Size(43, 13);
           this.label6.TabIndex = 1;
           this.label6.Text = "Domain";
           // 
           // ucRadioButton1
           // 
           this.ucRadioButton1.AutoSize = true;
           this.ucRadioButton1.Location = new System.Drawing.Point(36, 273);
           this.ucRadioButton1.Name = "ucRadioButton1";
           this.ucRadioButton1.Size = new System.Drawing.Size(94, 17);
           this.ucRadioButton1.TabIndex = 11;
           this.ucRadioButton1.TabStop = true;
           this.ucRadioButton1.Text = "A local domain";
           this.ucRadioButton1.UseVisualStyleBackColor = true;
           // 
           // ucRadioButton2
           // 
           this.ucRadioButton2.AutoSize = true;
           this.ucRadioButton2.Location = new System.Drawing.Point(36, 296);
           this.ucRadioButton2.Name = "ucRadioButton2";
           this.ucRadioButton2.Size = new System.Drawing.Size(115, 17);
           this.ucRadioButton2.TabIndex = 12;
           this.ucRadioButton2.TabStop = true;
           this.ucRadioButton2.Text = "An external domain";
           this.ucRadioButton2.UseVisualStyleBackColor = true;
           // 
           // ucCheckbox1
           // 
           this.ucCheckbox1.AutoSize = true;
           this.ucCheckbox1.Location = new System.Drawing.Point(9, 217);
           this.ucCheckbox1.Name = "ucCheckbox1";
           this.ucCheckbox1.Size = new System.Drawing.Size(68, 17);
           this.ucCheckbox1.TabIndex = 9;
           this.ucCheckbox1.Text = "Use SSL";
           this.ucCheckbox1.UseVisualStyleBackColor = true;
           // 
           // ucText1
           // 
           this.ucText1.Location = new System.Drawing.Point(204, 190);
           this.ucText1.Name = "ucText1";
           this.ucText1.Number = 0;
           this.ucText1.Number64 = ((long)(0));
           this.ucText1.Numeric = true;
           this.ucText1.Size = new System.Drawing.Size(85, 20);
           this.ucText1.TabIndex = 8;
           // 
           // ucText2
           // 
           this.ucText2.Location = new System.Drawing.Point(8, 190);
           this.ucText2.Name = "ucText2";
           this.ucText2.Number = 0;
           this.ucText2.Number64 = ((long)(0));
           this.ucText2.Numeric = false;
           this.ucText2.Size = new System.Drawing.Size(190, 20);
           this.ucText2.TabIndex = 6;
           // 
           // ucText3
           // 
           this.ucText3.Location = new System.Drawing.Point(7, 75);
           this.ucText3.Multiline = true;
           this.ucText3.Name = "ucText3";
           this.ucText3.Number = 0;
           this.ucText3.Number64 = ((long)(0));
           this.ucText3.Numeric = false;
           this.ucText3.Size = new System.Drawing.Size(282, 91);
           this.ucText3.TabIndex = 4;
           // 
           // ucText4
           // 
           this.ucText4.Location = new System.Drawing.Point(6, 26);
           this.ucText4.Name = "ucText4";
           this.ucText4.Number = 0;
           this.ucText4.Number64 = ((long)(0));
           this.ucText4.Numeric = false;
           this.ucText4.Size = new System.Drawing.Size(187, 20);
           this.ucText4.TabIndex = 2;
           // 
           // labelConnectionSecurity
           // 
           this.labelConnectionSecurity.AutoSize = true;
           this.labelConnectionSecurity.Location = new System.Drawing.Point(7, 216);
           this.labelConnectionSecurity.Name = "labelConnectionSecurity";
           this.labelConnectionSecurity.Size = new System.Drawing.Size(100, 13);
           this.labelConnectionSecurity.TabIndex = 64;
           this.labelConnectionSecurity.Text = "Connection security";
           // 
           // comboConnectionSecurity
           // 
           this.comboConnectionSecurity.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
           this.comboConnectionSecurity.FormattingEnabled = true;
           this.comboConnectionSecurity.Location = new System.Drawing.Point(9, 232);
           this.comboConnectionSecurity.Name = "comboConnectionSecurity";
           this.comboConnectionSecurity.Size = new System.Drawing.Size(171, 21);
           this.comboConnectionSecurity.TabIndex = 63;
           // 
           // ucRoute
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.tabControl);
           this.Name = "ucRoute";
           this.Size = new System.Drawing.Size(747, 481);
           this.tabControl.ResumeLayout(false);
           this.tabPage1.ResumeLayout(false);
           this.tabPage1.PerformLayout();
           this.panel2.ResumeLayout(false);
           this.panel2.PerformLayout();
           this.panel1.ResumeLayout(false);
           this.panel1.PerformLayout();
           this.tabPage2.ResumeLayout(false);
           this.tabPage2.PerformLayout();
           this.tabPage3.ResumeLayout(false);
           this.tabPage3.PerformLayout();
           this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.TabControl tabControl;
       private System.Windows.Forms.TabPage tabPage1;
       private System.Windows.Forms.TabPage tabPage2;
       private System.Windows.Forms.TabPage tabPage3;
       private hMailServer.Shared.ucText textTargetSMTPPort;
       private System.Windows.Forms.Label labelTargetSMTPPort;
       private hMailServer.Shared.ucText textTargetSMTPHost;
       private System.Windows.Forms.Label labelTargetSMTPServer;
       private hMailServer.Shared.ucText textDescription;
       private System.Windows.Forms.Label labelDescription;
       private hMailServer.Shared.ucText textDomainName;
       private System.Windows.Forms.Label labelDomain;
       private hMailServer.Shared.ucText textNumberOfMinutesBetween;
       private System.Windows.Forms.Label labelNoOfMinutesBetween;
       private hMailServer.Shared.ucText textNumberOfTries;
       private System.Windows.Forms.Label labelNoOfTries;
       private hMailServer.Administrator.Controls.ucCheckbox checkServerRequiresAuth;
       private System.Windows.Forms.Label labelPassword;
       private hMailServer.Shared.ucText textUsername;
       private System.Windows.Forms.Label label1;
       private hMailServer.Administrator.Controls.ucPassword textPassword;
       private hMailServer.Administrator.Controls.ucRadioButton radioRouteForAddresses;
       private hMailServer.Administrator.Controls.ucRadioButton radioRouteForAll;
       private hMailServer.Administrator.Controls.ucRadioButton ucRadioButton1;
       private hMailServer.Administrator.Controls.ucRadioButton ucRadioButton2;
       private System.Windows.Forms.Label label2;
       private hMailServer.Administrator.Controls.ucCheckbox ucCheckbox1;
       private hMailServer.Shared.ucText ucText1;
       private System.Windows.Forms.Label label3;
       private hMailServer.Shared.ucText ucText2;
       private System.Windows.Forms.Label label4;
       private hMailServer.Shared.ucText ucText3;
       private System.Windows.Forms.Label label5;
       private hMailServer.Shared.ucText ucText4;
       private System.Windows.Forms.Label label6;
       private ucListView listAddresses;
       private System.Windows.Forms.Button buttonDeleteWhiteList;
       private System.Windows.Forms.Button buttonAddRecipient;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.Panel panel2;
       private hMailServer.Administrator.Controls.ucRadioButton radioTreatRecipientAsExternalDomain;
       private hMailServer.Administrator.Controls.ucRadioButton radioTreatRecipientAsLocalDomain;
       private System.Windows.Forms.Label labelTreatSenderAsLocalDomain;
       private System.Windows.Forms.Panel panel1;
       private hMailServer.Administrator.Controls.ucRadioButton radioTreatSenderAsExternalDomain;
       private hMailServer.Administrator.Controls.ucRadioButton radioTreatSenderAsLocalDomain;
       private System.Windows.Forms.Label labelTreatThisRouteAs;
       private System.Windows.Forms.Label labelConnectionSecurity;
       private hMailServer.Administrator.Controls.ucComboBox comboConnectionSecurity;
    }
}
