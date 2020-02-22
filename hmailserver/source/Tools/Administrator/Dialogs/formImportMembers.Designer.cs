namespace hMailServer.Administrator.Dialogs
{
   partial class formImportMembers
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formImportMembers));
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.btnCancel = new System.Windows.Forms.Button();
         this.label1 = new System.Windows.Forms.Label();
         this.ucImportFile = new hMailServer.Shared.ucText();
         this.buttonSelectImportFile = new System.Windows.Forms.Button();
         this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
         this.buttonImport = new System.Windows.Forms.Button();
         this.progressBar1 = new System.Windows.Forms.ProgressBar();
         this.ucDeleteRecipientsNotInList = new hMailServer.Administrator.Controls.ucCheckbox();
         this.listItems = new hMailServer.Administrator.ucListView();
         this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
         this.SuspendLayout();
         // 
         // groupBox1
         // 
         this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.groupBox1.Location = new System.Drawing.Point(19, 359);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(580, 5);
         this.groupBox1.TabIndex = 27;
         this.groupBox1.TabStop = false;
         // 
         // btnCancel
         // 
         this.btnCancel.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
         this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.btnCancel.Location = new System.Drawing.Point(510, 370);
         this.btnCancel.Name = "btnCancel";
         this.btnCancel.Size = new System.Drawing.Size(89, 25);
         this.btnCancel.TabIndex = 4;
         this.btnCancel.Text = "&Close";
         this.btnCancel.UseVisualStyleBackColor = true;
         this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
         // 
         // label1
         // 
         this.label1.AutoSize = true;
         this.label1.Location = new System.Drawing.Point(15, 13);
         this.label1.Name = "label1";
         this.label1.Size = new System.Drawing.Size(26, 13);
         this.label1.TabIndex = 28;
         this.label1.Text = "File:";
         // 
         // ucImportFile
         // 
         this.ucImportFile.Location = new System.Drawing.Point(18, 30);
         this.ucImportFile.Name = "ucImportFile";
         this.ucImportFile.Number = 0;
         this.ucImportFile.Number64 = ((long)(0));
         this.ucImportFile.Numeric = false;
         this.ucImportFile.ReadOnly = true;
         this.ucImportFile.Size = new System.Drawing.Size(385, 20);
         this.ucImportFile.TabIndex = 0;
         // 
         // buttonSelectImportFile
         // 
         this.buttonSelectImportFile.Location = new System.Drawing.Point(410, 29);
         this.buttonSelectImportFile.Name = "buttonSelectImportFile";
         this.buttonSelectImportFile.Size = new System.Drawing.Size(75, 23);
         this.buttonSelectImportFile.TabIndex = 1;
         this.buttonSelectImportFile.Text = "...";
         this.buttonSelectImportFile.UseVisualStyleBackColor = true;
         this.buttonSelectImportFile.Click += new System.EventHandler(this.buttonSelectImportFile_Click);
         // 
         // openFileDialog1
         // 
         this.openFileDialog1.FileName = "openFileDialog1";
         // 
         // buttonImport
         // 
         this.buttonImport.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.buttonImport.Enabled = false;
         this.buttonImport.Location = new System.Drawing.Point(16, 320);
         this.buttonImport.Name = "buttonImport";
         this.buttonImport.Size = new System.Drawing.Size(89, 25);
         this.buttonImport.TabIndex = 3;
         this.buttonImport.Text = "&Import";
         this.buttonImport.UseVisualStyleBackColor = true;
         this.buttonImport.Click += new System.EventHandler(this.buttonImport_Click);
         // 
         // progressBar1
         // 
         this.progressBar1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.progressBar1.Location = new System.Drawing.Point(113, 321);
         this.progressBar1.Name = "progressBar1";
         this.progressBar1.Size = new System.Drawing.Size(484, 23);
         this.progressBar1.TabIndex = 32;
         // 
         // ucDeleteRecipientsNotInList
         // 
         this.ucDeleteRecipientsNotInList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.ucDeleteRecipientsNotInList.AutoSize = true;
         this.ucDeleteRecipientsNotInList.Location = new System.Drawing.Point(17, 298);
         this.ucDeleteRecipientsNotInList.Name = "ucDeleteRecipientsNotInList";
         this.ucDeleteRecipientsNotInList.Size = new System.Drawing.Size(181, 17);
         this.ucDeleteRecipientsNotInList.TabIndex = 33;
         this.ucDeleteRecipientsNotInList.Text = "Delete recipients not in import-file";
         this.ucDeleteRecipientsNotInList.UseVisualStyleBackColor = true;
         this.ucDeleteRecipientsNotInList.CheckedChanged += new System.EventHandler(this.ucDeleteRecipientsNotInList_CheckedChanged);
         // 
         // listItems
         // 
         this.listItems.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.listItems.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
         this.listItems.FullRowSelect = true;
         this.listItems.Location = new System.Drawing.Point(18, 58);
         this.listItems.Name = "listItems";
         this.listItems.Size = new System.Drawing.Size(579, 234);
         this.listItems.TabIndex = 2;
         this.listItems.UseCompatibleStateImageBehavior = false;
         this.listItems.View = System.Windows.Forms.View.Details;
         // 
         // columnHeader1
         // 
         this.columnHeader1.Text = "Name";
         this.columnHeader1.Width = 250;
         // 
         // formImportMembers
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(611, 407);
         this.Controls.Add(this.ucDeleteRecipientsNotInList);
         this.Controls.Add(this.progressBar1);
         this.Controls.Add(this.buttonImport);
         this.Controls.Add(this.buttonSelectImportFile);
         this.Controls.Add(this.ucImportFile);
         this.Controls.Add(this.label1);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.btnCancel);
         this.Controls.Add(this.listItems);
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.MinimumSize = new System.Drawing.Size(381, 314);
         this.Name = "formImportMembers";
         this.ShowInTaskbar = false;
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "Import";
         this.Load += new System.EventHandler(this.formImportMembers_Load);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private ucListView listItems;
      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Button btnCancel;
      private System.Windows.Forms.ColumnHeader columnHeader1;
      private System.Windows.Forms.Label label1;
      private hMailServer.Shared.ucText ucImportFile;
      private System.Windows.Forms.Button buttonSelectImportFile;
      private System.Windows.Forms.OpenFileDialog openFileDialog1;
      private System.Windows.Forms.Button buttonImport;
      private System.Windows.Forms.ProgressBar progressBar1;
      private hMailServer.Administrator.Controls.ucCheckbox ucDeleteRecipientsNotInList;
   }
}