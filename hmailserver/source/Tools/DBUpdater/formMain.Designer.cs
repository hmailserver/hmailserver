namespace DBUpdater
{
   partial class formMain
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formMain));
         this.labelCurrentVersionLabel = new System.Windows.Forms.Label();
         this.labelRequiredDatabaseVersionLabel = new System.Windows.Forms.Label();
         this.labelRequiredDatabaseVersion = new System.Windows.Forms.Label();
         this.labelCurrentDatabaseVersion = new System.Windows.Forms.Label();
         this.listRequiredUpgrades = new System.Windows.Forms.ListView();
         this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
         this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
         this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
         this.buttonUpgrade = new System.Windows.Forms.Button();
         this.buttonClose = new System.Windows.Forms.Button();
         this.labelUpgradeSteps = new System.Windows.Forms.Label();
         this.labelRunBackup = new System.Windows.Forms.Label();
         this.SuspendLayout();
         // 
         // labelCurrentVersionLabel
         // 
         this.labelCurrentVersionLabel.AutoSize = true;
         this.labelCurrentVersionLabel.Location = new System.Drawing.Point(8, 8);
         this.labelCurrentVersionLabel.Name = "labelCurrentVersionLabel";
         this.labelCurrentVersionLabel.Size = new System.Drawing.Size(125, 13);
         this.labelCurrentVersionLabel.TabIndex = 0;
         this.labelCurrentVersionLabel.Text = "Current database version";
         // 
         // labelRequiredDatabaseVersionLabel
         // 
         this.labelRequiredDatabaseVersionLabel.AutoSize = true;
         this.labelRequiredDatabaseVersionLabel.Location = new System.Drawing.Point(8, 32);
         this.labelRequiredDatabaseVersionLabel.Name = "labelRequiredDatabaseVersionLabel";
         this.labelRequiredDatabaseVersionLabel.Size = new System.Drawing.Size(134, 13);
         this.labelRequiredDatabaseVersionLabel.TabIndex = 1;
         this.labelRequiredDatabaseVersionLabel.Text = "Required database version";
         // 
         // labelRequiredDatabaseVersion
         // 
         this.labelRequiredDatabaseVersion.AutoSize = true;
         this.labelRequiredDatabaseVersion.Location = new System.Drawing.Point(160, 32);
         this.labelRequiredDatabaseVersion.Name = "labelRequiredDatabaseVersion";
         this.labelRequiredDatabaseVersion.Size = new System.Drawing.Size(10, 13);
         this.labelRequiredDatabaseVersion.TabIndex = 3;
         this.labelRequiredDatabaseVersion.Text = "-";
         // 
         // labelCurrentDatabaseVersion
         // 
         this.labelCurrentDatabaseVersion.AutoSize = true;
         this.labelCurrentDatabaseVersion.Location = new System.Drawing.Point(160, 8);
         this.labelCurrentDatabaseVersion.Name = "labelCurrentDatabaseVersion";
         this.labelCurrentDatabaseVersion.Size = new System.Drawing.Size(10, 13);
         this.labelCurrentDatabaseVersion.TabIndex = 2;
         this.labelCurrentDatabaseVersion.Text = "-";
         // 
         // listRequiredUpgrades
         // 
         this.listRequiredUpgrades.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.listRequiredUpgrades.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3});
         this.listRequiredUpgrades.FullRowSelect = true;
         this.listRequiredUpgrades.Location = new System.Drawing.Point(11, 82);
         this.listRequiredUpgrades.Name = "listRequiredUpgrades";
         this.listRequiredUpgrades.Size = new System.Drawing.Size(452, 179);
         this.listRequiredUpgrades.TabIndex = 4;
         this.listRequiredUpgrades.UseCompatibleStateImageBehavior = false;
         this.listRequiredUpgrades.View = System.Windows.Forms.View.Details;
         this.listRequiredUpgrades.DoubleClick += new System.EventHandler(this.listRequiredUpgrades_DoubleClick);
         // 
         // columnHeader1
         // 
         this.columnHeader1.Text = "From";
         this.columnHeader1.Width = 150;
         // 
         // columnHeader2
         // 
         this.columnHeader2.Text = "To";
         this.columnHeader2.Width = 150;
         // 
         // columnHeader3
         // 
         this.columnHeader3.Text = "Status";
         this.columnHeader3.Width = 120;
         // 
         // buttonUpgrade
         // 
         this.buttonUpgrade.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonUpgrade.Location = new System.Drawing.Point(248, 318);
         this.buttonUpgrade.Name = "buttonUpgrade";
         this.buttonUpgrade.Size = new System.Drawing.Size(100, 25);
         this.buttonUpgrade.TabIndex = 5;
         this.buttonUpgrade.Text = "&Upgrade";
         this.buttonUpgrade.UseVisualStyleBackColor = true;
         this.buttonUpgrade.Click += new System.EventHandler(this.buttonUpgrade_Click);
         // 
         // buttonClose
         // 
         this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonClose.Location = new System.Drawing.Point(360, 318);
         this.buttonClose.Name = "buttonClose";
         this.buttonClose.Size = new System.Drawing.Size(100, 25);
         this.buttonClose.TabIndex = 6;
         this.buttonClose.Text = "&Close";
         this.buttonClose.UseVisualStyleBackColor = true;
         this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
         // 
         // labelUpgradeSteps
         // 
         this.labelUpgradeSteps.AutoSize = true;
         this.labelUpgradeSteps.Location = new System.Drawing.Point(8, 56);
         this.labelUpgradeSteps.Name = "labelUpgradeSteps";
         this.labelUpgradeSteps.Size = new System.Drawing.Size(120, 13);
         this.labelUpgradeSteps.TabIndex = 7;
         this.labelUpgradeSteps.Text = "Required upgrade steps";
         // 
         // labelRunBackup
         // 
         this.labelRunBackup.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.labelRunBackup.ForeColor = System.Drawing.SystemColors.ControlText;
         this.labelRunBackup.Location = new System.Drawing.Point(8, 268);
         this.labelRunBackup.Name = "labelRunBackup";
         this.labelRunBackup.Size = new System.Drawing.Size(452, 33);
         this.labelRunBackup.TabIndex = 8;
         this.labelRunBackup.Text = "Before performing the database upgrade, you should take a backup of your database" +
             ". If the upgrade fails, restore the database.";
         // 
         // formMain
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(475, 351);
         this.Controls.Add(this.labelRunBackup);
         this.Controls.Add(this.labelUpgradeSteps);
         this.Controls.Add(this.buttonClose);
         this.Controls.Add(this.buttonUpgrade);
         this.Controls.Add(this.listRequiredUpgrades);
         this.Controls.Add(this.labelRequiredDatabaseVersion);
         this.Controls.Add(this.labelCurrentDatabaseVersion);
         this.Controls.Add(this.labelRequiredDatabaseVersionLabel);
         this.Controls.Add(this.labelCurrentVersionLabel);
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.MinimumSize = new System.Drawing.Size(431, 278);
         this.Name = "formMain";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
         this.Text = "hMailServer Database Upgrade";
         this.Shown += new System.EventHandler(this.formMain_Shown);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      public System.Windows.Forms.Label labelCurrentVersionLabel;
      public System.Windows.Forms.Label labelRequiredDatabaseVersionLabel;
      public System.Windows.Forms.Label labelRequiredDatabaseVersion;
      public System.Windows.Forms.Label labelCurrentDatabaseVersion;
      private System.Windows.Forms.ListView listRequiredUpgrades;
      private System.Windows.Forms.ColumnHeader columnHeader1;
      private System.Windows.Forms.ColumnHeader columnHeader2;
      private System.Windows.Forms.ColumnHeader columnHeader3;
      private System.Windows.Forms.Button buttonUpgrade;
      private System.Windows.Forms.Button buttonClose;
      private System.Windows.Forms.Label labelUpgradeSteps;
      private System.Windows.Forms.Label labelRunBackup;
   }
}

