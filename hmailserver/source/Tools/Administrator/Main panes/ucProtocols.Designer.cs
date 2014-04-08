namespace hMailServer.Administrator
{
    partial class ucProtocols
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
           this.labelProtocols = new System.Windows.Forms.Label();
           this.checkSMTP = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkPOP3 = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkIMAP = new hMailServer.Administrator.Controls.ucCheckbox();
           this.SuspendLayout();
           // 
           // labelProtocols
           // 
           this.labelProtocols.Location = new System.Drawing.Point(18, 14);
           this.labelProtocols.Name = "labelProtocols";
           this.labelProtocols.Size = new System.Drawing.Size(365, 37);
           this.labelProtocols.TabIndex = 0;
           this.labelProtocols.Text = "If you change the settings below you must restart the server before your changes " +
               "take affect.";
           // 
           // checkSMTP
           // 
           this.checkSMTP.AutoSize = true;
           this.checkSMTP.Location = new System.Drawing.Point(34, 54);
           this.checkSMTP.Name = "checkSMTP";
           this.checkSMTP.Size = new System.Drawing.Size(56, 17);
           this.checkSMTP.TabIndex = 1;
           this.checkSMTP.Text = "SMTP";
           this.checkSMTP.UseVisualStyleBackColor = true;
           // 
           // checkPOP3
           // 
           this.checkPOP3.AutoSize = true;
           this.checkPOP3.Location = new System.Drawing.Point(34, 77);
           this.checkPOP3.Name = "checkPOP3";
           this.checkPOP3.Size = new System.Drawing.Size(54, 17);
           this.checkPOP3.TabIndex = 2;
           this.checkPOP3.Text = "POP3";
           this.checkPOP3.UseVisualStyleBackColor = true;
           // 
           // checkIMAP
           // 
           this.checkIMAP.AutoSize = true;
           this.checkIMAP.Location = new System.Drawing.Point(34, 100);
           this.checkIMAP.Name = "checkIMAP";
           this.checkIMAP.Size = new System.Drawing.Size(52, 17);
           this.checkIMAP.TabIndex = 3;
           this.checkIMAP.Text = "IMAP";
           this.checkIMAP.UseVisualStyleBackColor = true;
           // 
           // ucProtocols
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.checkIMAP);
           this.Controls.Add(this.checkPOP3);
           this.Controls.Add(this.checkSMTP);
           this.Controls.Add(this.labelProtocols);
           this.Name = "ucProtocols";
           this.Size = new System.Drawing.Size(417, 277);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private System.Windows.Forms.Label labelProtocols;
       private hMailServer.Administrator.Controls.ucCheckbox checkSMTP;
       private hMailServer.Administrator.Controls.ucCheckbox checkPOP3;
       private hMailServer.Administrator.Controls.ucCheckbox checkIMAP;
    }
}
