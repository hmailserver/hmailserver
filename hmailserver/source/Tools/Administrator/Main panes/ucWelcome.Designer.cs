namespace hMailServer.Administrator
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
            this.buttonAddDomain = new System.Windows.Forms.Button();
            this.labelGettingStarted = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // buttonAddDomain
            // 
            this.buttonAddDomain.Location = new System.Drawing.Point(26, 33);
            this.buttonAddDomain.Name = "buttonAddDomain";
            this.buttonAddDomain.Size = new System.Drawing.Size(126, 25);
            this.buttonAddDomain.TabIndex = 0;
            this.buttonAddDomain.Text = "Add domain...";
            this.buttonAddDomain.UseVisualStyleBackColor = true;
            this.buttonAddDomain.Click += new System.EventHandler(this.buttonAddDomain_Click);
            // 
            // labelGettingStarted
            // 
            this.labelGettingStarted.AutoSize = true;
            this.labelGettingStarted.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelGettingStarted.Location = new System.Drawing.Point(8, 8);
            this.labelGettingStarted.Name = "labelGettingStarted";
            this.labelGettingStarted.Size = new System.Drawing.Size(94, 13);
            this.labelGettingStarted.TabIndex = 1;
            this.labelGettingStarted.Text = "Getting started";
            // 
            // ucWelcome
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.labelGettingStarted);
            this.Controls.Add(this.buttonAddDomain);
            this.Name = "ucWelcome";
            this.Size = new System.Drawing.Size(573, 447);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonAddDomain;
        private System.Windows.Forms.Label labelGettingStarted;
    }
}
