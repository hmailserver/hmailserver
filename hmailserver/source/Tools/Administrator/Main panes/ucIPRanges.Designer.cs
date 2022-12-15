namespace hMailServer.Administrator
{
    partial class ucIPRanges
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
           this.columnPriority = new System.Windows.Forms.ColumnHeader();
           this.columnHeader4 = new System.Windows.Forms.ColumnHeader();
           this.buttonDefault = new System.Windows.Forms.Button();
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
            this.columnHeader2,
            this.columnPriority,
            this.columnHeader4});
           this.listObjects.FullRowSelect = true;
           this.listObjects.Location = new System.Drawing.Point(0, 0);
           this.listObjects.Name = "listObjects";
           this.listObjects.Size = new System.Drawing.Size(464, 352);
           this.listObjects.TabIndex = 5;
           this.listObjects.UseCompatibleStateImageBehavior = false;
           this.listObjects.View = System.Windows.Forms.View.Details;
           // 
           // columnName
           // 
           this.columnName.Text = "Name";
           this.columnName.Width = 120;
           // 
           // columnHeader1
           // 
           this.columnHeader1.Tag = "IPAddress";
           this.columnHeader1.Text = "Lower IP";
           this.columnHeader1.Width = 100;
           // 
           // columnHeader2
           // 
           this.columnHeader2.Tag = "IPAddress";
           this.columnHeader2.Text = "Upper IP";
           this.columnHeader2.Width = 100;
           // 
           // columnPriority
           // 
           this.columnPriority.Tag = "Numeric";
           this.columnPriority.Text = "Priority";
           this.columnPriority.Width = 50;
           // 
           // columnHeader4
           // 
           this.columnHeader4.Tag = "DateTime";
           this.columnHeader4.Text = "Expires";
           this.columnHeader4.Width = 150;
           // 
           // buttonDefault
           // 
           this.buttonDefault.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonDefault.Location = new System.Drawing.Point(472, 103);
           this.buttonDefault.Name = "buttonDefault";
           this.buttonDefault.Size = new System.Drawing.Size(100, 25);
           this.buttonDefault.TabIndex = 52;
           this.buttonDefault.Text = "Default";
           this.buttonDefault.UseVisualStyleBackColor = true;
           this.buttonDefault.Click += new System.EventHandler(this.buttonDefault_Click);
           // 
           // ucIPRanges
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.buttonDefault);
           this.Controls.Add(this.listObjects);
           this.Name = "ucIPRanges";
           this.Load += new System.EventHandler(this.ucIPRanges_Load);
           this.Controls.SetChildIndex(this.listObjects, 0);
           this.Controls.SetChildIndex(this.buttonDefault, 0);
           this.ResumeLayout(false);

        }

        #endregion

       private ucListView listObjects;
       private System.Windows.Forms.ColumnHeader columnName;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.ColumnHeader columnHeader2;
       private System.Windows.Forms.ColumnHeader columnPriority;
       private System.Windows.Forms.Button buttonDefault;
       private System.Windows.Forms.ColumnHeader columnHeader4;
    }
}
