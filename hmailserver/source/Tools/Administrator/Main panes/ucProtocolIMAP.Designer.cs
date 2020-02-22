namespace hMailServer.Administrator
{
    partial class ucProtocolIMAP
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
         this.tabControl = new System.Windows.Forms.TabControl();
         this.tabPage1 = new System.Windows.Forms.TabPage();
         this.textWelcomeMessage = new hMailServer.Shared.ucText();
         this.labelWelcome = new System.Windows.Forms.Label();
         this.labelOther = new System.Windows.Forms.Label();
         this.textMaxIMAPConnections = new hMailServer.Shared.ucText();
         this.labelMaxPOP3Connections = new System.Windows.Forms.Label();
         this.labelConnections = new System.Windows.Forms.Label();
         this.tabPage3 = new System.Windows.Forms.TabPage();
         this.buttonEditFolders = new System.Windows.Forms.Button();
         this.textIMAPPublicFolderName = new hMailServer.Shared.ucText();
         this.labelPublicFolderName = new System.Windows.Forms.Label();
         this.tabPage2 = new System.Windows.Forms.TabPage();
         this.labelIMAPMasterUser = new System.Windows.Forms.Label();
         this.textIMAPMasterUser = new hMailServer.Shared.ucText();
         this.checkEnableIMAPInitialResponse = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkEnableIMAPPlain = new hMailServer.Administrator.Controls.ucCheckbox();
         this.labelHierarchyDelimiter = new System.Windows.Forms.Label();
         this.comboIMAPHierarchyDelimiter = new hMailServer.Administrator.Controls.ucComboBox();
         this.labelOther2 = new System.Windows.Forms.Label();
         this.checkEnableIMAPACL = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkEnableIMAPIdle = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkEnableIMAPQuota = new hMailServer.Administrator.Controls.ucCheckbox();
         this.labelExtensions = new System.Windows.Forms.Label();
         this.checkEnableIMAPSort = new hMailServer.Administrator.Controls.ucCheckbox();
         this.tabControl.SuspendLayout();
         this.tabPage1.SuspendLayout();
         this.tabPage3.SuspendLayout();
         this.tabPage2.SuspendLayout();
         this.SuspendLayout();
         // 
         // tabControl
         // 
         this.tabControl.Controls.Add(this.tabPage1);
         this.tabControl.Controls.Add(this.tabPage3);
         this.tabControl.Controls.Add(this.tabPage2);
         this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
         this.tabControl.Location = new System.Drawing.Point(0, 0);
         this.tabControl.Name = "tabControl";
         this.tabControl.SelectedIndex = 0;
         this.tabControl.Size = new System.Drawing.Size(497, 346);
         this.tabControl.TabIndex = 0;
         // 
         // tabPage1
         // 
         this.tabPage1.Controls.Add(this.textWelcomeMessage);
         this.tabPage1.Controls.Add(this.labelWelcome);
         this.tabPage1.Controls.Add(this.labelOther);
         this.tabPage1.Controls.Add(this.textMaxIMAPConnections);
         this.tabPage1.Controls.Add(this.labelMaxPOP3Connections);
         this.tabPage1.Controls.Add(this.labelConnections);
         this.tabPage1.Location = new System.Drawing.Point(4, 22);
         this.tabPage1.Name = "tabPage1";
         this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
         this.tabPage1.Size = new System.Drawing.Size(489, 320);
         this.tabPage1.TabIndex = 0;
         this.tabPage1.Text = "General";
         this.tabPage1.UseVisualStyleBackColor = true;
         // 
         // textWelcomeMessage
         // 
         this.textWelcomeMessage.Location = new System.Drawing.Point(27, 127);
         this.textWelcomeMessage.Name = "textWelcomeMessage";
         this.textWelcomeMessage.Number = 0;
         this.textWelcomeMessage.Number64 = ((long)(0));
         this.textWelcomeMessage.Numeric = false;
         this.textWelcomeMessage.Size = new System.Drawing.Size(227, 20);
         this.textWelcomeMessage.TabIndex = 16;
         // 
         // labelWelcome
         // 
         this.labelWelcome.AutoSize = true;
         this.labelWelcome.Location = new System.Drawing.Point(25, 108);
         this.labelWelcome.Name = "labelWelcome";
         this.labelWelcome.Size = new System.Drawing.Size(97, 13);
         this.labelWelcome.TabIndex = 15;
         this.labelWelcome.Text = "Welcome message";
         // 
         // labelOther
         // 
         this.labelOther.AutoSize = true;
         this.labelOther.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelOther.Location = new System.Drawing.Point(8, 86);
         this.labelOther.Name = "labelOther";
         this.labelOther.Size = new System.Drawing.Size(38, 13);
         this.labelOther.TabIndex = 14;
         this.labelOther.Text = "Other";
         // 
         // textMaxIMAPConnections
         // 
         this.textMaxIMAPConnections.Location = new System.Drawing.Point(26, 52);
         this.textMaxIMAPConnections.Name = "textMaxIMAPConnections";
         this.textMaxIMAPConnections.Number = 0;
         this.textMaxIMAPConnections.Number64 = ((long)(0));
         this.textMaxIMAPConnections.Numeric = true;
         this.textMaxIMAPConnections.Size = new System.Drawing.Size(117, 20);
         this.textMaxIMAPConnections.TabIndex = 13;
         // 
         // labelMaxPOP3Connections
         // 
         this.labelMaxPOP3Connections.AutoSize = true;
         this.labelMaxPOP3Connections.Location = new System.Drawing.Point(24, 33);
         this.labelMaxPOP3Connections.Name = "labelMaxPOP3Connections";
         this.labelMaxPOP3Connections.Size = new System.Drawing.Size(300, 13);
         this.labelMaxPOP3Connections.TabIndex = 12;
         this.labelMaxPOP3Connections.Text = "Maximum number of simultaneous connections (0 for unlimited)";
         // 
         // labelConnections
         // 
         this.labelConnections.AutoSize = true;
         this.labelConnections.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelConnections.Location = new System.Drawing.Point(8, 8);
         this.labelConnections.Name = "labelConnections";
         this.labelConnections.Size = new System.Drawing.Size(77, 13);
         this.labelConnections.TabIndex = 11;
         this.labelConnections.Text = "Connections";
         // 
         // tabPage3
         // 
         this.tabPage3.Controls.Add(this.buttonEditFolders);
         this.tabPage3.Controls.Add(this.textIMAPPublicFolderName);
         this.tabPage3.Controls.Add(this.labelPublicFolderName);
         this.tabPage3.Location = new System.Drawing.Point(4, 22);
         this.tabPage3.Name = "tabPage3";
         this.tabPage3.Size = new System.Drawing.Size(489, 320);
         this.tabPage3.TabIndex = 2;
         this.tabPage3.Text = "Public folders";
         this.tabPage3.UseVisualStyleBackColor = true;
         // 
         // buttonEditFolders
         // 
         this.buttonEditFolders.Location = new System.Drawing.Point(16, 64);
         this.buttonEditFolders.Name = "buttonEditFolders";
         this.buttonEditFolders.Size = new System.Drawing.Size(100, 25);
         this.buttonEditFolders.TabIndex = 23;
         this.buttonEditFolders.Text = "Edit folders";
         this.buttonEditFolders.UseVisualStyleBackColor = true;
         this.buttonEditFolders.Click += new System.EventHandler(this.buttonEditFolders_Click);
         // 
         // textIMAPPublicFolderName
         // 
         this.textIMAPPublicFolderName.Location = new System.Drawing.Point(16, 32);
         this.textIMAPPublicFolderName.Name = "textIMAPPublicFolderName";
         this.textIMAPPublicFolderName.Number = 0;
         this.textIMAPPublicFolderName.Number64 = ((long)(0));
         this.textIMAPPublicFolderName.Numeric = false;
         this.textIMAPPublicFolderName.Size = new System.Drawing.Size(139, 20);
         this.textIMAPPublicFolderName.TabIndex = 22;
         // 
         // labelPublicFolderName
         // 
         this.labelPublicFolderName.AutoSize = true;
         this.labelPublicFolderName.Location = new System.Drawing.Point(14, 13);
         this.labelPublicFolderName.Name = "labelPublicFolderName";
         this.labelPublicFolderName.Size = new System.Drawing.Size(94, 13);
         this.labelPublicFolderName.TabIndex = 21;
         this.labelPublicFolderName.Text = "Public folder name";
         // 
         // tabPage2
         // 
         this.tabPage2.Controls.Add(this.labelIMAPMasterUser);
         this.tabPage2.Controls.Add(this.textIMAPMasterUser);
         this.tabPage2.Controls.Add(this.checkEnableIMAPInitialResponse);
         this.tabPage2.Controls.Add(this.checkEnableIMAPPlain);
         this.tabPage2.Controls.Add(this.labelHierarchyDelimiter);
         this.tabPage2.Controls.Add(this.comboIMAPHierarchyDelimiter);
         this.tabPage2.Controls.Add(this.labelOther2);
         this.tabPage2.Controls.Add(this.checkEnableIMAPACL);
         this.tabPage2.Controls.Add(this.checkEnableIMAPIdle);
         this.tabPage2.Controls.Add(this.checkEnableIMAPQuota);
         this.tabPage2.Controls.Add(this.labelExtensions);
         this.tabPage2.Controls.Add(this.checkEnableIMAPSort);
         this.tabPage2.Location = new System.Drawing.Point(4, 22);
         this.tabPage2.Name = "tabPage2";
         this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
         this.tabPage2.Size = new System.Drawing.Size(489, 320);
         this.tabPage2.TabIndex = 1;
         this.tabPage2.Text = "Advanced";
         this.tabPage2.UseVisualStyleBackColor = true;
         // 
         // labelIMAPMasterUser
         // 
         this.labelIMAPMasterUser.AutoSize = true;
         this.labelIMAPMasterUser.Location = new System.Drawing.Point(53, 176);
         this.labelIMAPMasterUser.Name = "labelIMAPMasterUser";
         this.labelIMAPMasterUser.Size = new System.Drawing.Size(91, 13);
         this.labelIMAPMasterUser.TabIndex = 27;
         this.labelIMAPMasterUser.Text = "IMAP Master user";
         // 
         // textIMAPMasterUser
         // 
         this.textIMAPMasterUser.Location = new System.Drawing.Point(56, 192);
         this.textIMAPMasterUser.Name = "textIMAPMasterUser";
         this.textIMAPMasterUser.Number = 0;
         this.textIMAPMasterUser.Number64 = ((long)(0));
         this.textIMAPMasterUser.Numeric = false;
         this.textIMAPMasterUser.Size = new System.Drawing.Size(154, 20);
         this.textIMAPMasterUser.TabIndex = 26;
         // 
         // checkEnableIMAPInitialResponse
         // 
         this.checkEnableIMAPInitialResponse.AutoSize = true;
         this.checkEnableIMAPInitialResponse.Location = new System.Drawing.Point(35, 155);
         this.checkEnableIMAPInitialResponse.Name = "checkEnableIMAPInitialResponse";
         this.checkEnableIMAPInitialResponse.Size = new System.Drawing.Size(160, 17);
         this.checkEnableIMAPInitialResponse.TabIndex = 25;
         this.checkEnableIMAPInitialResponse.Text = "SASL Initial Client Response";
         this.checkEnableIMAPInitialResponse.UseVisualStyleBackColor = true;
         // 
         // checkEnableIMAPPlain
         // 
         this.checkEnableIMAPPlain.AutoSize = true;
         this.checkEnableIMAPPlain.Location = new System.Drawing.Point(35, 132);
         this.checkEnableIMAPPlain.Name = "checkEnableIMAPPlain";
         this.checkEnableIMAPPlain.Size = new System.Drawing.Size(79, 17);
         this.checkEnableIMAPPlain.TabIndex = 24;
         this.checkEnableIMAPPlain.Text = "SASL Plain";
         this.checkEnableIMAPPlain.UseVisualStyleBackColor = true;
         // 
         // labelHierarchyDelimiter
         // 
         this.labelHierarchyDelimiter.AutoSize = true;
         this.labelHierarchyDelimiter.Location = new System.Drawing.Point(304, 48);
         this.labelHierarchyDelimiter.Name = "labelHierarchyDelimiter";
         this.labelHierarchyDelimiter.Size = new System.Drawing.Size(93, 13);
         this.labelHierarchyDelimiter.TabIndex = 21;
         this.labelHierarchyDelimiter.Text = "Hierarchy delimiter";
         // 
         // comboIMAPHierarchyDelimiter
         // 
         this.comboIMAPHierarchyDelimiter.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
         this.comboIMAPHierarchyDelimiter.FormattingEnabled = true;
         this.comboIMAPHierarchyDelimiter.Items.AddRange(new object[] {
            ".",
            "/",
            "\\"});
         this.comboIMAPHierarchyDelimiter.Location = new System.Drawing.Point(307, 64);
         this.comboIMAPHierarchyDelimiter.Name = "comboIMAPHierarchyDelimiter";
         this.comboIMAPHierarchyDelimiter.Size = new System.Drawing.Size(102, 21);
         this.comboIMAPHierarchyDelimiter.TabIndex = 20;
         // 
         // labelOther2
         // 
         this.labelOther2.AutoSize = true;
         this.labelOther2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelOther2.Location = new System.Drawing.Point(287, 16);
         this.labelOther2.Name = "labelOther2";
         this.labelOther2.Size = new System.Drawing.Size(38, 13);
         this.labelOther2.TabIndex = 19;
         this.labelOther2.Text = "Other";
         // 
         // checkEnableIMAPACL
         // 
         this.checkEnableIMAPACL.AutoSize = true;
         this.checkEnableIMAPACL.Location = new System.Drawing.Point(35, 110);
         this.checkEnableIMAPACL.Name = "checkEnableIMAPACL";
         this.checkEnableIMAPACL.Size = new System.Drawing.Size(46, 17);
         this.checkEnableIMAPACL.TabIndex = 18;
         this.checkEnableIMAPACL.Text = "ACL";
         this.checkEnableIMAPACL.UseVisualStyleBackColor = true;
         // 
         // checkEnableIMAPIdle
         // 
         this.checkEnableIMAPIdle.AutoSize = true;
         this.checkEnableIMAPIdle.Location = new System.Drawing.Point(35, 87);
         this.checkEnableIMAPIdle.Name = "checkEnableIMAPIdle";
         this.checkEnableIMAPIdle.Size = new System.Drawing.Size(43, 17);
         this.checkEnableIMAPIdle.TabIndex = 17;
         this.checkEnableIMAPIdle.Text = "Idle";
         this.checkEnableIMAPIdle.UseVisualStyleBackColor = true;
         // 
         // checkEnableIMAPQuota
         // 
         this.checkEnableIMAPQuota.AutoSize = true;
         this.checkEnableIMAPQuota.Location = new System.Drawing.Point(35, 64);
         this.checkEnableIMAPQuota.Name = "checkEnableIMAPQuota";
         this.checkEnableIMAPQuota.Size = new System.Drawing.Size(55, 17);
         this.checkEnableIMAPQuota.TabIndex = 16;
         this.checkEnableIMAPQuota.Text = "Quota";
         this.checkEnableIMAPQuota.UseVisualStyleBackColor = true;
         // 
         // labelExtensions
         // 
         this.labelExtensions.AutoSize = true;
         this.labelExtensions.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelExtensions.Location = new System.Drawing.Point(15, 16);
         this.labelExtensions.Name = "labelExtensions";
         this.labelExtensions.Size = new System.Drawing.Size(68, 13);
         this.labelExtensions.TabIndex = 15;
         this.labelExtensions.Text = "Extensions";
         // 
         // checkEnableIMAPSort
         // 
         this.checkEnableIMAPSort.AutoSize = true;
         this.checkEnableIMAPSort.Location = new System.Drawing.Point(35, 41);
         this.checkEnableIMAPSort.Name = "checkEnableIMAPSort";
         this.checkEnableIMAPSort.Size = new System.Drawing.Size(45, 17);
         this.checkEnableIMAPSort.TabIndex = 14;
         this.checkEnableIMAPSort.Text = "Sort";
         this.checkEnableIMAPSort.UseVisualStyleBackColor = true;
         // 
         // ucProtocolIMAP
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.tabControl);
         this.Name = "ucProtocolIMAP";
         this.Size = new System.Drawing.Size(497, 346);
         this.tabControl.ResumeLayout(false);
         this.tabPage1.ResumeLayout(false);
         this.tabPage1.PerformLayout();
         this.tabPage3.ResumeLayout(false);
         this.tabPage3.PerformLayout();
         this.tabPage2.ResumeLayout(false);
         this.tabPage2.PerformLayout();
         this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.TabControl tabControl;
       private System.Windows.Forms.TabPage tabPage1;
       private System.Windows.Forms.TabPage tabPage2;
       private hMailServer.Shared.ucText textWelcomeMessage;
       private System.Windows.Forms.Label labelWelcome;
       private System.Windows.Forms.Label labelOther;
       private hMailServer.Shared.ucText textMaxIMAPConnections;
       private System.Windows.Forms.Label labelMaxPOP3Connections;
       private System.Windows.Forms.Label labelConnections;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnableIMAPACL;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnableIMAPIdle;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnableIMAPQuota;
       private System.Windows.Forms.Label labelExtensions;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnableIMAPSort;
       private System.Windows.Forms.TabPage tabPage3;
       private hMailServer.Shared.ucText textIMAPPublicFolderName;
       private System.Windows.Forms.Label labelPublicFolderName;
       private System.Windows.Forms.Button buttonEditFolders;
       private System.Windows.Forms.Label labelOther2;
       private System.Windows.Forms.Label labelHierarchyDelimiter;
       private hMailServer.Administrator.Controls.ucComboBox comboIMAPHierarchyDelimiter;
       private Controls.ucCheckbox checkEnableIMAPInitialResponse;
       private Controls.ucCheckbox checkEnableIMAPPlain;
       private Shared.ucText textIMAPMasterUser;
       private System.Windows.Forms.Label labelIMAPMasterUser;
    }
}
