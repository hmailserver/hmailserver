namespace hMailServer.Administrator
{
    partial class ucLogging
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
            this.labelLog = new System.Windows.Forms.Label();
            this.labelSettings = new System.Windows.Forms.Label();
            this.buttonShowLogs = new System.Windows.Forms.Button();
            this.checkKeepFilesOpen = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkLogAWStats = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkLogDebug = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkLogTCPIP = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkLogIMAP = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkLogPOP3 = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkLogSMTP = new hMailServer.Administrator.Controls.ucCheckbox();
            this.chkLogApplication = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkEnableLogging = new hMailServer.Administrator.Controls.ucCheckbox();
            this.SuspendLayout();
            // 
            // labelLog
            // 
            this.labelLog.AutoSize = true;
            this.labelLog.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelLog.Location = new System.Drawing.Point(5, 39);
            this.labelLog.Name = "labelLog";
            this.labelLog.Size = new System.Drawing.Size(28, 13);
            this.labelLog.TabIndex = 29;
            this.labelLog.Text = "Log";
            // 
            // labelSettings
            // 
            this.labelSettings.AutoSize = true;
            this.labelSettings.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelSettings.Location = new System.Drawing.Point(5, 238);
            this.labelSettings.Name = "labelSettings";
            this.labelSettings.Size = new System.Drawing.Size(53, 13);
            this.labelSettings.TabIndex = 38;
            this.labelSettings.Text = "Settings";
            // 
            // buttonShowLogs
            // 
            this.buttonShowLogs.Location = new System.Drawing.Point(8, 324);
            this.buttonShowLogs.Name = "buttonShowLogs";
            this.buttonShowLogs.Size = new System.Drawing.Size(112, 26);
            this.buttonShowLogs.TabIndex = 40;
            this.buttonShowLogs.Text = "Show logs";
            this.buttonShowLogs.UseVisualStyleBackColor = true;
            this.buttonShowLogs.Click += new System.EventHandler(this.buttonShowLogs_Click);
            // 
            // checkKeepFilesOpen
            // 
            this.checkKeepFilesOpen.AutoSize = true;
            this.checkKeepFilesOpen.Location = new System.Drawing.Point(30, 264);
            this.checkKeepFilesOpen.Name = "checkKeepFilesOpen";
            this.checkKeepFilesOpen.Size = new System.Drawing.Size(99, 17);
            this.checkKeepFilesOpen.TabIndex = 41;
            this.checkKeepFilesOpen.Text = "Keep files open";
            this.checkKeepFilesOpen.UseVisualStyleBackColor = true;
            // 
            // checkLogAWStats
            // 
            this.checkLogAWStats.AutoSize = true;
            this.checkLogAWStats.Location = new System.Drawing.Point(30, 205);
            this.checkLogAWStats.Name = "checkLogAWStats";
            this.checkLogAWStats.Size = new System.Drawing.Size(68, 17);
            this.checkLogAWStats.TabIndex = 37;
            this.checkLogAWStats.Text = "AWStats";
            this.checkLogAWStats.UseVisualStyleBackColor = true;
            // 
            // checkLogDebug
            // 
            this.checkLogDebug.AutoSize = true;
            this.checkLogDebug.Location = new System.Drawing.Point(30, 182);
            this.checkLogDebug.Name = "checkLogDebug";
            this.checkLogDebug.Size = new System.Drawing.Size(58, 17);
            this.checkLogDebug.TabIndex = 36;
            this.checkLogDebug.Text = "Debug";
            this.checkLogDebug.UseVisualStyleBackColor = true;
            // 
            // checkLogTCPIP
            // 
            this.checkLogTCPIP.AutoSize = true;
            this.checkLogTCPIP.Location = new System.Drawing.Point(30, 159);
            this.checkLogTCPIP.Name = "checkLogTCPIP";
            this.checkLogTCPIP.Size = new System.Drawing.Size(62, 17);
            this.checkLogTCPIP.TabIndex = 35;
            this.checkLogTCPIP.Text = "TCP/IP";
            this.checkLogTCPIP.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.checkLogTCPIP.UseVisualStyleBackColor = true;
            // 
            // checkLogIMAP
            // 
            this.checkLogIMAP.AutoSize = true;
            this.checkLogIMAP.Location = new System.Drawing.Point(30, 136);
            this.checkLogIMAP.Name = "checkLogIMAP";
            this.checkLogIMAP.Size = new System.Drawing.Size(52, 17);
            this.checkLogIMAP.TabIndex = 34;
            this.checkLogIMAP.Text = "IMAP";
            this.checkLogIMAP.UseVisualStyleBackColor = true;
            // 
            // checkLogPOP3
            // 
            this.checkLogPOP3.AutoSize = true;
            this.checkLogPOP3.Location = new System.Drawing.Point(30, 113);
            this.checkLogPOP3.Name = "checkLogPOP3";
            this.checkLogPOP3.Size = new System.Drawing.Size(54, 17);
            this.checkLogPOP3.TabIndex = 33;
            this.checkLogPOP3.Text = "POP3";
            this.checkLogPOP3.UseVisualStyleBackColor = true;
            // 
            // checkLogSMTP
            // 
            this.checkLogSMTP.AutoSize = true;
            this.checkLogSMTP.Location = new System.Drawing.Point(30, 90);
            this.checkLogSMTP.Name = "checkLogSMTP";
            this.checkLogSMTP.Size = new System.Drawing.Size(56, 17);
            this.checkLogSMTP.TabIndex = 32;
            this.checkLogSMTP.Text = "SMTP";
            this.checkLogSMTP.UseVisualStyleBackColor = true;
            // 
            // chkLogApplication
            // 
            this.chkLogApplication.AutoSize = true;
            this.chkLogApplication.Location = new System.Drawing.Point(30, 67);
            this.chkLogApplication.Name = "chkLogApplication";
            this.chkLogApplication.Size = new System.Drawing.Size(78, 17);
            this.chkLogApplication.TabIndex = 31;
            this.chkLogApplication.Text = "Application";
            this.chkLogApplication.UseVisualStyleBackColor = true;
            // 
            // checkEnableLogging
            // 
            this.checkEnableLogging.AutoSize = true;
            this.checkEnableLogging.Location = new System.Drawing.Point(8, 8);
            this.checkEnableLogging.Name = "checkEnableLogging";
            this.checkEnableLogging.Size = new System.Drawing.Size(65, 17);
            this.checkEnableLogging.TabIndex = 30;
            this.checkEnableLogging.Text = "Enabled";
            this.checkEnableLogging.UseVisualStyleBackColor = true;
            this.checkEnableLogging.CheckedChanged += new System.EventHandler(this.checkEnableLogging_CheckedChanged);
            // 
            // ucLogging
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.checkKeepFilesOpen);
            this.Controls.Add(this.buttonShowLogs);
            this.Controls.Add(this.labelSettings);
            this.Controls.Add(this.checkLogAWStats);
            this.Controls.Add(this.checkLogDebug);
            this.Controls.Add(this.checkLogTCPIP);
            this.Controls.Add(this.checkLogIMAP);
            this.Controls.Add(this.checkLogPOP3);
            this.Controls.Add(this.checkLogSMTP);
            this.Controls.Add(this.chkLogApplication);
            this.Controls.Add(this.checkEnableLogging);
            this.Controls.Add(this.labelLog);
            this.Name = "ucLogging";
            this.Size = new System.Drawing.Size(561, 497);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

       private hMailServer.Administrator.Controls.ucCheckbox checkEnableLogging;
       private System.Windows.Forms.Label labelLog;
       private hMailServer.Administrator.Controls.ucCheckbox chkLogApplication;
       private hMailServer.Administrator.Controls.ucCheckbox checkLogSMTP;
       private hMailServer.Administrator.Controls.ucCheckbox checkLogPOP3;
       private hMailServer.Administrator.Controls.ucCheckbox checkLogIMAP;
       private hMailServer.Administrator.Controls.ucCheckbox checkLogTCPIP;
       private hMailServer.Administrator.Controls.ucCheckbox checkLogDebug;
       private hMailServer.Administrator.Controls.ucCheckbox checkLogAWStats;
       private System.Windows.Forms.Label labelSettings;
       private System.Windows.Forms.Button buttonShowLogs;
       private hMailServer.Administrator.Controls.ucCheckbox checkKeepFilesOpen;
    }
}
