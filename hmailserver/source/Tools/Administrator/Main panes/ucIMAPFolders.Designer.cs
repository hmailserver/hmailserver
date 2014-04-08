namespace hMailServer.Administrator
{
   partial class ucIMAPFolders
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
          this.treeFolders = new System.Windows.Forms.TreeView();
          this.contextMenuStrip = new System.Windows.Forms.ContextMenuStrip(this.components);
          this.createFolderToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
          this.deleteFolderToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
          this.groupBox = new System.Windows.Forms.GroupBox();
          this.buttonEmptyFolder = new System.Windows.Forms.Button();
          this.labelFolderActions = new System.Windows.Forms.Label();
          this.btnEditPermissions = new System.Windows.Forms.Button();
          this.labelName = new System.Windows.Forms.Label();
          this.labelFolderProperties = new System.Windows.Forms.Label();
          this.textName = new hMailServer.Shared.ucText();
          this.contextMenuStrip.SuspendLayout();
          this.groupBox.SuspendLayout();
          this.SuspendLayout();
          // 
          // treeFolders
          // 
          this.treeFolders.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                      | System.Windows.Forms.AnchorStyles.Left)));
          this.treeFolders.BackColor = System.Drawing.SystemColors.Window;
          this.treeFolders.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.treeFolders.Location = new System.Drawing.Point(0, 0);
          this.treeFolders.Name = "treeFolders";
          this.treeFolders.Size = new System.Drawing.Size(173, 296);
          this.treeFolders.TabIndex = 0;
          this.treeFolders.Tag = "";
          this.treeFolders.MouseUp += new System.Windows.Forms.MouseEventHandler(this.treeFolders_MouseUp);
          this.treeFolders.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeFolders_AfterSelect);
          this.treeFolders.BeforeSelect += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeFolders_BeforeSelect);
          // 
          // contextMenuStrip
          // 
          this.contextMenuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.createFolderToolStripMenuItem,
            this.deleteFolderToolStripMenuItem});
          this.contextMenuStrip.Name = "contextMenuStrip";
          this.contextMenuStrip.Size = new System.Drawing.Size(135, 48);
          this.contextMenuStrip.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip_Opening);
          // 
          // createFolderToolStripMenuItem
          // 
          this.createFolderToolStripMenuItem.Name = "createFolderToolStripMenuItem";
          this.createFolderToolStripMenuItem.Size = new System.Drawing.Size(134, 22);
          this.createFolderToolStripMenuItem.Text = "Create folder";
          this.createFolderToolStripMenuItem.Click += new System.EventHandler(this.createFolderToolStripMenuItem_Click);
          // 
          // deleteFolderToolStripMenuItem
          // 
          this.deleteFolderToolStripMenuItem.Name = "deleteFolderToolStripMenuItem";
          this.deleteFolderToolStripMenuItem.Size = new System.Drawing.Size(134, 22);
          this.deleteFolderToolStripMenuItem.Text = "Delete folder";
          this.deleteFolderToolStripMenuItem.Click += new System.EventHandler(this.deleteFolderToolStripMenuItem_Click);
          // 
          // groupBox
          // 
          this.groupBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                      | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.groupBox.Controls.Add(this.buttonEmptyFolder);
          this.groupBox.Controls.Add(this.labelFolderActions);
          this.groupBox.Controls.Add(this.btnEditPermissions);
          this.groupBox.Controls.Add(this.labelName);
          this.groupBox.Controls.Add(this.textName);
          this.groupBox.Controls.Add(this.labelFolderProperties);
          this.groupBox.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.groupBox.Location = new System.Drawing.Point(175, 0);
          this.groupBox.Name = "groupBox";
          this.groupBox.Size = new System.Drawing.Size(512, 296);
          this.groupBox.TabIndex = 1;
          this.groupBox.TabStop = false;
          // 
          // buttonEmptyFolder
          // 
          this.buttonEmptyFolder.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.buttonEmptyFolder.Location = new System.Drawing.Point(32, 176);
          this.buttonEmptyFolder.Name = "buttonEmptyFolder";
          this.buttonEmptyFolder.Size = new System.Drawing.Size(112, 25);
          this.buttonEmptyFolder.TabIndex = 29;
          this.buttonEmptyFolder.Text = "Empty folder";
          this.buttonEmptyFolder.UseVisualStyleBackColor = true;
          this.buttonEmptyFolder.Click += new System.EventHandler(this.buttonEmptyFolder_Click);
          // 
          // labelFolderActions
          // 
          this.labelFolderActions.AutoSize = true;
          this.labelFolderActions.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelFolderActions.Location = new System.Drawing.Point(16, 152);
          this.labelFolderActions.Name = "labelFolderActions";
          this.labelFolderActions.Size = new System.Drawing.Size(49, 13);
          this.labelFolderActions.TabIndex = 28;
          this.labelFolderActions.Text = "Actions";
          // 
          // btnEditPermissions
          // 
          this.btnEditPermissions.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.btnEditPermissions.Location = new System.Drawing.Point(32, 96);
          this.btnEditPermissions.Name = "btnEditPermissions";
          this.btnEditPermissions.Size = new System.Drawing.Size(142, 25);
          this.btnEditPermissions.TabIndex = 27;
          this.btnEditPermissions.Text = "Edit permissions...";
          this.btnEditPermissions.UseVisualStyleBackColor = true;
          this.btnEditPermissions.Click += new System.EventHandler(this.btnEditPermissions_Click);
          // 
          // labelName
          // 
          this.labelName.AutoSize = true;
          this.labelName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelName.Location = new System.Drawing.Point(32, 48);
          this.labelName.Name = "labelName";
          this.labelName.Size = new System.Drawing.Size(34, 13);
          this.labelName.TabIndex = 26;
          this.labelName.Text = "Name";
          // 
          // labelFolderProperties
          // 
          this.labelFolderProperties.AutoSize = true;
          this.labelFolderProperties.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelFolderProperties.Location = new System.Drawing.Point(16, 24);
          this.labelFolderProperties.Name = "labelFolderProperties";
          this.labelFolderProperties.Size = new System.Drawing.Size(66, 13);
          this.labelFolderProperties.TabIndex = 24;
          this.labelFolderProperties.Text = "Properties";
          // 
          // textName
          // 
          this.textName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.textName.Location = new System.Drawing.Point(32, 64);
          this.textName.Name = "textName";
          this.textName.Number = 0;
          this.textName.Numeric = false;
          this.textName.Size = new System.Drawing.Size(222, 21);
          this.textName.TabIndex = 25;
          this.textName.TextChanged += new System.EventHandler(this.textName_TextChanged);
          // 
          // ucIMAPFolders
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.Controls.Add(this.groupBox);
          this.Controls.Add(this.treeFolders);
          this.Name = "ucIMAPFolders";
          this.Size = new System.Drawing.Size(687, 296);
          this.contextMenuStrip.ResumeLayout(false);
          this.groupBox.ResumeLayout(false);
          this.groupBox.PerformLayout();
          this.ResumeLayout(false);

      }

      #endregion

      private System.Windows.Forms.TreeView treeFolders;
      private System.Windows.Forms.GroupBox groupBox;
      private System.Windows.Forms.Button btnEditPermissions;
      private System.Windows.Forms.Label labelName;
      private hMailServer.Shared.ucText textName;
      private System.Windows.Forms.Label labelFolderProperties;
      private System.Windows.Forms.Button buttonEmptyFolder;
      private System.Windows.Forms.Label labelFolderActions;
      private System.Windows.Forms.ContextMenuStrip contextMenuStrip;
      private System.Windows.Forms.ToolStripMenuItem deleteFolderToolStripMenuItem;
      private System.Windows.Forms.ToolStripMenuItem createFolderToolStripMenuItem;
   }
}
