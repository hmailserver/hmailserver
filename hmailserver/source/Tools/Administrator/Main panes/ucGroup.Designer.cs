namespace hMailServer.Administrator
{
    partial class ucGroup
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
           this.labelName = new System.Windows.Forms.Label();
           this.listMembers = new hMailServer.Administrator.ucListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.buttonSelect = new System.Windows.Forms.Button();
           this.buttonDelete = new System.Windows.Forms.Button();
           this.labelMembers = new System.Windows.Forms.Label();
           this.textName = new hMailServer.Shared.ucText();
           this.SuspendLayout();
           // 
           // labelName
           // 
           this.labelName.AutoSize = true;
           this.labelName.Location = new System.Drawing.Point(5, 8);
           this.labelName.Name = "labelName";
           this.labelName.Size = new System.Drawing.Size(35, 13);
           this.labelName.TabIndex = 4;
           this.labelName.Text = "Name";
           // 
           // listMembers
           // 
           this.listMembers.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listMembers.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
           this.listMembers.FullRowSelect = true;
           this.listMembers.Location = new System.Drawing.Point(8, 72);
           this.listMembers.Name = "listMembers";
           this.listMembers.Size = new System.Drawing.Size(472, 280);
           this.listMembers.TabIndex = 39;
           this.listMembers.UseCompatibleStateImageBehavior = false;
           this.listMembers.View = System.Windows.Forms.View.Details;
           this.listMembers.SelectedIndexChanged += new System.EventHandler(this.listMembers_SelectedIndexChanged);
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "Address";
           this.columnHeader1.Width = 250;
           // 
           // buttonSelect
           // 
           this.buttonSelect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonSelect.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonSelect.Location = new System.Drawing.Point(488, 72);
           this.buttonSelect.Name = "buttonSelect";
           this.buttonSelect.Size = new System.Drawing.Size(100, 25);
           this.buttonSelect.TabIndex = 44;
           this.buttonSelect.Text = "&Select...";
           this.buttonSelect.UseVisualStyleBackColor = true;
           this.buttonSelect.Click += new System.EventHandler(this.buttonSelect_Click);
           // 
           // buttonDelete
           // 
           this.buttonDelete.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonDelete.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonDelete.Location = new System.Drawing.Point(488, 103);
           this.buttonDelete.Name = "buttonDelete";
           this.buttonDelete.Size = new System.Drawing.Size(100, 25);
           this.buttonDelete.TabIndex = 43;
           this.buttonDelete.Text = "Remove";
           this.buttonDelete.UseVisualStyleBackColor = true;
           this.buttonDelete.Click += new System.EventHandler(this.buttonDelete_Click);
           // 
           // labelMembers
           // 
           this.labelMembers.AutoSize = true;
           this.labelMembers.Location = new System.Drawing.Point(5, 56);
           this.labelMembers.Name = "labelMembers";
           this.labelMembers.Size = new System.Drawing.Size(50, 13);
           this.labelMembers.TabIndex = 45;
           this.labelMembers.Text = "Members";
           // 
           // textName
           // 
           this.textName.Location = new System.Drawing.Point(8, 24);
           this.textName.Name = "textName";
           this.textName.Number = 0;
           this.textName.Numeric = false;
           this.textName.Size = new System.Drawing.Size(215, 20);
           this.textName.TabIndex = 47;
           // 
           // ucGroup
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.textName);
           this.Controls.Add(this.labelMembers);
           this.Controls.Add(this.buttonSelect);
           this.Controls.Add(this.buttonDelete);
           this.Controls.Add(this.listMembers);
           this.Controls.Add(this.labelName);
           this.Name = "ucGroup";
           this.Size = new System.Drawing.Size(595, 371);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private System.Windows.Forms.Label labelName;
       private ucListView listMembers;
       private System.Windows.Forms.Button buttonSelect;
       private System.Windows.Forms.Button buttonDelete;
       private System.Windows.Forms.Label labelMembers;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private hMailServer.Shared.ucText textName;
    }
}
