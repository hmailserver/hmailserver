namespace hMailServer.Administrator.Controls
{
   partial class ucEmailEdit
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
         this.textMailbox = new System.Windows.Forms.TextBox();
         this.labelAt = new System.Windows.Forms.Label();
         this.textDomainName = new System.Windows.Forms.TextBox();
         this.SuspendLayout();
         // 
         // textMailbox
         // 
         this.textMailbox.Location = new System.Drawing.Point(0, 0);
         this.textMailbox.Name = "textMailbox";
         this.textMailbox.Size = new System.Drawing.Size(139, 20);
         this.textMailbox.TabIndex = 0;
         this.textMailbox.TextChanged += new System.EventHandler(this.textMailbox_TextChanged);
         // 
         // labelAt
         // 
         this.labelAt.AutoSize = true;
         this.labelAt.Location = new System.Drawing.Point(139, 3);
         this.labelAt.Name = "labelAt";
         this.labelAt.Size = new System.Drawing.Size(18, 13);
         this.labelAt.TabIndex = 1;
         this.labelAt.Text = "@";
         // 
         // textDomainName
         // 
         this.textDomainName.Location = new System.Drawing.Point(158, 0);
         this.textDomainName.Name = "textDomainName";
         this.textDomainName.Size = new System.Drawing.Size(122, 20);
         this.textDomainName.TabIndex = 2;
         this.textDomainName.TextChanged += new System.EventHandler(this.textDomainName_TextChanged);
         // 
         // ucEmailEdit
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.textDomainName);
         this.Controls.Add(this.labelAt);
         this.Controls.Add(this.textMailbox);
         this.Name = "ucEmailEdit";
         this.Size = new System.Drawing.Size(292, 21);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.TextBox textMailbox;
      private System.Windows.Forms.Label labelAt;
      private System.Windows.Forms.TextBox textDomainName;
   }
}
