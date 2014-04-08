namespace hMailServer.Administrator
{
    partial class ucProtocolPOP3
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
           this.textMaxPOP3Connections = new hMailServer.Shared.ucText();
           this.labelMaxPOP3Connections = new System.Windows.Forms.Label();
           this.labelConnections = new System.Windows.Forms.Label();
           this.labelOther = new System.Windows.Forms.Label();
           this.textWelcomeMessage = new hMailServer.Shared.ucText();
           this.labelWelcome = new System.Windows.Forms.Label();
           this.SuspendLayout();
           // 
           // textMaxPOP3Connections
           // 
           this.textMaxPOP3Connections.Location = new System.Drawing.Point(25, 51);
           this.textMaxPOP3Connections.Name = "textMaxPOP3Connections";
           this.textMaxPOP3Connections.Number = 0;
           this.textMaxPOP3Connections.Numeric = true;
           this.textMaxPOP3Connections.Size = new System.Drawing.Size(117, 20);
           this.textMaxPOP3Connections.TabIndex = 7;
           // 
           // labelMaxPOP3Connections
           // 
           this.labelMaxPOP3Connections.AutoSize = true;
           this.labelMaxPOP3Connections.Location = new System.Drawing.Point(23, 32);
           this.labelMaxPOP3Connections.Name = "labelMaxPOP3Connections";
           this.labelMaxPOP3Connections.Size = new System.Drawing.Size(300, 13);
           this.labelMaxPOP3Connections.TabIndex = 6;
           this.labelMaxPOP3Connections.Text = "Maximum number of simultaneous connections (0 for unlimited)";
           // 
           // labelConnections
           // 
           this.labelConnections.AutoSize = true;
           this.labelConnections.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelConnections.Location = new System.Drawing.Point(7, 7);
           this.labelConnections.Name = "labelConnections";
           this.labelConnections.Size = new System.Drawing.Size(77, 13);
           this.labelConnections.TabIndex = 5;
           this.labelConnections.Text = "Connections";
           // 
           // labelOther
           // 
           this.labelOther.AutoSize = true;
           this.labelOther.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelOther.Location = new System.Drawing.Point(7, 85);
           this.labelOther.Name = "labelOther";
           this.labelOther.Size = new System.Drawing.Size(38, 13);
           this.labelOther.TabIndex = 8;
           this.labelOther.Text = "Other";
           // 
           // textWelcomeMessage
           // 
           this.textWelcomeMessage.Location = new System.Drawing.Point(26, 126);
           this.textWelcomeMessage.Name = "textWelcomeMessage";
           this.textWelcomeMessage.Number = 0;
           this.textWelcomeMessage.Numeric = false;
           this.textWelcomeMessage.Size = new System.Drawing.Size(225, 20);
           this.textWelcomeMessage.TabIndex = 10;
           // 
           // labelWelcome
           // 
           this.labelWelcome.AutoSize = true;
           this.labelWelcome.Location = new System.Drawing.Point(24, 107);
           this.labelWelcome.Name = "labelWelcome";
           this.labelWelcome.Size = new System.Drawing.Size(97, 13);
           this.labelWelcome.TabIndex = 9;
           this.labelWelcome.Text = "Welcome message";
           // 
           // ucProtocolPOP3
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.textWelcomeMessage);
           this.Controls.Add(this.labelWelcome);
           this.Controls.Add(this.labelOther);
           this.Controls.Add(this.textMaxPOP3Connections);
           this.Controls.Add(this.labelMaxPOP3Connections);
           this.Controls.Add(this.labelConnections);
           this.Name = "ucProtocolPOP3";
           this.Size = new System.Drawing.Size(387, 208);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private hMailServer.Shared.ucText textMaxPOP3Connections;
       private System.Windows.Forms.Label labelMaxPOP3Connections;
       private System.Windows.Forms.Label labelConnections;
       private System.Windows.Forms.Label labelOther;
       private hMailServer.Shared.ucText textWelcomeMessage;
       private System.Windows.Forms.Label labelWelcome;
    }
}
