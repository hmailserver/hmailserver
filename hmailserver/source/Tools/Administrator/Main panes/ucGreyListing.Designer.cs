namespace hMailServer.Administrator
{
    partial class ucGreyListing
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
           this.tabControl = new System.Windows.Forms.TabControl();
           this.tabPage1 = new System.Windows.Forms.TabPage();
           this.checkBypassGreylistingOnSPFSuccess = new hMailServer.Administrator.Controls.ucCheckbox();
           this.labelDaysBeforeRemovingUsed = new System.Windows.Forms.Label();
           this.textGreyListingFinalDelete = new hMailServer.Shared.ucText();
           this.labelDaysBeforeRemovingUnused = new System.Windows.Forms.Label();
           this.textGreyListingInitialDelete = new hMailServer.Shared.ucText();
           this.labelMinutesToDefer = new System.Windows.Forms.Label();
           this.textGreyListingInitialDelay = new hMailServer.Shared.ucText();
           this.checkEnable = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabPage2 = new System.Windows.Forms.TabPage();
           this.buttonEditWhiteList = new System.Windows.Forms.Button();
           this.buttonDeleteWhiteList = new System.Windows.Forms.Button();
           this.buttonAddWhiteList = new System.Windows.Forms.Button();
           this.listWhitelistingRecords = new hMailServer.Administrator.ucListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
           this.checkBypassGreyListingOnMailFromMX = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabControl.SuspendLayout();
           this.tabPage1.SuspendLayout();
           this.tabPage2.SuspendLayout();
           this.SuspendLayout();
           // 
           // tabControl
           // 
           this.tabControl.Controls.Add(this.tabPage1);
           this.tabControl.Controls.Add(this.tabPage2);
           this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
           this.tabControl.Location = new System.Drawing.Point(0, 0);
           this.tabControl.Name = "tabControl";
           this.tabControl.SelectedIndex = 0;
           this.tabControl.Size = new System.Drawing.Size(464, 356);
           this.tabControl.TabIndex = 1;
           // 
           // tabPage1
           // 
           this.tabPage1.Controls.Add(this.checkBypassGreyListingOnMailFromMX);
           this.tabPage1.Controls.Add(this.checkBypassGreylistingOnSPFSuccess);
           this.tabPage1.Controls.Add(this.labelDaysBeforeRemovingUsed);
           this.tabPage1.Controls.Add(this.textGreyListingFinalDelete);
           this.tabPage1.Controls.Add(this.labelDaysBeforeRemovingUnused);
           this.tabPage1.Controls.Add(this.textGreyListingInitialDelete);
           this.tabPage1.Controls.Add(this.labelMinutesToDefer);
           this.tabPage1.Controls.Add(this.textGreyListingInitialDelay);
           this.tabPage1.Controls.Add(this.checkEnable);
           this.tabPage1.Location = new System.Drawing.Point(4, 22);
           this.tabPage1.Name = "tabPage1";
           this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
           this.tabPage1.Size = new System.Drawing.Size(456, 330);
           this.tabPage1.TabIndex = 0;
           this.tabPage1.Text = "General";
           this.tabPage1.UseVisualStyleBackColor = true;
           // 
           // checkBypassGreylistingOnSPFSuccess
           // 
           this.checkBypassGreylistingOnSPFSuccess.AutoSize = true;
           this.checkBypassGreylistingOnSPFSuccess.Location = new System.Drawing.Point(16, 186);
           this.checkBypassGreylistingOnSPFSuccess.Name = "checkBypassGreylistingOnSPFSuccess";
           this.checkBypassGreylistingOnSPFSuccess.Size = new System.Drawing.Size(175, 17);
           this.checkBypassGreylistingOnSPFSuccess.TabIndex = 36;
           this.checkBypassGreylistingOnSPFSuccess.Text = "Bypass Greylisting on SPF Pass";
           this.checkBypassGreylistingOnSPFSuccess.UseVisualStyleBackColor = true;
           // 
           // labelDaysBeforeRemovingUsed
           // 
           this.labelDaysBeforeRemovingUsed.AutoSize = true;
           this.labelDaysBeforeRemovingUsed.Location = new System.Drawing.Point(32, 131);
           this.labelDaysBeforeRemovingUsed.Name = "labelDaysBeforeRemovingUsed";
           this.labelDaysBeforeRemovingUsed.Size = new System.Drawing.Size(174, 13);
           this.labelDaysBeforeRemovingUsed.TabIndex = 35;
           this.labelDaysBeforeRemovingUsed.Text = "Days before removing used records";
           // 
           // textGreyListingFinalDelete
           // 
           this.textGreyListingFinalDelete.Location = new System.Drawing.Point(35, 147);
           this.textGreyListingFinalDelete.Name = "textGreyListingFinalDelete";
           this.textGreyListingFinalDelete.Number = 0;
           this.textGreyListingFinalDelete.Numeric = true;
           this.textGreyListingFinalDelete.Size = new System.Drawing.Size(90, 20);
           this.textGreyListingFinalDelete.TabIndex = 34;
           // 
           // labelDaysBeforeRemovingUnused
           // 
           this.labelDaysBeforeRemovingUnused.AutoSize = true;
           this.labelDaysBeforeRemovingUnused.Location = new System.Drawing.Point(32, 85);
           this.labelDaysBeforeRemovingUnused.Name = "labelDaysBeforeRemovingUnused";
           this.labelDaysBeforeRemovingUnused.Size = new System.Drawing.Size(186, 13);
           this.labelDaysBeforeRemovingUnused.TabIndex = 33;
           this.labelDaysBeforeRemovingUnused.Text = "Days before removing unused records";
           // 
           // textGreyListingInitialDelete
           // 
           this.textGreyListingInitialDelete.Location = new System.Drawing.Point(35, 101);
           this.textGreyListingInitialDelete.Name = "textGreyListingInitialDelete";
           this.textGreyListingInitialDelete.Number = 0;
           this.textGreyListingInitialDelete.Numeric = true;
           this.textGreyListingInitialDelete.Size = new System.Drawing.Size(90, 20);
           this.textGreyListingInitialDelete.TabIndex = 32;
           // 
           // labelMinutesToDefer
           // 
           this.labelMinutesToDefer.AutoSize = true;
           this.labelMinutesToDefer.Location = new System.Drawing.Point(32, 40);
           this.labelMinutesToDefer.Name = "labelMinutesToDefer";
           this.labelMinutesToDefer.Size = new System.Drawing.Size(165, 13);
           this.labelMinutesToDefer.TabIndex = 31;
           this.labelMinutesToDefer.Text = "Minutes to defer delivery attempts";
           // 
           // textGreyListingInitialDelay
           // 
           this.textGreyListingInitialDelay.Location = new System.Drawing.Point(35, 56);
           this.textGreyListingInitialDelay.Name = "textGreyListingInitialDelay";
           this.textGreyListingInitialDelay.Number = 0;
           this.textGreyListingInitialDelay.Numeric = true;
           this.textGreyListingInitialDelay.Size = new System.Drawing.Size(90, 20);
           this.textGreyListingInitialDelay.TabIndex = 30;
           // 
           // checkEnable
           // 
           this.checkEnable.AutoSize = true;
           this.checkEnable.Location = new System.Drawing.Point(16, 16);
           this.checkEnable.Name = "checkEnable";
           this.checkEnable.Size = new System.Drawing.Size(59, 17);
           this.checkEnable.TabIndex = 29;
           this.checkEnable.Text = "Enable";
           this.checkEnable.UseVisualStyleBackColor = true;
           this.checkEnable.CheckedChanged += new System.EventHandler(this.checkEnable_CheckedChanged);
           // 
           // tabPage2
           // 
           this.tabPage2.Controls.Add(this.buttonEditWhiteList);
           this.tabPage2.Controls.Add(this.buttonDeleteWhiteList);
           this.tabPage2.Controls.Add(this.buttonAddWhiteList);
           this.tabPage2.Controls.Add(this.listWhitelistingRecords);
           this.tabPage2.Location = new System.Drawing.Point(4, 22);
           this.tabPage2.Name = "tabPage2";
           this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
           this.tabPage2.Size = new System.Drawing.Size(456, 330);
           this.tabPage2.TabIndex = 1;
           this.tabPage2.Text = "White listing";
           this.tabPage2.UseVisualStyleBackColor = true;
           // 
           // buttonEditWhiteList
           // 
           this.buttonEditWhiteList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonEditWhiteList.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonEditWhiteList.Location = new System.Drawing.Point(352, 47);
           this.buttonEditWhiteList.Name = "buttonEditWhiteList";
           this.buttonEditWhiteList.Size = new System.Drawing.Size(100, 25);
           this.buttonEditWhiteList.TabIndex = 38;
           this.buttonEditWhiteList.Text = "&Edit...";
           this.buttonEditWhiteList.UseVisualStyleBackColor = true;
           this.buttonEditWhiteList.Click += new System.EventHandler(this.buttonEditWhiteList_Click);
           // 
           // buttonDeleteWhiteList
           // 
           this.buttonDeleteWhiteList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonDeleteWhiteList.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonDeleteWhiteList.Location = new System.Drawing.Point(352, 78);
           this.buttonDeleteWhiteList.Name = "buttonDeleteWhiteList";
           this.buttonDeleteWhiteList.Size = new System.Drawing.Size(100, 25);
           this.buttonDeleteWhiteList.TabIndex = 37;
           this.buttonDeleteWhiteList.Text = "Remove";
           this.buttonDeleteWhiteList.UseVisualStyleBackColor = true;
           this.buttonDeleteWhiteList.Click += new System.EventHandler(this.buttonDeleteWhiteList_Click);
           // 
           // buttonAddWhiteList
           // 
           this.buttonAddWhiteList.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonAddWhiteList.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonAddWhiteList.Location = new System.Drawing.Point(352, 16);
           this.buttonAddWhiteList.Name = "buttonAddWhiteList";
           this.buttonAddWhiteList.Size = new System.Drawing.Size(100, 25);
           this.buttonAddWhiteList.TabIndex = 36;
           this.buttonAddWhiteList.Text = "&Add...";
           this.buttonAddWhiteList.UseVisualStyleBackColor = true;
           this.buttonAddWhiteList.Click += new System.EventHandler(this.buttonAddWhiteList_Click);
           // 
           // listWhitelistingRecords
           // 
           this.listWhitelistingRecords.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listWhitelistingRecords.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
           this.listWhitelistingRecords.FullRowSelect = true;
           this.listWhitelistingRecords.Location = new System.Drawing.Point(15, 12);
           this.listWhitelistingRecords.Name = "listWhitelistingRecords";
           this.listWhitelistingRecords.Size = new System.Drawing.Size(329, 301);
           this.listWhitelistingRecords.TabIndex = 0;
           this.listWhitelistingRecords.UseCompatibleStateImageBehavior = false;
           this.listWhitelistingRecords.View = System.Windows.Forms.View.Details;
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "IP address";
           this.columnHeader1.Tag = "IPAddress";
           this.columnHeader1.Width = 100;
           // 
           // columnHeader2
           // 
           this.columnHeader2.Text = "Description";
           this.columnHeader2.Width = 210;
           // 
           // checkBypassGreyListingOnMailFromMX
           // 
           this.checkBypassGreyListingOnMailFromMX.AutoSize = true;
           this.checkBypassGreyListingOnMailFromMX.Location = new System.Drawing.Point(16, 209);
           this.checkBypassGreyListingOnMailFromMX.Name = "checkBypassGreyListingOnMailFromMX";
           this.checkBypassGreyListingOnMailFromMX.Size = new System.Drawing.Size(319, 17);
           this.checkBypassGreyListingOnMailFromMX.TabIndex = 37;
           this.checkBypassGreyListingOnMailFromMX.Text = "Bypass Greylisting when message arrives from A or MX record.";
           this.checkBypassGreyListingOnMailFromMX.UseVisualStyleBackColor = true;
           // 
           // ucGreyListing
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.tabControl);
           this.Name = "ucGreyListing";
           this.Size = new System.Drawing.Size(464, 356);
           this.tabControl.ResumeLayout(false);
           this.tabPage1.ResumeLayout(false);
           this.tabPage1.PerformLayout();
           this.tabPage2.ResumeLayout(false);
           this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.TabControl tabControl;
       private System.Windows.Forms.TabPage tabPage1;
       private System.Windows.Forms.Label labelDaysBeforeRemovingUsed;
       private hMailServer.Shared.ucText textGreyListingFinalDelete;
       private System.Windows.Forms.Label labelDaysBeforeRemovingUnused;
       private hMailServer.Shared.ucText textGreyListingInitialDelete;
       private System.Windows.Forms.Label labelMinutesToDefer;
       private hMailServer.Shared.ucText textGreyListingInitialDelay;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnable;
       private System.Windows.Forms.TabPage tabPage2;
       private ucListView listWhitelistingRecords;
       private System.Windows.Forms.Button buttonEditWhiteList;
       private System.Windows.Forms.Button buttonDeleteWhiteList;
       private System.Windows.Forms.Button buttonAddWhiteList;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.ColumnHeader columnHeader2;
       private hMailServer.Administrator.Controls.ucCheckbox checkBypassGreylistingOnSPFSuccess;
       private hMailServer.Administrator.Controls.ucCheckbox checkBypassGreyListingOnMailFromMX;
    }
}
