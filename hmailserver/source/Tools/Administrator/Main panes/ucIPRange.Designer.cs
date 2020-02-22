namespace hMailServer.Administrator
{
    partial class ucIPRange
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
           this.labelGeneral = new System.Windows.Forms.Label();
           this.labelName = new System.Windows.Forms.Label();
           this.labelPriority = new System.Windows.Forms.Label();
           this.labelLowerIP = new System.Windows.Forms.Label();
           this.labelUpperIP = new System.Windows.Forms.Label();
           this.labelAllowConnections = new System.Windows.Forms.Label();
           this.labelAllowDeliveries = new System.Windows.Forms.Label();
           this.label1 = new System.Windows.Forms.Label();
           this.label2 = new System.Windows.Forms.Label();
           this.checkRequireSMTPAuthLocalToExternal = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkRequireSMTPAuthExternalToLocal = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkRequireSMTPAuthExternalToExternal = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkRequireSMTPAuthLocalToLocal = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkExpires = new hMailServer.Administrator.Controls.ucCheckbox();
           this.dateTimeExpiresTime = new hMailServer.Administrator.Controls.ucDateTimePicker();
           this.checkAntiVirus = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkSpamProtection = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAllowDeliveiesFromL2R = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAllowDeliveiesFromR2L = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAllowDeliveiesFromR2R = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAllowDeliveiesFromL2L = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAllowPOP3 = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAllowIMAP = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAllowSMTP = new hMailServer.Administrator.Controls.ucCheckbox();
           this.textPriority = new hMailServer.Shared.ucText();
           this.textName = new hMailServer.Shared.ucText();
           this.textLowerIPAddress = new hMailServer.Administrator.Controls.ucIPAddress();
           this.textUpperIPAddress = new hMailServer.Administrator.Controls.ucIPAddress();
           this.checkRequireSSLTLSForAuth = new hMailServer.Administrator.Controls.ucCheckbox();
           this.SuspendLayout();
           // 
           // labelGeneral
           // 
           this.labelGeneral.AutoSize = true;
           this.labelGeneral.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelGeneral.Location = new System.Drawing.Point(7, 7);
           this.labelGeneral.Name = "labelGeneral";
           this.labelGeneral.Size = new System.Drawing.Size(51, 13);
           this.labelGeneral.TabIndex = 12;
           this.labelGeneral.Text = "General";
           // 
           // labelName
           // 
           this.labelName.AutoSize = true;
           this.labelName.Location = new System.Drawing.Point(29, 32);
           this.labelName.Name = "labelName";
           this.labelName.Size = new System.Drawing.Size(35, 13);
           this.labelName.TabIndex = 13;
           this.labelName.Text = "Name";
           // 
           // labelPriority
           // 
           this.labelPriority.AutoSize = true;
           this.labelPriority.Location = new System.Drawing.Point(276, 32);
           this.labelPriority.Name = "labelPriority";
           this.labelPriority.Size = new System.Drawing.Size(38, 13);
           this.labelPriority.TabIndex = 15;
           this.labelPriority.Text = "Priority";
           // 
           // labelLowerIP
           // 
           this.labelLowerIP.AutoSize = true;
           this.labelLowerIP.Location = new System.Drawing.Point(29, 75);
           this.labelLowerIP.Name = "labelLowerIP";
           this.labelLowerIP.Size = new System.Drawing.Size(49, 13);
           this.labelLowerIP.TabIndex = 17;
           this.labelLowerIP.Text = "Lower IP";
           // 
           // labelUpperIP
           // 
           this.labelUpperIP.AutoSize = true;
           this.labelUpperIP.Location = new System.Drawing.Point(276, 75);
           this.labelUpperIP.Name = "labelUpperIP";
           this.labelUpperIP.Size = new System.Drawing.Size(49, 13);
           this.labelUpperIP.TabIndex = 18;
           this.labelUpperIP.Text = "Upper IP";
           // 
           // labelAllowConnections
           // 
           this.labelAllowConnections.AutoSize = true;
           this.labelAllowConnections.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelAllowConnections.Location = new System.Drawing.Point(3, 182);
           this.labelAllowConnections.Name = "labelAllowConnections";
           this.labelAllowConnections.Size = new System.Drawing.Size(110, 13);
           this.labelAllowConnections.TabIndex = 27;
           this.labelAllowConnections.Text = "Allow connections";
           // 
           // labelAllowDeliveries
           // 
           this.labelAllowDeliveries.AutoSize = true;
           this.labelAllowDeliveries.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelAllowDeliveries.Location = new System.Drawing.Point(3, 287);
           this.labelAllowDeliveries.Name = "labelAllowDeliveries";
           this.labelAllowDeliveries.Size = new System.Drawing.Size(123, 13);
           this.labelAllowDeliveries.TabIndex = 34;
           this.labelAllowDeliveries.Text = "Allow deliveries from";
           // 
           // label1
           // 
           this.label1.AutoSize = true;
           this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.label1.Location = new System.Drawing.Point(276, 180);
           this.label1.Name = "label1";
           this.label1.Size = new System.Drawing.Size(38, 13);
           this.label1.TabIndex = 39;
           this.label1.Text = "Other";
           // 
           // label2
           // 
           this.label2.AutoSize = true;
           this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.label2.Location = new System.Drawing.Point(276, 287);
           this.label2.Name = "label2";
           this.label2.Size = new System.Drawing.Size(174, 13);
           this.label2.TabIndex = 55;
           this.label2.Text = "Require SMTP authentication";
           // 
           // checkRequireSMTPAuthLocalToExternal
           // 
           this.checkRequireSMTPAuthLocalToExternal.AutoSize = true;
           this.checkRequireSMTPAuthLocalToExternal.Checked = true;
           this.checkRequireSMTPAuthLocalToExternal.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkRequireSMTPAuthLocalToExternal.Location = new System.Drawing.Point(293, 337);
           this.checkRequireSMTPAuthLocalToExternal.Name = "checkRequireSMTPAuthLocalToExternal";
           this.checkRequireSMTPAuthLocalToExternal.Size = new System.Drawing.Size(185, 17);
           this.checkRequireSMTPAuthLocalToExternal.TabIndex = 59;
           this.checkRequireSMTPAuthLocalToExternal.Text = "Local to external e-mail addresses";
           this.checkRequireSMTPAuthLocalToExternal.UseVisualStyleBackColor = true;
           // 
           // checkRequireSMTPAuthExternalToLocal
           // 
           this.checkRequireSMTPAuthExternalToLocal.AutoSize = true;
           this.checkRequireSMTPAuthExternalToLocal.Location = new System.Drawing.Point(293, 360);
           this.checkRequireSMTPAuthExternalToLocal.Name = "checkRequireSMTPAuthExternalToLocal";
           this.checkRequireSMTPAuthExternalToLocal.Size = new System.Drawing.Size(182, 17);
           this.checkRequireSMTPAuthExternalToLocal.TabIndex = 58;
           this.checkRequireSMTPAuthExternalToLocal.Text = "External to local e-mail addresses";
           this.checkRequireSMTPAuthExternalToLocal.UseVisualStyleBackColor = true;
           // 
           // checkRequireSMTPAuthExternalToExternal
           // 
           this.checkRequireSMTPAuthExternalToExternal.AutoSize = true;
           this.checkRequireSMTPAuthExternalToExternal.Checked = true;
           this.checkRequireSMTPAuthExternalToExternal.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkRequireSMTPAuthExternalToExternal.Location = new System.Drawing.Point(293, 383);
           this.checkRequireSMTPAuthExternalToExternal.Name = "checkRequireSMTPAuthExternalToExternal";
           this.checkRequireSMTPAuthExternalToExternal.Size = new System.Drawing.Size(197, 17);
           this.checkRequireSMTPAuthExternalToExternal.TabIndex = 57;
           this.checkRequireSMTPAuthExternalToExternal.Text = "External to external e-mail addresses";
           this.checkRequireSMTPAuthExternalToExternal.UseVisualStyleBackColor = true;
           // 
           // checkRequireSMTPAuthLocalToLocal
           // 
           this.checkRequireSMTPAuthLocalToLocal.AutoSize = true;
           this.checkRequireSMTPAuthLocalToLocal.Checked = true;
           this.checkRequireSMTPAuthLocalToLocal.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkRequireSMTPAuthLocalToLocal.Location = new System.Drawing.Point(293, 314);
           this.checkRequireSMTPAuthLocalToLocal.Name = "checkRequireSMTPAuthLocalToLocal";
           this.checkRequireSMTPAuthLocalToLocal.Size = new System.Drawing.Size(170, 17);
           this.checkRequireSMTPAuthLocalToLocal.TabIndex = 56;
           this.checkRequireSMTPAuthLocalToLocal.Text = "Local to local e-mail addresses";
           this.checkRequireSMTPAuthLocalToLocal.UseVisualStyleBackColor = true;
           // 
           // checkExpires
           // 
           this.checkExpires.AutoSize = true;
           this.checkExpires.Location = new System.Drawing.Point(32, 126);
           this.checkExpires.Name = "checkExpires";
           this.checkExpires.Size = new System.Drawing.Size(60, 17);
           this.checkExpires.TabIndex = 51;
           this.checkExpires.Text = "Expires";
           this.checkExpires.UseVisualStyleBackColor = true;
           this.checkExpires.CheckedChanged += new System.EventHandler(this.checkExpires_CheckedChanged);
           // 
           // dateTimeExpiresTime
           // 
           this.dateTimeExpiresTime.CustomFormat = "yyyy\'-\'MM\'-\'dd HH\':\'mm\':\'ss";
           this.dateTimeExpiresTime.Enabled = false;
           this.dateTimeExpiresTime.Format = System.Windows.Forms.DateTimePickerFormat.Custom;
           this.dateTimeExpiresTime.Location = new System.Drawing.Point(32, 149);
           this.dateTimeExpiresTime.Name = "dateTimeExpiresTime";
           this.dateTimeExpiresTime.Size = new System.Drawing.Size(174, 20);
           this.dateTimeExpiresTime.TabIndex = 50;
           this.dateTimeExpiresTime.Value = new System.DateTime(2008, 12, 30, 18, 16, 13, 78);
           // 
           // checkAntiVirus
           // 
           this.checkAntiVirus.AutoSize = true;
           this.checkAntiVirus.Checked = true;
           this.checkAntiVirus.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkAntiVirus.Location = new System.Drawing.Point(294, 230);
           this.checkAntiVirus.Name = "checkAntiVirus";
           this.checkAntiVirus.Size = new System.Drawing.Size(69, 17);
           this.checkAntiVirus.TabIndex = 41;
           this.checkAntiVirus.Text = "Anti-virus";
           this.checkAntiVirus.UseVisualStyleBackColor = true;
           // 
           // checkSpamProtection
           // 
           this.checkSpamProtection.AutoSize = true;
           this.checkSpamProtection.Checked = true;
           this.checkSpamProtection.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkSpamProtection.Location = new System.Drawing.Point(294, 207);
           this.checkSpamProtection.Name = "checkSpamProtection";
           this.checkSpamProtection.Size = new System.Drawing.Size(72, 17);
           this.checkSpamProtection.TabIndex = 40;
           this.checkSpamProtection.Text = "Anti-spam";
           this.checkSpamProtection.UseVisualStyleBackColor = true;
           // 
           // checkAllowDeliveiesFromL2R
           // 
           this.checkAllowDeliveiesFromL2R.AutoSize = true;
           this.checkAllowDeliveiesFromL2R.Checked = true;
           this.checkAllowDeliveiesFromL2R.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkAllowDeliveiesFromL2R.Location = new System.Drawing.Point(28, 337);
           this.checkAllowDeliveiesFromL2R.Name = "checkAllowDeliveiesFromL2R";
           this.checkAllowDeliveiesFromL2R.Size = new System.Drawing.Size(185, 17);
           this.checkAllowDeliveiesFromL2R.TabIndex = 38;
           this.checkAllowDeliveiesFromL2R.Text = "Local to external e-mail addresses";
           this.checkAllowDeliveiesFromL2R.UseVisualStyleBackColor = true;
           this.checkAllowDeliveiesFromL2R.CheckedChanged += new System.EventHandler(this.checkAllowDeliveiesFromL2R_CheckedChanged);
           // 
           // checkAllowDeliveiesFromR2L
           // 
           this.checkAllowDeliveiesFromR2L.AutoSize = true;
           this.checkAllowDeliveiesFromR2L.Checked = true;
           this.checkAllowDeliveiesFromR2L.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkAllowDeliveiesFromR2L.Location = new System.Drawing.Point(28, 360);
           this.checkAllowDeliveiesFromR2L.Name = "checkAllowDeliveiesFromR2L";
           this.checkAllowDeliveiesFromR2L.Size = new System.Drawing.Size(182, 17);
           this.checkAllowDeliveiesFromR2L.TabIndex = 37;
           this.checkAllowDeliveiesFromR2L.Text = "External to local e-mail addresses";
           this.checkAllowDeliveiesFromR2L.UseVisualStyleBackColor = true;
           this.checkAllowDeliveiesFromR2L.CheckedChanged += new System.EventHandler(this.checkAllowDeliveiesFromR2L_CheckedChanged);
           // 
           // checkAllowDeliveiesFromR2R
           // 
           this.checkAllowDeliveiesFromR2R.AutoSize = true;
           this.checkAllowDeliveiesFromR2R.Checked = true;
           this.checkAllowDeliveiesFromR2R.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkAllowDeliveiesFromR2R.Location = new System.Drawing.Point(28, 383);
           this.checkAllowDeliveiesFromR2R.Name = "checkAllowDeliveiesFromR2R";
           this.checkAllowDeliveiesFromR2R.Size = new System.Drawing.Size(197, 17);
           this.checkAllowDeliveiesFromR2R.TabIndex = 36;
           this.checkAllowDeliveiesFromR2R.Text = "External to external e-mail addresses";
           this.checkAllowDeliveiesFromR2R.UseVisualStyleBackColor = true;
           this.checkAllowDeliveiesFromR2R.CheckedChanged += new System.EventHandler(this.checkAllowDeliveiesFromR2R_CheckedChanged);
           // 
           // checkAllowDeliveiesFromL2L
           // 
           this.checkAllowDeliveiesFromL2L.AutoSize = true;
           this.checkAllowDeliveiesFromL2L.Checked = true;
           this.checkAllowDeliveiesFromL2L.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkAllowDeliveiesFromL2L.Location = new System.Drawing.Point(28, 314);
           this.checkAllowDeliveiesFromL2L.Name = "checkAllowDeliveiesFromL2L";
           this.checkAllowDeliveiesFromL2L.Size = new System.Drawing.Size(170, 17);
           this.checkAllowDeliveiesFromL2L.TabIndex = 35;
           this.checkAllowDeliveiesFromL2L.Text = "Local to local e-mail addresses";
           this.checkAllowDeliveiesFromL2L.UseVisualStyleBackColor = true;
           this.checkAllowDeliveiesFromL2L.CheckedChanged += new System.EventHandler(this.checkAllowDeliveiesFromL2L_CheckedChanged);
           // 
           // checkAllowPOP3
           // 
           this.checkAllowPOP3.AutoSize = true;
           this.checkAllowPOP3.Checked = true;
           this.checkAllowPOP3.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkAllowPOP3.Location = new System.Drawing.Point(28, 231);
           this.checkAllowPOP3.Name = "checkAllowPOP3";
           this.checkAllowPOP3.Size = new System.Drawing.Size(54, 17);
           this.checkAllowPOP3.TabIndex = 30;
           this.checkAllowPOP3.Text = "POP3";
           this.checkAllowPOP3.UseVisualStyleBackColor = true;
           // 
           // checkAllowIMAP
           // 
           this.checkAllowIMAP.AutoSize = true;
           this.checkAllowIMAP.Checked = true;
           this.checkAllowIMAP.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkAllowIMAP.Location = new System.Drawing.Point(28, 254);
           this.checkAllowIMAP.Name = "checkAllowIMAP";
           this.checkAllowIMAP.Size = new System.Drawing.Size(52, 17);
           this.checkAllowIMAP.TabIndex = 29;
           this.checkAllowIMAP.Text = "IMAP";
           this.checkAllowIMAP.UseVisualStyleBackColor = true;
           // 
           // checkAllowSMTP
           // 
           this.checkAllowSMTP.AutoSize = true;
           this.checkAllowSMTP.Checked = true;
           this.checkAllowSMTP.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkAllowSMTP.Location = new System.Drawing.Point(28, 208);
           this.checkAllowSMTP.Name = "checkAllowSMTP";
           this.checkAllowSMTP.Size = new System.Drawing.Size(56, 17);
           this.checkAllowSMTP.TabIndex = 28;
           this.checkAllowSMTP.Text = "SMTP";
           this.checkAllowSMTP.UseVisualStyleBackColor = true;
           // 
           // textPriority
           // 
           this.textPriority.Location = new System.Drawing.Point(279, 48);
           this.textPriority.Name = "textPriority";
           this.textPriority.Number = 0;
           this.textPriority.Number64 = ((long)(0));
           this.textPriority.Numeric = true;
           this.textPriority.Size = new System.Drawing.Size(90, 20);
           this.textPriority.TabIndex = 14;
           // 
           // textName
           // 
           this.textName.Location = new System.Drawing.Point(32, 48);
           this.textName.Name = "textName";
           this.textName.Number = 0;
           this.textName.Number64 = ((long)(0));
           this.textName.Numeric = false;
           this.textName.Size = new System.Drawing.Size(129, 20);
           this.textName.TabIndex = 11;
           // 
           // textLowerIPAddress
           // 
           this.textLowerIPAddress.Location = new System.Drawing.Point(32, 91);
           this.textLowerIPAddress.Name = "textLowerIPAddress";
           this.textLowerIPAddress.Size = new System.Drawing.Size(239, 20);
           this.textLowerIPAddress.TabIndex = 60;
           // 
           // textUpperIPAddress
           // 
           this.textUpperIPAddress.Location = new System.Drawing.Point(277, 91);
           this.textUpperIPAddress.Name = "textUpperIPAddress";
           this.textUpperIPAddress.Size = new System.Drawing.Size(239, 20);
           this.textUpperIPAddress.TabIndex = 61;
           // 
           // checkRequireSSLTLSForAuth
           // 
           this.checkRequireSSLTLSForAuth.AutoSize = true;
           this.checkRequireSSLTLSForAuth.Checked = true;
           this.checkRequireSSLTLSForAuth.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkRequireSSLTLSForAuth.Location = new System.Drawing.Point(293, 253);
           this.checkRequireSSLTLSForAuth.Name = "checkRequireSSLTLSForAuth";
           this.checkRequireSSLTLSForAuth.Size = new System.Drawing.Size(196, 17);
           this.checkRequireSSLTLSForAuth.TabIndex = 62;
           this.checkRequireSSLTLSForAuth.Text = "Require SSL/TLS for authentication";
           this.checkRequireSSLTLSForAuth.UseVisualStyleBackColor = true;
           // 
           // ucIPRange
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.checkRequireSSLTLSForAuth);
           this.Controls.Add(this.textUpperIPAddress);
           this.Controls.Add(this.textLowerIPAddress);
           this.Controls.Add(this.checkRequireSMTPAuthLocalToExternal);
           this.Controls.Add(this.checkRequireSMTPAuthExternalToLocal);
           this.Controls.Add(this.checkRequireSMTPAuthExternalToExternal);
           this.Controls.Add(this.checkRequireSMTPAuthLocalToLocal);
           this.Controls.Add(this.label2);
           this.Controls.Add(this.checkExpires);
           this.Controls.Add(this.dateTimeExpiresTime);
           this.Controls.Add(this.checkAntiVirus);
           this.Controls.Add(this.checkSpamProtection);
           this.Controls.Add(this.label1);
           this.Controls.Add(this.checkAllowDeliveiesFromL2R);
           this.Controls.Add(this.checkAllowDeliveiesFromR2L);
           this.Controls.Add(this.checkAllowDeliveiesFromR2R);
           this.Controls.Add(this.checkAllowDeliveiesFromL2L);
           this.Controls.Add(this.labelAllowDeliveries);
           this.Controls.Add(this.checkAllowPOP3);
           this.Controls.Add(this.checkAllowIMAP);
           this.Controls.Add(this.checkAllowSMTP);
           this.Controls.Add(this.labelAllowConnections);
           this.Controls.Add(this.labelUpperIP);
           this.Controls.Add(this.labelLowerIP);
           this.Controls.Add(this.labelPriority);
           this.Controls.Add(this.textPriority);
           this.Controls.Add(this.labelName);
           this.Controls.Add(this.labelGeneral);
           this.Controls.Add(this.textName);
           this.Name = "ucIPRange";
           this.Size = new System.Drawing.Size(646, 430);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private System.Windows.Forms.Label labelGeneral;
       private hMailServer.Shared.ucText textName;
       private System.Windows.Forms.Label labelName;
       private System.Windows.Forms.Label labelPriority;
       private hMailServer.Shared.ucText textPriority;
       private System.Windows.Forms.Label labelLowerIP;
       private System.Windows.Forms.Label labelUpperIP;
       private System.Windows.Forms.Label labelAllowConnections;
       private hMailServer.Administrator.Controls.ucCheckbox checkAllowSMTP;
       private hMailServer.Administrator.Controls.ucCheckbox checkAllowIMAP;
       private hMailServer.Administrator.Controls.ucCheckbox checkAllowPOP3;
       private System.Windows.Forms.Label labelAllowDeliveries;
       private hMailServer.Administrator.Controls.ucCheckbox checkAllowDeliveiesFromL2L;
       private hMailServer.Administrator.Controls.ucCheckbox checkAllowDeliveiesFromR2R;
       private hMailServer.Administrator.Controls.ucCheckbox checkAllowDeliveiesFromR2L;
       private hMailServer.Administrator.Controls.ucCheckbox checkAllowDeliveiesFromL2R;
       private System.Windows.Forms.Label label1;
       private hMailServer.Administrator.Controls.ucCheckbox checkSpamProtection;
       private hMailServer.Administrator.Controls.ucCheckbox checkAntiVirus;
       private hMailServer.Administrator.Controls.ucDateTimePicker dateTimeExpiresTime;
       private hMailServer.Administrator.Controls.ucCheckbox checkExpires;
       private System.Windows.Forms.Label label2;
       private hMailServer.Administrator.Controls.ucCheckbox checkRequireSMTPAuthLocalToExternal;
       private hMailServer.Administrator.Controls.ucCheckbox checkRequireSMTPAuthExternalToLocal;
       private hMailServer.Administrator.Controls.ucCheckbox checkRequireSMTPAuthExternalToExternal;
       private hMailServer.Administrator.Controls.ucCheckbox checkRequireSMTPAuthLocalToLocal;
       private hMailServer.Administrator.Controls.ucIPAddress textLowerIPAddress;
       private hMailServer.Administrator.Controls.ucIPAddress textUpperIPAddress;
       private hMailServer.Administrator.Controls.ucCheckbox checkRequireSSLTLSForAuth;
    }
}
