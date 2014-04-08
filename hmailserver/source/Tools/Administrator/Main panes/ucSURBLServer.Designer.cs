namespace hMailServer.Administrator
{
    partial class ucSURBLServer
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
           this.labelSpamScore = new System.Windows.Forms.Label();
           this.labelRejectionMessage = new System.Windows.Forms.Label();
           this.labelDNSHost = new System.Windows.Forms.Label();
           this.textSpamScore = new hMailServer.Shared.ucText();
           this.textRejectionMessage = new hMailServer.Shared.ucText();
           this.textDNSHost = new hMailServer.Shared.ucText();
           this.checkEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.SuspendLayout();
           // 
           // labelSpamScore
           // 
           this.labelSpamScore.AutoSize = true;
           this.labelSpamScore.Location = new System.Drawing.Point(29, 115);
           this.labelSpamScore.Name = "labelSpamScore";
           this.labelSpamScore.Size = new System.Drawing.Size(35, 13);
           this.labelSpamScore.TabIndex = 25;
           this.labelSpamScore.Text = "Score";
           // 
           // labelRejectionMessage
           // 
           this.labelRejectionMessage.AutoSize = true;
           this.labelRejectionMessage.Location = new System.Drawing.Point(29, 76);
           this.labelRejectionMessage.Name = "labelRejectionMessage";
           this.labelRejectionMessage.Size = new System.Drawing.Size(97, 13);
           this.labelRejectionMessage.TabIndex = 23;
           this.labelRejectionMessage.Text = "Rejection message";
           // 
           // labelDNSHost
           // 
           this.labelDNSHost.AutoSize = true;
           this.labelDNSHost.Location = new System.Drawing.Point(29, 33);
           this.labelDNSHost.Name = "labelDNSHost";
           this.labelDNSHost.Size = new System.Drawing.Size(55, 13);
           this.labelDNSHost.TabIndex = 19;
           this.labelDNSHost.Text = "DNS Host";
           // 
           // textSpamScore
           // 
           this.textSpamScore.Location = new System.Drawing.Point(32, 131);
           this.textSpamScore.Name = "textSpamScore";
           this.textSpamScore.Number = 0;
           this.textSpamScore.Numeric = true;
           this.textSpamScore.Size = new System.Drawing.Size(107, 20);
           this.textSpamScore.TabIndex = 24;
           // 
           // textRejectionMessage
           // 
           this.textRejectionMessage.Location = new System.Drawing.Point(32, 92);
           this.textRejectionMessage.Name = "textRejectionMessage";
           this.textRejectionMessage.Number = 0;
           this.textRejectionMessage.Numeric = false;
           this.textRejectionMessage.Size = new System.Drawing.Size(175, 20);
           this.textRejectionMessage.TabIndex = 22;
           // 
           // textDNSHost
           // 
           this.textDNSHost.Location = new System.Drawing.Point(32, 49);
           this.textDNSHost.Name = "textDNSHost";
           this.textDNSHost.Number = 0;
           this.textDNSHost.Numeric = false;
           this.textDNSHost.Size = new System.Drawing.Size(175, 20);
           this.textDNSHost.TabIndex = 18;
           // 
           // checkEnabled
           // 
           this.checkEnabled.AutoSize = true;
           this.checkEnabled.Location = new System.Drawing.Point(9, 9);
           this.checkEnabled.Name = "checkEnabled";
           this.checkEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkEnabled.TabIndex = 17;
           this.checkEnabled.Text = "Enabled";
           this.checkEnabled.UseVisualStyleBackColor = true;
           this.checkEnabled.CheckedChanged += new System.EventHandler(this.checkEnabled_CheckedChanged);
           // 
           // ucSURBLServer
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.labelSpamScore);
           this.Controls.Add(this.textSpamScore);
           this.Controls.Add(this.labelRejectionMessage);
           this.Controls.Add(this.textRejectionMessage);
           this.Controls.Add(this.labelDNSHost);
           this.Controls.Add(this.textDNSHost);
           this.Controls.Add(this.checkEnabled);
           this.Name = "ucSURBLServer";
           this.Size = new System.Drawing.Size(246, 211);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private System.Windows.Forms.Label labelSpamScore;
       private hMailServer.Shared.ucText textSpamScore;
       private System.Windows.Forms.Label labelRejectionMessage;
       private hMailServer.Shared.ucText textRejectionMessage;
       private System.Windows.Forms.Label labelDNSHost;
       private hMailServer.Shared.ucText textDNSHost;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnabled;
    }
}
