namespace hMailServer.Administrator
{
    partial class ucMXQuery
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
           this.labelEmailAddress = new System.Windows.Forms.Label();
           this.buttonResolve = new System.Windows.Forms.Button();
           this.labelMailServers = new System.Windows.Forms.Label();
           this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
           this.copyToClipboard = new System.Windows.Forms.ToolStripMenuItem();
           this.listMailServers = new hMailServer.Administrator.ucListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.textEmailAddress = new hMailServer.Shared.ucText();
           this.contextMenuStrip1.SuspendLayout();
           this.SuspendLayout();
           // 
           // labelEmailAddress
           // 
           this.labelEmailAddress.AutoSize = true;
           this.labelEmailAddress.Location = new System.Drawing.Point(5, 6);
           this.labelEmailAddress.Name = "labelEmailAddress";
           this.labelEmailAddress.Size = new System.Drawing.Size(75, 13);
           this.labelEmailAddress.TabIndex = 3;
           this.labelEmailAddress.Text = "E-mail address";
           // 
           // buttonResolve
           // 
           this.buttonResolve.Location = new System.Drawing.Point(8, 58);
           this.buttonResolve.Name = "buttonResolve";
           this.buttonResolve.Size = new System.Drawing.Size(100, 25);
           this.buttonResolve.TabIndex = 5;
           this.buttonResolve.Text = "&Resolve";
           this.buttonResolve.UseVisualStyleBackColor = true;
           this.buttonResolve.Click += new System.EventHandler(this.buttonResolve_Click);
           // 
           // labelMailServers
           // 
           this.labelMailServers.AutoSize = true;
           this.labelMailServers.Location = new System.Drawing.Point(5, 100);
           this.labelMailServers.Name = "labelMailServers";
           this.labelMailServers.Size = new System.Drawing.Size(63, 13);
           this.labelMailServers.TabIndex = 6;
           this.labelMailServers.Text = "Mail servers";
           // 
           // contextMenuStrip1
           // 
           this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.copyToClipboard});
           this.contextMenuStrip1.Name = "contextMenuStrip1";
           this.contextMenuStrip1.Size = new System.Drawing.Size(155, 48);
           // 
           // copyToClipboard
           // 
           this.copyToClipboard.Name = "copyToClipboard";
           this.copyToClipboard.Size = new System.Drawing.Size(154, 22);
           this.copyToClipboard.Text = "Copy to clipboard";
           this.copyToClipboard.Click += new System.EventHandler(this.copyToClipboard_Click);
           // 
           // listMailServers
           // 
           this.listMailServers.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listMailServers.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
           this.listMailServers.ContextMenuStrip = this.contextMenuStrip1;
           this.listMailServers.FullRowSelect = true;
           this.listMailServers.Location = new System.Drawing.Point(8, 116);
           this.listMailServers.Name = "listMailServers";
           this.listMailServers.Size = new System.Drawing.Size(418, 185);
           this.listMailServers.TabIndex = 7;
           this.listMailServers.UseCompatibleStateImageBehavior = false;
           this.listMailServers.View = System.Windows.Forms.View.Details;
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "IP address";
           this.columnHeader1.Width = 150;
           // 
           // textEmailAddress
           // 
           this.textEmailAddress.Location = new System.Drawing.Point(8, 24);
           this.textEmailAddress.Name = "textEmailAddress";
           this.textEmailAddress.Number = 0;
           this.textEmailAddress.Numeric = false;
           this.textEmailAddress.Size = new System.Drawing.Size(200, 20);
           this.textEmailAddress.TabIndex = 4;
           // 
           // ucMXQuery
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.listMailServers);
           this.Controls.Add(this.labelMailServers);
           this.Controls.Add(this.buttonResolve);
           this.Controls.Add(this.textEmailAddress);
           this.Controls.Add(this.labelEmailAddress);
           this.Name = "ucMXQuery";
           this.Size = new System.Drawing.Size(517, 324);
           this.contextMenuStrip1.ResumeLayout(false);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private hMailServer.Shared.ucText textEmailAddress;
       private System.Windows.Forms.Label labelEmailAddress;
       private System.Windows.Forms.Button buttonResolve;
       private System.Windows.Forms.Label labelMailServers;
       private ucListView listMailServers;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
       private System.Windows.Forms.ToolStripMenuItem copyToClipboard;
    }
}
