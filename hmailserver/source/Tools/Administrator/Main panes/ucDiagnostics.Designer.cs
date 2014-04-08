namespace hMailServer.Administrator
{
   partial class ucDiagnostics
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
          this.components = new System.ComponentModel.Container();
          System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ucDiagnostics));
          this.labelInfo = new System.Windows.Forms.Label();
          this.labelSelectDomain = new System.Windows.Forms.Label();
          this.buttonPerformTests = new System.Windows.Forms.Button();
          this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
          this.buttonSaveToFile = new System.Windows.Forms.Button();
          this.buttonCopyToClipboard = new System.Windows.Forms.Button();
          this.treeResults = new System.Windows.Forms.TreeView();
          this.imageList1 = new System.Windows.Forms.ImageList(this.components);
          this.textboxTestDomain = new System.Windows.Forms.TextBox();
          this.labelTestDomain = new System.Windows.Forms.Label();
          this.comboLocalDomain = new hMailServer.Administrator.Controls.ucComboBox();
          this.SuspendLayout();
          // 
          // labelInfo
          // 
          this.labelInfo.AutoSize = true;
          this.labelInfo.Location = new System.Drawing.Point(14, 13);
          this.labelInfo.Name = "labelInfo";
          this.labelInfo.Size = new System.Drawing.Size(332, 13);
          this.labelInfo.TabIndex = 5;
          this.labelInfo.Text = "This tool lets you perform diagnostics on your hMailServer installation.";
          // 
          // labelSelectDomain
          // 
          this.labelSelectDomain.AutoSize = true;
          this.labelSelectDomain.Location = new System.Drawing.Point(14, 47);
          this.labelSelectDomain.Name = "labelSelectDomain";
          this.labelSelectDomain.Size = new System.Drawing.Size(211, 13);
          this.labelSelectDomain.TabIndex = 6;
          this.labelSelectDomain.Text = "Select the domain you want to run tests on.";
          // 
          // buttonPerformTests
          // 
          this.buttonPerformTests.Location = new System.Drawing.Point(17, 105);
          this.buttonPerformTests.Name = "buttonPerformTests";
          this.buttonPerformTests.Size = new System.Drawing.Size(98, 26);
          this.buttonPerformTests.TabIndex = 11;
          this.buttonPerformTests.Text = "Start";
          this.buttonPerformTests.UseVisualStyleBackColor = true;
          this.buttonPerformTests.Click += new System.EventHandler(this.buttonPerformTests_Click);
          // 
          // saveFileDialog1
          // 
          this.saveFileDialog1.DefaultExt = "*.txt";
          this.saveFileDialog1.FileName = "hMailServer diagnostics log.txt";
          this.saveFileDialog1.Filter = "Text file|*.txt";
          // 
          // buttonSaveToFile
          // 
          this.buttonSaveToFile.Enabled = false;
          this.buttonSaveToFile.Location = new System.Drawing.Point(322, 105);
          this.buttonSaveToFile.Name = "buttonSaveToFile";
          this.buttonSaveToFile.Size = new System.Drawing.Size(149, 26);
          this.buttonSaveToFile.TabIndex = 15;
          this.buttonSaveToFile.Text = "Save";
          this.buttonSaveToFile.UseVisualStyleBackColor = true;
          this.buttonSaveToFile.Click += new System.EventHandler(this.buttonSaveToFile_Click);
          // 
          // buttonCopyToClipboard
          // 
          this.buttonCopyToClipboard.Enabled = false;
          this.buttonCopyToClipboard.Location = new System.Drawing.Point(151, 105);
          this.buttonCopyToClipboard.Name = "buttonCopyToClipboard";
          this.buttonCopyToClipboard.Size = new System.Drawing.Size(165, 26);
          this.buttonCopyToClipboard.TabIndex = 14;
          this.buttonCopyToClipboard.Text = "Copy to clipboard";
          this.buttonCopyToClipboard.UseVisualStyleBackColor = true;
          this.buttonCopyToClipboard.Click += new System.EventHandler(this.buttonCopyToClipboard_Click);
          // 
          // treeResults
          // 
          this.treeResults.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                      | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.treeResults.ImageIndex = 0;
          this.treeResults.ImageList = this.imageList1;
          this.treeResults.Location = new System.Drawing.Point(20, 161);
          this.treeResults.Name = "treeResults";
          this.treeResults.SelectedImageIndex = 0;
          this.treeResults.Size = new System.Drawing.Size(649, 268);
          this.treeResults.TabIndex = 16;
          // 
          // imageList1
          // 
          this.imageList1.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList1.ImageStream")));
          this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
          this.imageList1.Images.SetKeyName(0, "bullet_ball_red.ico");
          this.imageList1.Images.SetKeyName(1, "bullet_ball_green.ico");
          // 
          // textboxTestDomain
          // 
          this.textboxTestDomain.Location = new System.Drawing.Point(327, 67);
          this.textboxTestDomain.Name = "textboxTestDomain";
          this.textboxTestDomain.Size = new System.Drawing.Size(214, 20);
          this.textboxTestDomain.TabIndex = 17;
          this.textboxTestDomain.Text = "mail.hmailserver.com";
          // 
          // labelTestDomain
          // 
          this.labelTestDomain.AutoSize = true;
          this.labelTestDomain.Location = new System.Drawing.Point(324, 47);
          this.labelTestDomain.Name = "labelTestDomain";
          this.labelTestDomain.Size = new System.Drawing.Size(159, 13);
          this.labelTestDomain.TabIndex = 18;
          this.labelTestDomain.Text = "Outbound port 25 test hostname";
          // 
          // comboLocalDomain
          // 
          this.comboLocalDomain.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboLocalDomain.FormattingEnabled = true;
          this.comboLocalDomain.Location = new System.Drawing.Point(17, 66);
          this.comboLocalDomain.Name = "comboLocalDomain";
          this.comboLocalDomain.Size = new System.Drawing.Size(174, 21);
          this.comboLocalDomain.TabIndex = 4;
          // 
          // ucDiagnostics
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.Controls.Add(this.labelTestDomain);
          this.Controls.Add(this.textboxTestDomain);
          this.Controls.Add(this.treeResults);
          this.Controls.Add(this.buttonSaveToFile);
          this.Controls.Add(this.buttonCopyToClipboard);
          this.Controls.Add(this.buttonPerformTests);
          this.Controls.Add(this.labelSelectDomain);
          this.Controls.Add(this.labelInfo);
          this.Controls.Add(this.comboLocalDomain);
          this.Name = "ucDiagnostics";
          this.Size = new System.Drawing.Size(689, 453);
          this.ResumeLayout(false);
          this.PerformLayout();

      }

      #endregion

      private hMailServer.Administrator.Controls.ucComboBox comboLocalDomain;
      private System.Windows.Forms.Label labelInfo;
      private System.Windows.Forms.Label labelSelectDomain;
      private System.Windows.Forms.Button buttonPerformTests;
      private System.Windows.Forms.SaveFileDialog saveFileDialog1;
      private System.Windows.Forms.Button buttonSaveToFile;
      private System.Windows.Forms.Button buttonCopyToClipboard;
      private System.Windows.Forms.TreeView treeResults;
      private System.Windows.Forms.ImageList imageList1;
      private System.Windows.Forms.TextBox textboxTestDomain;
      private System.Windows.Forms.Label labelTestDomain;
   }
}
