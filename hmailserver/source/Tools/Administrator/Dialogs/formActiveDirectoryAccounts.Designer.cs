namespace hMailServer.Administrator.Dialogs
{
   partial class formActiveDirectoryAccounts
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formActiveDirectoryAccounts));
         this.buttonOK = new System.Windows.Forms.Button();
         this.buttonCancel = new System.Windows.Forms.Button();
         this.comboDomain = new System.Windows.Forms.ComboBox();
         this.labelDomain = new System.Windows.Forms.Label();
         this.labelAccounts = new System.Windows.Forms.Label();
         this.listAccounts = new hMailServer.Administrator.ucListView();
         this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
         this.SuspendLayout();
         // 
         // buttonOK
         // 
         this.buttonOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonOK.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.buttonOK.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.buttonOK.Location = new System.Drawing.Point(236, 277);
         this.buttonOK.Name = "buttonOK";
         this.buttonOK.Size = new System.Drawing.Size(97, 28);
         this.buttonOK.TabIndex = 32;
         this.buttonOK.Text = "&OK";
         this.buttonOK.UseVisualStyleBackColor = true;
         this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
         // 
         // buttonCancel
         // 
         this.buttonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.buttonCancel.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.buttonCancel.Location = new System.Drawing.Point(340, 277);
         this.buttonCancel.Name = "buttonCancel";
         this.buttonCancel.Size = new System.Drawing.Size(97, 28);
         this.buttonCancel.TabIndex = 31;
         this.buttonCancel.Text = "&Cancel";
         this.buttonCancel.UseVisualStyleBackColor = true;
         // 
         // comboDomain
         // 
         this.comboDomain.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
         this.comboDomain.FormattingEnabled = true;
         this.comboDomain.Location = new System.Drawing.Point(24, 32);
         this.comboDomain.Name = "comboDomain";
         this.comboDomain.Size = new System.Drawing.Size(328, 21);
         this.comboDomain.TabIndex = 33;
         this.comboDomain.SelectedValueChanged += new System.EventHandler(this.comboDomain_SelectedValueChanged);
         // 
         // labelDomain
         // 
         this.labelDomain.AutoSize = true;
         this.labelDomain.Location = new System.Drawing.Point(8, 8);
         this.labelDomain.Name = "labelDomain";
         this.labelDomain.Size = new System.Drawing.Size(46, 13);
         this.labelDomain.TabIndex = 34;
         this.labelDomain.Text = "Domain:";
         // 
         // labelAccounts
         // 
         this.labelAccounts.AutoSize = true;
         this.labelAccounts.Location = new System.Drawing.Point(8, 72);
         this.labelAccounts.Name = "labelAccounts";
         this.labelAccounts.Size = new System.Drawing.Size(55, 13);
         this.labelAccounts.TabIndex = 36;
         this.labelAccounts.Text = "Accounts:";
         // 
         // listAccounts
         // 
         this.listAccounts.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.listAccounts.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
         this.listAccounts.FullRowSelect = true;
         this.listAccounts.Location = new System.Drawing.Point(24, 96);
         this.listAccounts.Name = "listAccounts";
         this.listAccounts.Size = new System.Drawing.Size(420, 173);
         this.listAccounts.TabIndex = 35;
         this.listAccounts.UseCompatibleStateImageBehavior = false;
         this.listAccounts.View = System.Windows.Forms.View.Details;
         // 
         // columnHeader1
         // 
         this.columnHeader1.Text = "Address";
         this.columnHeader1.Width = 300;
         // 
         // formActiveDirectoryAccounts
         // 
         this.AcceptButton = this.buttonOK;
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.CancelButton = this.buttonCancel;
         this.ClientSize = new System.Drawing.Size(455, 313);
         this.Controls.Add(this.labelAccounts);
         this.Controls.Add(this.listAccounts);
         this.Controls.Add(this.labelDomain);
         this.Controls.Add(this.comboDomain);
         this.Controls.Add(this.buttonOK);
         this.Controls.Add(this.buttonCancel);
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.MinimumSize = new System.Drawing.Size(463, 303);
         this.Name = "formActiveDirectoryAccounts";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "Active Directory accounts";
         this.Shown += new System.EventHandler(this.formActiveDirectoryAccounts_Shown);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Button buttonOK;
      private System.Windows.Forms.Button buttonCancel;
      private System.Windows.Forms.ComboBox comboDomain;
      private System.Windows.Forms.Label labelDomain;
      private ucListView listAccounts;
      private System.Windows.Forms.Label labelAccounts;
      private System.Windows.Forms.ColumnHeader columnHeader1;
   }
}