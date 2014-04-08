namespace hMailServer.Administrator
{
    partial class ucTCPIPPort
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
           this.labelProtocol = new System.Windows.Forms.Label();
           this.labelTCPIPAddress = new System.Windows.Forms.Label();
           this.labelTCPIPPort = new System.Windows.Forms.Label();
           this.labelSSLCertificate = new System.Windows.Forms.Label();
           this.labelRestartInfo = new System.Windows.Forms.Label();
           this.textIPAddress = new hMailServer.Administrator.Controls.ucIPAddress();
           this.checkEnableSSL = new hMailServer.Administrator.Controls.ucCheckbox();
           this.comboSSLCertificate = new hMailServer.Administrator.Controls.ucComboBox();
           this.textTCPIPPort = new hMailServer.Shared.ucText();
           this.comboProtocol = new hMailServer.Administrator.Controls.ucComboBox();
           this.SuspendLayout();
           // 
           // labelProtocol
           // 
           this.labelProtocol.AutoSize = true;
           this.labelProtocol.Location = new System.Drawing.Point(5, 8);
           this.labelProtocol.Name = "labelProtocol";
           this.labelProtocol.Size = new System.Drawing.Size(46, 13);
           this.labelProtocol.TabIndex = 20;
           this.labelProtocol.Text = "Protocol";
           // 
           // labelTCPIPAddress
           // 
           this.labelTCPIPAddress.AutoSize = true;
           this.labelTCPIPAddress.Location = new System.Drawing.Point(5, 58);
           this.labelTCPIPAddress.Name = "labelTCPIPAddress";
           this.labelTCPIPAddress.Size = new System.Drawing.Size(83, 13);
           this.labelTCPIPAddress.TabIndex = 23;
           this.labelTCPIPAddress.Text = "TCP/IP address";
           // 
           // labelTCPIPPort
           // 
           this.labelTCPIPPort.AutoSize = true;
           this.labelTCPIPPort.Location = new System.Drawing.Point(5, 104);
           this.labelTCPIPPort.Name = "labelTCPIPPort";
           this.labelTCPIPPort.Size = new System.Drawing.Size(64, 13);
           this.labelTCPIPPort.TabIndex = 28;
           this.labelTCPIPPort.Text = "TCP/IP port";
           // 
           // labelSSLCertificate
           // 
           this.labelSSLCertificate.AutoSize = true;
           this.labelSSLCertificate.Location = new System.Drawing.Point(27, 174);
           this.labelSSLCertificate.Name = "labelSSLCertificate";
           this.labelSSLCertificate.Size = new System.Drawing.Size(77, 13);
           this.labelSSLCertificate.TabIndex = 31;
           this.labelSSLCertificate.Text = "SSL Certificate";
           // 
           // labelRestartInfo
           // 
           this.labelRestartInfo.AutoSize = true;
           this.labelRestartInfo.Location = new System.Drawing.Point(3, 237);
           this.labelRestartInfo.Name = "labelRestartInfo";
           this.labelRestartInfo.Size = new System.Drawing.Size(451, 13);
           this.labelRestartInfo.TabIndex = 34;
           this.labelRestartInfo.Text = "If you change these settings, hMailServer needs to be restarted for the changes t" +
               "o take effect.";
           // 
           // textIPAddress
           // 
           this.textIPAddress.Location = new System.Drawing.Point(8, 74);
           this.textIPAddress.Name = "textIPAddress";
           this.textIPAddress.Size = new System.Drawing.Size(239, 20);
           this.textIPAddress.TabIndex = 35;
           // 
           // checkEnableSSL
           // 
           this.checkEnableSSL.AutoSize = true;
           this.checkEnableSSL.Location = new System.Drawing.Point(6, 150);
           this.checkEnableSSL.Name = "checkEnableSSL";
           this.checkEnableSSL.Size = new System.Drawing.Size(68, 17);
           this.checkEnableSSL.TabIndex = 33;
           this.checkEnableSSL.Text = "Use SSL";
           this.checkEnableSSL.UseVisualStyleBackColor = true;
           this.checkEnableSSL.CheckedChanged += new System.EventHandler(this.checkEnableSSL_CheckedChanged);
           // 
           // comboSSLCertificate
           // 
           this.comboSSLCertificate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
           this.comboSSLCertificate.FormattingEnabled = true;
           this.comboSSLCertificate.Location = new System.Drawing.Point(30, 190);
           this.comboSSLCertificate.Name = "comboSSLCertificate";
           this.comboSSLCertificate.Size = new System.Drawing.Size(171, 21);
           this.comboSSLCertificate.TabIndex = 30;
           // 
           // textTCPIPPort
           // 
           this.textTCPIPPort.Location = new System.Drawing.Point(8, 120);
           this.textTCPIPPort.Name = "textTCPIPPort";
           this.textTCPIPPort.Number = 0;
           this.textTCPIPPort.Numeric = true;
           this.textTCPIPPort.Size = new System.Drawing.Size(46, 20);
           this.textTCPIPPort.TabIndex = 29;
           this.textTCPIPPort.Text = "0";
           // 
           // comboProtocol
           // 
           this.comboProtocol.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
           this.comboProtocol.FormattingEnabled = true;
           this.comboProtocol.Location = new System.Drawing.Point(8, 24);
           this.comboProtocol.Name = "comboProtocol";
           this.comboProtocol.Size = new System.Drawing.Size(171, 21);
           this.comboProtocol.TabIndex = 0;
           // 
           // ucTCPIPPort
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.textIPAddress);
           this.Controls.Add(this.labelRestartInfo);
           this.Controls.Add(this.checkEnableSSL);
           this.Controls.Add(this.labelSSLCertificate);
           this.Controls.Add(this.comboSSLCertificate);
           this.Controls.Add(this.textTCPIPPort);
           this.Controls.Add(this.labelTCPIPPort);
           this.Controls.Add(this.labelTCPIPAddress);
           this.Controls.Add(this.labelProtocol);
           this.Controls.Add(this.comboProtocol);
           this.Name = "ucTCPIPPort";
           this.Size = new System.Drawing.Size(494, 280);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private hMailServer.Administrator.Controls.ucComboBox comboProtocol;
       private System.Windows.Forms.Label labelProtocol;
       private System.Windows.Forms.Label labelTCPIPAddress;
       private hMailServer.Shared.ucText textTCPIPPort;
       private System.Windows.Forms.Label labelTCPIPPort;
       private System.Windows.Forms.Label labelSSLCertificate;
       private hMailServer.Administrator.Controls.ucComboBox comboSSLCertificate;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnableSSL;
       private System.Windows.Forms.Label labelRestartInfo;
       private hMailServer.Administrator.Controls.ucIPAddress textIPAddress;
    }
}
