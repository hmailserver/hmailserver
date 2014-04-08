namespace DBSetup.Pages
{
   partial class ucDBConnectionInfo
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
          this.labelDatabaseServerAddress = new System.Windows.Forms.Label();
          this.textServerAddress = new System.Windows.Forms.TextBox();
          this.labelPort = new System.Windows.Forms.Label();
          this.textDatabaseName = new System.Windows.Forms.TextBox();
          this.labelDatabaseName = new System.Windows.Forms.Label();
          this.labelAuthentication = new System.Windows.Forms.Label();
          this.radioUseServerAuthentication = new System.Windows.Forms.RadioButton();
          this.textUsername = new System.Windows.Forms.TextBox();
          this.labelUsername = new System.Windows.Forms.Label();
          this.labelPassword = new System.Windows.Forms.Label();
          this.textPassword = new System.Windows.Forms.TextBox();
          this.radioUseWindowsAuthentication = new System.Windows.Forms.RadioButton();
          this.textServerPort = new hMailServer.Shared.ucText();
          this.SuspendLayout();
          // 
          // labelDatabaseServerAddress
          // 
          this.labelDatabaseServerAddress.AutoSize = true;
          this.labelDatabaseServerAddress.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelDatabaseServerAddress.Location = new System.Drawing.Point(11, 10);
          this.labelDatabaseServerAddress.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
          this.labelDatabaseServerAddress.Name = "labelDatabaseServerAddress";
          this.labelDatabaseServerAddress.Size = new System.Drawing.Size(158, 17);
          this.labelDatabaseServerAddress.TabIndex = 0;
          this.labelDatabaseServerAddress.Text = "Database server address";
          // 
          // textServerAddress
          // 
          this.textServerAddress.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.textServerAddress.Location = new System.Drawing.Point(11, 30);
          this.textServerAddress.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
          this.textServerAddress.Name = "textServerAddress";
          this.textServerAddress.Size = new System.Drawing.Size(255, 24);
          this.textServerAddress.TabIndex = 4;
          // 
          // labelPort
          // 
          this.labelPort.AutoSize = true;
          this.labelPort.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelPort.Location = new System.Drawing.Point(299, 10);
          this.labelPort.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
          this.labelPort.Name = "labelPort";
          this.labelPort.Size = new System.Drawing.Size(34, 17);
          this.labelPort.TabIndex = 2;
          this.labelPort.Text = "Port";
          // 
          // textDatabaseName
          // 
          this.textDatabaseName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.textDatabaseName.Location = new System.Drawing.Point(11, 89);
          this.textDatabaseName.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
          this.textDatabaseName.Name = "textDatabaseName";
          this.textDatabaseName.Size = new System.Drawing.Size(191, 24);
          this.textDatabaseName.TabIndex = 5;
          // 
          // labelDatabaseName
          // 
          this.labelDatabaseName.AutoSize = true;
          this.labelDatabaseName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelDatabaseName.Location = new System.Drawing.Point(11, 69);
          this.labelDatabaseName.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
          this.labelDatabaseName.Name = "labelDatabaseName";
          this.labelDatabaseName.Size = new System.Drawing.Size(103, 17);
          this.labelDatabaseName.TabIndex = 4;
          this.labelDatabaseName.Text = "Database name";
          // 
          // labelAuthentication
          // 
          this.labelAuthentication.AutoSize = true;
          this.labelAuthentication.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelAuthentication.Location = new System.Drawing.Point(11, 128);
          this.labelAuthentication.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
          this.labelAuthentication.Name = "labelAuthentication";
          this.labelAuthentication.Size = new System.Drawing.Size(110, 17);
          this.labelAuthentication.TabIndex = 6;
          this.labelAuthentication.Text = "Authentication";
          // 
          // radioUseServerAuthentication
          // 
          this.radioUseServerAuthentication.AutoSize = true;
          this.radioUseServerAuthentication.Checked = true;
          this.radioUseServerAuthentication.Location = new System.Drawing.Point(43, 158);
          this.radioUseServerAuthentication.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
          this.radioUseServerAuthentication.Name = "radioUseServerAuthentication";
          this.radioUseServerAuthentication.Size = new System.Drawing.Size(188, 21);
          this.radioUseServerAuthentication.TabIndex = 7;
          this.radioUseServerAuthentication.TabStop = true;
          this.radioUseServerAuthentication.Text = "Use server authentication";
          this.radioUseServerAuthentication.UseVisualStyleBackColor = true;
          this.radioUseServerAuthentication.CheckedChanged += new System.EventHandler(this.radioUseServerAuthentication_CheckedChanged);
          // 
          // textUsername
          // 
          this.textUsername.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.textUsername.Location = new System.Drawing.Point(213, 197);
          this.textUsername.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
          this.textUsername.Name = "textUsername";
          this.textUsername.Size = new System.Drawing.Size(191, 24);
          this.textUsername.TabIndex = 2;
          // 
          // labelUsername
          // 
          this.labelUsername.AutoSize = true;
          this.labelUsername.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelUsername.Location = new System.Drawing.Point(75, 197);
          this.labelUsername.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
          this.labelUsername.Name = "labelUsername";
          this.labelUsername.Size = new System.Drawing.Size(69, 17);
          this.labelUsername.TabIndex = 9;
          this.labelUsername.Text = "Username";
          // 
          // labelPassword
          // 
          this.labelPassword.AutoSize = true;
          this.labelPassword.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelPassword.Location = new System.Drawing.Point(75, 236);
          this.labelPassword.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
          this.labelPassword.Name = "labelPassword";
          this.labelPassword.Size = new System.Drawing.Size(66, 17);
          this.labelPassword.TabIndex = 11;
          this.labelPassword.Text = "Password";
          // 
          // textPassword
          // 
          this.textPassword.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.textPassword.Location = new System.Drawing.Point(213, 236);
          this.textPassword.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
          this.textPassword.Name = "textPassword";
          this.textPassword.PasswordChar = '*';
          this.textPassword.Size = new System.Drawing.Size(191, 24);
          this.textPassword.TabIndex = 10;
          // 
          // radioUseWindowsAuthentication
          // 
          this.radioUseWindowsAuthentication.AutoSize = true;
          this.radioUseWindowsAuthentication.Location = new System.Drawing.Point(43, 291);
          this.radioUseWindowsAuthentication.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
          this.radioUseWindowsAuthentication.Name = "radioUseWindowsAuthentication";
          this.radioUseWindowsAuthentication.Size = new System.Drawing.Size(204, 21);
          this.radioUseWindowsAuthentication.TabIndex = 12;
          this.radioUseWindowsAuthentication.Text = "Use Windows authentication";
          this.radioUseWindowsAuthentication.UseVisualStyleBackColor = true;
          this.radioUseWindowsAuthentication.CheckedChanged += new System.EventHandler(this.radioUseWindowsAuthentication_CheckedChanged);
          // 
          // textServerPort
          // 
          this.textServerPort.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.textServerPort.Location = new System.Drawing.Point(299, 30);
          this.textServerPort.Margin = new System.Windows.Forms.Padding(4);
          this.textServerPort.MaxLength = 5;
          this.textServerPort.Name = "textServerPort";
          this.textServerPort.Number = 0;
          this.textServerPort.Numeric = true;
          this.textServerPort.Size = new System.Drawing.Size(127, 24);
          this.textServerPort.TabIndex = 3;
          // 
          // ucDBConnectionInfo
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.Controls.Add(this.radioUseWindowsAuthentication);
          this.Controls.Add(this.labelPassword);
          this.Controls.Add(this.textPassword);
          this.Controls.Add(this.labelUsername);
          this.Controls.Add(this.textUsername);
          this.Controls.Add(this.radioUseServerAuthentication);
          this.Controls.Add(this.labelAuthentication);
          this.Controls.Add(this.textDatabaseName);
          this.Controls.Add(this.labelDatabaseName);
          this.Controls.Add(this.textServerPort);
          this.Controls.Add(this.labelPort);
          this.Controls.Add(this.textServerAddress);
          this.Controls.Add(this.labelDatabaseServerAddress);
          this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
          this.Name = "ucDBConnectionInfo";
          this.Size = new System.Drawing.Size(633, 336);
          this.ResumeLayout(false);
          this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Label labelDatabaseServerAddress;
      private System.Windows.Forms.TextBox textServerAddress;
      private System.Windows.Forms.Label labelPort;
      private hMailServer.Shared.ucText textServerPort;
      private System.Windows.Forms.TextBox textDatabaseName;
      private System.Windows.Forms.Label labelDatabaseName;
      private System.Windows.Forms.Label labelAuthentication;
      private System.Windows.Forms.RadioButton radioUseServerAuthentication;
      private System.Windows.Forms.TextBox textUsername;
      private System.Windows.Forms.Label labelUsername;
      private System.Windows.Forms.Label labelPassword;
      private System.Windows.Forms.TextBox textPassword;
      private System.Windows.Forms.RadioButton radioUseWindowsAuthentication;
   }
}
