namespace hMailServer.Administrator
{
    partial class ucServerSendout
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
           this.labelSendTo = new System.Windows.Forms.Label();
           this.labelEmail = new System.Windows.Forms.Label();
           this.labelFromName = new System.Windows.Forms.Label();
           this.labelFromAddress = new System.Windows.Forms.Label();
           this.labelSubject = new System.Windows.Forms.Label();
           this.labelBody = new System.Windows.Forms.Label();
           this.buttonSend = new System.Windows.Forms.Button();
           this.textWildcard = new hMailServer.Shared.ucText();
           this.textBody = new hMailServer.Shared.ucText();
           this.textSubject = new hMailServer.Shared.ucText();
           this.textFromAddress = new hMailServer.Shared.ucText();
           this.textFromName = new hMailServer.Shared.ucText();
           this.radioMatchingWildcard = new hMailServer.Administrator.Controls.ucRadioButton();
           this.comboDomains = new hMailServer.Administrator.Controls.ucComboBox();
           this.radioSpecificDomain = new hMailServer.Administrator.Controls.ucRadioButton();
           this.radioToAllAccounts = new hMailServer.Administrator.Controls.ucRadioButton();
           this.SuspendLayout();
           // 
           // labelSendTo
           // 
           this.labelSendTo.AutoSize = true;
           this.labelSendTo.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelSendTo.Location = new System.Drawing.Point(8, 11);
           this.labelSendTo.Name = "labelSendTo";
           this.labelSendTo.Size = new System.Drawing.Size(51, 13);
           this.labelSendTo.TabIndex = 0;
           this.labelSendTo.Text = "Send to";
           // 
           // labelEmail
           // 
           this.labelEmail.AutoSize = true;
           this.labelEmail.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelEmail.Location = new System.Drawing.Point(11, 176);
           this.labelEmail.Name = "labelEmail";
           this.labelEmail.Size = new System.Drawing.Size(37, 13);
           this.labelEmail.TabIndex = 5;
           this.labelEmail.Text = "Email";
           // 
           // labelFromName
           // 
           this.labelFromName.AutoSize = true;
           this.labelFromName.Location = new System.Drawing.Point(32, 200);
           this.labelFromName.Name = "labelFromName";
           this.labelFromName.Size = new System.Drawing.Size(67, 13);
           this.labelFromName.TabIndex = 18;
           this.labelFromName.Text = "From (Name)";
           // 
           // labelFromAddress
           // 
           this.labelFromAddress.AutoSize = true;
           this.labelFromAddress.Location = new System.Drawing.Point(29, 245);
           this.labelFromAddress.Name = "labelFromAddress";
           this.labelFromAddress.Size = new System.Drawing.Size(77, 13);
           this.labelFromAddress.TabIndex = 20;
           this.labelFromAddress.Text = "From (Address)";
           // 
           // labelSubject
           // 
           this.labelSubject.AutoSize = true;
           this.labelSubject.Location = new System.Drawing.Point(29, 288);
           this.labelSubject.Name = "labelSubject";
           this.labelSubject.Size = new System.Drawing.Size(43, 13);
           this.labelSubject.TabIndex = 22;
           this.labelSubject.Text = "Subject";
           // 
           // labelBody
           // 
           this.labelBody.AutoSize = true;
           this.labelBody.Location = new System.Drawing.Point(224, 200);
           this.labelBody.Name = "labelBody";
           this.labelBody.Size = new System.Drawing.Size(31, 13);
           this.labelBody.TabIndex = 23;
           this.labelBody.Text = "Body";
           // 
           // buttonSend
           // 
           this.buttonSend.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonSend.Location = new System.Drawing.Point(472, 400);
           this.buttonSend.Name = "buttonSend";
           this.buttonSend.Size = new System.Drawing.Size(100, 25);
           this.buttonSend.TabIndex = 25;
           this.buttonSend.Text = "Send";
           this.buttonSend.UseVisualStyleBackColor = true;
           this.buttonSend.Click += new System.EventHandler(this.buttonSend_Click);
           // 
           // textWildcard
           // 
           this.textWildcard.Location = new System.Drawing.Point(48, 144);
           this.textWildcard.Name = "textWildcard";
           this.textWildcard.Number = 0;
           this.textWildcard.Numeric = false;
           this.textWildcard.Size = new System.Drawing.Size(175, 20);
           this.textWildcard.TabIndex = 26;
           // 
           // textBody
           // 
           this.textBody.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.textBody.Location = new System.Drawing.Point(222, 216);
           this.textBody.Multiline = true;
           this.textBody.Name = "textBody";
           this.textBody.Number = 0;
           this.textBody.Numeric = false;
           this.textBody.Size = new System.Drawing.Size(355, 174);
           this.textBody.TabIndex = 24;
           // 
           // textSubject
           // 
           this.textSubject.Location = new System.Drawing.Point(32, 304);
           this.textSubject.Name = "textSubject";
           this.textSubject.Number = 0;
           this.textSubject.Numeric = false;
           this.textSubject.Size = new System.Drawing.Size(175, 20);
           this.textSubject.TabIndex = 21;
           // 
           // textFromAddress
           // 
           this.textFromAddress.Location = new System.Drawing.Point(32, 261);
           this.textFromAddress.Name = "textFromAddress";
           this.textFromAddress.Number = 0;
           this.textFromAddress.Numeric = false;
           this.textFromAddress.Size = new System.Drawing.Size(175, 20);
           this.textFromAddress.TabIndex = 19;
           // 
           // textFromName
           // 
           this.textFromName.Location = new System.Drawing.Point(32, 216);
           this.textFromName.Name = "textFromName";
           this.textFromName.Number = 0;
           this.textFromName.Numeric = false;
           this.textFromName.Size = new System.Drawing.Size(175, 20);
           this.textFromName.TabIndex = 17;
           // 
           // radioMatchingWildcard
           // 
           this.radioMatchingWildcard.AutoSize = true;
           this.radioMatchingWildcard.Location = new System.Drawing.Point(32, 120);
           this.radioMatchingWildcard.Name = "radioMatchingWildcard";
           this.radioMatchingWildcard.Size = new System.Drawing.Size(158, 17);
           this.radioMatchingWildcard.TabIndex = 4;
           this.radioMatchingWildcard.Text = "Accounts matching wildcard";
           this.radioMatchingWildcard.UseVisualStyleBackColor = true;
           this.radioMatchingWildcard.CheckedChanged += new System.EventHandler(this.radioMatchingWildcard_CheckedChanged);
           // 
           // comboDomains
           // 
           this.comboDomains.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
           this.comboDomains.FormattingEnabled = true;
           this.comboDomains.Location = new System.Drawing.Point(50, 85);
           this.comboDomains.Name = "comboDomains";
           this.comboDomains.Size = new System.Drawing.Size(174, 21);
           this.comboDomains.TabIndex = 3;
           // 
           // radioSpecificDomain
           // 
           this.radioSpecificDomain.AutoSize = true;
           this.radioSpecificDomain.Location = new System.Drawing.Point(29, 59);
           this.radioSpecificDomain.Name = "radioSpecificDomain";
           this.radioSpecificDomain.Size = new System.Drawing.Size(100, 17);
           this.radioSpecificDomain.TabIndex = 2;
           this.radioSpecificDomain.Text = "Specific domain";
           this.radioSpecificDomain.UseVisualStyleBackColor = true;
           this.radioSpecificDomain.CheckedChanged += new System.EventHandler(this.radioSpecificDomain_CheckedChanged);
           // 
           // radioToAllAccounts
           // 
           this.radioToAllAccounts.AutoSize = true;
           this.radioToAllAccounts.Checked = true;
           this.radioToAllAccounts.Location = new System.Drawing.Point(29, 36);
           this.radioToAllAccounts.Name = "radioToAllAccounts";
           this.radioToAllAccounts.Size = new System.Drawing.Size(83, 17);
           this.radioToAllAccounts.TabIndex = 1;
           this.radioToAllAccounts.TabStop = true;
           this.radioToAllAccounts.Text = "All accounts";
           this.radioToAllAccounts.UseVisualStyleBackColor = true;
           this.radioToAllAccounts.CheckedChanged += new System.EventHandler(this.radioToAllAccounts_CheckedChanged);
           // 
           // ucServerSendout
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.textWildcard);
           this.Controls.Add(this.buttonSend);
           this.Controls.Add(this.textBody);
           this.Controls.Add(this.labelBody);
           this.Controls.Add(this.labelSubject);
           this.Controls.Add(this.textSubject);
           this.Controls.Add(this.labelFromAddress);
           this.Controls.Add(this.textFromAddress);
           this.Controls.Add(this.labelFromName);
           this.Controls.Add(this.textFromName);
           this.Controls.Add(this.labelEmail);
           this.Controls.Add(this.radioMatchingWildcard);
           this.Controls.Add(this.comboDomains);
           this.Controls.Add(this.radioSpecificDomain);
           this.Controls.Add(this.radioToAllAccounts);
           this.Controls.Add(this.labelSendTo);
           this.Name = "ucServerSendout";
           this.Size = new System.Drawing.Size(589, 436);
           this.ResumeLayout(false);
           this.PerformLayout();

        }

        #endregion

       private System.Windows.Forms.Label labelSendTo;
       private hMailServer.Administrator.Controls.ucRadioButton radioToAllAccounts;
       private hMailServer.Administrator.Controls.ucRadioButton radioSpecificDomain;
       private hMailServer.Administrator.Controls.ucComboBox comboDomains;
       private hMailServer.Administrator.Controls.ucRadioButton radioMatchingWildcard;
       private System.Windows.Forms.Label labelEmail;
       private System.Windows.Forms.Label labelFromName;
       private hMailServer.Shared.ucText textFromName;
       private System.Windows.Forms.Label labelFromAddress;
       private hMailServer.Shared.ucText textFromAddress;
       private System.Windows.Forms.Label labelSubject;
       private hMailServer.Shared.ucText textSubject;
       private System.Windows.Forms.Label labelBody;
       private hMailServer.Shared.ucText textBody;
       private System.Windows.Forms.Button buttonSend;
       private hMailServer.Shared.ucText textWildcard;
    }
}
