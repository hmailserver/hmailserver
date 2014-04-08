namespace hMailServer.Administrator
{
    partial class ucDNSBlackList
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
           this.labelDNSHost = new System.Windows.Forms.Label();
           this.labelExpectedResult = new System.Windows.Forms.Label();
           this.labelRejectionMessage = new System.Windows.Forms.Label();
           this.labelSpamScore = new System.Windows.Forms.Label();
           this.textSpamScore = new hMailServer.Shared.ucText();
           this.textRejectionMessage = new hMailServer.Shared.ucText();
           this.textExpectedResult = new hMailServer.Shared.ucText();
           this.textDNSHost = new hMailServer.Shared.ucText();
           this.checkEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.SuspendLayout();
           // 
           // labelDNSHost
           // 
           this.labelDNSHost.AutoSize = true;
           this.labelDNSHost.Location = new System.Drawing.Point(28, 32);
           this.labelDNSHost.Name = "labelDNSHost";
           this.labelDNSHost.Size = new System.Drawing.Size(55, 13);
           this.labelDNSHost.TabIndex = 10;
           this.labelDNSHost.Text = "DNS Host";
           // 
           // labelExpectedResult
           // 
           this.labelExpectedResult.AutoSize = true;
           this.labelExpectedResult.Location = new System.Drawing.Point(28, 75);
           this.labelExpectedResult.Name = "labelExpectedResult";
           this.labelExpectedResult.Size = new System.Drawing.Size(80, 13);
           this.labelExpectedResult.TabIndex = 12;
           this.labelExpectedResult.Text = "Expected result";
           // 
           // labelRejectionMessage
           // 
           this.labelRejectionMessage.AutoSize = true;
           this.labelRejectionMessage.Location = new System.Drawing.Point(28, 116);
           this.labelRejectionMessage.Name = "labelRejectionMessage";
           this.labelRejectionMessage.Size = new System.Drawing.Size(97, 13);
           this.labelRejectionMessage.TabIndex = 14;
           this.labelRejectionMessage.Text = "Rejection message";
           // 
           // labelSpamScore
           // 
           this.labelSpamScore.AutoSize = true;
           this.labelSpamScore.Location = new System.Drawing.Point(28, 155);
           this.labelSpamScore.Name = "labelSpamScore";
           this.labelSpamScore.Size = new System.Drawing.Size(35, 13);
           this.labelSpamScore.TabIndex = 16;
           this.labelSpamScore.Text = "Score";
           // 
           // textSpamScore
           // 
           this.textSpamScore.Location = new System.Drawing.Point(31, 171);
           this.textSpamScore.Name = "textSpamScore";
           this.textSpamScore.Number = 0;
           this.textSpamScore.Numeric = true;
           this.textSpamScore.Size = new System.Drawing.Size(94, 20);
           this.textSpamScore.TabIndex = 15;
           // 
           // textRejectionMessage
           // 
           this.textRejectionMessage.Location = new System.Drawing.Point(31, 132);
           this.textRejectionMessage.Name = "textRejectionMessage";
           this.textRejectionMessage.Number = 0;
           this.textRejectionMessage.Numeric = false;
           this.textRejectionMessage.Size = new System.Drawing.Size(175, 20);
           this.textRejectionMessage.TabIndex = 13;
           // 
           // textExpectedResult
           // 
           this.textExpectedResult.Location = new System.Drawing.Point(31, 91);
           this.textExpectedResult.Name = "textExpectedResult";
           this.textExpectedResult.Number = 0;
           this.textExpectedResult.Numeric = false;
           this.textExpectedResult.Size = new System.Drawing.Size(175, 20);
           this.textExpectedResult.TabIndex = 11;
           // 
           // textDNSHost
           // 
           this.textDNSHost.Location = new System.Drawing.Point(31, 48);
           this.textDNSHost.Name = "textDNSHost";
           this.textDNSHost.Number = 0;
           this.textDNSHost.Numeric = false;
           this.textDNSHost.Size = new System.Drawing.Size(175, 20);
           this.textDNSHost.TabIndex = 9;
           // 
           // checkEnabled
           // 
           this.checkEnabled.AutoSize = true;
           this.checkEnabled.Location = new System.Drawing.Point(8, 8);
           this.checkEnabled.Name = "checkEnabled";
           this.checkEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkEnabled.TabIndex = 8;
           this.checkEnabled.Text = "Enabled";
           this.checkEnabled.UseVisualStyleBackColor = true;
           this.checkEnabled.CheckedChanged += new System.EventHandler(this.checkEnabled_CheckedChanged);
           // 
           // ucDNSBlackList
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.labelSpamScore);
           this.Controls.Add(this.textSpamScore);
           this.Controls.Add(this.labelRejectionMessage);
           this.Controls.Add(this.textRejectionMessage);
           this.Controls.Add(this.labelExpectedResult);
           this.Controls.Add(this.textExpectedResult);
           this.Controls.Add(this.labelDNSHost);
           this.Controls.Add(this.textDNSHost);
           this.Controls.Add(this.checkEnabled);
           this.Name = "ucDNSBlackList";
           this.Size = new System.Drawing.Size(395, 270);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private hMailServer.Administrator.Controls.ucCheckbox checkEnabled;
       private hMailServer.Shared.ucText textDNSHost;
       private System.Windows.Forms.Label labelDNSHost;
       private System.Windows.Forms.Label labelExpectedResult;
       private hMailServer.Shared.ucText textExpectedResult;
       private System.Windows.Forms.Label labelRejectionMessage;
       private hMailServer.Shared.ucText textRejectionMessage;
       private System.Windows.Forms.Label labelSpamScore;
       private hMailServer.Shared.ucText textSpamScore;
    }
}
