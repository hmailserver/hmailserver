namespace hMailServer.Administrator
{
    partial class ucAdvanced
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
           this.textDefaultDomain = new hMailServer.Shared.ucText();
           this.labelDefaultDomain = new System.Windows.Forms.Label();
           this.labelGeneral = new System.Windows.Forms.Label();
           this.textAdministratorPassword = new hMailServer.Shared.ucText();
           this.labelAdministrationPassword = new System.Windows.Forms.Label();
           this.labelPleaseRemember = new System.Windows.Forms.Label();
           this.SuspendLayout();
           // 
           // textDefaultDomain
           // 
           this.textDefaultDomain.Location = new System.Drawing.Point(30, 58);
           this.textDefaultDomain.Name = "textDefaultDomain";
           this.textDefaultDomain.Number = 0;
           this.textDefaultDomain.Numeric = false;
           this.textDefaultDomain.Size = new System.Drawing.Size(178, 20);
           this.textDefaultDomain.TabIndex = 7;
           // 
           // labelDefaultDomain
           // 
           this.labelDefaultDomain.AutoSize = true;
           this.labelDefaultDomain.Location = new System.Drawing.Point(28, 39);
           this.labelDefaultDomain.Name = "labelDefaultDomain";
           this.labelDefaultDomain.Size = new System.Drawing.Size(78, 13);
           this.labelDefaultDomain.TabIndex = 6;
           this.labelDefaultDomain.Text = "Default domain";
           // 
           // labelGeneral
           // 
           this.labelGeneral.AutoSize = true;
           this.labelGeneral.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelGeneral.Location = new System.Drawing.Point(12, 14);
           this.labelGeneral.Name = "labelGeneral";
           this.labelGeneral.Size = new System.Drawing.Size(51, 13);
           this.labelGeneral.TabIndex = 5;
           this.labelGeneral.Text = "General";
           // 
           // textAdministratorPassword
           // 
           this.textAdministratorPassword.Location = new System.Drawing.Point(31, 148);
           this.textAdministratorPassword.Name = "textAdministratorPassword";
           this.textAdministratorPassword.Number = 0;
           this.textAdministratorPassword.Numeric = false;
           this.textAdministratorPassword.PasswordChar = '*';
           this.textAdministratorPassword.Size = new System.Drawing.Size(177, 20);
           this.textAdministratorPassword.TabIndex = 9;
           // 
           // labelAdministrationPassword
           // 
           this.labelAdministrationPassword.AutoSize = true;
           this.labelAdministrationPassword.Location = new System.Drawing.Point(28, 91);
           this.labelAdministrationPassword.Name = "labelAdministrationPassword";
           this.labelAdministrationPassword.Size = new System.Drawing.Size(120, 13);
           this.labelAdministrationPassword.TabIndex = 8;
           this.labelAdministrationPassword.Text = "Administration password";
           // 
           // labelPleaseRemember
           // 
           this.labelPleaseRemember.Location = new System.Drawing.Point(28, 113);
           this.labelPleaseRemember.Name = "labelPleaseRemember";
           this.labelPleaseRemember.Size = new System.Drawing.Size(361, 32);
           this.labelPleaseRemember.TabIndex = 10;
           this.labelPleaseRemember.Text = "You need this password to be able to manage your hMailServer installation, so ple" +
               "ase remember it.";
           // 
           // ucAdvanced
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.labelPleaseRemember);
           this.Controls.Add(this.textAdministratorPassword);
           this.Controls.Add(this.labelAdministrationPassword);
           this.Controls.Add(this.textDefaultDomain);
           this.Controls.Add(this.labelDefaultDomain);
           this.Controls.Add(this.labelGeneral);
           this.Name = "ucAdvanced";
           this.Size = new System.Drawing.Size(561, 274);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private hMailServer.Shared.ucText textDefaultDomain;
       private System.Windows.Forms.Label labelDefaultDomain;
       private System.Windows.Forms.Label labelGeneral;
       private hMailServer.Shared.ucText textAdministratorPassword;
       private System.Windows.Forms.Label labelAdministrationPassword;
       private System.Windows.Forms.Label labelPleaseRemember;
    }
}
