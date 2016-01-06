namespace hMailServer.Administrator
{
    partial class ucBackup
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
         this.labelAutomate = new System.Windows.Forms.Label();
         this.buttonShowLog = new System.Windows.Forms.Button();
         this.buttonStartBackup = new System.Windows.Forms.Button();
         this.checkCompressFiles = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkBackupMessages = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkBackupDomains = new hMailServer.Administrator.Controls.ucCheckbox();
         this.labelBackup = new System.Windows.Forms.Label();
         this.buttonSelectBackupPath = new System.Windows.Forms.Button();
         this.textDestination = new hMailServer.Shared.ucText();
         this.labelDestination = new System.Windows.Forms.Label();
         this.checkBackupSettings = new hMailServer.Administrator.Controls.ucCheckbox();
         this.tabPage2 = new System.Windows.Forms.TabPage();
         this.buttonShowLog2 = new System.Windows.Forms.Button();
         this.buttonStartRestore = new System.Windows.Forms.Button();
         this.labelRestore = new System.Windows.Forms.Label();
         this.buttonSelectBackupFile = new System.Windows.Forms.Button();
         this.labelBackupFile = new System.Windows.Forms.Label();
         this.checkRestoreMessages = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkRestoreDomains = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkRestoreSettings = new hMailServer.Administrator.Controls.ucCheckbox();
         this.textRestoreFile = new hMailServer.Shared.ucText();
         this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
         this.folderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
         this.tabControl.SuspendLayout();
         this.tabPage1.SuspendLayout();
         this.tabPage2.SuspendLayout();
         this.SuspendLayout();
         // 
         // tabControl
         // 
         this.tabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
         this.tabControl.Controls.Add(this.tabPage1);
         this.tabControl.Controls.Add(this.tabPage2);
         this.tabControl.Location = new System.Drawing.Point(3, 0);
         this.tabControl.Name = "tabControl";
         this.tabControl.SelectedIndex = 0;
         this.tabControl.Size = new System.Drawing.Size(478, 399);
         this.tabControl.TabIndex = 0;
         // 
         // tabPage1
         // 
         this.tabPage1.Controls.Add(this.labelAutomate);
         this.tabPage1.Controls.Add(this.buttonShowLog);
         this.tabPage1.Controls.Add(this.buttonStartBackup);
         this.tabPage1.Controls.Add(this.checkCompressFiles);
         this.tabPage1.Controls.Add(this.checkBackupMessages);
         this.tabPage1.Controls.Add(this.checkBackupDomains);
         this.tabPage1.Controls.Add(this.labelBackup);
         this.tabPage1.Controls.Add(this.buttonSelectBackupPath);
         this.tabPage1.Controls.Add(this.textDestination);
         this.tabPage1.Controls.Add(this.labelDestination);
         this.tabPage1.Controls.Add(this.checkBackupSettings);
         this.tabPage1.Location = new System.Drawing.Point(4, 22);
         this.tabPage1.Name = "tabPage1";
         this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
         this.tabPage1.Size = new System.Drawing.Size(470, 373);
         this.tabPage1.TabIndex = 0;
         this.tabPage1.Text = "Backup";
         this.tabPage1.UseVisualStyleBackColor = true;
         // 
         // labelAutomate
         // 
         this.labelAutomate.Location = new System.Drawing.Point(8, 201);
         this.labelAutomate.Name = "labelAutomate";
         this.labelAutomate.Size = new System.Drawing.Size(440, 28);
         this.labelAutomate.TabIndex = 30;
         this.labelAutomate.Text = "If you want to automate the backup process, please use the script StartBackup.vbs" +
    " found under \\hMailServer\\Addons\\Utilities.";
         // 
         // buttonShowLog
         // 
         this.buttonShowLog.Location = new System.Drawing.Point(123, 243);
         this.buttonShowLog.Name = "buttonShowLog";
         this.buttonShowLog.Size = new System.Drawing.Size(100, 25);
         this.buttonShowLog.TabIndex = 28;
         this.buttonShowLog.Text = "Show log...";
         this.buttonShowLog.UseVisualStyleBackColor = true;
         this.buttonShowLog.Click += new System.EventHandler(this.buttonShowLog_Click);
         // 
         // buttonStartBackup
         // 
         this.buttonStartBackup.Location = new System.Drawing.Point(11, 243);
         this.buttonStartBackup.Name = "buttonStartBackup";
         this.buttonStartBackup.Size = new System.Drawing.Size(100, 25);
         this.buttonStartBackup.TabIndex = 27;
         this.buttonStartBackup.Text = "Start";
         this.buttonStartBackup.UseVisualStyleBackColor = true;
         this.buttonStartBackup.Click += new System.EventHandler(this.buttonStartBackup_Click);
         // 
         // checkCompressFiles
         // 
         this.checkCompressFiles.AutoSize = true;
         this.checkCompressFiles.Location = new System.Drawing.Point(46, 155);
         this.checkCompressFiles.Name = "checkCompressFiles";
         this.checkCompressFiles.Size = new System.Drawing.Size(93, 17);
         this.checkCompressFiles.TabIndex = 26;
         this.checkCompressFiles.Text = "Compress files";
         this.checkCompressFiles.UseVisualStyleBackColor = true;
         // 
         // checkBackupMessages
         // 
         this.checkBackupMessages.AutoSize = true;
         this.checkBackupMessages.Location = new System.Drawing.Point(25, 132);
         this.checkBackupMessages.Name = "checkBackupMessages";
         this.checkBackupMessages.Size = new System.Drawing.Size(74, 17);
         this.checkBackupMessages.TabIndex = 25;
         this.checkBackupMessages.Text = "Messages";
         this.checkBackupMessages.UseVisualStyleBackColor = true;
         // 
         // checkBackupDomains
         // 
         this.checkBackupDomains.AutoSize = true;
         this.checkBackupDomains.Location = new System.Drawing.Point(25, 109);
         this.checkBackupDomains.Name = "checkBackupDomains";
         this.checkBackupDomains.Size = new System.Drawing.Size(67, 17);
         this.checkBackupDomains.TabIndex = 24;
         this.checkBackupDomains.Text = "Domains";
         this.checkBackupDomains.UseVisualStyleBackColor = true;
         // 
         // labelBackup
         // 
         this.labelBackup.AutoSize = true;
         this.labelBackup.Location = new System.Drawing.Point(8, 59);
         this.labelBackup.Name = "labelBackup";
         this.labelBackup.Size = new System.Drawing.Size(44, 13);
         this.labelBackup.TabIndex = 23;
         this.labelBackup.Text = "Backup";
         // 
         // buttonSelectBackupPath
         // 
         this.buttonSelectBackupPath.Location = new System.Drawing.Point(296, 24);
         this.buttonSelectBackupPath.Name = "buttonSelectBackupPath";
         this.buttonSelectBackupPath.Size = new System.Drawing.Size(47, 19);
         this.buttonSelectBackupPath.TabIndex = 22;
         this.buttonSelectBackupPath.Text = "...";
         this.buttonSelectBackupPath.UseVisualStyleBackColor = true;
         this.buttonSelectBackupPath.Click += new System.EventHandler(this.buttonSelectCertificate_Click);
         // 
         // textDestination
         // 
         this.textDestination.Location = new System.Drawing.Point(8, 24);
         this.textDestination.Name = "textDestination";
         this.textDestination.Number = 0;
         this.textDestination.Number64 = ((long)(0));
         this.textDestination.Numeric = false;
         this.textDestination.Size = new System.Drawing.Size(282, 20);
         this.textDestination.TabIndex = 3;
         // 
         // labelDestination
         // 
         this.labelDestination.AutoSize = true;
         this.labelDestination.Location = new System.Drawing.Point(6, 5);
         this.labelDestination.Name = "labelDestination";
         this.labelDestination.Size = new System.Drawing.Size(60, 13);
         this.labelDestination.TabIndex = 2;
         this.labelDestination.Text = "Destination";
         // 
         // checkBackupSettings
         // 
         this.checkBackupSettings.AutoSize = true;
         this.checkBackupSettings.Location = new System.Drawing.Point(25, 86);
         this.checkBackupSettings.Name = "checkBackupSettings";
         this.checkBackupSettings.Size = new System.Drawing.Size(64, 17);
         this.checkBackupSettings.TabIndex = 1;
         this.checkBackupSettings.Text = "Settings";
         this.checkBackupSettings.UseVisualStyleBackColor = true;
         // 
         // tabPage2
         // 
         this.tabPage2.Controls.Add(this.buttonShowLog2);
         this.tabPage2.Controls.Add(this.buttonStartRestore);
         this.tabPage2.Controls.Add(this.labelRestore);
         this.tabPage2.Controls.Add(this.buttonSelectBackupFile);
         this.tabPage2.Controls.Add(this.labelBackupFile);
         this.tabPage2.Controls.Add(this.checkRestoreMessages);
         this.tabPage2.Controls.Add(this.checkRestoreDomains);
         this.tabPage2.Controls.Add(this.checkRestoreSettings);
         this.tabPage2.Controls.Add(this.textRestoreFile);
         this.tabPage2.Location = new System.Drawing.Point(4, 22);
         this.tabPage2.Name = "tabPage2";
         this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
         this.tabPage2.Size = new System.Drawing.Size(470, 373);
         this.tabPage2.TabIndex = 1;
         this.tabPage2.Text = "Restore";
         this.tabPage2.UseVisualStyleBackColor = true;
         // 
         // buttonShowLog2
         // 
         this.buttonShowLog2.Location = new System.Drawing.Point(120, 232);
         this.buttonShowLog2.Name = "buttonShowLog2";
         this.buttonShowLog2.Size = new System.Drawing.Size(100, 25);
         this.buttonShowLog2.TabIndex = 33;
         this.buttonShowLog2.Text = "Show log...";
         this.buttonShowLog2.UseVisualStyleBackColor = true;
         this.buttonShowLog2.Click += new System.EventHandler(this.buttonShowLog2_Click);
         // 
         // buttonStartRestore
         // 
         this.buttonStartRestore.Enabled = false;
         this.buttonStartRestore.Location = new System.Drawing.Point(8, 232);
         this.buttonStartRestore.Name = "buttonStartRestore";
         this.buttonStartRestore.Size = new System.Drawing.Size(100, 25);
         this.buttonStartRestore.TabIndex = 32;
         this.buttonStartRestore.Text = "Start";
         this.buttonStartRestore.UseVisualStyleBackColor = true;
         this.buttonStartRestore.Click += new System.EventHandler(this.buttonStartRestore_Click);
         // 
         // labelRestore
         // 
         this.labelRestore.AutoSize = true;
         this.labelRestore.Location = new System.Drawing.Point(8, 57);
         this.labelRestore.Name = "labelRestore";
         this.labelRestore.Size = new System.Drawing.Size(44, 13);
         this.labelRestore.TabIndex = 26;
         this.labelRestore.Text = "Restore";
         // 
         // buttonSelectBackupFile
         // 
         this.buttonSelectBackupFile.Location = new System.Drawing.Point(296, 25);
         this.buttonSelectBackupFile.Name = "buttonSelectBackupFile";
         this.buttonSelectBackupFile.Size = new System.Drawing.Size(47, 19);
         this.buttonSelectBackupFile.TabIndex = 25;
         this.buttonSelectBackupFile.Text = "...";
         this.buttonSelectBackupFile.UseVisualStyleBackColor = true;
         this.buttonSelectBackupFile.Click += new System.EventHandler(this.buttonSelectBackupFile_Click);
         // 
         // labelBackupFile
         // 
         this.labelBackupFile.AutoSize = true;
         this.labelBackupFile.Location = new System.Drawing.Point(6, 7);
         this.labelBackupFile.Name = "labelBackupFile";
         this.labelBackupFile.Size = new System.Drawing.Size(60, 13);
         this.labelBackupFile.TabIndex = 23;
         this.labelBackupFile.Text = "Backup file";
         // 
         // checkRestoreMessages
         // 
         this.checkRestoreMessages.AutoSize = true;
         this.checkRestoreMessages.Enabled = false;
         this.checkRestoreMessages.Location = new System.Drawing.Point(51, 130);
         this.checkRestoreMessages.Name = "checkRestoreMessages";
         this.checkRestoreMessages.Size = new System.Drawing.Size(74, 17);
         this.checkRestoreMessages.TabIndex = 29;
         this.checkRestoreMessages.Text = "Messages";
         this.checkRestoreMessages.UseVisualStyleBackColor = true;
         // 
         // checkRestoreDomains
         // 
         this.checkRestoreDomains.AutoSize = true;
         this.checkRestoreDomains.Enabled = false;
         this.checkRestoreDomains.Location = new System.Drawing.Point(30, 107);
         this.checkRestoreDomains.Name = "checkRestoreDomains";
         this.checkRestoreDomains.Size = new System.Drawing.Size(67, 17);
         this.checkRestoreDomains.TabIndex = 28;
         this.checkRestoreDomains.Text = "Domains";
         this.checkRestoreDomains.UseVisualStyleBackColor = true;
         // 
         // checkRestoreSettings
         // 
         this.checkRestoreSettings.AutoSize = true;
         this.checkRestoreSettings.Enabled = false;
         this.checkRestoreSettings.Location = new System.Drawing.Point(30, 84);
         this.checkRestoreSettings.Name = "checkRestoreSettings";
         this.checkRestoreSettings.Size = new System.Drawing.Size(64, 17);
         this.checkRestoreSettings.TabIndex = 27;
         this.checkRestoreSettings.Text = "Settings";
         this.checkRestoreSettings.UseVisualStyleBackColor = true;
         // 
         // textRestoreFile
         // 
         this.textRestoreFile.Location = new System.Drawing.Point(8, 24);
         this.textRestoreFile.Name = "textRestoreFile";
         this.textRestoreFile.Number = 0;
         this.textRestoreFile.Number64 = ((long)(0));
         this.textRestoreFile.Numeric = false;
         this.textRestoreFile.Size = new System.Drawing.Size(282, 20);
         this.textRestoreFile.TabIndex = 24;
         this.textRestoreFile.TextChanged += new System.EventHandler(this.textRestoreFile_TextChanged);
         // 
         // openFileDialog
         // 
         this.openFileDialog.Filter = "hMailServer Backup File (HMBackup*.7z)|HMBackup*.7z";
         // 
         // ucBackup
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.tabControl);
         this.Name = "ucBackup";
         this.Size = new System.Drawing.Size(482, 400);
         this.tabControl.ResumeLayout(false);
         this.tabPage1.ResumeLayout(false);
         this.tabPage1.PerformLayout();
         this.tabPage2.ResumeLayout(false);
         this.tabPage2.PerformLayout();
         this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.TabControl tabControl;
       private System.Windows.Forms.TabPage tabPage1;
       private System.Windows.Forms.TabPage tabPage2;
       private hMailServer.Administrator.Controls.ucCheckbox checkBackupSettings;
       private hMailServer.Shared.ucText textDestination;
       private System.Windows.Forms.Label labelDestination;
       private System.Windows.Forms.Label labelBackup;
       private System.Windows.Forms.Button buttonSelectBackupPath;
       private hMailServer.Administrator.Controls.ucCheckbox checkCompressFiles;
       private hMailServer.Administrator.Controls.ucCheckbox checkBackupMessages;
       private hMailServer.Administrator.Controls.ucCheckbox checkBackupDomains;
       private System.Windows.Forms.Button buttonSelectBackupFile;
       private hMailServer.Shared.ucText textRestoreFile;
       private System.Windows.Forms.Label labelBackupFile;
       private System.Windows.Forms.Label labelRestore;
       private hMailServer.Administrator.Controls.ucCheckbox checkRestoreMessages;
       private hMailServer.Administrator.Controls.ucCheckbox checkRestoreDomains;
       private hMailServer.Administrator.Controls.ucCheckbox checkRestoreSettings;
       private System.Windows.Forms.Button buttonShowLog;
       private System.Windows.Forms.Button buttonStartBackup;
       private System.Windows.Forms.OpenFileDialog openFileDialog;
       private System.Windows.Forms.Button buttonShowLog2;
       private System.Windows.Forms.Button buttonStartRestore;
       private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog;
       private System.Windows.Forms.Label labelAutomate;
    }
}
