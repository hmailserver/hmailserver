namespace hMailServer.Administrator
{
   partial class ucItemsView
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
         this.buttonEdit = new System.Windows.Forms.Button();
         this.buttonRemove = new System.Windows.Forms.Button();
         this.buttonAdd = new System.Windows.Forms.Button();
         this.SuspendLayout();
         // 
         // buttonEdit
         // 
         this.buttonEdit.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonEdit.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.buttonEdit.Location = new System.Drawing.Point(472, 40);
         this.buttonEdit.Name = "buttonEdit";
         this.buttonEdit.Size = new System.Drawing.Size(100, 25);
         this.buttonEdit.TabIndex = 50;
         this.buttonEdit.Text = "&Edit...";
         this.buttonEdit.UseVisualStyleBackColor = true;
         this.buttonEdit.Click += new System.EventHandler(this.buttonEdit_Click);
         // 
         // buttonRemove
         // 
         this.buttonRemove.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonRemove.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.buttonRemove.Location = new System.Drawing.Point(472, 72);
         this.buttonRemove.Name = "buttonRemove";
         this.buttonRemove.Size = new System.Drawing.Size(100, 25);
         this.buttonRemove.TabIndex = 49;
         this.buttonRemove.Text = "Remove";
         this.buttonRemove.UseVisualStyleBackColor = true;
         this.buttonRemove.Click += new System.EventHandler(this.buttonRemove_Click);
         // 
         // buttonAdd
         // 
         this.buttonAdd.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
         this.buttonAdd.DialogResult = System.Windows.Forms.DialogResult.OK;
         this.buttonAdd.Location = new System.Drawing.Point(472, 8);
         this.buttonAdd.Name = "buttonAdd";
         this.buttonAdd.Size = new System.Drawing.Size(100, 25);
         this.buttonAdd.TabIndex = 48;
         this.buttonAdd.Text = "&Add...";
         this.buttonAdd.UseVisualStyleBackColor = true;
         this.buttonAdd.Click += new System.EventHandler(this.buttonAdd_Click);
         // 
         // ucItemsView
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.buttonEdit);
         this.Controls.Add(this.buttonRemove);
         this.Controls.Add(this.buttonAdd);
         this.Name = "ucItemsView";
         this.Size = new System.Drawing.Size(584, 355);
         this.ResumeLayout(false);

      }

      #endregion

      private System.Windows.Forms.Button buttonEdit;
      private System.Windows.Forms.Button buttonRemove;
      private System.Windows.Forms.Button buttonAdd;
   }
}
