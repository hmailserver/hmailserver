namespace hMailServer.Administrator
{
   partial class formWhiteListAddress
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
         this.textDescription = new hMailServer.Shared.ucText();
         this.labelDescription = new System.Windows.Forms.Label();
         this.textLowerIP = new hMailServer.Shared.ucText();
         this.labelLowerIP = new System.Windows.Forms.Label();
         this.textUpperIP = new hMailServer.Shared.ucText();
         this.labelUpperIP = new System.Windows.Forms.Label();
         this.textEmailAddress = new hMailServer.Shared.ucText();
         this.labelEmailAddress = new System.Windows.Forms.Label();
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.btnCancel = new System.Windows.Forms.Button();
         this.btnOK = new System.Windows.Forms.Button();
         this.SuspendLayout();
         // 
         // textDescription
         // 
         this.textDescription.Location = new System.Drawing.Point(12, 27);
         this.textDescription.Name = "textDescription";
         this.textDescription.Number = 0;
         this.textDescription.Numeric = false;
         this.textDescription.Size = new System.Drawing.Size(257, 20);
         this.textDescription.TabIndex = 5;
         // 
         // labelDescription
         // 
         this.labelDescription.AutoSize = true;
         this.labelDescription.Location = new System.Drawing.Point(7, 10);
         this.labelDescription.Name = "labelDescription";
         this.labelDescription.Size = new System.Drawing.Size(60, 13);
         this.labelDescription.TabIndex = 4;
         this.labelDescription.Text = "Description";
         // 
         // textLowerIP
         // 
         this.textLowerIP.Location = new System.Drawing.Point(12, 74);
         this.textLowerIP.Name = "textLowerIP";
         this.textLowerIP.Number = 0;
         this.textLowerIP.Numeric = false;
         this.textLowerIP.Size = new System.Drawing.Size(114, 20);
         this.textLowerIP.TabIndex = 7;
         this.textLowerIP.Text = "0.0.0.0";
         // 
         // labelLowerIP
         // 
         this.labelLowerIP.AutoSize = true;
         this.labelLowerIP.Location = new System.Drawing.Point(7, 57);
         this.labelLowerIP.Name = "labelLowerIP";
         this.labelLowerIP.Size = new System.Drawing.Size(49, 13);
         this.labelLowerIP.TabIndex = 6;
         this.labelLowerIP.Text = "Lower IP";
         // 
         // textUpperIP
         // 
         this.textUpperIP.Location = new System.Drawing.Point(138, 74);
         this.textUpperIP.Name = "textUpperIP";
         this.textUpperIP.Number = 0;
         this.textUpperIP.Numeric = false;
         this.textUpperIP.Size = new System.Drawing.Size(131, 20);
         this.textUpperIP.TabIndex = 9;
         this.textUpperIP.Text = "255.255.255.255";
         // 
         // labelUpperIP
         // 
         this.labelUpperIP.AutoSize = true;
         this.labelUpperIP.Location = new System.Drawing.Point(133, 57);
         this.labelUpperIP.Name = "labelUpperIP";
         this.labelUpperIP.Size = new System.Drawing.Size(49, 13);
         this.labelUpperIP.TabIndex = 8;
         this.labelUpperIP.Text = "Upper IP";
         // 
         // textEmailAddress
         // 
         this.textEmailAddress.Location = new System.Drawing.Point(10, 118);
         this.textEmailAddress.Name = "textEmailAddress";
         this.textEmailAddress.Number = 0;
         this.textEmailAddress.Numeric = false;
         this.textEmailAddress.Size = new System.Drawing.Size(257, 20);
         this.textEmailAddress.TabIndex = 11;
         // 
         // labelEmailAddress
         // 
         this.labelEmailAddress.AutoSize = true;
         this.labelEmailAddress.Location = new System.Drawing.Point(5, 101);
         this.labelEmailAddress.Name = "labelEmailAddress";
         this.labelEmailAddress.Size = new System.Drawing.Size(75, 13);
         this.labelEmailAddress.TabIndex = 10;
         this.labelEmailAddress.Text = "E-mail address";
         // 
         // groupBox1
         // 
         this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.groupBox1.Location = new System.Drawing.Point(4, 146);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(263, 5);
         this.groupBox1.TabIndex = 21;
         this.groupBox1.TabStop = false;
         // 
         // btnCancel
         // 
         this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.btnCancel.Location = new System.Drawing.Point(179, 157);
         this.btnCancel.Name = "btnCancel";
         this.btnCancel.Size = new System.Drawing.Size(89, 25);
         this.btnCancel.TabIndex = 20;
         this.btnCancel.Text = "&Cancel";
         this.btnCancel.UseVisualStyleBackColor = true;
         // 
         // btnOK
         // 
         this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.btnOK.Location = new System.Drawing.Point(84, 157);
         this.btnOK.Name = "btnOK";
         this.btnOK.Size = new System.Drawing.Size(89, 25);
         this.btnOK.TabIndex = 19;
         this.btnOK.Text = "&OK";
         this.btnOK.UseVisualStyleBackColor = true;
         // 
         // formWhiteListAddress
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(281, 194);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.btnCancel);
         this.Controls.Add(this.btnOK);
         this.Controls.Add(this.textEmailAddress);
         this.Controls.Add(this.labelEmailAddress);
         this.Controls.Add(this.textUpperIP);
         this.Controls.Add(this.labelUpperIP);
         this.Controls.Add(this.textLowerIP);
         this.Controls.Add(this.labelLowerIP);
         this.Controls.Add(this.textDescription);
         this.Controls.Add(this.labelDescription);
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
         this.Name = "formWhiteListAddress";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "White listing";
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private hMailServer.Shared.ucText textDescription;
      private System.Windows.Forms.Label labelDescription;
      private hMailServer.Shared.ucText textLowerIP;
      private System.Windows.Forms.Label labelLowerIP;
      private hMailServer.Shared.ucText textUpperIP;
      private System.Windows.Forms.Label labelUpperIP;
      private hMailServer.Shared.ucText textEmailAddress;
      private System.Windows.Forms.Label labelEmailAddress;
      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Button btnCancel;
      private System.Windows.Forms.Button btnOK;
   }
}