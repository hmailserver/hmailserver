namespace hMailServer.Administrator.Dialogs
{
   partial class formUserAccounts
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
         this.labelDomain = new System.Windows.Forms.Label();
         this.comboDomain = new hMailServer.Administrator.Controls.ucComboBox();
         this.labelUsers = new System.Windows.Forms.Label();
         this.ucListView1 = new hMailServer.Administrator.ucListView();
         this.buttonAdd = new System.Windows.Forms.Button();
         this.buttonClose = new System.Windows.Forms.Button();
         this.SuspendLayout();
         // 
         // labelDomain
         // 
         this.labelDomain.AutoSize = true;
         this.labelDomain.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelDomain.Location = new System.Drawing.Point(12, 9);
         this.labelDomain.Name = "labelDomain";
         this.labelDomain.Size = new System.Drawing.Size(49, 13);
         this.labelDomain.TabIndex = 12;
         this.labelDomain.Text = "Domain";
         // 
         // comboDomain
         // 
         this.comboDomain.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.comboDomain.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
         this.comboDomain.FormattingEnabled = true;
         this.comboDomain.Location = new System.Drawing.Point(29, 33);
         this.comboDomain.Name = "comboDomain";
         this.comboDomain.Size = new System.Drawing.Size(377, 21);
         this.comboDomain.TabIndex = 13;
         // 
         // labelUsers
         // 
         this.labelUsers.AutoSize = true;
         this.labelUsers.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelUsers.Location = new System.Drawing.Point(12, 72);
         this.labelUsers.Name = "labelUsers";
         this.labelUsers.Size = new System.Drawing.Size(39, 13);
         this.labelUsers.TabIndex = 14;
         this.labelUsers.Text = "Users";
         // 
         // ucListView1
         // 
         this.ucListView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.ucListView1.FullRowSelect = true;
         this.ucListView1.Location = new System.Drawing.Point(15, 100);
         this.ucListView1.Name = "ucListView1";
         this.ucListView1.Size = new System.Drawing.Size(524, 215);
         this.ucListView1.TabIndex = 15;
         this.ucListView1.UseCompatibleStateImageBehavior = false;
         // 
         // buttonAdd
         // 
         this.buttonAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonAdd.Location = new System.Drawing.Point(355, 325);
         this.buttonAdd.Name = "buttonAdd";
         this.buttonAdd.Size = new System.Drawing.Size(89, 25);
         this.buttonAdd.TabIndex = 16;
         this.buttonAdd.Text = "&Add...";
         this.buttonAdd.UseVisualStyleBackColor = true;
         // 
         // buttonClose
         // 
         this.buttonClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonClose.Location = new System.Drawing.Point(450, 325);
         this.buttonClose.Name = "buttonClose";
         this.buttonClose.Size = new System.Drawing.Size(89, 25);
         this.buttonClose.TabIndex = 17;
         this.buttonClose.Text = "&Close";
         this.buttonClose.UseVisualStyleBackColor = true;
         // 
         // formUserAccounts
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(548, 356);
         this.Controls.Add(this.buttonClose);
         this.Controls.Add(this.buttonAdd);
         this.Controls.Add(this.ucListView1);
         this.Controls.Add(this.labelUsers);
         this.Controls.Add(this.comboDomain);
         this.Controls.Add(this.labelDomain);
         this.MaximizeBox = false;
         this.MinimizeBox = false;
         this.MinimumSize = new System.Drawing.Size(387, 269);
         this.Name = "formUserAccounts";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "User accounts";
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Label labelDomain;
      private hMailServer.Administrator.Controls.ucComboBox comboDomain;
      private System.Windows.Forms.Label labelUsers;
      private ucListView ucListView1;
      private System.Windows.Forms.Button buttonAdd;
      private System.Windows.Forms.Button buttonClose;
   }
}