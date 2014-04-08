namespace DataDirectorySynchronizer.Pages
{
   partial class ucSelectDomain
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
         this.labelSelectDomain = new System.Windows.Forms.Label();
         this.listViewDomains = new System.Windows.Forms.ListView();
         this.columnDomainName = new System.Windows.Forms.ColumnHeader();
         this.buttonSelectAll = new System.Windows.Forms.Button();
         this.buttonSelectNone = new System.Windows.Forms.Button();
         this.SuspendLayout();
         // 
         // labelSelectDomain
         // 
         this.labelSelectDomain.AutoSize = true;
         this.labelSelectDomain.Location = new System.Drawing.Point(25, 16);
         this.labelSelectDomain.Name = "labelSelectDomain";
         this.labelSelectDomain.Size = new System.Drawing.Size(212, 13);
         this.labelSelectDomain.TabIndex = 5;
         this.labelSelectDomain.Text = "Select the domain you want to synchronize.";
         // 
         // listViewDomains
         // 
         this.listViewDomains.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.listViewDomains.CheckBoxes = true;
         this.listViewDomains.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnDomainName});
         this.listViewDomains.FullRowSelect = true;
         this.listViewDomains.Location = new System.Drawing.Point(28, 50);
         this.listViewDomains.Name = "listViewDomains";
         this.listViewDomains.Size = new System.Drawing.Size(532, 218);
         this.listViewDomains.TabIndex = 7;
         this.listViewDomains.UseCompatibleStateImageBehavior = false;
         this.listViewDomains.View = System.Windows.Forms.View.Details;
         // 
         // columnDomainName
         // 
         this.columnDomainName.Text = "Domain name";
         this.columnDomainName.Width = 250;
         // 
         // buttonSelectAll
         // 
         this.buttonSelectAll.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.buttonSelectAll.Location = new System.Drawing.Point(29, 274);
         this.buttonSelectAll.Name = "buttonSelectAll";
         this.buttonSelectAll.Size = new System.Drawing.Size(131, 28);
         this.buttonSelectAll.TabIndex = 8;
         this.buttonSelectAll.Text = "Select all";
         this.buttonSelectAll.UseVisualStyleBackColor = true;
         this.buttonSelectAll.Click += new System.EventHandler(this.buttonSelectAll_Click);
         // 
         // buttonSelectNone
         // 
         this.buttonSelectNone.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
         this.buttonSelectNone.Location = new System.Drawing.Point(166, 274);
         this.buttonSelectNone.Name = "buttonSelectNone";
         this.buttonSelectNone.Size = new System.Drawing.Size(131, 28);
         this.buttonSelectNone.TabIndex = 9;
         this.buttonSelectNone.Text = "Select none";
         this.buttonSelectNone.UseVisualStyleBackColor = true;
         this.buttonSelectNone.Click += new System.EventHandler(this.buttonSelectNone_Click);
         // 
         // ucSelectDomain
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.buttonSelectNone);
         this.Controls.Add(this.buttonSelectAll);
         this.Controls.Add(this.listViewDomains);
         this.Controls.Add(this.labelSelectDomain);
         this.Name = "ucSelectDomain";
         this.Size = new System.Drawing.Size(581, 315);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Label labelSelectDomain;
      private System.Windows.Forms.ListView listViewDomains;
      private System.Windows.Forms.ColumnHeader columnDomainName;
      private System.Windows.Forms.Button buttonSelectAll;
      private System.Windows.Forms.Button buttonSelectNone;
   }
}
