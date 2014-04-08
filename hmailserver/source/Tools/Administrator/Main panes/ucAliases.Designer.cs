namespace hMailServer.Administrator
{
    partial class ucAliases
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
           this.listAliases = new hMailServer.Administrator.ucListView();
           this.columnName = new System.Windows.Forms.ColumnHeader();
           this.columnActive = new System.Windows.Forms.ColumnHeader();
           this.columnTo = new System.Windows.Forms.ColumnHeader();
           this.SuspendLayout();
           // 
           // listAliases
           // 
           this.listAliases.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listAliases.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnName,
            this.columnTo,
            this.columnActive});
           this.listAliases.FullRowSelect = true;
           this.listAliases.Location = new System.Drawing.Point(0, 0);
           this.listAliases.Name = "listAliases";
           this.listAliases.Size = new System.Drawing.Size(464, 352);
           this.listAliases.TabIndex = 2;
           this.listAliases.UseCompatibleStateImageBehavior = false;
           this.listAliases.View = System.Windows.Forms.View.Details;
           // 
           // columnName
           // 
           this.columnName.Text = "Redirect from";
           this.columnName.Width = 180;
           // 
           // columnActive
           // 
           this.columnActive.Text = "Enabled";
           this.columnActive.Width = 100;
           // 
           // columnTo
           // 
           this.columnTo.Text = "To";
           this.columnTo.Width = 180;
           // 
           // ucAliases
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.listAliases);
           this.Name = "ucAliases";
           this.Controls.SetChildIndex(this.listAliases, 0);
           this.ResumeLayout(false);

        }

        #endregion

       private ucListView listAliases;
       private System.Windows.Forms.ColumnHeader columnName;
       private System.Windows.Forms.ColumnHeader columnActive;
       private System.Windows.Forms.ColumnHeader columnTo;
    }
}
