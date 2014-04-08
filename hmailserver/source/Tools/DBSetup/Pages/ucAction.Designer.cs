namespace DBSetup.Pages
{
   partial class ucAction
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
         this.radioCreateNew = new System.Windows.Forms.RadioButton();
         this.radioSelectNew = new System.Windows.Forms.RadioButton();
         this.labelCreateNewHelp = new System.Windows.Forms.Label();
         this.label1 = new System.Windows.Forms.Label();
         this.SuspendLayout();
         // 
         // radioCreateNew
         // 
         this.radioCreateNew.AutoSize = true;
         this.radioCreateNew.Checked = true;
         this.radioCreateNew.Location = new System.Drawing.Point(8, 8);
         this.radioCreateNew.Name = "radioCreateNew";
         this.radioCreateNew.Size = new System.Drawing.Size(194, 17);
         this.radioCreateNew.TabIndex = 0;
         this.radioCreateNew.TabStop = true;
         this.radioCreateNew.Text = "Create a new hMailServer database";
         this.radioCreateNew.UseVisualStyleBackColor = true;
         // 
         // radioSelectNew
         // 
         this.radioSelectNew.AutoSize = true;
         this.radioSelectNew.Location = new System.Drawing.Point(8, 80);
         this.radioSelectNew.Name = "radioSelectNew";
         this.radioSelectNew.Size = new System.Drawing.Size(228, 17);
         this.radioSelectNew.TabIndex = 1;
         this.radioSelectNew.Text = "Select a new hMailServer default database";
         this.radioSelectNew.UseVisualStyleBackColor = true;
         // 
         // labelCreateNewHelp
         // 
         this.labelCreateNewHelp.Location = new System.Drawing.Point(32, 32);
         this.labelCreateNewHelp.Name = "labelCreateNewHelp";
         this.labelCreateNewHelp.Size = new System.Drawing.Size(416, 48);
         this.labelCreateNewHelp.TabIndex = 2;
         this.labelCreateNewHelp.Text = "Select this option if you do not already have a hMailServer database and you want" +
             " to create one now. hMailServer requires one database to run.";
         // 
         // label1
         // 
         this.label1.Location = new System.Drawing.Point(32, 104);
         this.label1.Name = "label1";
         this.label1.Size = new System.Drawing.Size(416, 48);
         this.label1.TabIndex = 3;
         this.label1.Text = "Select this option if you want to change the default hMailServer server or databa" +
             "se.";
         // 
         // ucAction
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.label1);
         this.Controls.Add(this.labelCreateNewHelp);
         this.Controls.Add(this.radioSelectNew);
         this.Controls.Add(this.radioCreateNew);
         this.Name = "ucAction";
         this.Size = new System.Drawing.Size(472, 195);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.RadioButton radioCreateNew;
      private System.Windows.Forms.RadioButton radioSelectNew;
      private System.Windows.Forms.Label labelCreateNewHelp;
      private System.Windows.Forms.Label label1;
   }
}
