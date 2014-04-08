namespace hMailServer.Administrator
{
   partial class ucGlobalRules
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
         this.rules = new hMailServer.Administrator.ucRules();
         this.SuspendLayout();
         // 
         // rules
         // 
         this.rules.Dock = System.Windows.Forms.DockStyle.Fill;
         this.rules.Location = new System.Drawing.Point(0, 0);
         this.rules.Name = "rules";
         this.rules.Size = new System.Drawing.Size(789, 461);
         this.rules.TabIndex = 0;
         // 
         // ucGlobalRules
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.rules);
         this.Name = "ucGlobalRules";
         this.Size = new System.Drawing.Size(789, 461);
         this.ResumeLayout(false);

      }

      #endregion

      private ucRules rules;
   }
}
