namespace hMailServer.Administrator
{
    partial class ucSSLCertificate
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
            this.labelPrivateKeyFile = new System.Windows.Forms.Label();
            this.labelCertificateFile = new System.Windows.Forms.Label();
            this.labelName = new System.Windows.Forms.Label();
            this.buttonSelectCertificate = new System.Windows.Forms.Button();
            this.buttonSelectKey = new System.Windows.Forms.Button();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.textPrivateKeyFile = new hMailServer.Shared.ucText();
            this.textCertificateFile = new hMailServer.Shared.ucText();
            this.textName = new hMailServer.Shared.ucText();
            this.SuspendLayout();
            // 
            // labelPrivateKeyFile
            // 
            this.labelPrivateKeyFile.AutoSize = true;
            this.labelPrivateKeyFile.Location = new System.Drawing.Point(5, 92);
            this.labelPrivateKeyFile.Name = "labelPrivateKeyFile";
            this.labelPrivateKeyFile.Size = new System.Drawing.Size(76, 13);
            this.labelPrivateKeyFile.TabIndex = 20;
            this.labelPrivateKeyFile.Text = "Private key file";
            // 
            // labelCertificateFile
            // 
            this.labelCertificateFile.AutoSize = true;
            this.labelCertificateFile.Location = new System.Drawing.Point(5, 51);
            this.labelCertificateFile.Name = "labelCertificateFile";
            this.labelCertificateFile.Size = new System.Drawing.Size(70, 13);
            this.labelCertificateFile.TabIndex = 18;
            this.labelCertificateFile.Text = "Certificate file";
            // 
            // labelName
            // 
            this.labelName.AutoSize = true;
            this.labelName.Location = new System.Drawing.Point(5, 8);
            this.labelName.Name = "labelName";
            this.labelName.Size = new System.Drawing.Size(35, 13);
            this.labelName.TabIndex = 16;
            this.labelName.Text = "Name";
            // 
            // buttonSelectCertificate
            // 
            this.buttonSelectCertificate.Location = new System.Drawing.Point(293, 67);
            this.buttonSelectCertificate.Name = "buttonSelectCertificate";
            this.buttonSelectCertificate.Size = new System.Drawing.Size(47, 19);
            this.buttonSelectCertificate.TabIndex = 21;
            this.buttonSelectCertificate.Text = "...";
            this.buttonSelectCertificate.UseVisualStyleBackColor = true;
            this.buttonSelectCertificate.Click += new System.EventHandler(this.buttonSelectCertificate_Click);
            // 
            // buttonSelectKey
            // 
            this.buttonSelectKey.Location = new System.Drawing.Point(293, 109);
            this.buttonSelectKey.Name = "buttonSelectKey";
            this.buttonSelectKey.Size = new System.Drawing.Size(47, 19);
            this.buttonSelectKey.TabIndex = 22;
            this.buttonSelectKey.Text = "...";
            this.buttonSelectKey.UseVisualStyleBackColor = true;
            this.buttonSelectKey.Click += new System.EventHandler(this.buttonSelectKey_Click);
            // 
            // textPrivateKeyFile
            // 
            this.textPrivateKeyFile.Location = new System.Drawing.Point(8, 108);
            this.textPrivateKeyFile.Name = "textPrivateKeyFile";
            this.textPrivateKeyFile.Number = 0;
            this.textPrivateKeyFile.Numeric = false;
            this.textPrivateKeyFile.Size = new System.Drawing.Size(279, 20);
            this.textPrivateKeyFile.TabIndex = 19;
            // 
            // textCertificateFile
            // 
            this.textCertificateFile.Location = new System.Drawing.Point(8, 67);
            this.textCertificateFile.Name = "textCertificateFile";
            this.textCertificateFile.Number = 0;
            this.textCertificateFile.Numeric = false;
            this.textCertificateFile.Size = new System.Drawing.Size(279, 20);
            this.textCertificateFile.TabIndex = 17;
            // 
            // textName
            // 
            this.textName.Location = new System.Drawing.Point(8, 24);
            this.textName.Name = "textName";
            this.textName.Number = 0;
            this.textName.Numeric = false;
            this.textName.Size = new System.Drawing.Size(175, 20);
            this.textName.TabIndex = 15;
            // 
            // ucSSLCertificate
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.buttonSelectKey);
            this.Controls.Add(this.buttonSelectCertificate);
            this.Controls.Add(this.labelPrivateKeyFile);
            this.Controls.Add(this.textPrivateKeyFile);
            this.Controls.Add(this.labelCertificateFile);
            this.Controls.Add(this.textCertificateFile);
            this.Controls.Add(this.labelName);
            this.Controls.Add(this.textName);
            this.Name = "ucSSLCertificate";
            this.Size = new System.Drawing.Size(370, 268);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

       private System.Windows.Forms.Label labelPrivateKeyFile;
       private hMailServer.Shared.ucText textPrivateKeyFile;
       private System.Windows.Forms.Label labelCertificateFile;
       private hMailServer.Shared.ucText textCertificateFile;
       private System.Windows.Forms.Label labelName;
       private hMailServer.Shared.ucText textName;
       private System.Windows.Forms.Button buttonSelectCertificate;
       private System.Windows.Forms.Button buttonSelectKey;
       private System.Windows.Forms.OpenFileDialog openFileDialog;
    }
}
