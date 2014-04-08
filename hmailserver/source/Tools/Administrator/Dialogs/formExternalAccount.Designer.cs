namespace hMailServer.Administrator.Dialogs
{
   partial class formExternalAccount
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

      #region Windows Form Designer generated code

      /// <summary>
      /// Required method for Designer support - do not modify
      /// the contents of this method with the code editor.
      /// </summary>
      private void InitializeComponent()
      {
         this.labelName = new System.Windows.Forms.Label();
         this.labelServerType = new System.Windows.Forms.Label();
         this.labelPassword = new System.Windows.Forms.Label();
         this.labelUsername = new System.Windows.Forms.Label();
         this.labelTCPIPPort = new System.Windows.Forms.Label();
         this.labelServerAddress = new System.Windows.Forms.Label();
         this.labelSettings = new System.Windows.Forms.Label();
         this.labelMinutesBetweenDownload = new System.Windows.Forms.Label();
         this.labelDays = new System.Windows.Forms.Label();
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.btnCancel = new System.Windows.Forms.Button();
         this.btnOK = new System.Windows.Forms.Button();
         this.buttonDownloadNow = new System.Windows.Forms.Button();
         this.labelServerInfo = new System.Windows.Forms.Label();
         this.textPassword = new hMailServer.Administrator.Controls.ucPassword();
         this.textDaysToKeepMessages = new hMailServer.Shared.ucText();
         this.radioNeverDeleteMessages = new hMailServer.Administrator.Controls.ucRadioButton();
         this.radioDeleteMessagesAfter = new hMailServer.Administrator.Controls.ucRadioButton();
         this.radioDeleteImmediately = new hMailServer.Administrator.Controls.ucRadioButton();
         this.checkProcessMIMEDate = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkProcessMIMERecipients = new hMailServer.Administrator.Controls.ucCheckbox();
         this.textMinutesBetweenFetch = new hMailServer.Shared.ucText();
         this.checkUseSSL = new hMailServer.Administrator.Controls.ucCheckbox();
         this.textUsername = new hMailServer.Shared.ucText();
         this.textPort = new hMailServer.Shared.ucText();
         this.textServer = new hMailServer.Shared.ucText();
         this.comboServerType = new hMailServer.Administrator.Controls.ucComboBox();
         this.textName = new hMailServer.Shared.ucText();
         this.checkEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkUseAntiSpam = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkUseAntiVirus = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkEnableRouteRecipients = new hMailServer.Administrator.Controls.ucCheckbox();
         this.SuspendLayout();
         // 
         // labelName
         // 
         this.labelName.AutoSize = true;
         this.labelName.Location = new System.Drawing.Point(8, 8);
         this.labelName.Name = "labelName";
         this.labelName.Size = new System.Drawing.Size(35, 13);
         this.labelName.TabIndex = 17;
         this.labelName.Text = "Name";
         // 
         // labelServerType
         // 
         this.labelServerType.AutoSize = true;
         this.labelServerType.Location = new System.Drawing.Point(19, 101);
         this.labelServerType.Name = "labelServerType";
         this.labelServerType.Size = new System.Drawing.Size(31, 13);
         this.labelServerType.TabIndex = 19;
         this.labelServerType.Text = "Type";
         // 
         // labelPassword
         // 
         this.labelPassword.AutoSize = true;
         this.labelPassword.Location = new System.Drawing.Point(179, 221);
         this.labelPassword.Name = "labelPassword";
         this.labelPassword.Size = new System.Drawing.Size(53, 13);
         this.labelPassword.TabIndex = 37;
         this.labelPassword.Text = "Password";
         // 
         // labelUsername
         // 
         this.labelUsername.AutoSize = true;
         this.labelUsername.Location = new System.Drawing.Point(21, 221);
         this.labelUsername.Name = "labelUsername";
         this.labelUsername.Size = new System.Drawing.Size(58, 13);
         this.labelUsername.TabIndex = 35;
         this.labelUsername.Text = "User name";
         // 
         // labelTCPIPPort
         // 
         this.labelTCPIPPort.AutoSize = true;
         this.labelTCPIPPort.Location = new System.Drawing.Point(176, 152);
         this.labelTCPIPPort.Name = "labelTCPIPPort";
         this.labelTCPIPPort.Size = new System.Drawing.Size(64, 13);
         this.labelTCPIPPort.TabIndex = 33;
         this.labelTCPIPPort.Text = "TCP/IP port";
         // 
         // labelServerAddress
         // 
         this.labelServerAddress.AutoSize = true;
         this.labelServerAddress.Location = new System.Drawing.Point(22, 152);
         this.labelServerAddress.Name = "labelServerAddress";
         this.labelServerAddress.Size = new System.Drawing.Size(78, 13);
         this.labelServerAddress.TabIndex = 31;
         this.labelServerAddress.Text = "Server address";
         // 
         // labelSettings
         // 
         this.labelSettings.AutoSize = true;
         this.labelSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelSettings.Location = new System.Drawing.Point(8, 280);
         this.labelSettings.Name = "labelSettings";
         this.labelSettings.Size = new System.Drawing.Size(53, 13);
         this.labelSettings.TabIndex = 40;
         this.labelSettings.Text = "Settings";
         // 
         // labelMinutesBetweenDownload
         // 
         this.labelMinutesBetweenDownload.AutoSize = true;
         this.labelMinutesBetweenDownload.Location = new System.Drawing.Point(24, 304);
         this.labelMinutesBetweenDownload.Name = "labelMinutesBetweenDownload";
         this.labelMinutesBetweenDownload.Size = new System.Drawing.Size(137, 13);
         this.labelMinutesBetweenDownload.TabIndex = 41;
         this.labelMinutesBetweenDownload.Text = "Minutes between download";
         // 
         // labelDays
         // 
         this.labelDays.AutoSize = true;
         this.labelDays.Location = new System.Drawing.Point(319, 493);
         this.labelDays.Name = "labelDays";
         this.labelDays.Size = new System.Drawing.Size(29, 13);
         this.labelDays.TabIndex = 49;
         this.labelDays.Text = "days";
         // 
         // groupBox1
         // 
         this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.groupBox1.Location = new System.Drawing.Point(15, 545);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(361, 4);
         this.groupBox1.TabIndex = 55;
         this.groupBox1.TabStop = false;
         // 
         // btnCancel
         // 
         this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.btnCancel.Location = new System.Drawing.Point(286, 555);
         this.btnCancel.Name = "btnCancel";
         this.btnCancel.Size = new System.Drawing.Size(89, 25);
         this.btnCancel.TabIndex = 54;
         this.btnCancel.Text = "&Cancel";
         this.btnCancel.UseVisualStyleBackColor = true;
         // 
         // btnOK
         // 
         this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.btnOK.Location = new System.Drawing.Point(191, 555);
         this.btnOK.Name = "btnOK";
         this.btnOK.Size = new System.Drawing.Size(89, 25);
         this.btnOK.TabIndex = 53;
         this.btnOK.Text = "&OK";
         this.btnOK.UseVisualStyleBackColor = true;
         // 
         // buttonDownloadNow
         // 
         this.buttonDownloadNow.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonDownloadNow.Location = new System.Drawing.Point(36, 555);
         this.buttonDownloadNow.Name = "buttonDownloadNow";
         this.buttonDownloadNow.Size = new System.Drawing.Size(116, 25);
         this.buttonDownloadNow.TabIndex = 56;
         this.buttonDownloadNow.Text = "Download now";
         this.buttonDownloadNow.UseVisualStyleBackColor = true;
         this.buttonDownloadNow.Click += new System.EventHandler(this.buttonDownloadNow_Click);
         // 
         // labelServerInfo
         // 
         this.labelServerInfo.AutoSize = true;
         this.labelServerInfo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelServerInfo.Location = new System.Drawing.Point(8, 80);
         this.labelServerInfo.Name = "labelServerInfo";
         this.labelServerInfo.Size = new System.Drawing.Size(110, 13);
         this.labelServerInfo.TabIndex = 15;
         this.labelServerInfo.Text = "Server information";
         // 
         // textPassword
         // 
         this.textPassword.Location = new System.Drawing.Point(180, 237);
         this.textPassword.Name = "textPassword";
         this.textPassword.Size = new System.Drawing.Size(128, 20);
         this.textPassword.TabIndex = 57;
         this.textPassword.Text = "<< Encrypted >>";
         // 
         // textDaysToKeepMessages
         // 
         this.textDaysToKeepMessages.Location = new System.Drawing.Point(224, 490);
         this.textDaysToKeepMessages.Name = "textDaysToKeepMessages";
         this.textDaysToKeepMessages.Number = 7;
         this.textDaysToKeepMessages.Number64 = ((long)(7));
         this.textDaysToKeepMessages.Numeric = true;
         this.textDaysToKeepMessages.Size = new System.Drawing.Size(78, 20);
         this.textDaysToKeepMessages.TabIndex = 48;
         this.textDaysToKeepMessages.Text = "7";
         // 
         // radioNeverDeleteMessages
         // 
         this.radioNeverDeleteMessages.AutoSize = true;
         this.radioNeverDeleteMessages.Checked = true;
         this.radioNeverDeleteMessages.Location = new System.Drawing.Point(21, 514);
         this.radioNeverDeleteMessages.Name = "radioNeverDeleteMessages";
         this.radioNeverDeleteMessages.Size = new System.Drawing.Size(139, 17);
         this.radioNeverDeleteMessages.TabIndex = 47;
         this.radioNeverDeleteMessages.TabStop = true;
         this.radioNeverDeleteMessages.Text = "Do not delete messages";
         this.radioNeverDeleteMessages.UseVisualStyleBackColor = true;
         // 
         // radioDeleteMessagesAfter
         // 
         this.radioDeleteMessagesAfter.AutoSize = true;
         this.radioDeleteMessagesAfter.Location = new System.Drawing.Point(21, 491);
         this.radioDeleteMessagesAfter.Name = "radioDeleteMessagesAfter";
         this.radioDeleteMessagesAfter.Size = new System.Drawing.Size(130, 17);
         this.radioDeleteMessagesAfter.TabIndex = 46;
         this.radioDeleteMessagesAfter.Text = "Delete messages after";
         this.radioDeleteMessagesAfter.UseVisualStyleBackColor = true;
         // 
         // radioDeleteImmediately
         // 
         this.radioDeleteImmediately.AutoSize = true;
         this.radioDeleteImmediately.Location = new System.Drawing.Point(21, 468);
         this.radioDeleteImmediately.Name = "radioDeleteImmediately";
         this.radioDeleteImmediately.Size = new System.Drawing.Size(163, 17);
         this.radioDeleteImmediately.TabIndex = 45;
         this.radioDeleteImmediately.Text = "Delete messages immediately";
         this.radioDeleteImmediately.UseVisualStyleBackColor = true;
         // 
         // checkProcessMIMEDate
         // 
         this.checkProcessMIMEDate.AutoSize = true;
         this.checkProcessMIMEDate.Location = new System.Drawing.Point(24, 395);
         this.checkProcessMIMEDate.Name = "checkProcessMIMEDate";
         this.checkProcessMIMEDate.Size = new System.Drawing.Size(198, 17);
         this.checkProcessMIMEDate.TabIndex = 44;
         this.checkProcessMIMEDate.Text = "Retrieve date from Received header";
         this.checkProcessMIMEDate.UseVisualStyleBackColor = true;
         this.checkProcessMIMEDate.CheckedChanged += new System.EventHandler(this.checkProcessMIMEDate_CheckedChanged);
         // 
         // checkProcessMIMERecipients
         // 
         this.checkProcessMIMERecipients.AutoSize = true;
         this.checkProcessMIMERecipients.Location = new System.Drawing.Point(24, 349);
         this.checkProcessMIMERecipients.Name = "checkProcessMIMERecipients";
         this.checkProcessMIMERecipients.Size = new System.Drawing.Size(202, 17);
         this.checkProcessMIMERecipients.TabIndex = 43;
         this.checkProcessMIMERecipients.Text = "Deliver to recipients in MIME headers";
         this.checkProcessMIMERecipients.UseVisualStyleBackColor = true;
         this.checkProcessMIMERecipients.CheckedChanged += new System.EventHandler(this.checkProcessMIMERecipients_CheckedChanged);
         // 
         // textMinutesBetweenFetch
         // 
         this.textMinutesBetweenFetch.Location = new System.Drawing.Point(24, 320);
         this.textMinutesBetweenFetch.Name = "textMinutesBetweenFetch";
         this.textMinutesBetweenFetch.Number = 30;
         this.textMinutesBetweenFetch.Number64 = ((long)(30));
         this.textMinutesBetweenFetch.Numeric = true;
         this.textMinutesBetweenFetch.Size = new System.Drawing.Size(146, 20);
         this.textMinutesBetweenFetch.TabIndex = 42;
         this.textMinutesBetweenFetch.Text = "30";
         // 
         // checkUseSSL
         // 
         this.checkUseSSL.AutoSize = true;
         this.checkUseSSL.Location = new System.Drawing.Point(24, 199);
         this.checkUseSSL.Name = "checkUseSSL";
         this.checkUseSSL.Size = new System.Drawing.Size(68, 17);
         this.checkUseSSL.TabIndex = 39;
         this.checkUseSSL.Text = "Use SSL";
         this.checkUseSSL.UseVisualStyleBackColor = true;
         this.checkUseSSL.CheckedChanged += new System.EventHandler(this.checkUseSSL_CheckedChanged);
         // 
         // textUsername
         // 
         this.textUsername.Location = new System.Drawing.Point(25, 237);
         this.textUsername.Name = "textUsername";
         this.textUsername.Number = 0;
         this.textUsername.Number64 = ((long)(0));
         this.textUsername.Numeric = false;
         this.textUsername.Size = new System.Drawing.Size(148, 20);
         this.textUsername.TabIndex = 36;
         // 
         // textPort
         // 
         this.textPort.Location = new System.Drawing.Point(176, 168);
         this.textPort.Name = "textPort";
         this.textPort.Number = 0;
         this.textPort.Number64 = ((long)(0));
         this.textPort.Numeric = true;
         this.textPort.Size = new System.Drawing.Size(84, 20);
         this.textPort.TabIndex = 34;
         // 
         // textServer
         // 
         this.textServer.Location = new System.Drawing.Point(24, 168);
         this.textServer.Name = "textServer";
         this.textServer.Number = 0;
         this.textServer.Number64 = ((long)(0));
         this.textServer.Numeric = false;
         this.textServer.Size = new System.Drawing.Size(146, 20);
         this.textServer.TabIndex = 32;
         // 
         // comboServerType
         // 
         this.comboServerType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
         this.comboServerType.FormattingEnabled = true;
         this.comboServerType.Location = new System.Drawing.Point(24, 120);
         this.comboServerType.Name = "comboServerType";
         this.comboServerType.Size = new System.Drawing.Size(146, 21);
         this.comboServerType.TabIndex = 20;
         // 
         // textName
         // 
         this.textName.Location = new System.Drawing.Point(8, 24);
         this.textName.Name = "textName";
         this.textName.Number = 0;
         this.textName.Number64 = ((long)(0));
         this.textName.Numeric = false;
         this.textName.Size = new System.Drawing.Size(159, 20);
         this.textName.TabIndex = 18;
         // 
         // checkEnabled
         // 
         this.checkEnabled.AutoSize = true;
         this.checkEnabled.Location = new System.Drawing.Point(8, 50);
         this.checkEnabled.Name = "checkEnabled";
         this.checkEnabled.Size = new System.Drawing.Size(65, 17);
         this.checkEnabled.TabIndex = 14;
         this.checkEnabled.Text = "Enabled";
         this.checkEnabled.UseVisualStyleBackColor = true;
         // 
         // checkUseAntiSpam
         // 
         this.checkUseAntiSpam.AutoSize = true;
         this.checkUseAntiSpam.Location = new System.Drawing.Point(23, 418);
         this.checkUseAntiSpam.Name = "checkUseAntiSpam";
         this.checkUseAntiSpam.Size = new System.Drawing.Size(72, 17);
         this.checkUseAntiSpam.TabIndex = 59;
         this.checkUseAntiSpam.Text = "Anti-spam";
         this.checkUseAntiSpam.UseVisualStyleBackColor = true;
         // 
         // checkUseAntiVirus
         // 
         this.checkUseAntiVirus.AutoSize = true;
         this.checkUseAntiVirus.Location = new System.Drawing.Point(24, 441);
         this.checkUseAntiVirus.Name = "checkUseAntiVirus";
         this.checkUseAntiVirus.Size = new System.Drawing.Size(69, 17);
         this.checkUseAntiVirus.TabIndex = 58;
         this.checkUseAntiVirus.Text = "Anti-virus";
         this.checkUseAntiVirus.UseVisualStyleBackColor = true;
         // 
         // checkEnableRouteRecipients
         // 
         this.checkEnableRouteRecipients.AutoSize = true;
         this.checkEnableRouteRecipients.Location = new System.Drawing.Point(45, 372);
         this.checkEnableRouteRecipients.Name = "checkEnableRouteRecipients";
         this.checkEnableRouteRecipients.Size = new System.Drawing.Size(126, 17);
         this.checkEnableRouteRecipients.TabIndex = 60;
         this.checkEnableRouteRecipients.Text = "Allow route recipients";
         this.checkEnableRouteRecipients.UseVisualStyleBackColor = true;
         // 
         // formExternalAccount
         // 
         this.AcceptButton = this.btnOK;
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.CancelButton = this.btnCancel;
         this.ClientSize = new System.Drawing.Size(390, 592);
         this.Controls.Add(this.checkEnableRouteRecipients);
         this.Controls.Add(this.checkUseAntiSpam);
         this.Controls.Add(this.checkUseAntiVirus);
         this.Controls.Add(this.textPassword);
         this.Controls.Add(this.buttonDownloadNow);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.btnCancel);
         this.Controls.Add(this.btnOK);
         this.Controls.Add(this.labelDays);
         this.Controls.Add(this.textDaysToKeepMessages);
         this.Controls.Add(this.radioNeverDeleteMessages);
         this.Controls.Add(this.radioDeleteMessagesAfter);
         this.Controls.Add(this.radioDeleteImmediately);
         this.Controls.Add(this.checkProcessMIMEDate);
         this.Controls.Add(this.checkProcessMIMERecipients);
         this.Controls.Add(this.textMinutesBetweenFetch);
         this.Controls.Add(this.labelMinutesBetweenDownload);
         this.Controls.Add(this.labelSettings);
         this.Controls.Add(this.checkUseSSL);
         this.Controls.Add(this.labelPassword);
         this.Controls.Add(this.textUsername);
         this.Controls.Add(this.labelUsername);
         this.Controls.Add(this.textPort);
         this.Controls.Add(this.labelTCPIPPort);
         this.Controls.Add(this.textServer);
         this.Controls.Add(this.labelServerAddress);
         this.Controls.Add(this.comboServerType);
         this.Controls.Add(this.labelServerType);
         this.Controls.Add(this.textName);
         this.Controls.Add(this.labelName);
         this.Controls.Add(this.labelServerInfo);
         this.Controls.Add(this.checkEnabled);
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
         this.MaximizeBox = false;
         this.MinimizeBox = false;
         this.Name = "formExternalAccount";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "External account";
         this.Load += new System.EventHandler(this.formExternalAccount_Load);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private hMailServer.Administrator.Controls.ucCheckbox checkEnabled;
      private hMailServer.Shared.ucText textName;
      private System.Windows.Forms.Label labelName;
      private System.Windows.Forms.Label labelServerType;
      private hMailServer.Administrator.Controls.ucComboBox comboServerType;
      private hMailServer.Administrator.Controls.ucCheckbox checkUseSSL;
      private System.Windows.Forms.Label labelPassword;
      private hMailServer.Shared.ucText textUsername;
      private System.Windows.Forms.Label labelUsername;
      private hMailServer.Shared.ucText textPort;
      private System.Windows.Forms.Label labelTCPIPPort;
      private hMailServer.Shared.ucText textServer;
      private System.Windows.Forms.Label labelServerAddress;
      private System.Windows.Forms.Label labelSettings;
      private hMailServer.Shared.ucText textMinutesBetweenFetch;
      private System.Windows.Forms.Label labelMinutesBetweenDownload;
      private hMailServer.Administrator.Controls.ucCheckbox checkProcessMIMERecipients;
      private hMailServer.Administrator.Controls.ucCheckbox checkProcessMIMEDate;
      private hMailServer.Administrator.Controls.ucRadioButton radioDeleteImmediately;
      private hMailServer.Administrator.Controls.ucRadioButton radioDeleteMessagesAfter;
      private hMailServer.Administrator.Controls.ucRadioButton radioNeverDeleteMessages;
      private hMailServer.Shared.ucText textDaysToKeepMessages;
      private System.Windows.Forms.Label labelDays;
      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Button btnCancel;
      private System.Windows.Forms.Button btnOK;
      private System.Windows.Forms.Button buttonDownloadNow;
      private hMailServer.Administrator.Controls.ucPassword textPassword;
      private System.Windows.Forms.Label labelServerInfo;
      private hMailServer.Administrator.Controls.ucCheckbox checkUseAntiSpam;
      private hMailServer.Administrator.Controls.ucCheckbox checkUseAntiVirus;
      private hMailServer.Administrator.Controls.ucCheckbox checkEnableRouteRecipients;
   }
}