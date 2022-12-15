namespace hMailServer.Administrator
{
    partial class ucIncomingRelays
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
            this.listObjects = new hMailServer.Administrator.ucListView();
            this.columnName = new System.Windows.Forms.ColumnHeader();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.SuspendLayout();
            // 
            // listObjects
            // 
            this.listObjects.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listObjects.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnName,
            this.columnHeader1,
            this.columnHeader2});
            this.listObjects.FullRowSelect = true;
            this.listObjects.Location = new System.Drawing.Point(3, 3);
            this.listObjects.Name = "listObjects";
            this.listObjects.Size = new System.Drawing.Size(455, 423);
            this.listObjects.TabIndex = 51;
            this.listObjects.UseCompatibleStateImageBehavior = false;
            this.listObjects.View = System.Windows.Forms.View.Details;
            // 
            // columnName
            // 
            this.columnName.Text = "Name";
            this.columnName.Width = 200;
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Lower IP";
            this.columnHeader1.Tag = "IPAddress";
            this.columnHeader1.Width = 100;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Upper IP";
            this.columnHeader2.Tag = "IPAddress";
            this.columnHeader2.Width = 100;
            // 
            // ucIncomingRelays
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.listObjects);
            this.Name = "ucIncomingRelays";
            this.Size = new System.Drawing.Size(578, 429);
            this.Controls.SetChildIndex(this.listObjects, 0);
            this.ResumeLayout(false);

        }

        #endregion

        private ucListView listObjects;
        private System.Windows.Forms.ColumnHeader columnName;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
    }
}
