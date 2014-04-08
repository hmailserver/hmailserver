namespace hMailServer.Administrator
{
    partial class ucAccounts
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
           this.listAccounts = new hMailServer.Administrator.ucListView();
           this.columnName = new System.Windows.Forms.ColumnHeader();
           this.columnActive = new System.Windows.Forms.ColumnHeader();
           this.SuspendLayout();
           // 
           // listAccounts
           // 
           this.listAccounts.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listAccounts.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnName,
            this.columnActive});
           this.listAccounts.FullRowSelect = true;
           this.listAccounts.Location = new System.Drawing.Point(0, 0);
           this.listAccounts.Name = "listAccounts";
           this.listAccounts.Size = new System.Drawing.Size(464, 352);
           this.listAccounts.TabIndex = 1;
           this.listAccounts.UseCompatibleStateImageBehavior = false;
           this.listAccounts.View = System.Windows.Forms.View.Details;
           // 
           // columnName
           // 
           this.columnName.Text = "Name";
           this.columnName.Width = 200;
           // 
           // columnActive
           // 
           this.columnActive.Text = "Enabled";
           this.columnActive.Width = 120;
           // 
           // ucAccounts
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.listAccounts);
           this.Name = "ucAccounts";
           this.Controls.SetChildIndex(this.listAccounts, 0);
           this.ResumeLayout(false);

        }

        #endregion

       private ucListView listAccounts;
       private System.Windows.Forms.ColumnHeader columnName;
       private System.Windows.Forms.ColumnHeader columnActive;
    }
}
