namespace DBSetup.Pages
{
   partial class ucCompleted
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
         this.labelOperationPerformed = new System.Windows.Forms.Label();
         this.SuspendLayout();
         // 
         // labelOperationPerformed
         // 
         this.labelOperationPerformed.AutoSize = true;
         this.labelOperationPerformed.Location = new System.Drawing.Point(8, 8);
         this.labelOperationPerformed.Name = "labelOperationPerformed";
         this.labelOperationPerformed.Size = new System.Drawing.Size(277, 13);
         this.labelOperationPerformed.TabIndex = 1;
         this.labelOperationPerformed.Text = "The wizard has completed. Press Close to exit the wizard.";
         // 
         // ucCompleted
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.labelOperationPerformed);
         this.Name = "ucCompleted";
         this.Size = new System.Drawing.Size(499, 196);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Label labelOperationPerformed;
   }
}
