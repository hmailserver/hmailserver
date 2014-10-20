namespace hMailServer.Administrator
{
    partial class formConnect
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formConnect));
         this.listServers = new System.Windows.Forms.ListView();
         this.columnHost = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
         this.columnUsername = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
         this.btnAdd = new System.Windows.Forms.Button();
         this.btnEdit = new System.Windows.Forms.Button();
         this.btnRemove = new System.Windows.Forms.Button();
         this.btnConnect = new System.Windows.Forms.Button();
         this.btnCancel = new System.Windows.Forms.Button();
         this.checkAutoConnect = new System.Windows.Forms.CheckBox();
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.SuspendLayout();
         // 
         // listServers
         // 
         this.listServers.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
         this.listServers.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHost,
            this.columnUsername});
         this.listServers.FullRowSelect = true;
         this.listServers.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
         this.listServers.Location = new System.Drawing.Point(6, 12);
         this.listServers.Name = "listServers";
         this.listServers.Size = new System.Drawing.Size(487, 176);
         this.listServers.TabIndex = 0;
         this.listServers.UseCompatibleStateImageBehavior = false;
         this.listServers.View = System.Windows.Forms.View.Details;
         this.listServers.DoubleClick += new System.EventHandler(this.listServers_DoubleClick);
         // 
         // columnHost
         // 
         this.columnHost.Text = "Host name";
         this.columnHost.Width = 150;
         // 
         // columnUsername
         // 
         this.columnUsername.Text = "hMailServer Username";
         this.columnUsername.Width = 150;
         // 
         // btnAdd
         // 
         this.btnAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
         this.btnAdd.Location = new System.Drawing.Point(499, 12);
         this.btnAdd.Name = "btnAdd";
         this.btnAdd.Size = new System.Drawing.Size(89, 25);
         this.btnAdd.TabIndex = 1;
         this.btnAdd.Text = "Add";
         this.btnAdd.UseVisualStyleBackColor = true;
         this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
         // 
         // btnEdit
         // 
         this.btnEdit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
         this.btnEdit.Location = new System.Drawing.Point(499, 43);
         this.btnEdit.Name = "btnEdit";
         this.btnEdit.Size = new System.Drawing.Size(89, 25);
         this.btnEdit.TabIndex = 2;
         this.btnEdit.Text = "Edit";
         this.btnEdit.UseVisualStyleBackColor = true;
         this.btnEdit.Click += new System.EventHandler(this.btnEdit_Click);
         // 
         // btnRemove
         // 
         this.btnRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
         this.btnRemove.Location = new System.Drawing.Point(499, 74);
         this.btnRemove.Name = "btnRemove";
         this.btnRemove.Size = new System.Drawing.Size(89, 25);
         this.btnRemove.TabIndex = 3;
         this.btnRemove.Text = "Remove";
         this.btnRemove.UseVisualStyleBackColor = true;
         this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
         // 
         // btnConnect
         // 
         this.btnConnect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnConnect.Location = new System.Drawing.Point(404, 231);
         this.btnConnect.Name = "btnConnect";
         this.btnConnect.Size = new System.Drawing.Size(89, 25);
         this.btnConnect.TabIndex = 4;
         this.btnConnect.Text = "Connect";
         this.btnConnect.UseVisualStyleBackColor = true;
         this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
         // 
         // btnCancel
         // 
         this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.btnCancel.Location = new System.Drawing.Point(499, 231);
         this.btnCancel.Name = "btnCancel";
         this.btnCancel.Size = new System.Drawing.Size(89, 25);
         this.btnCancel.TabIndex = 5;
         this.btnCancel.Text = "Cancel";
         this.btnCancel.UseVisualStyleBackColor = true;
         this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
         // 
         // checkAutoConnect
         // 
         this.checkAutoConnect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.checkAutoConnect.AutoSize = true;
         this.checkAutoConnect.Location = new System.Drawing.Point(8, 195);
         this.checkAutoConnect.Name = "checkAutoConnect";
         this.checkAutoConnect.Size = new System.Drawing.Size(183, 17);
         this.checkAutoConnect.TabIndex = 6;
         this.checkAutoConnect.Text = "Automatically connect on start-up";
         this.checkAutoConnect.UseVisualStyleBackColor = true;
         // 
         // groupBox1
         // 
         this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
         this.groupBox1.Location = new System.Drawing.Point(6, 218);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(584, 5);
         this.groupBox1.TabIndex = 7;
         this.groupBox1.TabStop = false;
         // 
         // formConnect
         // 
         this.AcceptButton = this.btnConnect;
         this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
         this.CancelButton = this.btnCancel;
         this.ClientSize = new System.Drawing.Size(593, 264);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.checkAutoConnect);
         this.Controls.Add(this.btnCancel);
         this.Controls.Add(this.btnConnect);
         this.Controls.Add(this.btnRemove);
         this.Controls.Add(this.btnEdit);
         this.Controls.Add(this.btnAdd);
         this.Controls.Add(this.listServers);
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.Name = "formConnect";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "Connect";
         this.Load += new System.EventHandler(this.formConnect_Load);
         this.ResumeLayout(false);
         this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListView listServers;
        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.Button btnEdit;
        private System.Windows.Forms.Button btnRemove;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.CheckBox checkAutoConnect;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ColumnHeader columnHost;
        private System.Windows.Forms.ColumnHeader columnUsername;
    }
}