namespace hMailServer.Administrator
{
    partial class ucGroups
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
           this.listGroups = new hMailServer.Administrator.ucListView();
           this.columnName = new System.Windows.Forms.ColumnHeader();
           this.SuspendLayout();
           // 
           // listGroups
           // 
           this.listGroups.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listGroups.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnName});
           this.listGroups.FullRowSelect = true;
           this.listGroups.Location = new System.Drawing.Point(0, 0);
           this.listGroups.Name = "listGroups";
           this.listGroups.Size = new System.Drawing.Size(464, 352);
           this.listGroups.TabIndex = 3;
           this.listGroups.UseCompatibleStateImageBehavior = false;
           this.listGroups.View = System.Windows.Forms.View.Details;
           // 
           // columnName
           // 
           this.columnName.Text = "Name";
           this.columnName.Width = 200;
           // 
           // ucGroups
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.listGroups);
           this.Name = "ucGroups";
           this.Controls.SetChildIndex(this.listGroups, 0);
           this.ResumeLayout(false);

        }

        #endregion

       private ucListView listGroups;
       private System.Windows.Forms.ColumnHeader columnName;
    }
}
