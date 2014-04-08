namespace hMailServer.Administrator
{
    partial class ucTCPIPPorts
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
           this.buttonDefault = new System.Windows.Forms.Button();
           this.SuspendLayout();
           // 
           // listObjects
           // 
           this.listObjects.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listObjects.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnName});
           this.listObjects.FullRowSelect = true;
           this.listObjects.Location = new System.Drawing.Point(0, 0);
           this.listObjects.Name = "listObjects";
           this.listObjects.Size = new System.Drawing.Size(464, 352);
           this.listObjects.TabIndex = 6;
           this.listObjects.UseCompatibleStateImageBehavior = false;
           this.listObjects.View = System.Windows.Forms.View.Details;
           // 
           // columnName
           // 
           this.columnName.Text = "Name";
           this.columnName.Width = 200;
           // 
           // buttonDefault
           // 
           this.buttonDefault.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonDefault.Location = new System.Drawing.Point(472, 103);
           this.buttonDefault.Name = "buttonDefault";
           this.buttonDefault.Size = new System.Drawing.Size(100, 25);
           this.buttonDefault.TabIndex = 51;
           this.buttonDefault.Text = "Default";
           this.buttonDefault.UseVisualStyleBackColor = true;
           this.buttonDefault.Click += new System.EventHandler(this.buttonDefault_Click);
           // 
           // ucTCPIPPorts
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.buttonDefault);
           this.Controls.Add(this.listObjects);
           this.Name = "ucTCPIPPorts";
           this.Controls.SetChildIndex(this.listObjects, 0);
           this.Controls.SetChildIndex(this.buttonDefault, 0);
           this.ResumeLayout(false);

        }

        #endregion

       private ucListView listObjects;
       private System.Windows.Forms.ColumnHeader columnName;
       private System.Windows.Forms.Button buttonDefault;
    }
}
