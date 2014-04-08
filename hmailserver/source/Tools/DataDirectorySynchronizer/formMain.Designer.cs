using hMailServer.Shared;
namespace DataDirectorySynchronizer
{
   partial class formMain
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formMain));
         this.wizard = new hMailServer.Shared.ucWizard();
         this.SuspendLayout();
         // 
         // wizard
         // 
         this.wizard.Dock = System.Windows.Forms.DockStyle.Fill;
         this.wizard.Location = new System.Drawing.Point(0, 0);
         this.wizard.Name = "wizard";
         this.wizard.Size = new System.Drawing.Size(835, 447);
         this.wizard.TabIndex = 0;
         this.wizard.Cancel += new System.EventHandler(this.wizard_Cancel);
         // 
         // formMain
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(835, 447);
         this.Controls.Add(this.wizard);
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.MinimumSize = new System.Drawing.Size(800, 350);
         this.Name = "formMain";
         this.Text = "Data Directory Synchronizer";
         this.Shown += new System.EventHandler(this.formMain_Shown);
         this.ResumeLayout(false);

      }

      #endregion

      private ucWizard wizard;
   }
}

