namespace hMailServer.Administrator
{
    partial class ucScripts
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
           this.buttonShowScripts = new System.Windows.Forms.Button();
           this.buttonCheckSyntax = new System.Windows.Forms.Button();
           this.buttonReloadScripts = new System.Windows.Forms.Button();
           this.comboLanguage = new hMailServer.Administrator.Controls.ucComboBox();
           this.checkEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.SuspendLayout();
           // 
           // buttonShowScripts
           // 
           this.buttonShowScripts.Location = new System.Drawing.Point(29, 75);
           this.buttonShowScripts.Name = "buttonShowScripts";
           this.buttonShowScripts.Size = new System.Drawing.Size(100, 25);
           this.buttonShowScripts.TabIndex = 16;
           this.buttonShowScripts.Text = "Show scripts";
           this.buttonShowScripts.UseVisualStyleBackColor = true;
           this.buttonShowScripts.Click += new System.EventHandler(this.buttonShowScripts_Click);
           // 
           // buttonCheckSyntax
           // 
           this.buttonCheckSyntax.Location = new System.Drawing.Point(29, 109);
           this.buttonCheckSyntax.Name = "buttonCheckSyntax";
           this.buttonCheckSyntax.Size = new System.Drawing.Size(100, 25);
           this.buttonCheckSyntax.TabIndex = 17;
           this.buttonCheckSyntax.Text = "Check syntax";
           this.buttonCheckSyntax.UseVisualStyleBackColor = true;
           this.buttonCheckSyntax.Click += new System.EventHandler(this.buttonCheckSyntax_Click);
           // 
           // buttonReloadScripts
           // 
           this.buttonReloadScripts.Location = new System.Drawing.Point(29, 143);
           this.buttonReloadScripts.Name = "buttonReloadScripts";
           this.buttonReloadScripts.Size = new System.Drawing.Size(100, 25);
           this.buttonReloadScripts.TabIndex = 18;
           this.buttonReloadScripts.Text = "Reload scripts";
           this.buttonReloadScripts.UseVisualStyleBackColor = true;
           this.buttonReloadScripts.Click += new System.EventHandler(this.buttonReloadScripts_Click);
           // 
           // comboLanguage
           // 
           this.comboLanguage.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
           this.comboLanguage.FormattingEnabled = true;
           this.comboLanguage.Location = new System.Drawing.Point(29, 39);
           this.comboLanguage.Name = "comboLanguage";
           this.comboLanguage.Size = new System.Drawing.Size(171, 21);
           this.comboLanguage.TabIndex = 15;
           // 
           // checkEnabled
           // 
           this.checkEnabled.AutoSize = true;
           this.checkEnabled.Location = new System.Drawing.Point(8, 8);
           this.checkEnabled.Name = "checkEnabled";
           this.checkEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkEnabled.TabIndex = 14;
           this.checkEnabled.Text = "Enabled";
           this.checkEnabled.UseVisualStyleBackColor = true;
           this.checkEnabled.CheckedChanged += new System.EventHandler(this.checkEnabled_CheckedChanged);
           // 
           // ucScripts
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.buttonReloadScripts);
           this.Controls.Add(this.buttonCheckSyntax);
           this.Controls.Add(this.buttonShowScripts);
           this.Controls.Add(this.comboLanguage);
           this.Controls.Add(this.checkEnabled);
           this.Name = "ucScripts";
           this.Size = new System.Drawing.Size(376, 213);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private hMailServer.Administrator.Controls.ucCheckbox checkEnabled;
       private hMailServer.Administrator.Controls.ucComboBox comboLanguage;
       private System.Windows.Forms.Button buttonShowScripts;
       private System.Windows.Forms.Button buttonCheckSyntax;
       private System.Windows.Forms.Button buttonReloadScripts;
    }
}
