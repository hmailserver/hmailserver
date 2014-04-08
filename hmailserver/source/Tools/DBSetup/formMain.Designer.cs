using hMailServer.Shared;

namespace DBSetup
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
          this.wizard.Margin = new System.Windows.Forms.Padding(5, 5, 5, 5);
          this.wizard.Name = "wizard";
          this.wizard.Size = new System.Drawing.Size(831, 464);
          this.wizard.TabIndex = 0;
          this.wizard.PageChanged += new hMailServer.Shared.ucWizard.PageChangedEventHandler(this.wizard_PageChanged);
          this.wizard.Cancel += new System.EventHandler(this.wizard_OnCancel);
          // 
          // formMain
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.ClientSize = new System.Drawing.Size(831, 464);
          this.Controls.Add(this.wizard);
          this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
          this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
          this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
          this.MaximizeBox = false;
          this.Name = "formMain";
          this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
          this.Text = "hMailServer Database Setup";
          this.Shown += new System.EventHandler(this.formMain_Shown);
          this.ResumeLayout(false);

      }

      #endregion

      private ucWizard wizard;
   }
}

