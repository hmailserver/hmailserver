namespace hMailServer.Administrator
{
   partial class formAbout
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formAbout));
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.btnClose = new System.Windows.Forms.Button();
         this.labelVersion = new System.Windows.Forms.Label();
         this.linkVisitors = new System.Windows.Forms.LinkLabel();
         this.labelAttribution = new System.Windows.Forms.Label();
         this.SuspendLayout();
         // 
         // groupBox1
         // 
         this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
         this.groupBox1.Location = new System.Drawing.Point(7, 151);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(291, 4);
         this.groupBox1.TabIndex = 10;
         this.groupBox1.TabStop = false;
         // 
         // btnClose
         // 
         this.btnClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnClose.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.btnClose.Location = new System.Drawing.Point(209, 164);
         this.btnClose.Name = "btnClose";
         this.btnClose.Size = new System.Drawing.Size(89, 25);
         this.btnClose.TabIndex = 9;
         this.btnClose.Text = "&Close";
         this.btnClose.UseVisualStyleBackColor = true;
         // 
         // labelVersion
         // 
         this.labelVersion.AutoSize = true;
         this.labelVersion.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelVersion.Location = new System.Drawing.Point(13, 12);
         this.labelVersion.Name = "labelVersion";
         this.labelVersion.Size = new System.Drawing.Size(85, 16);
         this.labelVersion.TabIndex = 11;
         this.labelVersion.Text = "hMailServer";
         // 
         // linkVisitors
         // 
         this.linkVisitors.AutoSize = true;
         this.linkVisitors.Location = new System.Drawing.Point(12, 38);
         this.linkVisitors.Name = "linkVisitors";
         this.linkVisitors.Size = new System.Drawing.Size(93, 13);
         this.linkVisitors.TabIndex = 13;
         this.linkVisitors.TabStop = true;
         this.linkVisitors.Text = "List of contributors";
         this.linkVisitors.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkVisitors_LinkClicked);
         // 
         // labelAttribution
         // 
         this.labelAttribution.AutoSize = true;
         this.labelAttribution.Location = new System.Drawing.Point(13, 66);
         this.labelAttribution.Name = "labelAttribution";
         this.labelAttribution.Size = new System.Drawing.Size(199, 26);
         this.labelAttribution.TabIndex = 14;
         this.labelAttribution.Text = "Please run installation program for full list \r\nof copyrights and attributions.";
         // 
         // formAbout
         // 
         this.AcceptButton = this.btnClose;
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(310, 195);
         this.Controls.Add(this.labelAttribution);
         this.Controls.Add(this.linkVisitors);
         this.Controls.Add(this.labelVersion);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.btnClose);
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.MaximizeBox = false;
         this.MinimizeBox = false;
         this.Name = "formAbout";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "About...";
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Button btnClose;
      private System.Windows.Forms.Label labelVersion;
      private System.Windows.Forms.LinkLabel linkVisitors;
      private System.Windows.Forms.Label labelAttribution;
   }
}