namespace hMailServer.Shared
{
    partial class formErrorMessage
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
           System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formErrorMessage));
           this.buttonClose = new System.Windows.Forms.Button();
           this.labelErrorMessage = new System.Windows.Forms.Label();
           this.textErrorMessage = new hMailServer.Shared.ucText();
           this.textErrorDetails = new hMailServer.Shared.ucText();
           this.labelErrorDetails = new System.Windows.Forms.Label();
           this.SuspendLayout();
           // 
           // buttonClose
           // 
           this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonClose.Location = new System.Drawing.Point(375, 284);
           this.buttonClose.Name = "buttonClose";
           this.buttonClose.Size = new System.Drawing.Size(107, 27);
           this.buttonClose.TabIndex = 0;
           this.buttonClose.Text = "&Close";
           this.buttonClose.UseVisualStyleBackColor = true;
           this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
           // 
           // labelErrorMessage
           // 
           this.labelErrorMessage.AutoSize = true;
           this.labelErrorMessage.Location = new System.Drawing.Point(14, 16);
           this.labelErrorMessage.Name = "labelErrorMessage";
           this.labelErrorMessage.Size = new System.Drawing.Size(74, 13);
           this.labelErrorMessage.TabIndex = 1;
           this.labelErrorMessage.Text = "Error message";
           // 
           // textErrorMessage
           // 
           this.textErrorMessage.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.textErrorMessage.Location = new System.Drawing.Point(17, 32);
           this.textErrorMessage.Multiline = true;
           this.textErrorMessage.Name = "textErrorMessage";
           this.textErrorMessage.Number = 0;
           this.textErrorMessage.Numeric = false;
           this.textErrorMessage.ReadOnly = true;
           this.textErrorMessage.ScrollBars = System.Windows.Forms.ScrollBars.Both;
           this.textErrorMessage.Size = new System.Drawing.Size(465, 40);
           this.textErrorMessage.TabIndex = 3;
           // 
           // textErrorDetails
           // 
           this.textErrorDetails.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.textErrorDetails.Location = new System.Drawing.Point(17, 105);
           this.textErrorDetails.Multiline = true;
           this.textErrorDetails.Name = "textErrorDetails";
           this.textErrorDetails.Number = 0;
           this.textErrorDetails.Numeric = false;
           this.textErrorDetails.ReadOnly = true;
           this.textErrorDetails.ScrollBars = System.Windows.Forms.ScrollBars.Both;
           this.textErrorDetails.Size = new System.Drawing.Size(465, 170);
           this.textErrorDetails.TabIndex = 4;
           // 
           // labelErrorDetails
           // 
           this.labelErrorDetails.AutoSize = true;
           this.labelErrorDetails.Location = new System.Drawing.Point(14, 89);
           this.labelErrorDetails.Name = "labelErrorDetails";
           this.labelErrorDetails.Size = new System.Drawing.Size(39, 13);
           this.labelErrorDetails.TabIndex = 5;
           this.labelErrorDetails.Text = "Details";
           // 
           // formErrorMessage
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.ClientSize = new System.Drawing.Size(494, 323);
           this.Controls.Add(this.labelErrorDetails);
           this.Controls.Add(this.textErrorDetails);
           this.Controls.Add(this.textErrorMessage);
           this.Controls.Add(this.labelErrorMessage);
           this.Controls.Add(this.buttonClose);
           this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
           this.Name = "formErrorMessage";
           this.Text = "hMailServer Administrator";
           this.Shown += new System.EventHandler(this.formErrorMessage_Shown);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonClose;
        private System.Windows.Forms.Label labelErrorMessage;
        private ucText textErrorMessage;
        private ucText textErrorDetails;
        private System.Windows.Forms.Label labelErrorDetails;
    }
}