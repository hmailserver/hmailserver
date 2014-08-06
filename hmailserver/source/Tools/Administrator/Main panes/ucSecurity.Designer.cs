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
           this.label1 = new System.Windows.Forms.Label();
           this.labelSSLTLS = new System.Windows.Forms.Label();
           this.textSslCipherList = new hMailServer.Shared.ucText();
           this.checkVerifyRemoteServerSslCertificate = new hMailServer.Administrator.Controls.ucCheckbox();
           this.SuspendLayout();
           // 
           // label1
           // 
           this.label1.AutoSize = true;
           this.label1.Location = new System.Drawing.Point(21, 88);
           this.label1.Name = "label1";
           this.label1.Size = new System.Drawing.Size(89, 13);
           this.label1.TabIndex = 16;
           this.label1.Text = "SSL/TLS ciphers";
           // 
           // labelSSLTLS
           // 
           this.labelSSLTLS.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelSSLTLS.Location = new System.Drawing.Point(21, 21);
           this.labelSSLTLS.Name = "labelSSLTLS";
           this.labelSSLTLS.Size = new System.Drawing.Size(154, 20);
           this.labelSSLTLS.TabIndex = 18;
           this.labelSSLTLS.Text = "SSL/TLS";
           // 
           // textSslCipherList
           // 
           this.textSslCipherList.Location = new System.Drawing.Point(24, 104);
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
           this.checkVerifyRemoteServerSslCertificate.Location = new System.Drawing.Point(24, 55);
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
           this.Controls.Add(this.label1);
           this.Controls.Add(this.checkVerifyRemoteServerSslCertificate);
           this.Name = "ucSecurity";
           this.Size = new System.Drawing.Size(783, 262);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

        private hMailServer.Administrator.Controls.ucCheckbox checkVerifyRemoteServerSslCertificate;
        private System.Windows.Forms.Label label1;
        private hMailServer.Shared.ucText textSslCipherList;
        private System.Windows.Forms.Label labelSSLTLS;

    }
}
