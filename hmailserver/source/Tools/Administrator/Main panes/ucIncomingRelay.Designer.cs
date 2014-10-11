namespace hMailServer.Administrator
{
    partial class ucIncomingRelay
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
         this.labelUpperIP = new System.Windows.Forms.Label();
         this.labelLowerIP = new System.Windows.Forms.Label();
         this.labelName = new System.Windows.Forms.Label();
         this.labelGeneral = new System.Windows.Forms.Label();
         this.textName = new hMailServer.Shared.ucText();
         this.textLower = new hMailServer.Administrator.Controls.ucIPAddress();
         this.textUpper = new hMailServer.Administrator.Controls.ucIPAddress();
         this.SuspendLayout();
         // 
         // labelUpperIP
         // 
         this.labelUpperIP.AutoSize = true;
         this.labelUpperIP.Location = new System.Drawing.Point(296, 75);
         this.labelUpperIP.Name = "labelUpperIP";
         this.labelUpperIP.Size = new System.Drawing.Size(49, 13);
         this.labelUpperIP.TabIndex = 48;
         this.labelUpperIP.Text = "Upper IP";
         // 
         // labelLowerIP
         // 
         this.labelLowerIP.AutoSize = true;
         this.labelLowerIP.Location = new System.Drawing.Point(29, 75);
         this.labelLowerIP.Name = "labelLowerIP";
         this.labelLowerIP.Size = new System.Drawing.Size(49, 13);
         this.labelLowerIP.TabIndex = 47;
         this.labelLowerIP.Text = "Lower IP";
         // 
         // labelName
         // 
         this.labelName.AutoSize = true;
         this.labelName.Location = new System.Drawing.Point(29, 32);
         this.labelName.Name = "labelName";
         this.labelName.Size = new System.Drawing.Size(35, 13);
         this.labelName.TabIndex = 44;
         this.labelName.Text = "Name";
         // 
         // labelGeneral
         // 
         this.labelGeneral.AutoSize = true;
         this.labelGeneral.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelGeneral.Location = new System.Drawing.Point(7, 7);
         this.labelGeneral.Name = "labelGeneral";
         this.labelGeneral.Size = new System.Drawing.Size(51, 13);
         this.labelGeneral.TabIndex = 43;
         this.labelGeneral.Text = "General";
         // 
         // textName
         // 
         this.textName.Location = new System.Drawing.Point(32, 48);
         this.textName.Name = "textName";
         this.textName.Number = 0;
         this.textName.Number64 = ((long)(0));
         this.textName.Numeric = false;
         this.textName.Size = new System.Drawing.Size(129, 20);
         this.textName.TabIndex = 42;
         // 
         // textLower
         // 
         this.textLower.Location = new System.Drawing.Point(32, 91);
         this.textLower.Name = "textLower";
         this.textLower.Size = new System.Drawing.Size(239, 20);
         this.textLower.TabIndex = 57;
         // 
         // textUpper
         // 
         this.textUpper.Location = new System.Drawing.Point(299, 91);
         this.textUpper.Name = "textUpper";
         this.textUpper.Size = new System.Drawing.Size(239, 20);
         this.textUpper.TabIndex = 58;
         // 
         // ucIncomingRelay
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.textUpper);
         this.Controls.Add(this.textLower);
         this.Controls.Add(this.labelUpperIP);
         this.Controls.Add(this.labelLowerIP);
         this.Controls.Add(this.labelName);
         this.Controls.Add(this.labelGeneral);
         this.Controls.Add(this.textName);
         this.Name = "ucIncomingRelay";
         this.Size = new System.Drawing.Size(635, 450);
         this.ResumeLayout(false);
         this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelUpperIP;
        private System.Windows.Forms.Label labelLowerIP;
        private System.Windows.Forms.Label labelName;
        private System.Windows.Forms.Label labelGeneral;
        private hMailServer.Shared.ucText textName;
        private hMailServer.Administrator.Controls.ucIPAddress textLower;
        private hMailServer.Administrator.Controls.ucIPAddress textUpper;
    }
}
