namespace hMailServer.Administrator.Dialogs
{
   partial class formSelectUsers
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

      #region Windows Form Designer generated code

      /// <summary>
      /// Required method for Designer support - do not modify
      /// the contents of this method with the code editor.
      /// </summary>
      private void InitializeComponent()
      {
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formSelectUsers));
         this.labelType = new System.Windows.Forms.Label();
         this.labelDomain = new System.Windows.Forms.Label();
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.btnCancel = new System.Windows.Forms.Button();
         this.btnOK = new System.Windows.Forms.Button();
         this.listItems = new hMailServer.Administrator.ucListView();
         this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
         this.comboDomains = new hMailServer.Administrator.Controls.ucComboBox();
         this.comboType = new hMailServer.Administrator.Controls.ucComboBox();
         this.SuspendLayout();
         // 
         // labelType
         // 
         this.labelType.AutoSize = true;
         this.labelType.Location = new System.Drawing.Point(9, 8);
         this.labelType.Name = "labelType";
         this.labelType.Size = new System.Drawing.Size(31, 13);
         this.labelType.TabIndex = 1;
         this.labelType.Text = "Type";
         // 
         // labelDomain
         // 
         this.labelDomain.AutoSize = true;
         this.labelDomain.Location = new System.Drawing.Point(9, 52);
         this.labelDomain.Name = "labelDomain";
         this.labelDomain.Size = new System.Drawing.Size(43, 13);
         this.labelDomain.TabIndex = 3;
         this.labelDomain.Text = "Domain";
         // 
         // groupBox1
         // 
         this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.groupBox1.Location = new System.Drawing.Point(20, 323);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(562, 5);
         this.groupBox1.TabIndex = 27;
         this.groupBox1.TabStop = false;
         // 
         // btnCancel
         // 
         this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.btnCancel.Location = new System.Drawing.Point(489, 334);
         this.btnCancel.Name = "btnCancel";
         this.btnCancel.Size = new System.Drawing.Size(89, 25);
         this.btnCancel.TabIndex = 26;
         this.btnCancel.Text = "&Cancel";
         this.btnCancel.UseVisualStyleBackColor = true;
         // 
         // btnOK
         // 
         this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.btnOK.Location = new System.Drawing.Point(394, 334);
         this.btnOK.Name = "btnOK";
         this.btnOK.Size = new System.Drawing.Size(89, 25);
         this.btnOK.TabIndex = 25;
         this.btnOK.Text = "&OK";
         this.btnOK.UseVisualStyleBackColor = true;
         this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
         // 
         // listItems
         // 
         this.listItems.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.listItems.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
         this.listItems.FullRowSelect = true;
         this.listItems.Location = new System.Drawing.Point(15, 103);
         this.listItems.Name = "listItems";
         this.listItems.Size = new System.Drawing.Size(563, 211);
         this.listItems.TabIndex = 5;
         this.listItems.UseCompatibleStateImageBehavior = false;
         this.listItems.View = System.Windows.Forms.View.Details;
         this.listItems.DoubleClick += new System.EventHandler(this.listItems_DoubleClick);
         // 
         // columnHeader1
         // 
         this.columnHeader1.Text = "Name";
         this.columnHeader1.Width = 250;
         // 
         // comboDomains
         // 
         this.comboDomains.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
         this.comboDomains.FormattingEnabled = true;
         this.comboDomains.Location = new System.Drawing.Point(12, 68);
         this.comboDomains.Name = "comboDomains";
         this.comboDomains.Size = new System.Drawing.Size(190, 21);
         this.comboDomains.TabIndex = 4;
         this.comboDomains.SelectedIndexChanged += new System.EventHandler(this.comboDomains_SelectedIndexChanged);
         // 
         // comboType
         // 
         this.comboType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
         this.comboType.FormattingEnabled = true;
         this.comboType.Location = new System.Drawing.Point(12, 24);
         this.comboType.Name = "comboType";
         this.comboType.Size = new System.Drawing.Size(190, 21);
         this.comboType.TabIndex = 2;
         this.comboType.SelectedIndexChanged += new System.EventHandler(this.comboType_SelectedIndexChanged);
         // 
         // formSelectUsers
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(595, 371);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.btnCancel);
         this.Controls.Add(this.btnOK);
         this.Controls.Add(this.listItems);
         this.Controls.Add(this.comboDomains);
         this.Controls.Add(this.labelDomain);
         this.Controls.Add(this.comboType);
         this.Controls.Add(this.labelType);
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.MinimumSize = new System.Drawing.Size(381, 314);
         this.Name = "formSelectUsers";
         this.ShowInTaskbar = false;
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "Select";
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Label labelType;
      private hMailServer.Administrator.Controls.ucComboBox comboType;
      private hMailServer.Administrator.Controls.ucComboBox comboDomains;
      private System.Windows.Forms.Label labelDomain;
      private ucListView listItems;
      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Button btnCancel;
      private System.Windows.Forms.Button btnOK;
      private System.Windows.Forms.ColumnHeader columnHeader1;
   }
}