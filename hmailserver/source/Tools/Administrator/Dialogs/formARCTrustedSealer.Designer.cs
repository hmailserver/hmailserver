namespace hMailServer.Administrator
{
   partial class formARCTrustedSealer
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
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.btnCancel = new System.Windows.Forms.Button();
         this.btnOK = new System.Windows.Forms.Button();
         this.labelDomain = new System.Windows.Forms.Label();
         this.labelDescription = new System.Windows.Forms.Label();
         this.textDomain = new hMailServer.Shared.ucText();
         this.textDescription = new hMailServer.Shared.ucText();
         this.SuspendLayout();
         // 
         // groupBox1
         // 
         this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.groupBox1.Location = new System.Drawing.Point(10, 137);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(247, 5);
         this.groupBox1.TabIndex = 32;
         this.groupBox1.TabStop = false;
         // 
         // btnCancel
         // 
         this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.btnCancel.Location = new System.Drawing.Point(169, 148);
         this.btnCancel.Name = "btnCancel";
         this.btnCancel.Size = new System.Drawing.Size(89, 25);
         this.btnCancel.TabIndex = 31;
         this.btnCancel.Text = "&Cancel";
         this.btnCancel.UseVisualStyleBackColor = true;
         // 
         // btnOK
         // 
         this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.btnOK.Location = new System.Drawing.Point(74, 148);
         this.btnOK.Name = "btnOK";
         this.btnOK.Size = new System.Drawing.Size(89, 25);
         this.btnOK.TabIndex = 30;
         this.btnOK.Text = "&OK";
         this.btnOK.UseVisualStyleBackColor = true;
         // 
         // labelDomain
         // 
         this.labelDomain.AutoSize = true;
         this.labelDomain.Location = new System.Drawing.Point(3, 7);
         this.labelDomain.Name = "labelDomain";
         this.labelDomain.Size = new System.Drawing.Size(43, 13);
         this.labelDomain.TabIndex = 24;
         this.labelDomain.Text = "Domain";
         // 
         // labelDescription
         // 
         this.labelDescription.AutoSize = true;
         this.labelDescription.Location = new System.Drawing.Point(3, 55);
         this.labelDescription.Name = "labelDescription";
         this.labelDescription.Size = new System.Drawing.Size(60, 13);
         this.labelDescription.TabIndex = 22;
         this.labelDescription.Text = "Description";
         // 
         // textDomain
         // 
         this.textDomain.Location = new System.Drawing.Point(8, 24);
         this.textDomain.Name = "textDomain";
         this.textDomain.Number = 0;
         this.textDomain.Numeric = false;
         this.textDomain.Size = new System.Drawing.Size(257, 20);
         this.textDomain.TabIndex = 25;
         // 
         // textDescription
         // 
         this.textDescription.Location = new System.Drawing.Point(8, 72);
         this.textDescription.Multiline = true;
         this.textDescription.Name = "textDescription";
         this.textDescription.Number = 0;
         this.textDescription.Numeric = false;
         this.textDescription.Size = new System.Drawing.Size(257, 48);
         this.textDescription.TabIndex = 23;
         // 
         // formARCTrustedSealer
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(271, 180);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.btnCancel);
         this.Controls.Add(this.btnOK);
         this.Controls.Add(this.textDomain);
         this.Controls.Add(this.labelDomain);
         this.Controls.Add(this.textDescription);
         this.Controls.Add(this.labelDescription);
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
         this.MaximizeBox = false;
         this.MinimizeBox = false;
         this.Name = "formARCTrustedSealer";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "ARC trusted sealer";
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Button btnCancel;
      private System.Windows.Forms.Button btnOK;
      private hMailServer.Shared.ucText textDomain;
      private System.Windows.Forms.Label labelDomain;
      private hMailServer.Shared.ucText textDescription;
      private System.Windows.Forms.Label labelDescription;
   }
}