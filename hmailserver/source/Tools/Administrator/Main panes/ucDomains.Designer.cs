namespace hMailServer.Administrator
{
    partial class ucDomains
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
           this.listDomains = new hMailServer.Administrator.ucListView();
           this.columnDomainName = new System.Windows.Forms.ColumnHeader();
           this.columnActive = new System.Windows.Forms.ColumnHeader();
           this.SuspendLayout();
           // 
           // listDomains
           // 
           this.listDomains.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listDomains.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnDomainName,
            this.columnActive});
           this.listDomains.FullRowSelect = true;
           this.listDomains.Location = new System.Drawing.Point(0, 0);
           this.listDomains.Name = "listDomains";
           this.listDomains.Size = new System.Drawing.Size(464, 352);
           this.listDomains.TabIndex = 0;
           this.listDomains.UseCompatibleStateImageBehavior = false;
           this.listDomains.View = System.Windows.Forms.View.Details;
           // 
           // columnDomainName
           // 
           this.columnDomainName.Text = "Domain name";
           this.columnDomainName.Width = 200;
           // 
           // columnActive
           // 
           this.columnActive.Text = "Enabled";
           this.columnActive.Width = 120;
           // 
           // ucDomains
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.listDomains);
           this.Name = "ucDomains";
           this.Controls.SetChildIndex(this.listDomains, 0);
           this.ResumeLayout(false);

        }

        #endregion

        private ucListView listDomains;
        private System.Windows.Forms.ColumnHeader columnDomainName;
       private System.Windows.Forms.ColumnHeader columnActive;
    }
}
