namespace TestBedUI
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
            this.labelFrom = new System.Windows.Forms.Label();
            this.textFrom = new System.Windows.Forms.TextBox();
            this.textTo = new System.Windows.Forms.TextBox();
            this.labelTo = new System.Windows.Forms.Label();
            this.textSubject = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.buttonSend = new System.Windows.Forms.Button();
            this.buttonSendWithEICAR = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // labelFrom
            // 
            this.labelFrom.AutoSize = true;
            this.labelFrom.Location = new System.Drawing.Point(13, 13);
            this.labelFrom.Name = "labelFrom";
            this.labelFrom.Size = new System.Drawing.Size(44, 17);
            this.labelFrom.TabIndex = 0;
            this.labelFrom.Text = "From:";
            // 
            // textFrom
            // 
            this.textFrom.Location = new System.Drawing.Point(16, 34);
            this.textFrom.Name = "textFrom";
            this.textFrom.Size = new System.Drawing.Size(185, 22);
            this.textFrom.TabIndex = 1;
            this.textFrom.Text = "test@test.com";
            // 
            // textTo
            // 
            this.textTo.Location = new System.Drawing.Point(16, 86);
            this.textTo.Name = "textTo";
            this.textTo.Size = new System.Drawing.Size(185, 22);
            this.textTo.TabIndex = 3;
            this.textTo.Text = "test@test.com";
            // 
            // labelTo
            // 
            this.labelTo.AutoSize = true;
            this.labelTo.Location = new System.Drawing.Point(13, 65);
            this.labelTo.Name = "labelTo";
            this.labelTo.Size = new System.Drawing.Size(29, 17);
            this.labelTo.TabIndex = 2;
            this.labelTo.Text = "To:";
            // 
            // textSubject
            // 
            this.textSubject.Location = new System.Drawing.Point(245, 34);
            this.textSubject.Name = "textSubject";
            this.textSubject.Size = new System.Drawing.Size(185, 22);
            this.textSubject.TabIndex = 5;
            this.textSubject.Text = "Test message";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(242, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(59, 17);
            this.label1.TabIndex = 4;
            this.label1.Text = "Subject:";
            // 
            // buttonSend
            // 
            this.buttonSend.Location = new System.Drawing.Point(209, 147);
            this.buttonSend.Name = "buttonSend";
            this.buttonSend.Size = new System.Drawing.Size(75, 23);
            this.buttonSend.TabIndex = 6;
            this.buttonSend.Text = "&Send";
            this.buttonSend.UseVisualStyleBackColor = true;
            this.buttonSend.Click += new System.EventHandler(this.buttonSend_Click);
            // 
            // buttonSendWithEICAR
            // 
            this.buttonSendWithEICAR.Location = new System.Drawing.Point(290, 147);
            this.buttonSendWithEICAR.Name = "buttonSendWithEICAR";
            this.buttonSendWithEICAR.Size = new System.Drawing.Size(140, 23);
            this.buttonSendWithEICAR.TabIndex = 7;
            this.buttonSendWithEICAR.Text = "Send with &EICAR";
            this.buttonSendWithEICAR.UseVisualStyleBackColor = true;
            this.buttonSendWithEICAR.Click += new System.EventHandler(this.buttonSendWithEICAR_Click);
            // 
            // formMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(631, 348);
            this.Controls.Add(this.buttonSendWithEICAR);
            this.Controls.Add(this.buttonSend);
            this.Controls.Add(this.textSubject);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textTo);
            this.Controls.Add(this.labelTo);
            this.Controls.Add(this.textFrom);
            this.Controls.Add(this.labelFrom);
            this.Name = "formMain";
            this.Text = "Test Bed";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelFrom;
        private System.Windows.Forms.TextBox textFrom;
        private System.Windows.Forms.TextBox textTo;
        private System.Windows.Forms.Label labelTo;
        private System.Windows.Forms.TextBox textSubject;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button buttonSend;
        private System.Windows.Forms.Button buttonSendWithEICAR;
    }
}

