namespace hMailServer.Administrator
{
    partial class ucDistributionList
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
           this.checkRequireSMTPAuthentication = new hMailServer.Administrator.Controls.ucCheckbox();
           this.labelSecurity = new System.Windows.Forms.Label();
           this.textRequireAddress = new hMailServer.Administrator.Controls.ucEmailEdit();
           this.optModeAnnouncements = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioModeMembership = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioModePublic = new hMailServer.Administrator.Controls.ucRadioButton();
           this.labelMode = new System.Windows.Forms.Label();
           this.checkEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.textAddress = new hMailServer.Administrator.Controls.ucEmailEdit();
           this.labelAddress = new System.Windows.Forms.Label();
           this.tabPage2 = new System.Windows.Forms.TabPage();
           this.buttonEdit = new System.Windows.Forms.Button();
           this.buttonSelectRecipients = new System.Windows.Forms.Button();
           this.buttonDeleteRecipient = new System.Windows.Forms.Button();
           this.buttonAddRecipient = new System.Windows.Forms.Button();
           this.listRecipients = new hMailServer.Administrator.ucListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.buttonImport = new System.Windows.Forms.Button();
           this.tabControl.SuspendLayout();
           this.tabPage1.SuspendLayout();
           this.tabPage2.SuspendLayout();
           this.SuspendLayout();
           // 
           // tabControl
           // 
           this.tabControl.Controls.Add(this.tabPage1);
           this.tabControl.Controls.Add(this.tabPage2);
           this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
           this.tabControl.Location = new System.Drawing.Point(0, 0);
           this.tabControl.Name = "tabControl";
           this.tabControl.SelectedIndex = 0;
           this.tabControl.Size = new System.Drawing.Size(635, 402);
           this.tabControl.TabIndex = 0;
           // 
           // tabPage1
           // 
           this.tabPage1.Controls.Add(this.checkRequireSMTPAuthentication);
           this.tabPage1.Controls.Add(this.labelSecurity);
           this.tabPage1.Controls.Add(this.textRequireAddress);
           this.tabPage1.Controls.Add(this.optModeAnnouncements);
           this.tabPage1.Controls.Add(this.radioModeMembership);
           this.tabPage1.Controls.Add(this.radioModePublic);
           this.tabPage1.Controls.Add(this.labelMode);
           this.tabPage1.Controls.Add(this.checkEnabled);
           this.tabPage1.Controls.Add(this.textAddress);
           this.tabPage1.Controls.Add(this.labelAddress);
           this.tabPage1.Location = new System.Drawing.Point(4, 22);
           this.tabPage1.Name = "tabPage1";
           this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
           this.tabPage1.Size = new System.Drawing.Size(627, 376);
           this.tabPage1.TabIndex = 0;
           this.tabPage1.Text = "General";
           this.tabPage1.UseVisualStyleBackColor = true;
           // 
           // checkRequireSMTPAuthentication
           // 
           this.checkRequireSMTPAuthentication.AutoSize = true;
           this.checkRequireSMTPAuthentication.Location = new System.Drawing.Point(44, 242);
           this.checkRequireSMTPAuthentication.Name = "checkRequireSMTPAuthentication";
           this.checkRequireSMTPAuthentication.Size = new System.Drawing.Size(167, 17);
           this.checkRequireSMTPAuthentication.TabIndex = 20;
           this.checkRequireSMTPAuthentication.Text = "Require SMTP Authentication";
           this.checkRequireSMTPAuthentication.UseVisualStyleBackColor = true;
           // 
           // labelSecurity
           // 
           this.labelSecurity.AutoSize = true;
           this.labelSecurity.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelSecurity.Location = new System.Drawing.Point(13, 215);
           this.labelSecurity.Name = "labelSecurity";
           this.labelSecurity.Size = new System.Drawing.Size(53, 13);
           this.labelSecurity.TabIndex = 19;
           this.labelSecurity.Text = "Security";
           // 
           // textRequireAddress
           // 
           this.textRequireAddress.Location = new System.Drawing.Point(61, 180);
           this.textRequireAddress.Name = "textRequireAddress";
           this.textRequireAddress.ReadOnlyHost = false;
           this.textRequireAddress.Size = new System.Drawing.Size(282, 25);
           this.textRequireAddress.TabIndex = 18;
           // 
           // optModeAnnouncements
           // 
           this.optModeAnnouncements.AutoSize = true;
           this.optModeAnnouncements.Location = new System.Drawing.Point(44, 157);
           this.optModeAnnouncements.Name = "optModeAnnouncements";
           this.optModeAnnouncements.Size = new System.Drawing.Size(337, 17);
           this.optModeAnnouncements.TabIndex = 16;
           this.optModeAnnouncements.TabStop = true;
           this.optModeAnnouncements.Text = "Announcements - Only allow messages from the following address:";
           this.optModeAnnouncements.UseVisualStyleBackColor = true;
           this.optModeAnnouncements.CheckedChanged += new System.EventHandler(this.optModeAnnouncements_CheckedChanged);
           // 
           // radioModeMembership
           // 
           this.radioModeMembership.AutoSize = true;
           this.radioModeMembership.Location = new System.Drawing.Point(44, 134);
           this.radioModeMembership.Name = "radioModeMembership";
           this.radioModeMembership.Size = new System.Drawing.Size(249, 17);
           this.radioModeMembership.TabIndex = 15;
           this.radioModeMembership.TabStop = true;
           this.radioModeMembership.Text = "Membership - Only members can send to the list";
           this.radioModeMembership.UseVisualStyleBackColor = true;
           // 
           // radioModePublic
           // 
           this.radioModePublic.AutoSize = true;
           this.radioModePublic.Location = new System.Drawing.Point(44, 111);
           this.radioModePublic.Name = "radioModePublic";
           this.radioModePublic.Size = new System.Drawing.Size(191, 17);
           this.radioModePublic.TabIndex = 14;
           this.radioModePublic.TabStop = true;
           this.radioModePublic.Text = "Public - Anyone can send to the list";
           this.radioModePublic.UseVisualStyleBackColor = true;
           // 
           // labelMode
           // 
           this.labelMode.AutoSize = true;
           this.labelMode.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelMode.Location = new System.Drawing.Point(13, 86);
           this.labelMode.Name = "labelMode";
           this.labelMode.Size = new System.Drawing.Size(38, 13);
           this.labelMode.TabIndex = 13;
           this.labelMode.Text = "Mode";
           // 
           // checkEnabled
           // 
           this.checkEnabled.AutoSize = true;
           this.checkEnabled.Checked = true;
           this.checkEnabled.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkEnabled.Location = new System.Drawing.Point(16, 56);
           this.checkEnabled.Name = "checkEnabled";
           this.checkEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkEnabled.TabIndex = 5;
           this.checkEnabled.Text = "Enabled";
           this.checkEnabled.UseVisualStyleBackColor = true;
           // 
           // textAddress
           // 
           this.textAddress.Location = new System.Drawing.Point(15, 29);
           this.textAddress.Name = "textAddress";
           this.textAddress.ReadOnlyHost = true;
           this.textAddress.Size = new System.Drawing.Size(289, 25);
           this.textAddress.TabIndex = 4;
           // 
           // labelAddress
           // 
           this.labelAddress.AutoSize = true;
           this.labelAddress.Location = new System.Drawing.Point(13, 12);
           this.labelAddress.Name = "labelAddress";
           this.labelAddress.Size = new System.Drawing.Size(45, 13);
           this.labelAddress.TabIndex = 3;
           this.labelAddress.Text = "Address";
           // 
           // tabPage2
           // 
           this.tabPage2.Controls.Add(this.buttonImport);
           this.tabPage2.Controls.Add(this.buttonEdit);
           this.tabPage2.Controls.Add(this.buttonSelectRecipients);
           this.tabPage2.Controls.Add(this.buttonDeleteRecipient);
           this.tabPage2.Controls.Add(this.buttonAddRecipient);
           this.tabPage2.Controls.Add(this.listRecipients);
           this.tabPage2.Location = new System.Drawing.Point(4, 22);
           this.tabPage2.Name = "tabPage2";
           this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
           this.tabPage2.Size = new System.Drawing.Size(627, 376);
           this.tabPage2.TabIndex = 1;
           this.tabPage2.Text = "Members";
           this.tabPage2.UseVisualStyleBackColor = true;
           // 
           // buttonEdit
           // 
           this.buttonEdit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonEdit.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonEdit.Location = new System.Drawing.Point(520, 101);
           this.buttonEdit.Name = "buttonEdit";
           this.buttonEdit.Size = new System.Drawing.Size(100, 25);
           this.buttonEdit.TabIndex = 43;
           this.buttonEdit.Text = "&Edit...";
           this.buttonEdit.UseVisualStyleBackColor = true;
           this.buttonEdit.Click += new System.EventHandler(this.buttonEdit_Click);
           // 
           // buttonSelectRecipients
           // 
           this.buttonSelectRecipients.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonSelectRecipients.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonSelectRecipients.Location = new System.Drawing.Point(520, 40);
           this.buttonSelectRecipients.Name = "buttonSelectRecipients";
           this.buttonSelectRecipients.Size = new System.Drawing.Size(100, 25);
           this.buttonSelectRecipients.TabIndex = 42;
           this.buttonSelectRecipients.Text = "&Select...";
           this.buttonSelectRecipients.UseVisualStyleBackColor = true;
           this.buttonSelectRecipients.Click += new System.EventHandler(this.buttonSelectRecipients_Click);
           // 
           // buttonDeleteRecipient
           // 
           this.buttonDeleteRecipient.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonDeleteRecipient.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonDeleteRecipient.Location = new System.Drawing.Point(520, 133);
           this.buttonDeleteRecipient.Name = "buttonDeleteRecipient";
           this.buttonDeleteRecipient.Size = new System.Drawing.Size(100, 25);
           this.buttonDeleteRecipient.TabIndex = 41;
           this.buttonDeleteRecipient.Text = "Remove";
           this.buttonDeleteRecipient.UseVisualStyleBackColor = true;
           this.buttonDeleteRecipient.Click += new System.EventHandler(this.buttonDeleteRecipient_Click);
           // 
           // buttonAddRecipient
           // 
           this.buttonAddRecipient.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonAddRecipient.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonAddRecipient.Location = new System.Drawing.Point(520, 8);
           this.buttonAddRecipient.Name = "buttonAddRecipient";
           this.buttonAddRecipient.Size = new System.Drawing.Size(100, 25);
           this.buttonAddRecipient.TabIndex = 40;
           this.buttonAddRecipient.Text = "&Add...";
           this.buttonAddRecipient.UseVisualStyleBackColor = true;
           this.buttonAddRecipient.Click += new System.EventHandler(this.buttonAddRecipient_Click);
           // 
           // listRecipients
           // 
           this.listRecipients.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listRecipients.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
           this.listRecipients.FullRowSelect = true;
           this.listRecipients.Location = new System.Drawing.Point(6, 6);
           this.listRecipients.Name = "listRecipients";
           this.listRecipients.Size = new System.Drawing.Size(510, 364);
           this.listRecipients.TabIndex = 39;
           this.listRecipients.UseCompatibleStateImageBehavior = false;
           this.listRecipients.View = System.Windows.Forms.View.Details;
           this.listRecipients.SelectedIndexChanged += new System.EventHandler(this.listRecipients_SelectedIndexChanged);
           this.listRecipients.DoubleClick += new System.EventHandler(this.listRecipients_DoubleClick);
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "Address";
           this.columnHeader1.Width = 250;
           // 
           // buttonImport
           // 
           this.buttonImport.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonImport.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonImport.Location = new System.Drawing.Point(520, 71);
           this.buttonImport.Name = "buttonImport";
           this.buttonImport.Size = new System.Drawing.Size(100, 25);
           this.buttonImport.TabIndex = 44;
           this.buttonImport.Text = "&Import...";
           this.buttonImport.UseVisualStyleBackColor = true;
           this.buttonImport.Click += new System.EventHandler(this.buttonImport_Click);
           // 
           // ucDistributionList
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.tabControl);
           this.Name = "ucDistributionList";
           this.Size = new System.Drawing.Size(635, 402);
           this.tabControl.ResumeLayout(false);
           this.tabPage1.ResumeLayout(false);
           this.tabPage1.PerformLayout();
           this.tabPage2.ResumeLayout(false);
           this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.TabControl tabControl;
       private System.Windows.Forms.TabPage tabPage1;
       private System.Windows.Forms.TabPage tabPage2;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnabled;
       private hMailServer.Administrator.Controls.ucEmailEdit textAddress;
       private System.Windows.Forms.Label labelAddress;
       private System.Windows.Forms.Label labelMode;
       private hMailServer.Administrator.Controls.ucRadioButton radioModeMembership;
       private hMailServer.Administrator.Controls.ucRadioButton radioModePublic;
       private hMailServer.Administrator.Controls.ucCheckbox checkRequireSMTPAuthentication;
       private System.Windows.Forms.Label labelSecurity;
       private hMailServer.Administrator.Controls.ucEmailEdit textRequireAddress;
       private hMailServer.Administrator.Controls.ucRadioButton optModeAnnouncements;
       private System.Windows.Forms.Button buttonSelectRecipients;
       private System.Windows.Forms.Button buttonDeleteRecipient;
       private System.Windows.Forms.Button buttonAddRecipient;
       private ucListView listRecipients;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.Button buttonEdit;
       private System.Windows.Forms.Button buttonImport;

     }
}
