namespace hMailServer.Shared
{
   partial class ucWizard
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
         this.panelTop = new System.Windows.Forms.Panel();
         this.labelTitle = new System.Windows.Forms.Label();
         this.panelMiddle = new System.Windows.Forms.Panel();
         this.panelBottom = new System.Windows.Forms.Panel();
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.buttonNext = new System.Windows.Forms.Button();
         this.buttonPrevious = new System.Windows.Forms.Button();
         this.buttonCancel = new System.Windows.Forms.Button();
         this.panelTop.SuspendLayout();
         this.panelBottom.SuspendLayout();
         this.SuspendLayout();
         // 
         // panelTop
         // 
         this.panelTop.BackColor = System.Drawing.Color.White;
         this.panelTop.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
         this.panelTop.Controls.Add(this.labelTitle);
         this.panelTop.Dock = System.Windows.Forms.DockStyle.Top;
         this.panelTop.Location = new System.Drawing.Point(0, 0);
         this.panelTop.Name = "panelTop";
         this.panelTop.Size = new System.Drawing.Size(546, 48);
         this.panelTop.TabIndex = 0;
         // 
         // labelTitle
         // 
         this.labelTitle.AutoSize = true;
         this.labelTitle.Location = new System.Drawing.Point(8, 8);
         this.labelTitle.Name = "labelTitle";
         this.labelTitle.Size = new System.Drawing.Size(10, 13);
         this.labelTitle.TabIndex = 0;
         this.labelTitle.Text = "-";
         // 
         // panelMiddle
         // 
         this.panelMiddle.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.panelMiddle.Location = new System.Drawing.Point(0, 48);
         this.panelMiddle.Name = "panelMiddle";
         this.panelMiddle.Size = new System.Drawing.Size(546, 192);
         this.panelMiddle.TabIndex = 1;
         // 
         // panelBottom
         // 
         this.panelBottom.BackColor = System.Drawing.SystemColors.Control;
         this.panelBottom.Controls.Add(this.groupBox1);
         this.panelBottom.Controls.Add(this.buttonNext);
         this.panelBottom.Controls.Add(this.buttonPrevious);
         this.panelBottom.Controls.Add(this.buttonCancel);
         this.panelBottom.Dock = System.Windows.Forms.DockStyle.Bottom;
         this.panelBottom.Location = new System.Drawing.Point(0, 240);
         this.panelBottom.Name = "panelBottom";
         this.panelBottom.Size = new System.Drawing.Size(546, 44);
         this.panelBottom.TabIndex = 2;
         // 
         // groupBox1
         // 
         this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.groupBox1.Location = new System.Drawing.Point(8, 0);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(533, 4);
         this.groupBox1.TabIndex = 0;
         this.groupBox1.TabStop = false;
         // 
         // buttonNext
         // 
         this.buttonNext.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonNext.BackColor = System.Drawing.SystemColors.Control;
         this.buttonNext.Location = new System.Drawing.Point(299, 10);
         this.buttonNext.Name = "buttonNext";
         this.buttonNext.Size = new System.Drawing.Size(100, 25);
         this.buttonNext.TabIndex = 0;
         this.buttonNext.Text = "Next >";
         this.buttonNext.UseVisualStyleBackColor = false;
         this.buttonNext.Click += new System.EventHandler(this.buttonNext_Click);
         // 
         // buttonPrevious
         // 
         this.buttonPrevious.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonPrevious.BackColor = System.Drawing.SystemColors.Control;
         this.buttonPrevious.Location = new System.Drawing.Point(187, 10);
         this.buttonPrevious.Name = "buttonPrevious";
         this.buttonPrevious.Size = new System.Drawing.Size(100, 25);
         this.buttonPrevious.TabIndex = 1;
         this.buttonPrevious.Text = "< Previous";
         this.buttonPrevious.UseVisualStyleBackColor = false;
         this.buttonPrevious.Click += new System.EventHandler(this.buttonPrevious_Click);
         // 
         // buttonCancel
         // 
         this.buttonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonCancel.BackColor = System.Drawing.SystemColors.Control;
         this.buttonCancel.Location = new System.Drawing.Point(431, 10);
         this.buttonCancel.Name = "buttonCancel";
         this.buttonCancel.Size = new System.Drawing.Size(100, 25);
         this.buttonCancel.TabIndex = 2;
         this.buttonCancel.Text = "Cancel";
         this.buttonCancel.UseVisualStyleBackColor = false;
         this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
         // 
         // ucWizard
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.panelBottom);
         this.Controls.Add(this.panelMiddle);
         this.Controls.Add(this.panelTop);
         this.Name = "ucWizard";
         this.Size = new System.Drawing.Size(546, 284);
         this.panelTop.ResumeLayout(false);
         this.panelTop.PerformLayout();
         this.panelBottom.ResumeLayout(false);
         this.ResumeLayout(false);

      }

      #endregion

      private System.Windows.Forms.Panel panelTop;
      private System.Windows.Forms.Panel panelMiddle;
      private System.Windows.Forms.Panel panelBottom;
      private System.Windows.Forms.Button buttonNext;
      private System.Windows.Forms.Button buttonPrevious;
      private System.Windows.Forms.Button buttonCancel;
      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Label labelTitle;
   }
}
