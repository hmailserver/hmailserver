namespace hMailServer.Administrator
{
   partial class ucSSLTLS
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
            this.labelsslTlsCiphers = new System.Windows.Forms.Label();
            this.textSslCipherList = new hMailServer.Shared.ucText();
            this.checkVerifyRemoteServerSslCertificate = new hMailServer.Administrator.Controls.ucCheckbox();
            this.labelVersions = new System.Windows.Forms.Label();
            this.checkTlsVersion10 = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkTlsVersion11 = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkTlsVersion12 = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkTlsVersion13 = new hMailServer.Administrator.Controls.ucCheckbox();
            this.SuspendLayout();
            // 
            // labelsslTlsCiphers
            // 
            this.labelsslTlsCiphers.AutoSize = true;
            this.labelsslTlsCiphers.Location = new System.Drawing.Point(7, 7);
            this.labelsslTlsCiphers.Name = "labelsslTlsCiphers";
            this.labelsslTlsCiphers.Size = new System.Drawing.Size(89, 13);
            this.labelsslTlsCiphers.TabIndex = 16;
            this.labelsslTlsCiphers.Text = "SSL/TLS ciphers";
            // 
            // textSslCipherList
            // 
            this.textSslCipherList.Location = new System.Drawing.Point(10, 23);
            this.textSslCipherList.Multiline = true;
            this.textSslCipherList.Name = "textSslCipherList";
            this.textSslCipherList.Number = 0;
            this.textSslCipherList.Number64 = ((long)(0));
            this.textSslCipherList.Numeric = false;
            this.textSslCipherList.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textSslCipherList.Size = new System.Drawing.Size(542, 158);
            this.textSslCipherList.TabIndex = 2;
            // 
            // checkVerifyRemoteServerSslCertificate
            // 
            this.checkVerifyRemoteServerSslCertificate.AutoSize = true;
            this.checkVerifyRemoteServerSslCertificate.Checked = true;
            this.checkVerifyRemoteServerSslCertificate.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkVerifyRemoteServerSslCertificate.Location = new System.Drawing.Point(10, 187);
            this.checkVerifyRemoteServerSslCertificate.Name = "checkVerifyRemoteServerSslCertificate";
            this.checkVerifyRemoteServerSslCertificate.Size = new System.Drawing.Size(221, 17);
            this.checkVerifyRemoteServerSslCertificate.TabIndex = 1;
            this.checkVerifyRemoteServerSslCertificate.Text = "Verify remote server SSL/TLS certificates";
            this.checkVerifyRemoteServerSslCertificate.UseVisualStyleBackColor = true;
            // 
            // labelVersions
            // 
            this.labelVersions.AutoSize = true;
            this.labelVersions.Location = new System.Drawing.Point(7, 219);
            this.labelVersions.Name = "labelVersions";
            this.labelVersions.Size = new System.Drawing.Size(47, 13);
            this.labelVersions.TabIndex = 20;
            this.labelVersions.Text = "Versions\r\n";
            // 
            // checkTlsVersion10
            // 
            this.checkTlsVersion10.AutoSize = true;
            this.checkTlsVersion10.Checked = true;
            this.checkTlsVersion10.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkTlsVersion10.Location = new System.Drawing.Point(33, 247);
            this.checkTlsVersion10.Name = "checkTlsVersion10";
            this.checkTlsVersion10.Size = new System.Drawing.Size(70, 17);
            this.checkTlsVersion10.TabIndex = 21;
            this.checkTlsVersion10.Text = "TLS v1.0";
            this.checkTlsVersion10.UseVisualStyleBackColor = true;
            // 
            // checkTlsVersion11
            // 
            this.checkTlsVersion11.AutoSize = true;
            this.checkTlsVersion11.Checked = true;
            this.checkTlsVersion11.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkTlsVersion11.Location = new System.Drawing.Point(33, 270);
            this.checkTlsVersion11.Name = "checkTlsVersion11";
            this.checkTlsVersion11.Size = new System.Drawing.Size(70, 17);
            this.checkTlsVersion11.TabIndex = 22;
            this.checkTlsVersion11.Text = "TLS v1.1";
            this.checkTlsVersion11.UseVisualStyleBackColor = true;
            // 
            // checkTlsVersion12
            // 
            this.checkTlsVersion12.AutoSize = true;
            this.checkTlsVersion12.Checked = true;
            this.checkTlsVersion12.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkTlsVersion12.Location = new System.Drawing.Point(33, 293);
            this.checkTlsVersion12.Name = "checkTlsVersion12";
            this.checkTlsVersion12.Size = new System.Drawing.Size(70, 17);
            this.checkTlsVersion12.TabIndex = 23;
            this.checkTlsVersion12.Text = "TLS v1.2";
            this.checkTlsVersion12.UseVisualStyleBackColor = true;
            // 
            // checkTlsVersion13
            // 
            this.checkTlsVersion13.AutoSize = true;
            this.checkTlsVersion13.Checked = true;
            this.checkTlsVersion13.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkTlsVersion13.Location = new System.Drawing.Point(33, 316);
            this.checkTlsVersion13.Name = "checkTlsVersion13";
            this.checkTlsVersion13.Size = new System.Drawing.Size(70, 17);
            this.checkTlsVersion13.TabIndex = 24;
            this.checkTlsVersion13.Text = "TLS v1.3";
            this.checkTlsVersion13.UseVisualStyleBackColor = true;
            // 
            // ucSSLTLS
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.checkTlsVersion13);
            this.Controls.Add(this.checkTlsVersion12);
            this.Controls.Add(this.checkTlsVersion11);
            this.Controls.Add(this.checkTlsVersion10);
            this.Controls.Add(this.labelVersions);
            this.Controls.Add(this.textSslCipherList);
            this.Controls.Add(this.labelsslTlsCiphers);
            this.Controls.Add(this.checkVerifyRemoteServerSslCertificate);
            this.Name = "ucSSLTLS";
            this.Size = new System.Drawing.Size(596, 495);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelsslTlsCiphers;
        private hMailServer.Shared.ucText textSslCipherList;
        private Controls.ucCheckbox checkVerifyRemoteServerSslCertificate;
        private System.Windows.Forms.Label labelVersions;
        private Controls.ucCheckbox checkTlsVersion10;
        private Controls.ucCheckbox checkTlsVersion11;
        private Controls.ucCheckbox checkTlsVersion12;
        private Controls.ucCheckbox checkTlsVersion13;

    }
}
