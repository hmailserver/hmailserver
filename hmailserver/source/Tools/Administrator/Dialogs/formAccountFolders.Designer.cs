namespace hMailServer.Administrator.Dialogs
{
   partial class formAccountFolders
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formAccountFolders));
         this.buttonClose = new System.Windows.Forms.Button();
         this.ucIMAPFolders1 = new hMailServer.Administrator.ucIMAPFolders();
         this.SuspendLayout();
         // 
         // buttonClose
         // 
         this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonClose.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.buttonClose.Location = new System.Drawing.Point(508, 335);
         this.buttonClose.Name = "buttonClose";
         this.buttonClose.Size = new System.Drawing.Size(97, 28);
         this.buttonClose.TabIndex = 29;
         this.buttonClose.Text = "&Close";
         this.buttonClose.UseVisualStyleBackColor = true;
         this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
         // 
         // ucIMAPFolders1
         // 
         this.ucIMAPFolders1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.ucIMAPFolders1.Location = new System.Drawing.Point(8, 8);
         this.ucIMAPFolders1.Name = "ucIMAPFolders1";
         this.ucIMAPFolders1.Size = new System.Drawing.Size(604, 319);
         this.ucIMAPFolders1.TabIndex = 0;
         // 
         // formAccountFolders
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(617, 368);
         this.Controls.Add(this.buttonClose);
         this.Controls.Add(this.ucIMAPFolders1);
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.MinimumSize = new System.Drawing.Size(578, 290);
         this.Name = "formAccountFolders";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "Folders";
         this.ResumeLayout(false);

      }

      #endregion

      private ucIMAPFolders ucIMAPFolders1;
      private System.Windows.Forms.Button buttonClose;
   }
}