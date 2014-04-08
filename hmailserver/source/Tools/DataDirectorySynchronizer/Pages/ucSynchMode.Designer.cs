namespace DataDirectorySynchronizer.Pages
{
    partial class ucSynchMode
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
           System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ucSynchMode));
           this.radioImport = new System.Windows.Forms.RadioButton();
           this.radioDelete = new System.Windows.Forms.RadioButton();
           this.labelHelper = new System.Windows.Forms.Label();
           this.SuspendLayout();
           // 
           // radioImport
           // 
           this.radioImport.AutoSize = true;
           this.radioImport.Checked = true;
           this.radioImport.Location = new System.Drawing.Point(18, 18);
           this.radioImport.Name = "radioImport";
           this.radioImport.Size = new System.Drawing.Size(335, 17);
           this.radioImport.TabIndex = 0;
           this.radioImport.TabStop = true;
           this.radioImport.Text = "Import messages which are not in the database into the database.";
           this.radioImport.UseVisualStyleBackColor = true;
           // 
           // radioDelete
           // 
           this.radioDelete.AutoSize = true;
           this.radioDelete.Location = new System.Drawing.Point(18, 41);
           this.radioDelete.Name = "radioDelete";
           this.radioDelete.Size = new System.Drawing.Size(294, 17);
           this.radioDelete.TabIndex = 1;
           this.radioDelete.TabStop = true;
           this.radioDelete.Text = "Deletes message which are not in the database from disk";
           this.radioDelete.UseVisualStyleBackColor = true;
           // 
           // labelHelper
           // 
           this.labelHelper.Location = new System.Drawing.Point(20, 82);
           this.labelHelper.Name = "labelHelper";
           this.labelHelper.Size = new System.Drawing.Size(391, 60);
           this.labelHelper.TabIndex = 2;
           this.labelHelper.Text = resources.GetString("labelHelper.Text");
           // 
           // ucSynchMode
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.labelHelper);
           this.Controls.Add(this.radioDelete);
           this.Controls.Add(this.radioImport);
           this.Name = "ucSynchMode";
           this.Size = new System.Drawing.Size(427, 184);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.RadioButton radioImport;
        private System.Windows.Forms.RadioButton radioDelete;
        private System.Windows.Forms.Label labelHelper;
    }
}
