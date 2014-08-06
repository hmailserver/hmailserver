namespace hMailServer.Administrator
{
    partial class ucAlias
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
           this.labelRedirectFrom = new System.Windows.Forms.Label();
           this.labelRedirectTo = new System.Windows.Forms.Label();
           this.checkEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.textValue = new hMailServer.Administrator.Controls.ucEmailEdit();
           this.textName = new hMailServer.Administrator.Controls.ucEmailEdit();
           this.SuspendLayout();
           // 
           // labelRedirectFrom
           // 
           this.labelRedirectFrom.AutoSize = true;
           this.labelRedirectFrom.Location = new System.Drawing.Point(8, 8);
           this.labelRedirectFrom.Name = "labelRedirectFrom";
           this.labelRedirectFrom.Size = new System.Drawing.Size(70, 13);
           this.labelRedirectFrom.TabIndex = 3;
           this.labelRedirectFrom.Text = "Redirect from";
           // 
           // labelRedirectTo
           // 
           this.labelRedirectTo.AutoSize = true;
           this.labelRedirectTo.Location = new System.Drawing.Point(8, 56);
           this.labelRedirectTo.Name = "labelRedirectTo";
           this.labelRedirectTo.Size = new System.Drawing.Size(20, 13);
           this.labelRedirectTo.TabIndex = 5;
           this.labelRedirectTo.Text = "To";
           // 
           // checkEnabled
           // 
           this.checkEnabled.AutoSize = true;
           this.checkEnabled.Checked = true;
           this.checkEnabled.CheckState = System.Windows.Forms.CheckState.Checked;
           this.checkEnabled.Location = new System.Drawing.Point(11, 103);
           this.checkEnabled.Name = "checkEnabled";
           this.checkEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkEnabled.TabIndex = 7;
           this.checkEnabled.Text = "Enabled";
           this.checkEnabled.UseVisualStyleBackColor = true;
           // 
           // textValue
           // 
           this.textValue.Location = new System.Drawing.Point(11, 72);
           this.textValue.Name = "textValue";
           this.textValue.ReadOnlyHost = false;
           this.textValue.Size = new System.Drawing.Size(282, 25);
           this.textValue.TabIndex = 6;
           // 
           // textName
           // 
           this.textName.Location = new System.Drawing.Point(11, 24);
           this.textName.Name = "textName";
           this.textName.ReadOnlyHost = true;
           this.textName.Size = new System.Drawing.Size(282, 25);
           this.textName.TabIndex = 4;
           // 
           // ucAlias
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.checkEnabled);
           this.Controls.Add(this.textValue);
           this.Controls.Add(this.labelRedirectTo);
           this.Controls.Add(this.textName);
           this.Controls.Add(this.labelRedirectFrom);
           this.Name = "ucAlias";
           this.Size = new System.Drawing.Size(430, 196);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private hMailServer.Administrator.Controls.ucEmailEdit textName;
       private System.Windows.Forms.Label labelRedirectFrom;
       private hMailServer.Administrator.Controls.ucEmailEdit textValue;
       private System.Windows.Forms.Label labelRedirectTo;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnabled;
    }
}
