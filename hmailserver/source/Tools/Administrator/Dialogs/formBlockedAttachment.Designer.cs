namespace hMailServer.Administrator.Dialogs
{
   partial class formBlockedAttachment
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
         this.labelWildcard = new System.Windows.Forms.Label();
         this.labelDescription = new System.Windows.Forms.Label();
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.btnCancel = new System.Windows.Forms.Button();
         this.btnOK = new System.Windows.Forms.Button();
         this.textDescription = new hMailServer.Shared.ucText();
         this.textWildcard = new hMailServer.Shared.ucText();
         this.SuspendLayout();
         // 
         // labelWildcard
         // 
         this.labelWildcard.AutoSize = true;
         this.labelWildcard.Location = new System.Drawing.Point(7, 8);
         this.labelWildcard.Name = "labelWildcard";
         this.labelWildcard.Size = new System.Drawing.Size(49, 13);
         this.labelWildcard.TabIndex = 2;
         this.labelWildcard.Text = "Wildcard";
         // 
         // labelDescription
         // 
         this.labelDescription.AutoSize = true;
         this.labelDescription.Location = new System.Drawing.Point(7, 57);
         this.labelDescription.Name = "labelDescription";
         this.labelDescription.Size = new System.Drawing.Size(60, 13);
         this.labelDescription.TabIndex = 4;
         this.labelDescription.Text = "Description";
         // 
         // groupBox1
         // 
         this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.groupBox1.Location = new System.Drawing.Point(12, 177);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(207, 5);
         this.groupBox1.TabIndex = 18;
         this.groupBox1.TabStop = false;
         // 
         // btnCancel
         // 
         this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.btnCancel.Location = new System.Drawing.Point(128, 186);
         this.btnCancel.Name = "btnCancel";
         this.btnCancel.Size = new System.Drawing.Size(89, 25);
         this.btnCancel.TabIndex = 17;
         this.btnCancel.Text = "&Cancel";
         this.btnCancel.UseVisualStyleBackColor = true;
         // 
         // btnOK
         // 
         this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.btnOK.Location = new System.Drawing.Point(33, 186);
         this.btnOK.Name = "btnOK";
         this.btnOK.Size = new System.Drawing.Size(89, 25);
         this.btnOK.TabIndex = 16;
         this.btnOK.Text = "&OK";
         this.btnOK.UseVisualStyleBackColor = true;
         // 
         // textDescription
         // 
         this.textDescription.Location = new System.Drawing.Point(12, 74);
         this.textDescription.Multiline = true;
         this.textDescription.Name = "textDescription";
         this.textDescription.Number = 0;
         this.textDescription.Numeric = false;
         this.textDescription.Size = new System.Drawing.Size(203, 95);
         this.textDescription.TabIndex = 5;
         // 
         // textWildcard
         // 
         this.textWildcard.Location = new System.Drawing.Point(12, 25);
         this.textWildcard.Name = "textWildcard";
         this.textWildcard.Number = 0;
         this.textWildcard.Numeric = false;
         this.textWildcard.Size = new System.Drawing.Size(203, 20);
         this.textWildcard.TabIndex = 3;
         // 
         // formBlockedAttachment
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(231, 223);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.btnCancel);
         this.Controls.Add(this.btnOK);
         this.Controls.Add(this.textDescription);
         this.Controls.Add(this.labelDescription);
         this.Controls.Add(this.textWildcard);
         this.Controls.Add(this.labelWildcard);
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
         this.Name = "formBlockedAttachment";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "Blocked attachment";
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private hMailServer.Shared.ucText textWildcard;
      private System.Windows.Forms.Label labelWildcard;
      private hMailServer.Shared.ucText textDescription;
      private System.Windows.Forms.Label labelDescription;
      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Button btnCancel;
      private System.Windows.Forms.Button btnOK;
   }
}