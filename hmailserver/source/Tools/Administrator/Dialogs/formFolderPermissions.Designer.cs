namespace hMailServer.Administrator.Dialogs
{
   partial class formFolderPermissions
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
         this.labelPermissions = new System.Windows.Forms.Label();
         this.buttonDeletePermission = new System.Windows.Forms.Button();
         this.buttonAddPermission = new System.Windows.Forms.Button();
         this.labelPermissionsFor = new System.Windows.Forms.Label();
         this.buttonSelectAll = new System.Windows.Forms.Button();
         this.buttonSelectNone = new System.Windows.Forms.Button();
         this.buttonClose = new System.Windows.Forms.Button();
         this.listACL = new hMailServer.Administrator.ucListView();
         this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
         this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
         this.checkPermissionKeepOtherFlags = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkPermissionKeepDeletedFlag = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkPermissionKeepSeenFlag = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkPermissionExpunge = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkPermissionInsert = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkPermissionAdminister = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkPermissionDeleteMailbox = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkPermissionCreateMailbox = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkPermissionRead = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkPermissionLookup = new hMailServer.Administrator.Controls.ucCheckbox();
         this.SuspendLayout();
         // 
         // labelPermissions
         // 
         this.labelPermissions.AutoSize = true;
         this.labelPermissions.Location = new System.Drawing.Point(16, 8);
         this.labelPermissions.Name = "labelPermissions";
         this.labelPermissions.Size = new System.Drawing.Size(62, 13);
         this.labelPermissions.TabIndex = 0;
         this.labelPermissions.Text = "Permissions";
         // 
         // buttonDeletePermission
         // 
         this.buttonDeletePermission.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonDeletePermission.Enabled = false;
         this.buttonDeletePermission.Location = new System.Drawing.Point(458, 51);
         this.buttonDeletePermission.Name = "buttonDeletePermission";
         this.buttonDeletePermission.Size = new System.Drawing.Size(89, 25);
         this.buttonDeletePermission.TabIndex = 43;
         this.buttonDeletePermission.Text = "Remove";
         this.buttonDeletePermission.UseVisualStyleBackColor = true;
         this.buttonDeletePermission.Click += new System.EventHandler(this.buttonDeleteWhiteList_Click);
         // 
         // buttonAddPermission
         // 
         this.buttonAddPermission.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonAddPermission.Location = new System.Drawing.Point(458, 20);
         this.buttonAddPermission.Name = "buttonAddPermission";
         this.buttonAddPermission.Size = new System.Drawing.Size(89, 25);
         this.buttonAddPermission.TabIndex = 42;
         this.buttonAddPermission.Text = "&Add...";
         this.buttonAddPermission.UseVisualStyleBackColor = true;
         this.buttonAddPermission.Click += new System.EventHandler(this.buttonAddPermission_Click);
         // 
         // labelPermissionsFor
         // 
         this.labelPermissionsFor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.labelPermissionsFor.AutoSize = true;
         this.labelPermissionsFor.Location = new System.Drawing.Point(16, 176);
         this.labelPermissionsFor.Name = "labelPermissionsFor";
         this.labelPermissionsFor.Size = new System.Drawing.Size(93, 13);
         this.labelPermissionsFor.TabIndex = 44;
         this.labelPermissionsFor.Text = "Permissions for %s";
         // 
         // buttonSelectAll
         // 
         this.buttonSelectAll.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.buttonSelectAll.Location = new System.Drawing.Point(12, 310);
         this.buttonSelectAll.Name = "buttonSelectAll";
         this.buttonSelectAll.Size = new System.Drawing.Size(89, 25);
         this.buttonSelectAll.TabIndex = 55;
         this.buttonSelectAll.Text = "Select all";
         this.buttonSelectAll.UseVisualStyleBackColor = true;
         this.buttonSelectAll.Click += new System.EventHandler(this.buttonSelectAll_Click);
         // 
         // buttonSelectNone
         // 
         this.buttonSelectNone.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.buttonSelectNone.Location = new System.Drawing.Point(107, 310);
         this.buttonSelectNone.Name = "buttonSelectNone";
         this.buttonSelectNone.Size = new System.Drawing.Size(89, 25);
         this.buttonSelectNone.TabIndex = 56;
         this.buttonSelectNone.Text = "Select none";
         this.buttonSelectNone.UseVisualStyleBackColor = true;
         this.buttonSelectNone.Click += new System.EventHandler(this.buttonSelectNone_Click);
         // 
         // buttonClose
         // 
         this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonClose.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.buttonClose.Location = new System.Drawing.Point(361, 310);
         this.buttonClose.Name = "buttonClose";
         this.buttonClose.Size = new System.Drawing.Size(89, 25);
         this.buttonClose.TabIndex = 57;
         this.buttonClose.Text = "&Close";
         this.buttonClose.UseVisualStyleBackColor = true;
         this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
         // 
         // listACL
         // 
         this.listACL.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
         this.listACL.FullRowSelect = true;
         this.listACL.Location = new System.Drawing.Point(24, 24);
         this.listACL.Name = "listACL";
         this.listACL.Size = new System.Drawing.Size(422, 139);
         this.listACL.TabIndex = 58;
         this.listACL.UseCompatibleStateImageBehavior = false;
         this.listACL.View = System.Windows.Forms.View.Details;
         this.listACL.SelectedIndexChanged += new System.EventHandler(this.listACL_SelectedIndexChanged);
         this.listACL.BeforeSelectedIndexChanged += new hMailServer.Administrator.ucListView.BeforeSelectedIndexChangedHandler(this.listACL_BeforeSelectedIndexChanged);
         // 
         // columnHeader1
         // 
         this.columnHeader1.Text = "Type";
         this.columnHeader1.Width = 100;
         // 
         // columnHeader2
         // 
         this.columnHeader2.Text = "Object";
         this.columnHeader2.Width = 200;
         // 
         // checkPermissionKeepOtherFlags
         // 
         this.checkPermissionKeepOtherFlags.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkPermissionKeepOtherFlags.AutoSize = true;
         this.checkPermissionKeepOtherFlags.Location = new System.Drawing.Point(352, 248);
         this.checkPermissionKeepOtherFlags.Name = "checkPermissionKeepOtherFlags";
         this.checkPermissionKeepOtherFlags.Size = new System.Drawing.Size(103, 17);
         this.checkPermissionKeepOtherFlags.TabIndex = 54;
         this.checkPermissionKeepOtherFlags.Text = "Keep other flags";
         this.checkPermissionKeepOtherFlags.UseVisualStyleBackColor = true;
         // 
         // checkPermissionKeepDeletedFlag
         // 
         this.checkPermissionKeepDeletedFlag.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkPermissionKeepDeletedFlag.AutoSize = true;
         this.checkPermissionKeepDeletedFlag.Location = new System.Drawing.Point(352, 225);
         this.checkPermissionKeepDeletedFlag.Name = "checkPermissionKeepDeletedFlag";
         this.checkPermissionKeepDeletedFlag.Size = new System.Drawing.Size(109, 17);
         this.checkPermissionKeepDeletedFlag.TabIndex = 53;
         this.checkPermissionKeepDeletedFlag.Text = "Keep deleted flag";
         this.checkPermissionKeepDeletedFlag.UseVisualStyleBackColor = true;
         // 
         // checkPermissionKeepSeenFlag
         // 
         this.checkPermissionKeepSeenFlag.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkPermissionKeepSeenFlag.AutoSize = true;
         this.checkPermissionKeepSeenFlag.Location = new System.Drawing.Point(352, 202);
         this.checkPermissionKeepSeenFlag.Name = "checkPermissionKeepSeenFlag";
         this.checkPermissionKeepSeenFlag.Size = new System.Drawing.Size(97, 17);
         this.checkPermissionKeepSeenFlag.TabIndex = 52;
         this.checkPermissionKeepSeenFlag.Text = "Keep seen flag";
         this.checkPermissionKeepSeenFlag.UseVisualStyleBackColor = true;
         // 
         // checkPermissionExpunge
         // 
         this.checkPermissionExpunge.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkPermissionExpunge.AutoSize = true;
         this.checkPermissionExpunge.Location = new System.Drawing.Point(177, 248);
         this.checkPermissionExpunge.Name = "checkPermissionExpunge";
         this.checkPermissionExpunge.Size = new System.Drawing.Size(68, 17);
         this.checkPermissionExpunge.TabIndex = 51;
         this.checkPermissionExpunge.Text = "Expunge";
         this.checkPermissionExpunge.UseVisualStyleBackColor = true;
         // 
         // checkPermissionInsert
         // 
         this.checkPermissionInsert.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkPermissionInsert.AutoSize = true;
         this.checkPermissionInsert.Location = new System.Drawing.Point(177, 225);
         this.checkPermissionInsert.Name = "checkPermissionInsert";
         this.checkPermissionInsert.Size = new System.Drawing.Size(73, 17);
         this.checkPermissionInsert.TabIndex = 50;
         this.checkPermissionInsert.Text = "Insert mail";
         this.checkPermissionInsert.UseVisualStyleBackColor = true;
         // 
         // checkPermissionAdminister
         // 
         this.checkPermissionAdminister.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkPermissionAdminister.AutoSize = true;
         this.checkPermissionAdminister.Location = new System.Drawing.Point(177, 202);
         this.checkPermissionAdminister.Name = "checkPermissionAdminister";
         this.checkPermissionAdminister.Size = new System.Drawing.Size(74, 17);
         this.checkPermissionAdminister.TabIndex = 49;
         this.checkPermissionAdminister.Text = "Administer";
         this.checkPermissionAdminister.UseVisualStyleBackColor = true;
         // 
         // checkPermissionDeleteMailbox
         // 
         this.checkPermissionDeleteMailbox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkPermissionDeleteMailbox.AutoSize = true;
         this.checkPermissionDeleteMailbox.Location = new System.Drawing.Point(24, 271);
         this.checkPermissionDeleteMailbox.Name = "checkPermissionDeleteMailbox";
         this.checkPermissionDeleteMailbox.Size = new System.Drawing.Size(95, 17);
         this.checkPermissionDeleteMailbox.TabIndex = 48;
         this.checkPermissionDeleteMailbox.Text = "Delete mailbox";
         this.checkPermissionDeleteMailbox.UseVisualStyleBackColor = true;
         // 
         // checkPermissionCreateMailbox
         // 
         this.checkPermissionCreateMailbox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkPermissionCreateMailbox.AutoSize = true;
         this.checkPermissionCreateMailbox.Location = new System.Drawing.Point(24, 248);
         this.checkPermissionCreateMailbox.Name = "checkPermissionCreateMailbox";
         this.checkPermissionCreateMailbox.Size = new System.Drawing.Size(95, 17);
         this.checkPermissionCreateMailbox.TabIndex = 47;
         this.checkPermissionCreateMailbox.Text = "Create mailbox";
         this.checkPermissionCreateMailbox.UseVisualStyleBackColor = true;
         // 
         // checkPermissionRead
         // 
         this.checkPermissionRead.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkPermissionRead.AutoSize = true;
         this.checkPermissionRead.Location = new System.Drawing.Point(24, 225);
         this.checkPermissionRead.Name = "checkPermissionRead";
         this.checkPermissionRead.Size = new System.Drawing.Size(52, 17);
         this.checkPermissionRead.TabIndex = 46;
         this.checkPermissionRead.Text = "Read";
         this.checkPermissionRead.UseVisualStyleBackColor = true;
         // 
         // checkPermissionLookup
         // 
         this.checkPermissionLookup.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkPermissionLookup.AutoSize = true;
         this.checkPermissionLookup.Location = new System.Drawing.Point(24, 202);
         this.checkPermissionLookup.Name = "checkPermissionLookup";
         this.checkPermissionLookup.Size = new System.Drawing.Size(62, 17);
         this.checkPermissionLookup.TabIndex = 45;
         this.checkPermissionLookup.Text = "Lookup";
         this.checkPermissionLookup.UseVisualStyleBackColor = true;
         // 
         // formFolderPermissions
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(553, 343);
         this.Controls.Add(this.listACL);
         this.Controls.Add(this.buttonClose);
         this.Controls.Add(this.buttonSelectNone);
         this.Controls.Add(this.buttonSelectAll);
         this.Controls.Add(this.checkPermissionKeepOtherFlags);
         this.Controls.Add(this.checkPermissionKeepDeletedFlag);
         this.Controls.Add(this.checkPermissionKeepSeenFlag);
         this.Controls.Add(this.checkPermissionExpunge);
         this.Controls.Add(this.checkPermissionInsert);
         this.Controls.Add(this.checkPermissionAdminister);
         this.Controls.Add(this.checkPermissionDeleteMailbox);
         this.Controls.Add(this.checkPermissionCreateMailbox);
         this.Controls.Add(this.checkPermissionRead);
         this.Controls.Add(this.checkPermissionLookup);
         this.Controls.Add(this.labelPermissionsFor);
         this.Controls.Add(this.buttonDeletePermission);
         this.Controls.Add(this.buttonAddPermission);
         this.Controls.Add(this.labelPermissions);
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
         this.MaximizeBox = false;
         this.MinimizeBox = false;
         this.Name = "formFolderPermissions";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "Permissions";
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Label labelPermissions;
      
      private System.Windows.Forms.Button buttonDeletePermission;
      private System.Windows.Forms.Button buttonAddPermission;
      private System.Windows.Forms.Label labelPermissionsFor;
      private hMailServer.Administrator.Controls.ucCheckbox checkPermissionLookup;
      private hMailServer.Administrator.Controls.ucCheckbox checkPermissionRead;
      private hMailServer.Administrator.Controls.ucCheckbox checkPermissionCreateMailbox;
      private hMailServer.Administrator.Controls.ucCheckbox checkPermissionDeleteMailbox;
      private hMailServer.Administrator.Controls.ucCheckbox checkPermissionAdminister;
      private hMailServer.Administrator.Controls.ucCheckbox checkPermissionInsert;
      private hMailServer.Administrator.Controls.ucCheckbox checkPermissionExpunge;
      private hMailServer.Administrator.Controls.ucCheckbox checkPermissionKeepSeenFlag;
      private hMailServer.Administrator.Controls.ucCheckbox checkPermissionKeepDeletedFlag;
      private hMailServer.Administrator.Controls.ucCheckbox checkPermissionKeepOtherFlags;
      private System.Windows.Forms.Button buttonSelectAll;
      private System.Windows.Forms.Button buttonSelectNone;
      private System.Windows.Forms.Button buttonClose;
      private ucListView listACL;
      private System.Windows.Forms.ColumnHeader columnHeader1;
      private System.Windows.Forms.ColumnHeader columnHeader2;
   }
}