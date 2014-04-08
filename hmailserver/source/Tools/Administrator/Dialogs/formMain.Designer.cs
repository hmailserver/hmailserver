namespace hMailServer.Administrator
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
           this.components = new System.ComponentModel.Container();
           System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formMain));
           this.buttonExit = new System.Windows.Forms.Button();
           this.groupBox1 = new System.Windows.Forms.GroupBox();
           this.splitContainer = new System.Windows.Forms.SplitContainer();
           this.treeNodes = new System.Windows.Forms.TreeView();
           this.imageList = new System.Windows.Forms.ImageList(this.components);
           this.buttonHelp = new System.Windows.Forms.Button();
           this.buttonSave = new System.Windows.Forms.Button();
           this.panelTopBar = new System.Windows.Forms.Panel();
           this.labelTopCaption = new System.Windows.Forms.Label();
           this.panelMain = new System.Windows.Forms.Panel();
           this.menuStrip = new System.Windows.Forms.MenuStrip();
           this.menuItemFile = new System.Windows.Forms.ToolStripMenuItem();
           this.menuItemSelectLanguage = new System.Windows.Forms.ToolStripMenuItem();
           this.menuItemConnect = new System.Windows.Forms.ToolStripMenuItem();
           this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
           this.menuItemExit = new System.Windows.Forms.ToolStripMenuItem();
           this.menuItemHelp = new System.Windows.Forms.ToolStripMenuItem();
           this.menuItemHelpIndex = new System.Windows.Forms.ToolStripMenuItem();
           this.menuItemHelpAbout = new System.Windows.Forms.ToolStripMenuItem();
           this.splitContainer.Panel1.SuspendLayout();
           this.splitContainer.Panel2.SuspendLayout();
           this.splitContainer.SuspendLayout();
           this.panelTopBar.SuspendLayout();
           this.menuStrip.SuspendLayout();
           this.SuspendLayout();
           // 
           // buttonExit
           // 
           this.buttonExit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonExit.Location = new System.Drawing.Point(691, 544);
           this.buttonExit.Name = "buttonExit";
           this.buttonExit.Size = new System.Drawing.Size(89, 25);
           this.buttonExit.TabIndex = 1;
           this.buttonExit.Text = "Exit";
           this.buttonExit.UseVisualStyleBackColor = true;
           this.buttonExit.Click += new System.EventHandler(this.buttonExit_Click);
           // 
           // groupBox1
           // 
           this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.groupBox1.Location = new System.Drawing.Point(4, 533);
           this.groupBox1.Name = "groupBox1";
           this.groupBox1.Size = new System.Drawing.Size(785, 5);
           this.groupBox1.TabIndex = 8;
           this.groupBox1.TabStop = false;
           // 
           // splitContainer
           // 
           this.splitContainer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.splitContainer.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
           this.splitContainer.Location = new System.Drawing.Point(2, 24);
           this.splitContainer.Name = "splitContainer";
           // 
           // splitContainer.Panel1
           // 
           this.splitContainer.Panel1.Controls.Add(this.treeNodes);
           // 
           // splitContainer.Panel2
           // 
           this.splitContainer.Panel2.Controls.Add(this.buttonHelp);
           this.splitContainer.Panel2.Controls.Add(this.buttonSave);
           this.splitContainer.Panel2.Controls.Add(this.panelTopBar);
           this.splitContainer.Panel2.Controls.Add(this.panelMain);
           this.splitContainer.Size = new System.Drawing.Size(786, 505);
           this.splitContainer.SplitterDistance = 282;
           this.splitContainer.TabIndex = 9;
           this.splitContainer.SplitterMoved += new System.Windows.Forms.SplitterEventHandler(this.splitContainer_SplitterMoved);
           // 
           // treeNodes
           // 
           this.treeNodes.BackColor = System.Drawing.SystemColors.Window;
           this.treeNodes.Dock = System.Windows.Forms.DockStyle.Fill;
           this.treeNodes.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.treeNodes.ForeColor = System.Drawing.SystemColors.WindowText;
           this.treeNodes.ImageIndex = 0;
           this.treeNodes.ImageList = this.imageList;
           this.treeNodes.LineColor = System.Drawing.Color.Silver;
           this.treeNodes.Location = new System.Drawing.Point(0, 0);
           this.treeNodes.Name = "treeNodes";
           this.treeNodes.SelectedImageIndex = 0;
           this.treeNodes.Size = new System.Drawing.Size(282, 505);
           this.treeNodes.TabIndex = 0;
           this.treeNodes.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeNodes_BeforeExpand);
           this.treeNodes.MouseUp += new System.Windows.Forms.MouseEventHandler(this.treeNodes_MouseUp);
           this.treeNodes.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeNodes_AfterSelect);
           this.treeNodes.AfterExpand += new System.Windows.Forms.TreeViewEventHandler(this.treeNodes_AfterExpand);
           // 
           // imageList
           // 
           this.imageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("imageList.ImageStream")));
           this.imageList.TransparentColor = System.Drawing.Color.Transparent;
           this.imageList.Images.SetKeyName(0, "star.ico");
           this.imageList.Images.SetKeyName(1, "world.ico");
           this.imageList.Images.SetKeyName(2, "server.ico");
           this.imageList.Images.SetKeyName(3, "folder.ico");
           this.imageList.Images.SetKeyName(4, "user.ico");
           this.imageList.Images.SetKeyName(5, "arrow_switch.ico");
           this.imageList.Images.SetKeyName(6, "arrow_out.ico");
           this.imageList.Images.SetKeyName(7, "arrow_out.ico");
           this.imageList.Images.SetKeyName(8, "page_white_go.ico");
           this.imageList.Images.SetKeyName(9, "server_connect.ico");
           this.imageList.Images.SetKeyName(10, "connect.ico");
           this.imageList.Images.SetKeyName(11, "routing_intersection_right.ico");
           this.imageList.Images.SetKeyName(12, "connect.ico");
           this.imageList.Images.SetKeyName(13, "spam_filter.ico");
           this.imageList.Images.SetKeyName(14, "email_delete.ico");
           this.imageList.Images.SetKeyName(15, "time_delete.ico");
           this.imageList.Images.SetKeyName(16, "email_link.ico");
           this.imageList.Images.SetKeyName(17, "bug_delete.ico");
           this.imageList.Images.SetKeyName(18, "directory_listing.ico");
           this.imageList.Images.SetKeyName(19, "understanding.ico");
           this.imageList.Images.SetKeyName(20, "stop.ico");
           this.imageList.Images.SetKeyName(21, "rosette.ico");
           this.imageList.Images.SetKeyName(22, "construction.ico");
           this.imageList.Images.SetKeyName(23, "arrow_in.ico");
           this.imageList.Images.SetKeyName(24, "arrow_branch.ico");
           this.imageList.Images.SetKeyName(25, "speedometer.ico");
           this.imageList.Images.SetKeyName(26, "text_align_justity.ico");
           this.imageList.Images.SetKeyName(27, "source_code.ico");
           this.imageList.Images.SetKeyName(28, "multitool.ico");
           this.imageList.Images.SetKeyName(29, "disk.ico");
           this.imageList.Images.SetKeyName(30, "compass.ico");
           this.imageList.Images.SetKeyName(31, "arrow_out.ico");
           this.imageList.Images.SetKeyName(32, "heart.ico");
           this.imageList.Images.SetKeyName(33, "information.ico");
           this.imageList.Images.SetKeyName(34, "setting_tools.ico");
           this.imageList.Images.SetKeyName(35, "chart_organisation.ico");
           // 
           // buttonHelp
           // 
           this.buttonHelp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
           this.buttonHelp.Location = new System.Drawing.Point(5, 473);
           this.buttonHelp.Name = "buttonHelp";
           this.buttonHelp.Size = new System.Drawing.Size(89, 25);
           this.buttonHelp.TabIndex = 5;
           this.buttonHelp.Text = "&Help";
           this.buttonHelp.UseVisualStyleBackColor = true;
           this.buttonHelp.Click += new System.EventHandler(this.buttonHelp_Click);
           // 
           // buttonSave
           // 
           this.buttonSave.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonSave.Location = new System.Drawing.Point(403, 473);
           this.buttonSave.Name = "buttonSave";
           this.buttonSave.Size = new System.Drawing.Size(89, 25);
           this.buttonSave.TabIndex = 4;
           this.buttonSave.Text = "&Save";
           this.buttonSave.UseVisualStyleBackColor = true;
           this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
           // 
           // panelTopBar
           // 
           this.panelTopBar.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
           this.panelTopBar.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
           this.panelTopBar.Controls.Add(this.labelTopCaption);
           this.panelTopBar.Dock = System.Windows.Forms.DockStyle.Top;
           this.panelTopBar.Location = new System.Drawing.Point(0, 0);
           this.panelTopBar.Name = "panelTopBar";
           this.panelTopBar.Size = new System.Drawing.Size(500, 27);
           this.panelTopBar.TabIndex = 3;
           // 
           // labelTopCaption
           // 
           this.labelTopCaption.AutoSize = true;
           this.labelTopCaption.Font = new System.Drawing.Font("Tahoma", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelTopCaption.ForeColor = System.Drawing.Color.Black;
           this.labelTopCaption.Location = new System.Drawing.Point(3, 5);
           this.labelTopCaption.Name = "labelTopCaption";
           this.labelTopCaption.Size = new System.Drawing.Size(46, 16);
           this.labelTopCaption.TabIndex = 0;
           this.labelTopCaption.Text = "label1";
           // 
           // panelMain
           // 
           this.panelMain.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.panelMain.BackColor = System.Drawing.SystemColors.Control;
           this.panelMain.Location = new System.Drawing.Point(5, 33);
           this.panelMain.Name = "panelMain";
           this.panelMain.Size = new System.Drawing.Size(494, 434);
           this.panelMain.TabIndex = 0;
           // 
           // menuStrip
           // 
           this.menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuItemFile,
            this.menuItemHelp});
           this.menuStrip.Location = new System.Drawing.Point(0, 0);
           this.menuStrip.Name = "menuStrip";
           this.menuStrip.Size = new System.Drawing.Size(792, 24);
           this.menuStrip.TabIndex = 10;
           this.menuStrip.Text = "menuStrip1";
           // 
           // menuItemFile
           // 
           this.menuItemFile.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuItemSelectLanguage,
            this.menuItemConnect,
            this.toolStripSeparator1,
            this.menuItemExit});
           this.menuItemFile.Name = "menuItemFile";
           this.menuItemFile.Size = new System.Drawing.Size(35, 20);
           this.menuItemFile.Text = "&File";
           // 
           // menuItemSelectLanguage
           // 
           this.menuItemSelectLanguage.Name = "menuItemSelectLanguage";
           this.menuItemSelectLanguage.Size = new System.Drawing.Size(162, 22);
           this.menuItemSelectLanguage.Text = "Select language...";
           this.menuItemSelectLanguage.Click += new System.EventHandler(this.menuItemSelectLanguage_Click);
           // 
           // menuItemConnect
           // 
           this.menuItemConnect.Name = "menuItemConnect";
           this.menuItemConnect.Size = new System.Drawing.Size(162, 22);
           this.menuItemConnect.Text = "Connect...";
           this.menuItemConnect.Click += new System.EventHandler(this.menuItemConnect_Click);
           // 
           // toolStripSeparator1
           // 
           this.toolStripSeparator1.Name = "toolStripSeparator1";
           this.toolStripSeparator1.Size = new System.Drawing.Size(159, 6);
           // 
           // menuItemExit
           // 
           this.menuItemExit.Name = "menuItemExit";
           this.menuItemExit.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.F4)));
           this.menuItemExit.Size = new System.Drawing.Size(162, 22);
           this.menuItemExit.Text = "Exit";
           this.menuItemExit.Click += new System.EventHandler(this.menuItemExit_Click);
           // 
           // menuItemHelp
           // 
           this.menuItemHelp.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuItemHelpIndex,
            this.menuItemHelpAbout});
           this.menuItemHelp.Name = "menuItemHelp";
           this.menuItemHelp.Size = new System.Drawing.Size(40, 20);
           this.menuItemHelp.Text = "Help";
           // 
           // menuItemHelpIndex
           // 
           this.menuItemHelpIndex.Name = "menuItemHelpIndex";
           this.menuItemHelpIndex.Size = new System.Drawing.Size(115, 22);
           this.menuItemHelpIndex.Text = "Index...";
           this.menuItemHelpIndex.Click += new System.EventHandler(this.menuItemHelpIndex_Click);
           // 
           // menuItemHelpAbout
           // 
           this.menuItemHelpAbout.Name = "menuItemHelpAbout";
           this.menuItemHelpAbout.Size = new System.Drawing.Size(115, 22);
           this.menuItemHelpAbout.Text = "About...";
           this.menuItemHelpAbout.Click += new System.EventHandler(this.menuItemHelpAbout_Click);
           // 
           // formMain
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.ClientSize = new System.Drawing.Size(792, 573);
           this.Controls.Add(this.splitContainer);
           this.Controls.Add(this.groupBox1);
           this.Controls.Add(this.buttonExit);
           this.Controls.Add(this.menuStrip);
           this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
           this.KeyPreview = true;
           this.MainMenuStrip = this.menuStrip;
           this.MinimumSize = new System.Drawing.Size(800, 600);
           this.Name = "formMain";
           this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
           this.Text = "hMailServer Administrator";
           this.Load += new System.EventHandler(this.formMain_Load);
           this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.formMain_FormClosing);
           this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.formMain_KeyDown);
           this.splitContainer.Panel1.ResumeLayout(false);
           this.splitContainer.Panel2.ResumeLayout(false);
           this.splitContainer.ResumeLayout(false);
           this.panelTopBar.ResumeLayout(false);
           this.panelTopBar.PerformLayout();
           this.menuStrip.ResumeLayout(false);
           this.menuStrip.PerformLayout();
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonExit;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.SplitContainer splitContainer;
        private System.Windows.Forms.TreeView treeNodes;
        private System.Windows.Forms.Panel panelMain;
        private System.Windows.Forms.Panel panelTopBar;
        private System.Windows.Forms.Label labelTopCaption;
        private System.Windows.Forms.MenuStrip menuStrip;
        private System.Windows.Forms.ToolStripMenuItem menuItemFile;
        private System.Windows.Forms.ToolStripMenuItem menuItemExit;
        private System.Windows.Forms.ToolStripMenuItem menuItemConnect;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.Button buttonSave;
       private System.Windows.Forms.ImageList imageList;
       private System.Windows.Forms.Button buttonHelp;
       private System.Windows.Forms.ToolStripMenuItem menuItemHelp;
       private System.Windows.Forms.ToolStripMenuItem menuItemHelpIndex;
       private System.Windows.Forms.ToolStripMenuItem menuItemHelpAbout;
       private System.Windows.Forms.ToolStripMenuItem menuItemSelectLanguage;
    }
}

