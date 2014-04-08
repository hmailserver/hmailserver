namespace VMwareIntegration
{
   partial class formConfigurations
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
         this.components = new System.ComponentModel.Container();
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formConfigurations));
         this.listConfigurations = new System.Windows.Forms.ListView();
         this.columnType = new System.Windows.Forms.ColumnHeader();
         this.columnOS = new System.Windows.Forms.ColumnHeader();
         this.columnDatabaseEngine = new System.Windows.Forms.ColumnHeader();
         this.columnSnapshot = new System.Windows.Forms.ColumnHeader();
         this.columnStatus = new System.Windows.Forms.ColumnHeader();
         this.buttonRunSelected = new System.Windows.Forms.Button();
         this.buttonClose = new System.Windows.Forms.Button();
         this.checkStopOnError = new System.Windows.Forms.CheckBox();
         this.buttonSelectAll = new System.Windows.Forms.Button();
         this.buttonSelectNone = new System.Windows.Forms.Button();
         this.checkShutdownOnSuccess = new System.Windows.Forms.CheckBox();
         this.listStatus = new System.Windows.Forms.ListView();
         this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
         this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
         this.timer = new System.Windows.Forms.Timer(this.components);
         this.buttonStop = new System.Windows.Forms.Button();
         this.SuspendLayout();
         // 
         // listConfigurations
         // 
         this.listConfigurations.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.listConfigurations.CheckBoxes = true;
         this.listConfigurations.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnType,
            this.columnOS,
            this.columnDatabaseEngine,
            this.columnSnapshot,
            this.columnStatus});
         this.listConfigurations.FullRowSelect = true;
         this.listConfigurations.Location = new System.Drawing.Point(9, 13);
         this.listConfigurations.Name = "listConfigurations";
         this.listConfigurations.ShowItemToolTips = true;
         this.listConfigurations.Size = new System.Drawing.Size(900, 348);
         this.listConfigurations.TabIndex = 0;
         this.listConfigurations.UseCompatibleStateImageBehavior = false;
         this.listConfigurations.View = System.Windows.Forms.View.Details;
         this.listConfigurations.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.listConfigurations_ItemCheck);
         // 
         // columnType
         // 
         this.columnType.Text = "Type";
         this.columnType.Width = 130;
         // 
         // columnOS
         // 
         this.columnOS.Text = "Operating system";
         this.columnOS.Width = 150;
         // 
         // columnDatabaseEngine
         // 
         this.columnDatabaseEngine.Text = "Database engine";
         this.columnDatabaseEngine.Width = 130;
         // 
         // columnSnapshot
         // 
         this.columnSnapshot.Text = "Snapshot";
         this.columnSnapshot.Width = 180;
         // 
         // columnStatus
         // 
         this.columnStatus.Text = "Status";
         this.columnStatus.Width = 180;
         // 
         // buttonRunSelected
         // 
         this.buttonRunSelected.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonRunSelected.Location = new System.Drawing.Point(589, 536);
         this.buttonRunSelected.Name = "buttonRunSelected";
         this.buttonRunSelected.Size = new System.Drawing.Size(100, 25);
         this.buttonRunSelected.TabIndex = 1;
         this.buttonRunSelected.Text = "Run";
         this.buttonRunSelected.UseVisualStyleBackColor = true;
         this.buttonRunSelected.Click += new System.EventHandler(this.buttonRunSelected_Click);
         // 
         // buttonClose
         // 
         this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonClose.Location = new System.Drawing.Point(809, 536);
         this.buttonClose.Name = "buttonClose";
         this.buttonClose.Size = new System.Drawing.Size(100, 25);
         this.buttonClose.TabIndex = 2;
         this.buttonClose.Text = "&Close";
         this.buttonClose.UseVisualStyleBackColor = true;
         this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
         // 
         // checkStopOnError
         // 
         this.checkStopOnError.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkStopOnError.AutoSize = true;
         this.checkStopOnError.Checked = true;
         this.checkStopOnError.CheckState = System.Windows.Forms.CheckState.Checked;
         this.checkStopOnError.Location = new System.Drawing.Point(10, 541);
         this.checkStopOnError.Name = "checkStopOnError";
         this.checkStopOnError.Size = new System.Drawing.Size(87, 17);
         this.checkStopOnError.TabIndex = 3;
         this.checkStopOnError.Text = "Stop on error";
         this.checkStopOnError.UseVisualStyleBackColor = true;
         this.checkStopOnError.CheckedChanged += new System.EventHandler(this.checkStopOnError_CheckedChanged);
         // 
         // buttonSelectAll
         // 
         this.buttonSelectAll.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.buttonSelectAll.Location = new System.Drawing.Point(288, 536);
         this.buttonSelectAll.Name = "buttonSelectAll";
         this.buttonSelectAll.Size = new System.Drawing.Size(100, 25);
         this.buttonSelectAll.TabIndex = 4;
         this.buttonSelectAll.Text = "Select all";
         this.buttonSelectAll.UseVisualStyleBackColor = true;
         this.buttonSelectAll.Click += new System.EventHandler(this.buttonSelectAll_Click);
         // 
         // buttonSelectNone
         // 
         this.buttonSelectNone.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.buttonSelectNone.Location = new System.Drawing.Point(394, 536);
         this.buttonSelectNone.Name = "buttonSelectNone";
         this.buttonSelectNone.Size = new System.Drawing.Size(100, 25);
         this.buttonSelectNone.TabIndex = 5;
         this.buttonSelectNone.Text = "Select none";
         this.buttonSelectNone.UseVisualStyleBackColor = true;
         this.buttonSelectNone.Click += new System.EventHandler(this.buttonSelectNone_Click);
         // 
         // checkShutdownOnSuccess
         // 
         this.checkShutdownOnSuccess.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkShutdownOnSuccess.AutoSize = true;
         this.checkShutdownOnSuccess.Location = new System.Drawing.Point(118, 541);
         this.checkShutdownOnSuccess.Name = "checkShutdownOnSuccess";
         this.checkShutdownOnSuccess.Size = new System.Drawing.Size(124, 17);
         this.checkShutdownOnSuccess.TabIndex = 6;
         this.checkShutdownOnSuccess.Text = "Shutdown if success";
         this.checkShutdownOnSuccess.UseVisualStyleBackColor = true;
         // 
         // listStatus
         // 
         this.listStatus.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.listStatus.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
         this.listStatus.FullRowSelect = true;
         this.listStatus.Location = new System.Drawing.Point(9, 369);
         this.listStatus.Name = "listStatus";
         this.listStatus.Size = new System.Drawing.Size(900, 161);
         this.listStatus.TabIndex = 7;
         this.listStatus.UseCompatibleStateImageBehavior = false;
         this.listStatus.View = System.Windows.Forms.View.Details;
         // 
         // columnHeader1
         // 
         this.columnHeader1.Text = "Item";
         this.columnHeader1.Width = 200;
         // 
         // columnHeader2
         // 
         this.columnHeader2.Text = "Description";
         this.columnHeader2.Width = 600;
         // 
         // timer
         // 
         this.timer.Enabled = true;
         this.timer.Interval = 1000;
         this.timer.Tick += new System.EventHandler(this.timer_Tick);
         // 
         // buttonStop
         // 
         this.buttonStop.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonStop.Enabled = false;
         this.buttonStop.Location = new System.Drawing.Point(703, 536);
         this.buttonStop.Name = "buttonStop";
         this.buttonStop.Size = new System.Drawing.Size(100, 25);
         this.buttonStop.TabIndex = 8;
         this.buttonStop.Text = "&Stop";
         this.buttonStop.UseVisualStyleBackColor = true;
         this.buttonStop.Click += new System.EventHandler(this.buttonStop_Click);
         // 
         // formConfigurations
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(917, 573);
         this.Controls.Add(this.buttonStop);
         this.Controls.Add(this.listStatus);
         this.Controls.Add(this.checkShutdownOnSuccess);
         this.Controls.Add(this.buttonSelectNone);
         this.Controls.Add(this.buttonSelectAll);
         this.Controls.Add(this.checkStopOnError);
         this.Controls.Add(this.buttonClose);
         this.Controls.Add(this.buttonRunSelected);
         this.Controls.Add(this.listConfigurations);
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.MinimumSize = new System.Drawing.Size(925, 600);
         this.Name = "formConfigurations";
         this.Text = "Test";
         this.Load += new System.EventHandler(this.formConfigurations_Load);
         this.Shown += new System.EventHandler(this.formConfigurations_Shown);
         this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.formConfigurations_FormClosing);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.ListView listConfigurations;
      private System.Windows.Forms.ColumnHeader columnSnapshot;
      private System.Windows.Forms.Button buttonRunSelected;
      private System.Windows.Forms.Button buttonClose;
      private System.Windows.Forms.ColumnHeader columnStatus;
      private System.Windows.Forms.CheckBox checkStopOnError;
      private System.Windows.Forms.Button buttonSelectAll;
      private System.Windows.Forms.Button buttonSelectNone;
      private System.Windows.Forms.ColumnHeader columnOS;
      private System.Windows.Forms.ColumnHeader columnType;
      private System.Windows.Forms.ColumnHeader columnDatabaseEngine;
      private System.Windows.Forms.CheckBox checkShutdownOnSuccess;
      private System.Windows.Forms.ListView listStatus;
      private System.Windows.Forms.ColumnHeader columnHeader1;
      private System.Windows.Forms.ColumnHeader columnHeader2;
      private System.Windows.Forms.Timer timer;
      private System.Windows.Forms.Button buttonStop;
   }
}