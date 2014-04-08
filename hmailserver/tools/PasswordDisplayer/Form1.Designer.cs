namespace PasswordDisplayer
{
   partial class formDatabasePassword
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
         this.textPassword = new System.Windows.Forms.TextBox();
         this.buttonCopyAndClose = new System.Windows.Forms.Button();
         this.SuspendLayout();
         // 
         // textPassword
         // 
         this.textPassword.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.textPassword.Location = new System.Drawing.Point(12, 12);
         this.textPassword.Name = "textPassword";
         this.textPassword.Size = new System.Drawing.Size(308, 26);
         this.textPassword.TabIndex = 0;
         // 
         // buttonCopyAndClose
         // 
         this.buttonCopyAndClose.Location = new System.Drawing.Point(173, 44);
         this.buttonCopyAndClose.Name = "buttonCopyAndClose";
         this.buttonCopyAndClose.Size = new System.Drawing.Size(147, 23);
         this.buttonCopyAndClose.TabIndex = 2;
         this.buttonCopyAndClose.Text = "Copy && close";
         this.buttonCopyAndClose.UseVisualStyleBackColor = true;
         this.buttonCopyAndClose.Click += new System.EventHandler(this.buttonCopyAndClose_Click);
         // 
         // formDatabasePassword
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(327, 72);
         this.Controls.Add(this.buttonCopyAndClose);
         this.Controls.Add(this.textPassword);
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
         this.MaximizeBox = false;
         this.MinimizeBox = false;
         this.Name = "formDatabasePassword";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
         this.Text = "Database password";
         this.Load += new System.EventHandler(this.formDatabasePassword_Load);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.TextBox textPassword;
      private System.Windows.Forms.Button buttonCopyAndClose;
   }
}

