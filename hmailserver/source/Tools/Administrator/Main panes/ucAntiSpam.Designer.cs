namespace hMailServer.Administrator
{
    partial class ucAntiSpam
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
           this.textMaximumMessageSize = new hMailServer.Shared.ucText();
           this.labelMaxSizeToScan = new System.Windows.Forms.Label();
           this.textPrependSubjectText = new hMailServer.Shared.ucText();
           this.textSpamDeleteThreshold = new hMailServer.Shared.ucText();
           this.labelSpamDeleteThreshold = new System.Windows.Forms.Label();
           this.checkPrependSubject = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAddHeaderReason = new hMailServer.Administrator.Controls.ucCheckbox();
           this.checkAddHeaderSpam = new hMailServer.Administrator.Controls.ucCheckbox();
           this.textSpamMarkThreshold = new hMailServer.Shared.ucText();
           this.labelSpamMarkThreshold = new System.Windows.Forms.Label();
           this.tabPage2 = new System.Windows.Forms.TabPage();
           this.textDKIMVerificationFailureScore = new hMailServer.Shared.ucText();
           this.checkDKIMVerificationEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.textCheckHostInHeloScore = new hMailServer.Shared.ucText();
           this.textCheckPTRScore = new hMailServer.Shared.ucText();
           this.textUseMXChecksScore = new hMailServer.Shared.ucText();
           this.textUseSPFScore = new hMailServer.Shared.ucText();
           this.labelScore = new System.Windows.Forms.Label();
           this.chkUseMXChecks = new hMailServer.Administrator.Controls.ucCheckbox();
           this.chkCheckHostInHelo = new hMailServer.Administrator.Controls.ucCheckbox();
           this.chkCheckPTR = new hMailServer.Administrator.Controls.ucCheckbox();
           this.chkUseSPF = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabPage3 = new System.Windows.Forms.TabPage();
           this.buttonTest = new System.Windows.Forms.Button();
           this.textSpamAssassinScore = new hMailServer.Shared.ucText();
           this.labelFixedScore = new System.Windows.Forms.Label();
           this.labelTCPIPPort = new System.Windows.Forms.Label();
           this.labelHostname = new System.Windows.Forms.Label();
           this.checkSpamAssassinMergeScore = new hMailServer.Administrator.Controls.ucCheckbox();
           this.textSpamAssassinPort = new hMailServer.Shared.ucText();
           this.textSpamAssassinHost = new hMailServer.Shared.ucText();
           this.checkSpamAssassinEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.ucText1 = new hMailServer.Shared.ucText();
           this.ucCheckbox3 = new hMailServer.Administrator.Controls.ucCheckbox();
           this.ucCheckbox4 = new hMailServer.Administrator.Controls.ucCheckbox();
           this.ucCheckbox5 = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabControl.SuspendLayout();
           this.tabPage1.SuspendLayout();
           this.tabPage2.SuspendLayout();
           this.tabPage3.SuspendLayout();
           this.SuspendLayout();
           // 
           // tabControl
           // 
           this.tabControl.Controls.Add(this.tabPage1);
           this.tabControl.Controls.Add(this.tabPage2);
           this.tabControl.Controls.Add(this.tabPage3);
           this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
           this.tabControl.Location = new System.Drawing.Point(0, 0);
           this.tabControl.Name = "tabControl";
           this.tabControl.SelectedIndex = 0;
           this.tabControl.Size = new System.Drawing.Size(488, 299);
           this.tabControl.TabIndex = 0;
           // 
           // tabPage1
           // 
           this.tabPage1.Controls.Add(this.textMaximumMessageSize);
           this.tabPage1.Controls.Add(this.labelMaxSizeToScan);
           this.tabPage1.Controls.Add(this.textPrependSubjectText);
           this.tabPage1.Controls.Add(this.textSpamDeleteThreshold);
           this.tabPage1.Controls.Add(this.labelSpamDeleteThreshold);
           this.tabPage1.Controls.Add(this.checkPrependSubject);
           this.tabPage1.Controls.Add(this.checkAddHeaderReason);
           this.tabPage1.Controls.Add(this.checkAddHeaderSpam);
           this.tabPage1.Controls.Add(this.textSpamMarkThreshold);
           this.tabPage1.Controls.Add(this.labelSpamMarkThreshold);
           this.tabPage1.Location = new System.Drawing.Point(4, 22);
           this.tabPage1.Name = "tabPage1";
           this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
           this.tabPage1.Size = new System.Drawing.Size(480, 273);
           this.tabPage1.TabIndex = 0;
           this.tabPage1.Text = "General";
           this.tabPage1.UseVisualStyleBackColor = true;
           // 
           // textMaximumMessageSize
           // 
           this.textMaximumMessageSize.Location = new System.Drawing.Point(20, 232);
           this.textMaximumMessageSize.Name = "textMaximumMessageSize";
           this.textMaximumMessageSize.Number = 0;
           this.textMaximumMessageSize.Numeric = true;
           this.textMaximumMessageSize.Size = new System.Drawing.Size(116, 20);
           this.textMaximumMessageSize.TabIndex = 29;
           // 
           // labelMaxSizeToScan
           // 
           this.labelMaxSizeToScan.AutoSize = true;
           this.labelMaxSizeToScan.Location = new System.Drawing.Point(18, 216);
           this.labelMaxSizeToScan.Name = "labelMaxSizeToScan";
           this.labelMaxSizeToScan.Size = new System.Drawing.Size(178, 13);
           this.labelMaxSizeToScan.TabIndex = 28;
           this.labelMaxSizeToScan.Text = "Maximum message size to scan (KB)";
           // 
           // textPrependSubjectText
           // 
           this.textPrependSubjectText.Location = new System.Drawing.Point(56, 141);
           this.textPrependSubjectText.Name = "textPrependSubjectText";
           this.textPrependSubjectText.Number = 0;
           this.textPrependSubjectText.Numeric = false;
           this.textPrependSubjectText.Size = new System.Drawing.Size(178, 20);
           this.textPrependSubjectText.TabIndex = 20;
           // 
           // textSpamDeleteThreshold
           // 
           this.textSpamDeleteThreshold.Location = new System.Drawing.Point(20, 188);
           this.textSpamDeleteThreshold.Name = "textSpamDeleteThreshold";
           this.textSpamDeleteThreshold.Number = 0;
           this.textSpamDeleteThreshold.Numeric = true;
           this.textSpamDeleteThreshold.Size = new System.Drawing.Size(178, 20);
           this.textSpamDeleteThreshold.TabIndex = 18;
           // 
           // labelSpamDeleteThreshold
           // 
           this.labelSpamDeleteThreshold.AutoSize = true;
           this.labelSpamDeleteThreshold.Location = new System.Drawing.Point(18, 172);
           this.labelSpamDeleteThreshold.Name = "labelSpamDeleteThreshold";
           this.labelSpamDeleteThreshold.Size = new System.Drawing.Size(112, 13);
           this.labelSpamDeleteThreshold.TabIndex = 17;
           this.labelSpamDeleteThreshold.Text = "Spam delete threshold";
           // 
           // checkPrependSubject
           // 
           this.checkPrependSubject.AutoSize = true;
           this.checkPrependSubject.Location = new System.Drawing.Point(37, 117);
           this.checkPrependSubject.Name = "checkPrependSubject";
           this.checkPrependSubject.Size = new System.Drawing.Size(139, 17);
           this.checkPrependSubject.TabIndex = 16;
           this.checkPrependSubject.Text = "Add to message subject";
           this.checkPrependSubject.UseVisualStyleBackColor = true;
           this.checkPrependSubject.CheckedChanged += new System.EventHandler(this.checkPrependSubject_CheckedChanged);
           // 
           // checkAddHeaderReason
           // 
           this.checkAddHeaderReason.AutoSize = true;
           this.checkAddHeaderReason.Location = new System.Drawing.Point(37, 94);
           this.checkAddHeaderReason.Name = "checkAddHeaderReason";
           this.checkAddHeaderReason.Size = new System.Drawing.Size(154, 17);
           this.checkAddHeaderReason.TabIndex = 15;
           this.checkAddHeaderReason.Text = "Add X-hMailServer-Reason";
           this.checkAddHeaderReason.UseVisualStyleBackColor = true;
           // 
           // checkAddHeaderSpam
           // 
           this.checkAddHeaderSpam.AutoSize = true;
           this.checkAddHeaderSpam.Location = new System.Drawing.Point(37, 71);
           this.checkAddHeaderSpam.Name = "checkAddHeaderSpam";
           this.checkAddHeaderSpam.Size = new System.Drawing.Size(144, 17);
           this.checkAddHeaderSpam.TabIndex = 14;
           this.checkAddHeaderSpam.Text = "Add X-hMailServer-Spam";
           this.checkAddHeaderSpam.UseVisualStyleBackColor = true;
           // 
           // textSpamMarkThreshold
           // 
           this.textSpamMarkThreshold.Location = new System.Drawing.Point(20, 36);
           this.textSpamMarkThreshold.Name = "textSpamMarkThreshold";
           this.textSpamMarkThreshold.Number = 0;
           this.textSpamMarkThreshold.Numeric = true;
           this.textSpamMarkThreshold.Size = new System.Drawing.Size(178, 20);
           this.textSpamMarkThreshold.TabIndex = 9;
           // 
           // labelSpamMarkThreshold
           // 
           this.labelSpamMarkThreshold.AutoSize = true;
           this.labelSpamMarkThreshold.Location = new System.Drawing.Point(18, 20);
           this.labelSpamMarkThreshold.Name = "labelSpamMarkThreshold";
           this.labelSpamMarkThreshold.Size = new System.Drawing.Size(106, 13);
           this.labelSpamMarkThreshold.TabIndex = 8;
           this.labelSpamMarkThreshold.Text = "Spam mark threshold";
           // 
           // tabPage2
           // 
           this.tabPage2.Controls.Add(this.textDKIMVerificationFailureScore);
           this.tabPage2.Controls.Add(this.checkDKIMVerificationEnabled);
           this.tabPage2.Controls.Add(this.textCheckHostInHeloScore);
           this.tabPage2.Controls.Add(this.textUseMXChecksScore);
           this.tabPage2.Controls.Add(this.textCheckPTRScore);
           this.tabPage2.Controls.Add(this.textUseSPFScore);
           this.tabPage2.Controls.Add(this.labelScore);
           this.tabPage2.Controls.Add(this.chkUseMXChecks);
           this.tabPage2.Controls.Add(this.chkCheckHostInHelo);
           this.tabPage2.Controls.Add(this.chkCheckPTR);
           this.tabPage2.Controls.Add(this.chkUseSPF);
           this.tabPage2.Location = new System.Drawing.Point(4, 22);
           this.tabPage2.Name = "tabPage2";
           this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
           this.tabPage2.Size = new System.Drawing.Size(480, 273);
           this.tabPage2.TabIndex = 1;
           this.tabPage2.Text = "Spam tests";
           this.tabPage2.UseVisualStyleBackColor = true;
           this.tabPage2.Click += new System.EventHandler(this.tabPage2_Click);

           // 
           // textCheckPTRScore
           // 
           this.textCheckPTRScore.Location = new System.Drawing.Point(316, 122);
           this.textCheckPTRScore.Name = "textCheckPTRScore";
           this.textCheckPTRScore.Number = 0;
           this.textCheckPTRScore.Numeric = true;
           this.textCheckPTRScore.Size = new System.Drawing.Size(64, 20);
           this.textCheckPTRScore.TabIndex = 23;
           // 
           // chkCheckPTR
           // 
           this.chkCheckPTR.AutoSize = true;
           this.chkCheckPTR.Location = new System.Drawing.Point(15, 122);
           this.chkCheckPTR.Name = "chkCheckPTR";
           this.chkCheckPTR.Size = new System.Drawing.Size(190, 17);
           this.chkCheckPTR.TabIndex = 22;
           this.chkCheckPTR.Text = "Check rDNS/PTR";
           this.chkCheckPTR.UseVisualStyleBackColor = true;
           this.chkCheckPTR.CheckedChanged += new System.EventHandler(this.chkCheckPTR_CheckedChanged);
           // 
           // textDKIMVerificationFailureScore
           // 
           this.textDKIMVerificationFailureScore.Location = new System.Drawing.Point(316, 152);
           this.textDKIMVerificationFailureScore.Name = "textDKIMVerificationFailureScore";
           this.textDKIMVerificationFailureScore.Number = 0;
           this.textDKIMVerificationFailureScore.Numeric = true;
           this.textDKIMVerificationFailureScore.Size = new System.Drawing.Size(64, 20);
           this.textDKIMVerificationFailureScore.TabIndex = 25;
           // 
           // checkDKIMVerificationEnabled
           // 
           this.checkDKIMVerificationEnabled.AutoSize = true;
           this.checkDKIMVerificationEnabled.Location = new System.Drawing.Point(15, 152);
           this.checkDKIMVerificationEnabled.Name = "checkDKIMVerificationEnabled";
           this.checkDKIMVerificationEnabled.Size = new System.Drawing.Size(166, 17);
           this.checkDKIMVerificationEnabled.TabIndex = 24;
           this.checkDKIMVerificationEnabled.Text = "Verify DKIM-Signature header";
           this.checkDKIMVerificationEnabled.UseVisualStyleBackColor = true;
           this.checkDKIMVerificationEnabled.CheckedChanged += new System.EventHandler(this.checkDKIMVerificationEnabled_CheckedChanged);
           // 
           // textCheckHostInHeloScore
           // 
           this.textCheckHostInHeloScore.Location = new System.Drawing.Point(316, 62);
           this.textCheckHostInHeloScore.Name = "textCheckHostInHeloScore";
           this.textCheckHostInHeloScore.Number = 0;
           this.textCheckHostInHeloScore.Numeric = true;
           this.textCheckHostInHeloScore.Size = new System.Drawing.Size(64, 20);
           this.textCheckHostInHeloScore.TabIndex = 21;
           // 
           // textUseMXChecksScore
           // 
           this.textUseMXChecksScore.Location = new System.Drawing.Point(316, 92);
           this.textUseMXChecksScore.Name = "textUseMXChecksScore";
           this.textUseMXChecksScore.Number = 0;
           this.textUseMXChecksScore.Numeric = true;
           this.textUseMXChecksScore.Size = new System.Drawing.Size(64, 20);
           this.textUseMXChecksScore.TabIndex = 20;
           // 
           // textUseSPFScore
           // 
           this.textUseSPFScore.Location = new System.Drawing.Point(316, 32);
           this.textUseSPFScore.Name = "textUseSPFScore";
           this.textUseSPFScore.Number = 0;
           this.textUseSPFScore.Numeric = true;
           this.textUseSPFScore.Size = new System.Drawing.Size(64, 20);
           this.textUseSPFScore.TabIndex = 19;
           // 
           // labelScore
           // 
           this.labelScore.AutoSize = true;
           this.labelScore.Location = new System.Drawing.Point(313, 16);
           this.labelScore.Name = "labelScore";
           this.labelScore.Size = new System.Drawing.Size(35, 13);
           this.labelScore.TabIndex = 18;
           this.labelScore.Text = "Score";
           // 
           // chkUseMXChecks
           // 
           this.chkUseMXChecks.AutoSize = true;
           this.chkUseMXChecks.Location = new System.Drawing.Point(15, 92);
           this.chkUseMXChecks.Name = "chkUseMXChecks";
           this.chkUseMXChecks.Size = new System.Drawing.Size(216, 17);
           this.chkUseMXChecks.TabIndex = 17;
           this.chkUseMXChecks.Text = "Check that sender has DNS-MX records";
           this.chkUseMXChecks.UseVisualStyleBackColor = true;
           this.chkUseMXChecks.CheckedChanged += new System.EventHandler(this.chkUseMXChecks_CheckedChanged);
           // 
           // chkCheckHostInHelo
           // 
           this.chkCheckHostInHelo.AutoSize = true;
           this.chkCheckHostInHelo.Location = new System.Drawing.Point(15, 62);
           this.chkCheckHostInHelo.Name = "chkCheckHostInHelo";
           this.chkCheckHostInHelo.Size = new System.Drawing.Size(190, 17);
           this.chkCheckHostInHelo.TabIndex = 16;
           this.chkCheckHostInHelo.Text = "Check host in the HELO command";
           this.chkCheckHostInHelo.UseVisualStyleBackColor = true;
           this.chkCheckHostInHelo.CheckedChanged += new System.EventHandler(this.chkCheckHostInHelo_CheckedChanged);
           // 
           // chkUseSPF
           // 
           this.chkUseSPF.AutoSize = true;
           this.chkUseSPF.Location = new System.Drawing.Point(15, 32);
           this.chkUseSPF.Name = "chkUseSPF";
           this.chkUseSPF.Size = new System.Drawing.Size(68, 17);
           this.chkUseSPF.TabIndex = 15;
           this.chkUseSPF.Text = "Use SPF";
           this.chkUseSPF.UseVisualStyleBackColor = true;
           this.chkUseSPF.CheckedChanged += new System.EventHandler(this.chkUseSPF_CheckedChanged);
           // 
           // tabPage3
           // 
           this.tabPage3.Controls.Add(this.buttonTest);
           this.tabPage3.Controls.Add(this.textSpamAssassinScore);
           this.tabPage3.Controls.Add(this.labelFixedScore);
           this.tabPage3.Controls.Add(this.labelTCPIPPort);
           this.tabPage3.Controls.Add(this.labelHostname);
           this.tabPage3.Controls.Add(this.checkSpamAssassinMergeScore);
           this.tabPage3.Controls.Add(this.textSpamAssassinPort);
           this.tabPage3.Controls.Add(this.textSpamAssassinHost);
           this.tabPage3.Controls.Add(this.checkSpamAssassinEnabled);
           this.tabPage3.Location = new System.Drawing.Point(4, 22);
           this.tabPage3.Name = "tabPage3";
           this.tabPage3.Size = new System.Drawing.Size(480, 273);
           this.tabPage3.TabIndex = 2;
           this.tabPage3.Text = "SpamAssassin";
           this.tabPage3.UseVisualStyleBackColor = true;
           // 
           // buttonTest
           // 
           this.buttonTest.Location = new System.Drawing.Point(35, 227);
           this.buttonTest.Name = "buttonTest";
           this.buttonTest.Size = new System.Drawing.Size(114, 27);
           this.buttonTest.TabIndex = 28;
           this.buttonTest.Text = "Test...";
           this.buttonTest.UseVisualStyleBackColor = true;
           this.buttonTest.Click += new System.EventHandler(this.buttonTest_Click);
           // 
           // textSpamAssassinScore
           // 
           this.textSpamAssassinScore.Location = new System.Drawing.Point(32, 184);
           this.textSpamAssassinScore.Name = "textSpamAssassinScore";
           this.textSpamAssassinScore.Number = 0;
           this.textSpamAssassinScore.Numeric = true;
           this.textSpamAssassinScore.Size = new System.Drawing.Size(78, 20);
           this.textSpamAssassinScore.TabIndex = 27;
           this.textSpamAssassinScore.TextChanged += new System.EventHandler(this.textSpamAssassinScore_TextChanged);
           // 
           // labelFixedScore
           // 
           this.labelFixedScore.AutoSize = true;
           this.labelFixedScore.Location = new System.Drawing.Point(32, 168);
           this.labelFixedScore.Name = "labelFixedScore";
           this.labelFixedScore.Size = new System.Drawing.Size(35, 13);
           this.labelFixedScore.TabIndex = 26;
           this.labelFixedScore.Text = "Score";
           // 
           // labelTCPIPPort
           // 
           this.labelTCPIPPort.AutoSize = true;
           this.labelTCPIPPort.Location = new System.Drawing.Point(32, 96);
           this.labelTCPIPPort.Name = "labelTCPIPPort";
           this.labelTCPIPPort.Size = new System.Drawing.Size(64, 13);
           this.labelTCPIPPort.TabIndex = 23;
           this.labelTCPIPPort.Text = "TCP/IP port";
           // 
           // labelHostname
           // 
           this.labelHostname.AutoSize = true;
           this.labelHostname.Location = new System.Drawing.Point(32, 48);
           this.labelHostname.Name = "labelHostname";
           this.labelHostname.Size = new System.Drawing.Size(58, 13);
           this.labelHostname.TabIndex = 21;
           this.labelHostname.Text = "Host name";
           this.labelHostname.Click += new System.EventHandler(this.label2_Click);
           // 
           // checkSpamAssassinMergeScore
           // 
           this.checkSpamAssassinMergeScore.AutoSize = true;
           this.checkSpamAssassinMergeScore.Location = new System.Drawing.Point(32, 144);
           this.checkSpamAssassinMergeScore.Name = "checkSpamAssassinMergeScore";
           this.checkSpamAssassinMergeScore.Size = new System.Drawing.Size(168, 17);
           this.checkSpamAssassinMergeScore.TabIndex = 25;
           this.checkSpamAssassinMergeScore.Text = "Use score from SpamAssassin";
           this.checkSpamAssassinMergeScore.UseVisualStyleBackColor = true;
           this.checkSpamAssassinMergeScore.CheckedChanged += new System.EventHandler(this.checkSpamAssassinMergeScore_CheckedChanged);
           // 
           // textSpamAssassinPort
           // 
           this.textSpamAssassinPort.Location = new System.Drawing.Point(32, 112);
           this.textSpamAssassinPort.Name = "textSpamAssassinPort";
           this.textSpamAssassinPort.Number = 0;
           this.textSpamAssassinPort.Numeric = true;
           this.textSpamAssassinPort.Size = new System.Drawing.Size(78, 20);
           this.textSpamAssassinPort.TabIndex = 24;
           // 
           // textSpamAssassinHost
           // 
           this.textSpamAssassinHost.Location = new System.Drawing.Point(32, 64);
           this.textSpamAssassinHost.Name = "textSpamAssassinHost";
           this.textSpamAssassinHost.Number = 0;
           this.textSpamAssassinHost.Numeric = false;
           this.textSpamAssassinHost.Size = new System.Drawing.Size(190, 20);
           this.textSpamAssassinHost.TabIndex = 22;
           this.textSpamAssassinHost.TextChanged += new System.EventHandler(this.ucText2_TextChanged);
           // 
           // checkSpamAssassinEnabled
           // 
           this.checkSpamAssassinEnabled.AutoSize = true;
           this.checkSpamAssassinEnabled.Location = new System.Drawing.Point(15, 14);
           this.checkSpamAssassinEnabled.Name = "checkSpamAssassinEnabled";
           this.checkSpamAssassinEnabled.Size = new System.Drawing.Size(116, 17);
           this.checkSpamAssassinEnabled.TabIndex = 20;
           this.checkSpamAssassinEnabled.Text = "Use SpamAssassin";
           this.checkSpamAssassinEnabled.UseVisualStyleBackColor = true;
           this.checkSpamAssassinEnabled.CheckedChanged += new System.EventHandler(this.checkSpamAssassinEnabled_CheckedChanged);
           // 
           // ucText1
           // 
           this.ucText1.Location = new System.Drawing.Point(20, 36);
           this.ucText1.Name = "ucText1";
           this.ucText1.Number = 0;
           this.ucText1.Numeric = false;
           this.ucText1.Size = new System.Drawing.Size(178, 20);
           this.ucText1.TabIndex = 9;
           // 
           // ucCheckbox3
           // 
           this.ucCheckbox3.AutoSize = true;
           this.ucCheckbox3.Location = new System.Drawing.Point(37, 117);
           this.ucCheckbox3.Name = "ucCheckbox3";
           this.ucCheckbox3.Size = new System.Drawing.Size(148, 17);
           this.ucCheckbox3.TabIndex = 16;
           this.ucCheckbox3.Text = "Prepend message subject";
           this.ucCheckbox3.UseVisualStyleBackColor = true;
           // 
           // ucCheckbox4
           // 
           this.ucCheckbox4.AutoSize = true;
           this.ucCheckbox4.Location = new System.Drawing.Point(37, 94);
           this.ucCheckbox4.Name = "ucCheckbox4";
           this.ucCheckbox4.Size = new System.Drawing.Size(154, 17);
           this.ucCheckbox4.TabIndex = 15;
           this.ucCheckbox4.Text = "Add X-hMailServer-Reason";
           this.ucCheckbox4.UseVisualStyleBackColor = true;
           // 
           // ucCheckbox5
           // 
           this.ucCheckbox5.AutoSize = true;
           this.ucCheckbox5.Location = new System.Drawing.Point(37, 71);
           this.ucCheckbox5.Name = "ucCheckbox5";
           this.ucCheckbox5.Size = new System.Drawing.Size(144, 17);
           this.ucCheckbox5.TabIndex = 14;
           this.ucCheckbox5.Text = "Add X-hMailServer-Spam";
           this.ucCheckbox5.UseVisualStyleBackColor = true;
           // 
           // ucAntiSpam
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.tabControl);
           this.Name = "ucAntiSpam";
           this.Size = new System.Drawing.Size(488, 299);
           this.tabControl.ResumeLayout(false);
           this.tabPage1.ResumeLayout(false);
           this.tabPage1.PerformLayout();
           this.tabPage2.ResumeLayout(false);
           this.tabPage2.PerformLayout();
           this.tabPage3.ResumeLayout(false);
           this.tabPage3.PerformLayout();
           this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.TabControl tabControl;
       private System.Windows.Forms.TabPage tabPage1;
       private System.Windows.Forms.TabPage tabPage2;
       private System.Windows.Forms.TabPage tabPage3;
       private hMailServer.Shared.ucText textSpamMarkThreshold;
       private System.Windows.Forms.Label labelSpamMarkThreshold;
       private hMailServer.Shared.ucText textSpamDeleteThreshold;
       private System.Windows.Forms.Label labelSpamDeleteThreshold;
       private hMailServer.Administrator.Controls.ucCheckbox checkPrependSubject;
       private hMailServer.Administrator.Controls.ucCheckbox checkAddHeaderReason;
       private hMailServer.Administrator.Controls.ucCheckbox checkAddHeaderSpam;
       private hMailServer.Shared.ucText ucText1;
       private hMailServer.Administrator.Controls.ucCheckbox ucCheckbox3;
       private hMailServer.Administrator.Controls.ucCheckbox ucCheckbox4;
       private hMailServer.Administrator.Controls.ucCheckbox ucCheckbox5;
       private hMailServer.Shared.ucText textCheckHostInHeloScore;
       private hMailServer.Shared.ucText textCheckPTRScore;
       private hMailServer.Shared.ucText textUseMXChecksScore;
       private hMailServer.Shared.ucText textUseSPFScore;
       private System.Windows.Forms.Label labelScore;
       private hMailServer.Administrator.Controls.ucCheckbox chkUseMXChecks;
       private hMailServer.Administrator.Controls.ucCheckbox chkCheckHostInHelo;
       private hMailServer.Administrator.Controls.ucCheckbox chkCheckPTR;
       private hMailServer.Administrator.Controls.ucCheckbox chkUseSPF;
       private hMailServer.Shared.ucText textSpamAssassinHost;
       private System.Windows.Forms.Label labelHostname;
       private hMailServer.Administrator.Controls.ucCheckbox checkSpamAssassinEnabled;
       private hMailServer.Shared.ucText textSpamAssassinPort;
       private System.Windows.Forms.Label labelTCPIPPort;
       private hMailServer.Shared.ucText textSpamAssassinScore;
       private System.Windows.Forms.Label labelFixedScore;
       private hMailServer.Administrator.Controls.ucCheckbox checkSpamAssassinMergeScore;
       private hMailServer.Shared.ucText textPrependSubjectText;
       private hMailServer.Shared.ucText textMaximumMessageSize;
       private System.Windows.Forms.Label labelMaxSizeToScan;
       private hMailServer.Shared.ucText textDKIMVerificationFailureScore;
       private hMailServer.Administrator.Controls.ucCheckbox checkDKIMVerificationEnabled;
       private System.Windows.Forms.Button buttonTest;
    }
}
