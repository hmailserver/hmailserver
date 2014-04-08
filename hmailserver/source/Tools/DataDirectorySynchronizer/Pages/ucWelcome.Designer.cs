namespace DataDirectorySynchronizer.Pages
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
         this.labelInfo = new System.Windows.Forms.Label();
         this.SuspendLayout();
         // 
         // labelInfo
         // 
         this.labelInfo.Location = new System.Drawing.Point(16, 8);
         this.labelInfo.Name = "labelInfo";
         this.labelInfo.Size = new System.Drawing.Size(379, 53);
         this.labelInfo.TabIndex = 0;
         this.labelInfo.Text = "This utility will synchronize your hMailServer data directory with your hMailServ" +
             "er database. It is very important that you backup both your database and your da" +
             "ta directory before proceeding.";
         // 
         // ucWelcome
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.labelInfo);
         this.Name = "ucWelcome";
         this.Size = new System.Drawing.Size(417, 202);
         this.ResumeLayout(false);

      }

      #endregion

      private System.Windows.Forms.Label labelInfo;
   }
}
