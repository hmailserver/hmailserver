namespace DBSetup.Pages
{
   partial class ucWelcome
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
         this.labelWelcomeText = new System.Windows.Forms.Label();
         this.SuspendLayout();
         // 
         // labelWelcomeText
         // 
         this.labelWelcomeText.Location = new System.Drawing.Point(8, 8);
         this.labelWelcomeText.Name = "labelWelcomeText";
         this.labelWelcomeText.Size = new System.Drawing.Size(368, 56);
         this.labelWelcomeText.TabIndex = 0;
         this.labelWelcomeText.Text = "Using this wizard, you can create a new hMailServer database and select a new def" +
             "ault database.";
         // 
         // ucWelcome
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.labelWelcomeText);
         this.Name = "ucWelcome";
         this.Size = new System.Drawing.Size(466, 208);
         this.ResumeLayout(false);

      }

      #endregion

      private System.Windows.Forms.Label labelWelcomeText;
   }
}
