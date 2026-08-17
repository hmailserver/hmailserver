namespace hMailServer.Administrator
{
   partial class ucPasswordHashing
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
            this.labelAlgorithm = new System.Windows.Forms.Label();
            this.comboAlgorithm = new hMailServer.Administrator.Controls.ucComboBox();
            this.labelMemoryCost = new System.Windows.Forms.Label();
            this.textMemoryCost = new hMailServer.Shared.ucText();
            this.labelIterations = new System.Windows.Forms.Label();
            this.textIterations = new hMailServer.Shared.ucText();
            this.checkAutoUpgrade = new hMailServer.Administrator.Controls.ucCheckbox();
            this.SuspendLayout();
            //
            // labelAlgorithm
            //
            this.labelAlgorithm.AutoSize = true;
            this.labelAlgorithm.Location = new System.Drawing.Point(7, 7);
            this.labelAlgorithm.Name = "labelAlgorithm";
            this.labelAlgorithm.Size = new System.Drawing.Size(50, 13);
            this.labelAlgorithm.TabIndex = 0;
            this.labelAlgorithm.Text = "Algorithm";
            //
            // comboAlgorithm
            //
            this.comboAlgorithm.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboAlgorithm.FormattingEnabled = true;
            this.comboAlgorithm.Location = new System.Drawing.Point(10, 23);
            this.comboAlgorithm.Name = "comboAlgorithm";
            this.comboAlgorithm.Size = new System.Drawing.Size(177, 21);
            this.comboAlgorithm.TabIndex = 1;
            this.comboAlgorithm.SelectedIndexChanged += new System.EventHandler(this.comboAlgorithm_SelectedIndexChanged);
            //
            // labelMemoryCost
            //
            this.labelMemoryCost.AutoSize = true;
            this.labelMemoryCost.Location = new System.Drawing.Point(7, 57);
            this.labelMemoryCost.Name = "labelMemoryCost";
            this.labelMemoryCost.Size = new System.Drawing.Size(126, 13);
            this.labelMemoryCost.TabIndex = 2;
            this.labelMemoryCost.Text = "Memory cost (kilobytes)";
            //
            // textMemoryCost
            //
            this.textMemoryCost.Location = new System.Drawing.Point(10, 73);
            this.textMemoryCost.Name = "textMemoryCost";
            this.textMemoryCost.Number = 0;
            this.textMemoryCost.Number64 = ((long)(0));
            this.textMemoryCost.Numeric = true;
            this.textMemoryCost.Size = new System.Drawing.Size(177, 20);
            this.textMemoryCost.TabIndex = 3;
            //
            // labelIterations
            //
            this.labelIterations.AutoSize = true;
            this.labelIterations.Location = new System.Drawing.Point(7, 107);
            this.labelIterations.Name = "labelIterations";
            this.labelIterations.Size = new System.Drawing.Size(50, 13);
            this.labelIterations.TabIndex = 4;
            this.labelIterations.Text = "Iterations";
            //
            // textIterations
            //
            this.textIterations.Location = new System.Drawing.Point(10, 123);
            this.textIterations.Name = "textIterations";
            this.textIterations.Number = 0;
            this.textIterations.Number64 = ((long)(0));
            this.textIterations.Numeric = true;
            this.textIterations.Size = new System.Drawing.Size(177, 20);
            this.textIterations.TabIndex = 5;
            //
            // checkAutoUpgrade
            //
            this.checkAutoUpgrade.AutoSize = true;
            this.checkAutoUpgrade.Location = new System.Drawing.Point(10, 160);
            this.checkAutoUpgrade.Name = "checkAutoUpgrade";
            this.checkAutoUpgrade.Size = new System.Drawing.Size(260, 17);
            this.checkAutoUpgrade.TabIndex = 6;
            this.checkAutoUpgrade.Text = "Upgrade stored passwords during logon";
            this.checkAutoUpgrade.UseVisualStyleBackColor = true;
            //
            // ucPasswordHashing
            //
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.checkAutoUpgrade);
            this.Controls.Add(this.textIterations);
            this.Controls.Add(this.labelIterations);
            this.Controls.Add(this.textMemoryCost);
            this.Controls.Add(this.labelMemoryCost);
            this.Controls.Add(this.comboAlgorithm);
            this.Controls.Add(this.labelAlgorithm);
            this.Name = "ucPasswordHashing";
            this.Size = new System.Drawing.Size(596, 495);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelAlgorithm;
        private Controls.ucComboBox comboAlgorithm;
        private System.Windows.Forms.Label labelMemoryCost;
        private hMailServer.Shared.ucText textMemoryCost;
        private System.Windows.Forms.Label labelIterations;
        private hMailServer.Shared.ucText textIterations;
        private Controls.ucCheckbox checkAutoUpgrade;

    }
}
