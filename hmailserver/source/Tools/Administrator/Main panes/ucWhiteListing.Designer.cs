namespace hMailServer.Administrator
{
    partial class ucWhiteListing
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
           this.buttonEditWhiteAddress = new System.Windows.Forms.Button();
           this.buttonDeleteWhiteAddress = new System.Windows.Forms.Button();
           this.buttonAddWhiteAddress = new System.Windows.Forms.Button();
           this.listWhiteAddresses = new hMailServer.Administrator.ucListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader4 = new System.Windows.Forms.ColumnHeader();
           this.SuspendLayout();
           // 
           // buttonEditWhiteAddress
           // 
           this.buttonEditWhiteAddress.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonEditWhiteAddress.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonEditWhiteAddress.Location = new System.Drawing.Point(543, 34);
           this.buttonEditWhiteAddress.Name = "buttonEditWhiteAddress";
           this.buttonEditWhiteAddress.Size = new System.Drawing.Size(100, 25);
           this.buttonEditWhiteAddress.TabIndex = 50;
           this.buttonEditWhiteAddress.Text = "&Edit...";
           this.buttonEditWhiteAddress.UseVisualStyleBackColor = false;
           this.buttonEditWhiteAddress.Click += new System.EventHandler(this.buttonEditWhiteAddress_Click);
           // 
           // buttonDeleteWhiteAddress
           // 
           this.buttonDeleteWhiteAddress.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonDeleteWhiteAddress.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonDeleteWhiteAddress.Location = new System.Drawing.Point(543, 65);
           this.buttonDeleteWhiteAddress.Name = "buttonDeleteWhiteAddress";
           this.buttonDeleteWhiteAddress.Size = new System.Drawing.Size(100, 25);
           this.buttonDeleteWhiteAddress.TabIndex = 49;
           this.buttonDeleteWhiteAddress.Text = "Remove";
           this.buttonDeleteWhiteAddress.UseVisualStyleBackColor = false;
           this.buttonDeleteWhiteAddress.Click += new System.EventHandler(this.buttonDeleteWhiteAddress_Click);
           // 
           // buttonAddWhiteAddress
           // 
           this.buttonAddWhiteAddress.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonAddWhiteAddress.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonAddWhiteAddress.Location = new System.Drawing.Point(543, 3);
           this.buttonAddWhiteAddress.Name = "buttonAddWhiteAddress";
           this.buttonAddWhiteAddress.Size = new System.Drawing.Size(100, 25);
           this.buttonAddWhiteAddress.TabIndex = 48;
           this.buttonAddWhiteAddress.Text = "&Add...";
           this.buttonAddWhiteAddress.UseVisualStyleBackColor = false;
           this.buttonAddWhiteAddress.Click += new System.EventHandler(this.buttonAddWhiteAddress_Click);
           // 
           // listWhiteAddresses
           // 
           this.listWhiteAddresses.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listWhiteAddresses.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3,
            this.columnHeader4});
           this.listWhiteAddresses.FullRowSelect = true;
           this.listWhiteAddresses.Location = new System.Drawing.Point(3, 3);
           this.listWhiteAddresses.Name = "listWhiteAddresses";
           this.listWhiteAddresses.Size = new System.Drawing.Size(525, 411);
           this.listWhiteAddresses.TabIndex = 47;
           this.listWhiteAddresses.UseCompatibleStateImageBehavior = false;
           this.listWhiteAddresses.View = System.Windows.Forms.View.Details;
           this.listWhiteAddresses.DoubleClick += new System.EventHandler(this.listWhiteAddresses_DoubleClick);
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "Description";
           this.columnHeader1.Width = 150;
           // 
           // columnHeader2
           // 
           this.columnHeader2.Text = "Lower IP";
           this.columnHeader2.Tag = "IPAddress";
           this.columnHeader2.Width = 100;
           // 
           // columnHeader3
           // 
           this.columnHeader3.Text = "Upper IP";
           this.columnHeader3.Tag = "IPAddress";
           this.columnHeader3.Width = 100;
           // 
           // columnHeader4
           // 
           this.columnHeader4.Text = "E-mail address";
           this.columnHeader4.Width = 100;
           // 
           // ucWhiteListing
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.buttonEditWhiteAddress);
           this.Controls.Add(this.buttonDeleteWhiteAddress);
           this.Controls.Add(this.buttonAddWhiteAddress);
           this.Controls.Add(this.listWhiteAddresses);
           this.Name = "ucWhiteListing";
           this.Size = new System.Drawing.Size(655, 417);
           this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.Button buttonEditWhiteAddress;
       private System.Windows.Forms.Button buttonDeleteWhiteAddress;
       private System.Windows.Forms.Button buttonAddWhiteAddress;
       private ucListView listWhiteAddresses;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.ColumnHeader columnHeader2;
       private System.Windows.Forms.ColumnHeader columnHeader3;
       private System.Windows.Forms.ColumnHeader columnHeader4;
    }
}
