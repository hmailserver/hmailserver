using hMailServer.Shared;
namespace hMailServer.Administrator
{
    partial class formServerInformation
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
         this.labelHostName = new System.Windows.Forms.Label();
         this.labelUsername = new System.Windows.Forms.Label();
         this.labelPassword = new System.Windows.Forms.Label();
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.btnSave = new System.Windows.Forms.Button();
         this.btnCancel = new System.Windows.Forms.Button();
         this.textUsername = new hMailServer.Shared.ucText();
         this.textHostname = new hMailServer.Shared.ucText();
         this.textPassword = new hMailServer.Administrator.Controls.ucPassword();
         this.radioSavePassword = new hMailServer.Administrator.Controls.ucRadioButton();
         this.radioAskWhenConnecting = new hMailServer.Administrator.Controls.ucRadioButton();
         this.SuspendLayout();
         // 
         // labelHostName
         // 
         this.labelHostName.AutoSize = true;
         this.labelHostName.Location = new System.Drawing.Point(7, 9);
         this.labelHostName.Name = "labelHostName";
         this.labelHostName.Size = new System.Drawing.Size(58, 13);
         this.labelHostName.TabIndex = 0;
         this.labelHostName.Text = "Host name";
         // 
         // labelUsername
         // 
         this.labelUsername.AutoSize = true;
         this.labelUsername.Location = new System.Drawing.Point(7, 55);
         this.labelUsername.Name = "labelUsername";
         this.labelUsername.Size = new System.Drawing.Size(114, 13);
         this.labelUsername.TabIndex = 2;
         this.labelUsername.Text = "hMailServer Username";
         // 
         // labelPassword
         // 
         this.labelPassword.AutoSize = true;
         this.labelPassword.Location = new System.Drawing.Point(7, 98);
         this.labelPassword.Name = "labelPassword";
         this.labelPassword.Size = new System.Drawing.Size(53, 13);
         this.labelPassword.TabIndex = 4;
         this.labelPassword.Text = "Password";
         // 
         // groupBox1
         // 
         this.groupBox1.Location = new System.Drawing.Point(12, 197);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(214, 7);
         this.groupBox1.TabIndex = 8;
         this.groupBox1.TabStop = false;
         // 
         // btnSave
         // 
         this.btnSave.Location = new System.Drawing.Point(42, 216);
         this.btnSave.Name = "btnSave";
         this.btnSave.Size = new System.Drawing.Size(89, 25);
         this.btnSave.TabIndex = 9;
         this.btnSave.Text = "Save";
         this.btnSave.UseVisualStyleBackColor = true;
         this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
         // 
         // btnCancel
         // 
         this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.btnCancel.Location = new System.Drawing.Point(137, 216);
         this.btnCancel.Name = "btnCancel";
         this.btnCancel.Size = new System.Drawing.Size(89, 25);
         this.btnCancel.TabIndex = 10;
         this.btnCancel.Text = "Cancel";
         this.btnCancel.UseVisualStyleBackColor = true;
         this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
         // 
         // textUsername
         // 
         this.textUsername.Location = new System.Drawing.Point(10, 71);
         this.textUsername.Name = "textUsername";
         this.textUsername.Number = 0;
         this.textUsername.Number64 = ((long)(0));
         this.textUsername.Numeric = false;
         this.textUsername.Size = new System.Drawing.Size(216, 20);
         this.textUsername.TabIndex = 3;
         // 
         // textHostname
         // 
         this.textHostname.Location = new System.Drawing.Point(10, 25);
         this.textHostname.Name = "textHostname";
         this.textHostname.Number = 0;
         this.textHostname.Number64 = ((long)(0));
         this.textHostname.Numeric = false;
         this.textHostname.Size = new System.Drawing.Size(214, 20);
         this.textHostname.TabIndex = 1;
         // 
         // textPassword
         // 
         this.textPassword.Location = new System.Drawing.Point(32, 160);
         this.textPassword.Name = "textPassword";
         this.textPassword.Size = new System.Drawing.Size(192, 20);
         this.textPassword.TabIndex = 13;
         this.textPassword.Text = "<< Encrypted >>";
         // 
         // radioSavePassword
         // 
         this.radioSavePassword.AutoSize = true;
         this.radioSavePassword.Location = new System.Drawing.Point(29, 139);
         this.radioSavePassword.Name = "radioSavePassword";
         this.radioSavePassword.Size = new System.Drawing.Size(98, 17);
         this.radioSavePassword.TabIndex = 12;
         this.radioSavePassword.Text = "Save password";
         this.radioSavePassword.UseVisualStyleBackColor = true;
         this.radioSavePassword.CheckedChanged += new System.EventHandler(this.radioSavePassword_CheckedChanged);
         // 
         // radioAskWhenConnecting
         // 
         this.radioAskWhenConnecting.AutoSize = true;
         this.radioAskWhenConnecting.Checked = true;
         this.radioAskWhenConnecting.Location = new System.Drawing.Point(29, 116);
         this.radioAskWhenConnecting.Name = "radioAskWhenConnecting";
         this.radioAskWhenConnecting.Size = new System.Drawing.Size(128, 17);
         this.radioAskWhenConnecting.TabIndex = 11;
         this.radioAskWhenConnecting.TabStop = true;
         this.radioAskWhenConnecting.Text = "Ask when connecting";
         this.radioAskWhenConnecting.UseVisualStyleBackColor = true;
         // 
         // formServerInformation
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(233, 248);
         this.Controls.Add(this.textPassword);
         this.Controls.Add(this.radioSavePassword);
         this.Controls.Add(this.radioAskWhenConnecting);
         this.Controls.Add(this.btnCancel);
         this.Controls.Add(this.btnSave);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.labelPassword);
         this.Controls.Add(this.textUsername);
         this.Controls.Add(this.labelUsername);
         this.Controls.Add(this.textHostname);
         this.Controls.Add(this.labelHostName);
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
         this.MaximizeBox = false;
         this.Name = "formServerInformation";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "Server information";
         this.ResumeLayout(false);
         this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelHostName;
        private ucText textHostname;
        private ucText textUsername;
        private System.Windows.Forms.Label labelUsername;
       private System.Windows.Forms.Label labelPassword;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnCancel;
        private hMailServer.Administrator.Controls.ucRadioButton radioAskWhenConnecting;
        private hMailServer.Administrator.Controls.ucRadioButton radioSavePassword;
       private hMailServer.Administrator.Controls.ucPassword textPassword;
    }
}