namespace hMailServer.Administrator
{
    partial class ucServerMessages
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
           this.SuspendLayout();
           // 
           // listObjects
           // 
           this.listObjects.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnName});
           this.listObjects.Dock = System.Windows.Forms.DockStyle.Fill;
           this.listObjects.FullRowSelect = true;
           this.listObjects.Location = new System.Drawing.Point(0, 0);
           this.listObjects.Name = "listObjects";
           this.listObjects.Size = new System.Drawing.Size(683, 352);
           this.listObjects.TabIndex = 6;
           this.listObjects.UseCompatibleStateImageBehavior = false;
           this.listObjects.View = System.Windows.Forms.View.Details;
           this.listObjects.DoubleClick += new System.EventHandler(this.listObjects_DoubleClick);
           // 
           // columnName
           // 
           this.columnName.Text = "Name";
           this.columnName.Width = 200;
           // 
           // ucServerMessages
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.listObjects);
           this.Name = "ucServerMessages";
           this.Size = new System.Drawing.Size(683, 352);
           this.ResumeLayout(false);

        }

        #endregion

       private ucListView listObjects;
       private System.Windows.Forms.ColumnHeader columnName;
    }
}
