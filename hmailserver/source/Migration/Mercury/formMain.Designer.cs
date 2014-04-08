namespace MercuryMigrate
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
         this.wizard = new DBSetup.ucWizard();
         this.SuspendLayout();
         // 
         // wizard
         // 
         this.wizard.Dock = System.Windows.Forms.DockStyle.Fill;
         this.wizard.Location = new System.Drawing.Point(0, 0);
         this.wizard.Name = "wizard";
         this.wizard.Size = new System.Drawing.Size(623, 377);
         this.wizard.TabIndex = 0;
         this.wizard.PageChanged += new DBSetup.ucWizard.PageChangedEventHandler(this.wizard_PageChanged);
         this.wizard.Cancel += new System.EventHandler(this.wizard_OnCancel);
         // 
         // formMain
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.ClientSize = new System.Drawing.Size(623, 377);
         this.Controls.Add(this.wizard);
         this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
         this.MaximizeBox = false;
         this.MaximumSize = new System.Drawing.Size(779, 409);
         this.MinimumSize = new System.Drawing.Size(582, 317);
         this.Name = "formMain";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
         this.Text = "Mercury to hMailServer Migration";
         this.Shown += new System.EventHandler(this.formMain_Shown);
         this.ResumeLayout(false);

      }

      #endregion

      private DBSetup.ucWizard wizard;
   }
}

