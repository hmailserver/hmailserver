namespace hMailServer.Administrator
{
    partial class ucMirror
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
           this.labelMirrorInfo = new System.Windows.Forms.Label();
           this.labelMirrorAddress = new System.Windows.Forms.Label();
           this.textMirrorAddress = new hMailServer.Shared.ucText();
           this.SuspendLayout();
           // 
           // labelMirrorInfo
           // 
           this.labelMirrorInfo.Location = new System.Drawing.Point(8, 8);
           this.labelMirrorInfo.Name = "labelMirrorInfo";
           this.labelMirrorInfo.Size = new System.Drawing.Size(394, 30);
           this.labelMirrorInfo.TabIndex = 0;
           this.labelMirrorInfo.Text = "A copy of all e-mails sent on this server, including both incoming and outgoing m" +
               "essages, will be sent to the e-mail address entered as mirror-address below.";
           // 
           // labelMirrorAddress
           // 
           this.labelMirrorAddress.AutoSize = true;
           this.labelMirrorAddress.Location = new System.Drawing.Point(8, 55);
           this.labelMirrorAddress.Name = "labelMirrorAddress";
           this.labelMirrorAddress.Size = new System.Drawing.Size(103, 13);
           this.labelMirrorAddress.TabIndex = 1;
           this.labelMirrorAddress.Text = "Mirror e-mail address";
           // 
           // textMirrorAddress
           // 
           this.textMirrorAddress.Location = new System.Drawing.Point(11, 73);
           this.textMirrorAddress.Name = "textMirrorAddress";
           this.textMirrorAddress.Number = 0;
           this.textMirrorAddress.Numeric = false;
           this.textMirrorAddress.Size = new System.Drawing.Size(302, 20);
           this.textMirrorAddress.TabIndex = 2;
           // 
           // ucMirror
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.textMirrorAddress);
           this.Controls.Add(this.labelMirrorAddress);
           this.Controls.Add(this.labelMirrorInfo);
           this.Name = "ucMirror";
           this.Size = new System.Drawing.Size(783, 262);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private System.Windows.Forms.Label labelMirrorInfo;
       private System.Windows.Forms.Label labelMirrorAddress;
       private hMailServer.Shared.ucText textMirrorAddress;
    }
}
