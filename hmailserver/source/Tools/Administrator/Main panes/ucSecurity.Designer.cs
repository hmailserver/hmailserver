namespace hMailServer.Administrator
{
   partial class ucSecurity
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
           this.labelSSLTLS = new System.Windows.Forms.Label();
           this.textSslCipherList = new hMailServer.Shared.ucText();
           this.checkVerifyRemoteServerSslCertificate = new hMailServer.Administrator.Controls.ucCheckbox();
           this.SuspendLayout();
           // 
           // labelsslTlsCiphers
           // 
           this.labelsslTlsCiphers.AutoSize = true;
           this.labelsslTlsCiphers.Location = new System.Drawing.Point(8, 56);
           this.labelsslTlsCiphers.Name = "labelsslTlsCiphers";
           this.labelsslTlsCiphers.Size = new System.Drawing.Size(92, 13);
           this.labelsslTlsCiphers.TabIndex = 16;
           this.labelsslTlsCiphers.Text = "SSL/TLS ciphers ";
           // 
           // labelSSLTLS
           // 
           this.labelSSLTLS.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelSSLTLS.Location = new System.Drawing.Point(8, 8);
           this.labelSSLTLS.Name = "labelSSLTLS";
           this.labelSSLTLS.Size = new System.Drawing.Size(154, 20);
           this.labelSSLTLS.TabIndex = 18;
           this.labelSSLTLS.Text = "SSL/TLS";
           // 
           // textSslCipherList
           // 
           this.textSslCipherList.Location = new System.Drawing.Point(11, 72);
           this.textSslCipherList.Multiline = true;
           this.textSslCipherList.Name = "textSslCipherList";
           this.textSslCipherList.Number = 0;
           this.textSslCipherList.Number64 = ((long)(0));
           this.textSslCipherList.Numeric = false;
           this.textSslCipherList.Size = new System.Drawing.Size(341, 123);
           this.textSslCipherList.TabIndex = 2;
           // 
           // checkVerifyRemoteServerSslCertificate
           // 
           this.checkVerifyRemoteServerSslCertificate.AutoSize = true;
           this.checkVerifyRemoteServerSslCertificate.Checked = true;
           this.checkVerifyRemoteServerSslCertificate.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkVerifyRemoteServerSslCertificate.Location = new System.Drawing.Point(11, 31);
           this.checkVerifyRemoteServerSslCertificate.Name = "checkVerifyRemoteServerSslCertificate";
           this.checkVerifyRemoteServerSslCertificate.Size = new System.Drawing.Size(221, 17);
           this.checkVerifyRemoteServerSslCertificate.TabIndex = 1;
           this.checkVerifyRemoteServerSslCertificate.Text = "Verify remote server SSL/TLS certificates";
           this.checkVerifyRemoteServerSslCertificate.UseVisualStyleBackColor = true;
           // 
           // ucSecurity
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.labelSSLTLS);
           this.Controls.Add(this.textSslCipherList);
           this.Controls.Add(this.labelsslTlsCiphers);
           this.Controls.Add(this.checkVerifyRemoteServerSslCertificate);
           this.Name = "ucSecurity";
           this.Size = new System.Drawing.Size(783, 262);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

        private hMailServer.Administrator.Controls.ucCheckbox checkVerifyRemoteServerSslCertificate;
        private System.Windows.Forms.Label labelsslTlsCiphers;
        private hMailServer.Shared.ucText textSslCipherList;
        private System.Windows.Forms.Label labelSSLTLS;

    }
}
