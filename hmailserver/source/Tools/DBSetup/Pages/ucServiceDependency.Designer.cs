namespace DBSetup.Pages
{
   partial class ucServiceDependency
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
         System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ucServiceDependency));
         this.labelDependency = new System.Windows.Forms.Label();
         this.comboServices = new System.Windows.Forms.ComboBox();
         this.labelService = new System.Windows.Forms.Label();
         this.SuspendLayout();
         // 
         // labelDependency
         // 
         this.labelDependency.Location = new System.Drawing.Point(8, 8);
         this.labelDependency.Name = "labelDependency";
         this.labelDependency.Size = new System.Drawing.Size(424, 88);
         this.labelDependency.TabIndex = 0;
         this.labelDependency.Text = resources.GetString("labelDependency.Text");
         // 
         // comboServices
         // 
         this.comboServices.FormattingEnabled = true;
         this.comboServices.Location = new System.Drawing.Point(16, 120);
         this.comboServices.Name = "comboServices";
         this.comboServices.Size = new System.Drawing.Size(368, 21);
         this.comboServices.Sorted = true;
         this.comboServices.TabIndex = 1;
         // 
         // labelService
         // 
         this.labelService.AutoSize = true;
         this.labelService.Location = new System.Drawing.Point(16, 104);
         this.labelService.Name = "labelService";
         this.labelService.Size = new System.Drawing.Size(43, 13);
         this.labelService.TabIndex = 2;
         this.labelService.Text = "Service";
         // 
         // ucServiceDependency
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.labelService);
         this.Controls.Add(this.comboServices);
         this.Controls.Add(this.labelDependency);
         this.Name = "ucServiceDependency";
         this.Size = new System.Drawing.Size(451, 224);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Label labelDependency;
      private System.Windows.Forms.ComboBox comboServices;
      private System.Windows.Forms.Label labelService;
   }
}
