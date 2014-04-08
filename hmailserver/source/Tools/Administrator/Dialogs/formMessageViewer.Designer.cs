namespace hMailServer.Administrator.Dialogs
{
   partial class formMessageViewer
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formMessageViewer));
         this.labelFile = new System.Windows.Forms.Label();
         this.buttonClose = new System.Windows.Forms.Button();
         this.textMessage = new hMailServer.Shared.ucText();
         this.textFile = new hMailServer.Shared.ucText();
         this.SuspendLayout();
         // 
         // labelFile
         // 
         this.labelFile.AutoSize = true;
         this.labelFile.Location = new System.Drawing.Point(12, 9);
         this.labelFile.Name = "labelFile";
         this.labelFile.Size = new System.Drawing.Size(23, 13);
         this.labelFile.TabIndex = 15;
         this.labelFile.Text = "File";
         // 
         // buttonClose
         // 
         this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonClose.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.buttonClose.Location = new System.Drawing.Point(470, 334);
         this.buttonClose.Name = "buttonClose";
         this.buttonClose.Size = new System.Drawing.Size(89, 25);
         this.buttonClose.TabIndex = 17;
         this.buttonClose.Text = "&Close";
         this.buttonClose.UseVisualStyleBackColor = true;
         this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
         // 
         // textMessage
         // 
         this.textMessage.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.textMessage.Location = new System.Drawing.Point(12, 57);
         this.textMessage.Multiline = true;
         this.textMessage.Name = "textMessage";
         this.textMessage.Number = 0;
         this.textMessage.Numeric = false;
         this.textMessage.ReadOnly = true;
         this.textMessage.ScrollBars = System.Windows.Forms.ScrollBars.Both;
         this.textMessage.Size = new System.Drawing.Size(547, 271);
         this.textMessage.TabIndex = 16;
         // 
         // textFile
         // 
         this.textFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.textFile.Location = new System.Drawing.Point(12, 25);
         this.textFile.Name = "textFile";
         this.textFile.Number = 0;
         this.textFile.Numeric = false;
         this.textFile.ReadOnly = true;
         this.textFile.Size = new System.Drawing.Size(547, 20);
         this.textFile.TabIndex = 14;
         // 
         // formMessageViewer
         // 
         this.AcceptButton = this.buttonClose;
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(571, 371);
         this.Controls.Add(this.buttonClose);
         this.Controls.Add(this.textMessage);
         this.Controls.Add(this.labelFile);
         this.Controls.Add(this.textFile);
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.Name = "formMessageViewer";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "Message viewer";
         this.Shown += new System.EventHandler(this.formMessageViewer_Shown);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private hMailServer.Shared.ucText textFile;
      private System.Windows.Forms.Label labelFile;
      private hMailServer.Shared.ucText textMessage;
      private System.Windows.Forms.Button buttonClose;
   }
}