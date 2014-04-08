namespace hMailServer.Administrator
{
    partial class ucAntiVirus
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
            this.textVirusScanMaxSize = new hMailServer.Shared.ucText();
            this.labelMaxSizeToScan = new System.Windows.Forms.Label();
            this.radioDeleteAttachments = new hMailServer.Administrator.Controls.ucRadioButton();
            this.checkNotifyReceiver = new hMailServer.Administrator.Controls.ucCheckbox();
            this.checkNotifySender = new hMailServer.Administrator.Controls.ucCheckbox();
            this.radioDeleteEmail = new hMailServer.Administrator.Controls.ucRadioButton();
            this.labelWhenAVirusIsFound = new System.Windows.Forms.Label();
            this.tabClamAV = new System.Windows.Forms.TabPage();
            this.buttonClamAVTest = new System.Windows.Forms.Button();
            this.labelTCPIPPort = new System.Windows.Forms.Label();
            this.labelHostname = new System.Windows.Forms.Label();
            this.textClamAVPort = new hMailServer.Shared.ucText();
            this.textClamAVHostName = new hMailServer.Shared.ucText();
            this.checkClamAVEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.buttonClamWinTest = new System.Windows.Forms.Button();
            this.buttonAutoDetect = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.textClamScanDatabase = new hMailServer.Shared.ucText();
            this.labelClamScanExecutable = new System.Windows.Forms.Label();
            this.textClamScanExecutable = new hMailServer.Shared.ucText();
            this.checkUseClamWin = new hMailServer.Administrator.Controls.ucCheckbox();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.buttonCustomScannerTest = new System.Windows.Forms.Button();
            this.labelReturnValue = new System.Windows.Forms.Label();
            this.textCustomScannerReturnValue = new hMailServer.Shared.ucText();
            this.labelScannerExecutable = new System.Windows.Forms.Label();
            this.textCustomScannerExecutable = new hMailServer.Shared.ucText();
            this.checkUseCustomScanner = new hMailServer.Administrator.Controls.ucCheckbox();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.checkBlockAttachmentsEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
            this.buttonEditBlockedAttachment = new System.Windows.Forms.Button();
            this.buttonDeleteBlockedAttachment = new System.Windows.Forms.Button();
            this.buttonAddBlockedAttachment = new System.Windows.Forms.Button();
            this.listBlockedAttachments = new hMailServer.Administrator.ucListView();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.tabControl.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabClamAV.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl
            // 
            this.tabControl.Controls.Add(this.tabPage1);
            this.tabControl.Controls.Add(this.tabClamAV);
            this.tabControl.Controls.Add(this.tabPage2);
            this.tabControl.Controls.Add(this.tabPage3);
            this.tabControl.Controls.Add(this.tabPage4);
            this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl.Location = new System.Drawing.Point(0, 0);
            this.tabControl.Margin = new System.Windows.Forms.Padding(4);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(975, 586);
            this.tabControl.TabIndex = 21;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.textVirusScanMaxSize);
            this.tabPage1.Controls.Add(this.labelMaxSizeToScan);
            this.tabPage1.Controls.Add(this.radioDeleteAttachments);
            this.tabPage1.Controls.Add(this.checkNotifyReceiver);
            this.tabPage1.Controls.Add(this.checkNotifySender);
            this.tabPage1.Controls.Add(this.radioDeleteEmail);
            this.tabPage1.Controls.Add(this.labelWhenAVirusIsFound);
            this.tabPage1.Location = new System.Drawing.Point(4, 25);
            this.tabPage1.Margin = new System.Windows.Forms.Padding(4);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(4);
            this.tabPage1.Size = new System.Drawing.Size(967, 557);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "General";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // textVirusScanMaxSize
            // 
            this.textVirusScanMaxSize.Location = new System.Drawing.Point(12, 187);
            this.textVirusScanMaxSize.Margin = new System.Windows.Forms.Padding(4);
            this.textVirusScanMaxSize.Name = "textVirusScanMaxSize";
            this.textVirusScanMaxSize.Number = 0;
            this.textVirusScanMaxSize.Numeric = true;
            this.textVirusScanMaxSize.Size = new System.Drawing.Size(153, 22);
            this.textVirusScanMaxSize.TabIndex = 27;
            // 
            // labelMaxSizeToScan
            // 
            this.labelMaxSizeToScan.AutoSize = true;
            this.labelMaxSizeToScan.Location = new System.Drawing.Point(9, 167);
            this.labelMaxSizeToScan.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelMaxSizeToScan.Name = "labelMaxSizeToScan";
            this.labelMaxSizeToScan.Size = new System.Drawing.Size(272, 17);
            this.labelMaxSizeToScan.TabIndex = 26;
            this.labelMaxSizeToScan.Text = "Maximum message size to virus scan (KB)";
            // 
            // radioDeleteAttachments
            // 
            this.radioDeleteAttachments.AutoSize = true;
            this.radioDeleteAttachments.Location = new System.Drawing.Point(44, 134);
            this.radioDeleteAttachments.Margin = new System.Windows.Forms.Padding(4);
            this.radioDeleteAttachments.Name = "radioDeleteAttachments";
            this.radioDeleteAttachments.Size = new System.Drawing.Size(151, 21);
            this.radioDeleteAttachments.TabIndex = 25;
            this.radioDeleteAttachments.Text = "Delete attachments";
            this.radioDeleteAttachments.UseVisualStyleBackColor = true;
            // 
            // checkNotifyReceiver
            // 
            this.checkNotifyReceiver.AutoSize = true;
            this.checkNotifyReceiver.Location = new System.Drawing.Point(79, 106);
            this.checkNotifyReceiver.Margin = new System.Windows.Forms.Padding(4);
            this.checkNotifyReceiver.Name = "checkNotifyReceiver";
            this.checkNotifyReceiver.Size = new System.Drawing.Size(124, 21);
            this.checkNotifyReceiver.TabIndex = 24;
            this.checkNotifyReceiver.Text = "Notify recipient";
            this.checkNotifyReceiver.UseVisualStyleBackColor = true;
            // 
            // checkNotifySender
            // 
            this.checkNotifySender.AutoSize = true;
            this.checkNotifySender.Location = new System.Drawing.Point(79, 78);
            this.checkNotifySender.Margin = new System.Windows.Forms.Padding(4);
            this.checkNotifySender.Name = "checkNotifySender";
            this.checkNotifySender.Size = new System.Drawing.Size(114, 21);
            this.checkNotifySender.TabIndex = 23;
            this.checkNotifySender.Text = "Notify sender";
            this.checkNotifySender.UseVisualStyleBackColor = true;
            // 
            // radioDeleteEmail
            // 
            this.radioDeleteEmail.AutoSize = true;
            this.radioDeleteEmail.Checked = true;
            this.radioDeleteEmail.Location = new System.Drawing.Point(44, 49);
            this.radioDeleteEmail.Margin = new System.Windows.Forms.Padding(4);
            this.radioDeleteEmail.Name = "radioDeleteEmail";
            this.radioDeleteEmail.Size = new System.Drawing.Size(112, 21);
            this.radioDeleteEmail.TabIndex = 22;
            this.radioDeleteEmail.TabStop = true;
            this.radioDeleteEmail.Text = "Delete e-mail";
            this.radioDeleteEmail.UseVisualStyleBackColor = true;
            this.radioDeleteEmail.CheckedChanged += new System.EventHandler(this.radioDeleteEmail_CheckedChanged);
            // 
            // labelWhenAVirusIsFound
            // 
            this.labelWhenAVirusIsFound.AutoSize = true;
            this.labelWhenAVirusIsFound.Location = new System.Drawing.Point(8, 17);
            this.labelWhenAVirusIsFound.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelWhenAVirusIsFound.Name = "labelWhenAVirusIsFound";
            this.labelWhenAVirusIsFound.Size = new System.Drawing.Size(145, 17);
            this.labelWhenAVirusIsFound.TabIndex = 21;
            this.labelWhenAVirusIsFound.Text = "When a virus is found";
            // 
            // tabClamAV
            // 
            this.tabClamAV.Controls.Add(this.buttonClamAVTest);
            this.tabClamAV.Controls.Add(this.labelTCPIPPort);
            this.tabClamAV.Controls.Add(this.labelHostname);
            this.tabClamAV.Controls.Add(this.textClamAVPort);
            this.tabClamAV.Controls.Add(this.textClamAVHostName);
            this.tabClamAV.Controls.Add(this.checkClamAVEnabled);
            this.tabClamAV.Location = new System.Drawing.Point(4, 25);
            this.tabClamAV.Name = "tabClamAV";
            this.tabClamAV.Size = new System.Drawing.Size(967, 557);
            this.tabClamAV.TabIndex = 4;
            this.tabClamAV.Text = "ClamAV";
            this.tabClamAV.UseVisualStyleBackColor = true;
            // 
            // buttonClamAVTest
            // 
            this.buttonClamAVTest.Location = new System.Drawing.Point(49, 172);
            this.buttonClamAVTest.Margin = new System.Windows.Forms.Padding(4);
            this.buttonClamAVTest.Name = "buttonClamAVTest";
            this.buttonClamAVTest.Size = new System.Drawing.Size(152, 33);
            this.buttonClamAVTest.TabIndex = 33;
            this.buttonClamAVTest.Text = "Test...";
            this.buttonClamAVTest.UseVisualStyleBackColor = true;
            this.buttonClamAVTest.Click += new System.EventHandler(this.buttonClamAVTest_Click);
            // 
            // labelTCPIPPort
            // 
            this.labelTCPIPPort.AutoSize = true;
            this.labelTCPIPPort.Location = new System.Drawing.Point(45, 111);
            this.labelTCPIPPort.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelTCPIPPort.Name = "labelTCPIPPort";
            this.labelTCPIPPort.Size = new System.Drawing.Size(80, 17);
            this.labelTCPIPPort.TabIndex = 31;
            this.labelTCPIPPort.Text = "TCP/IP port";
            // 
            // labelHostname
            // 
            this.labelHostname.AutoSize = true;
            this.labelHostname.Location = new System.Drawing.Point(45, 53);
            this.labelHostname.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelHostname.Name = "labelHostname";
            this.labelHostname.Size = new System.Drawing.Size(76, 17);
            this.labelHostname.TabIndex = 29;
            this.labelHostname.Text = "Host name";
            // 
            // textClamAVPort
            // 
            this.textClamAVPort.Location = new System.Drawing.Point(49, 130);
            this.textClamAVPort.Margin = new System.Windows.Forms.Padding(4);
            this.textClamAVPort.Name = "textClamAVPort";
            this.textClamAVPort.Number = 0;
            this.textClamAVPort.Numeric = true;
            this.textClamAVPort.Size = new System.Drawing.Size(103, 22);
            this.textClamAVPort.TabIndex = 32;
            // 
            // textClamAVHostName
            // 
            this.textClamAVHostName.Location = new System.Drawing.Point(49, 73);
            this.textClamAVHostName.Margin = new System.Windows.Forms.Padding(4);
            this.textClamAVHostName.Name = "textClamAVHostName";
            this.textClamAVHostName.Number = 0;
            this.textClamAVHostName.Numeric = false;
            this.textClamAVHostName.Size = new System.Drawing.Size(252, 22);
            this.textClamAVHostName.TabIndex = 30;
            // 
            // checkClamAVEnabled
            // 
            this.checkClamAVEnabled.AutoSize = true;
            this.checkClamAVEnabled.Checked = true;
            this.checkClamAVEnabled.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkClamAVEnabled.Location = new System.Drawing.Point(24, 22);
            this.checkClamAVEnabled.Margin = new System.Windows.Forms.Padding(4);
            this.checkClamAVEnabled.Name = "checkClamAVEnabled";
            this.checkClamAVEnabled.Size = new System.Drawing.Size(108, 21);
            this.checkClamAVEnabled.TabIndex = 14;
            this.checkClamAVEnabled.Text = "Use ClamAV";
            this.checkClamAVEnabled.UseVisualStyleBackColor = true;
            this.checkClamAVEnabled.CheckedChanged += new System.EventHandler(this.checkClamAVEnabled_CheckedChanged);
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.buttonClamWinTest);
            this.tabPage2.Controls.Add(this.buttonAutoDetect);
            this.tabPage2.Controls.Add(this.label1);
            this.tabPage2.Controls.Add(this.textClamScanDatabase);
            this.tabPage2.Controls.Add(this.labelClamScanExecutable);
            this.tabPage2.Controls.Add(this.textClamScanExecutable);
            this.tabPage2.Controls.Add(this.checkUseClamWin);
            this.tabPage2.Location = new System.Drawing.Point(4, 25);
            this.tabPage2.Margin = new System.Windows.Forms.Padding(4);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(4);
            this.tabPage2.Size = new System.Drawing.Size(967, 557);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "ClamWin";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // buttonClamWinTest
            // 
            this.buttonClamWinTest.Location = new System.Drawing.Point(49, 172);
            this.buttonClamWinTest.Margin = new System.Windows.Forms.Padding(4);
            this.buttonClamWinTest.Name = "buttonClamWinTest";
            this.buttonClamWinTest.Size = new System.Drawing.Size(152, 33);
            this.buttonClamWinTest.TabIndex = 34;
            this.buttonClamWinTest.Text = "Test...";
            this.buttonClamWinTest.UseVisualStyleBackColor = true;
            this.buttonClamWinTest.Click += new System.EventHandler(this.buttonClamWinTest_Click);
            // 
            // buttonAutoDetect
            // 
            this.buttonAutoDetect.Location = new System.Drawing.Point(491, 89);
            this.buttonAutoDetect.Margin = new System.Windows.Forms.Padding(4);
            this.buttonAutoDetect.Name = "buttonAutoDetect";
            this.buttonAutoDetect.Size = new System.Drawing.Size(133, 31);
            this.buttonAutoDetect.TabIndex = 32;
            this.buttonAutoDetect.Text = "Auto-detect";
            this.buttonAutoDetect.UseVisualStyleBackColor = true;
            this.buttonAutoDetect.Click += new System.EventHandler(this.buttonAutoDetect_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(45, 111);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(183, 17);
            this.label1.TabIndex = 31;
            this.label1.Text = "Path to ClamScan database";
            // 
            // textClamScanDatabase
            // 
            this.textClamScanDatabase.Location = new System.Drawing.Point(49, 130);
            this.textClamScanDatabase.Margin = new System.Windows.Forms.Padding(4);
            this.textClamScanDatabase.Name = "textClamScanDatabase";
            this.textClamScanDatabase.Number = 0;
            this.textClamScanDatabase.Numeric = false;
            this.textClamScanDatabase.Size = new System.Drawing.Size(419, 22);
            this.textClamScanDatabase.TabIndex = 30;
            // 
            // labelClamScanExecutable
            // 
            this.labelClamScanExecutable.AutoSize = true;
            this.labelClamScanExecutable.Location = new System.Drawing.Point(45, 53);
            this.labelClamScanExecutable.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelClamScanExecutable.Name = "labelClamScanExecutable";
            this.labelClamScanExecutable.Size = new System.Drawing.Size(143, 17);
            this.labelClamScanExecutable.TabIndex = 29;
            this.labelClamScanExecutable.Text = "ClamScan executable";
            // 
            // textClamScanExecutable
            // 
            this.textClamScanExecutable.Location = new System.Drawing.Point(49, 73);
            this.textClamScanExecutable.Margin = new System.Windows.Forms.Padding(4);
            this.textClamScanExecutable.Name = "textClamScanExecutable";
            this.textClamScanExecutable.Number = 0;
            this.textClamScanExecutable.Numeric = false;
            this.textClamScanExecutable.Size = new System.Drawing.Size(419, 22);
            this.textClamScanExecutable.TabIndex = 28;
            // 
            // checkUseClamWin
            // 
            this.checkUseClamWin.AutoSize = true;
            this.checkUseClamWin.Location = new System.Drawing.Point(24, 22);
            this.checkUseClamWin.Margin = new System.Windows.Forms.Padding(4);
            this.checkUseClamWin.Name = "checkUseClamWin";
            this.checkUseClamWin.Size = new System.Drawing.Size(114, 21);
            this.checkUseClamWin.TabIndex = 25;
            this.checkUseClamWin.Text = "Use ClamWin";
            this.checkUseClamWin.UseVisualStyleBackColor = true;
            this.checkUseClamWin.CheckedChanged += new System.EventHandler(this.checkUseClamWin_CheckedChanged);
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.buttonCustomScannerTest);
            this.tabPage3.Controls.Add(this.labelReturnValue);
            this.tabPage3.Controls.Add(this.textCustomScannerReturnValue);
            this.tabPage3.Controls.Add(this.labelScannerExecutable);
            this.tabPage3.Controls.Add(this.textCustomScannerExecutable);
            this.tabPage3.Controls.Add(this.checkUseCustomScanner);
            this.tabPage3.Location = new System.Drawing.Point(4, 25);
            this.tabPage3.Margin = new System.Windows.Forms.Padding(4);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(967, 557);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "External virus scanner";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // buttonCustomScannerTest
            // 
            this.buttonCustomScannerTest.Location = new System.Drawing.Point(49, 172);
            this.buttonCustomScannerTest.Margin = new System.Windows.Forms.Padding(4);
            this.buttonCustomScannerTest.Name = "buttonCustomScannerTest";
            this.buttonCustomScannerTest.Size = new System.Drawing.Size(152, 33);
            this.buttonCustomScannerTest.TabIndex = 37;
            this.buttonCustomScannerTest.Text = "Test...";
            this.buttonCustomScannerTest.UseVisualStyleBackColor = true;
            this.buttonCustomScannerTest.Click += new System.EventHandler(this.buttonCustomScannerTest_Click);
            // 
            // labelReturnValue
            // 
            this.labelReturnValue.AutoSize = true;
            this.labelReturnValue.Location = new System.Drawing.Point(45, 111);
            this.labelReturnValue.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelReturnValue.Name = "labelReturnValue";
            this.labelReturnValue.Size = new System.Drawing.Size(89, 17);
            this.labelReturnValue.TabIndex = 36;
            this.labelReturnValue.Text = "Return value";
            // 
            // textCustomScannerReturnValue
            // 
            this.textCustomScannerReturnValue.Location = new System.Drawing.Point(49, 130);
            this.textCustomScannerReturnValue.Margin = new System.Windows.Forms.Padding(4);
            this.textCustomScannerReturnValue.Name = "textCustomScannerReturnValue";
            this.textCustomScannerReturnValue.Number = 0;
            this.textCustomScannerReturnValue.Numeric = true;
            this.textCustomScannerReturnValue.Size = new System.Drawing.Size(157, 22);
            this.textCustomScannerReturnValue.TabIndex = 35;
            // 
            // labelScannerExecutable
            // 
            this.labelScannerExecutable.AutoSize = true;
            this.labelScannerExecutable.Location = new System.Drawing.Point(45, 53);
            this.labelScannerExecutable.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.labelScannerExecutable.Name = "labelScannerExecutable";
            this.labelScannerExecutable.Size = new System.Drawing.Size(133, 17);
            this.labelScannerExecutable.TabIndex = 34;
            this.labelScannerExecutable.Text = "Scanner executable";
            // 
            // textCustomScannerExecutable
            // 
            this.textCustomScannerExecutable.Location = new System.Drawing.Point(49, 73);
            this.textCustomScannerExecutable.Margin = new System.Windows.Forms.Padding(4);
            this.textCustomScannerExecutable.Name = "textCustomScannerExecutable";
            this.textCustomScannerExecutable.Number = 0;
            this.textCustomScannerExecutable.Numeric = false;
            this.textCustomScannerExecutable.Size = new System.Drawing.Size(328, 22);
            this.textCustomScannerExecutable.TabIndex = 33;
            // 
            // checkUseCustomScanner
            // 
            this.checkUseCustomScanner.AutoSize = true;
            this.checkUseCustomScanner.Location = new System.Drawing.Point(24, 22);
            this.checkUseCustomScanner.Margin = new System.Windows.Forms.Padding(4);
            this.checkUseCustomScanner.Name = "checkUseCustomScanner";
            this.checkUseCustomScanner.Size = new System.Drawing.Size(164, 21);
            this.checkUseCustomScanner.TabIndex = 32;
            this.checkUseCustomScanner.Text = "Use external scanner";
            this.checkUseCustomScanner.UseVisualStyleBackColor = true;
            this.checkUseCustomScanner.CheckedChanged += new System.EventHandler(this.checkUseCustomScanner_CheckedChanged);
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.checkBlockAttachmentsEnabled);
            this.tabPage4.Controls.Add(this.buttonEditBlockedAttachment);
            this.tabPage4.Controls.Add(this.buttonDeleteBlockedAttachment);
            this.tabPage4.Controls.Add(this.buttonAddBlockedAttachment);
            this.tabPage4.Controls.Add(this.listBlockedAttachments);
            this.tabPage4.Location = new System.Drawing.Point(4, 25);
            this.tabPage4.Margin = new System.Windows.Forms.Padding(4);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(967, 557);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "Block attachments";
            this.tabPage4.UseVisualStyleBackColor = true;
            // 
            // checkBlockAttachmentsEnabled
            // 
            this.checkBlockAttachmentsEnabled.AutoSize = true;
            this.checkBlockAttachmentsEnabled.Location = new System.Drawing.Point(11, 10);
            this.checkBlockAttachmentsEnabled.Margin = new System.Windows.Forms.Padding(4);
            this.checkBlockAttachmentsEnabled.Name = "checkBlockAttachmentsEnabled";
            this.checkBlockAttachmentsEnabled.Size = new System.Drawing.Size(330, 21);
            this.checkBlockAttachmentsEnabled.TabIndex = 51;
            this.checkBlockAttachmentsEnabled.Text = "Block attachments with the following extensions:";
            this.checkBlockAttachmentsEnabled.UseVisualStyleBackColor = true;
            // 
            // buttonEditBlockedAttachment
            // 
            this.buttonEditBlockedAttachment.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonEditBlockedAttachment.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.buttonEditBlockedAttachment.Location = new System.Drawing.Point(821, 79);
            this.buttonEditBlockedAttachment.Margin = new System.Windows.Forms.Padding(4);
            this.buttonEditBlockedAttachment.Name = "buttonEditBlockedAttachment";
            this.buttonEditBlockedAttachment.Size = new System.Drawing.Size(133, 31);
            this.buttonEditBlockedAttachment.TabIndex = 50;
            this.buttonEditBlockedAttachment.Text = "&Edit...";
            this.buttonEditBlockedAttachment.UseVisualStyleBackColor = true;
            this.buttonEditBlockedAttachment.Click += new System.EventHandler(this.buttonEditBlockedAttachment_Click);
            // 
            // buttonDeleteBlockedAttachment
            // 
            this.buttonDeleteBlockedAttachment.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonDeleteBlockedAttachment.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.buttonDeleteBlockedAttachment.Location = new System.Drawing.Point(821, 118);
            this.buttonDeleteBlockedAttachment.Margin = new System.Windows.Forms.Padding(4);
            this.buttonDeleteBlockedAttachment.Name = "buttonDeleteBlockedAttachment";
            this.buttonDeleteBlockedAttachment.Size = new System.Drawing.Size(133, 31);
            this.buttonDeleteBlockedAttachment.TabIndex = 49;
            this.buttonDeleteBlockedAttachment.Text = "Remove";
            this.buttonDeleteBlockedAttachment.UseVisualStyleBackColor = true;
            this.buttonDeleteBlockedAttachment.Click += new System.EventHandler(this.buttonDeleteBlockedAttachment_Click);
            // 
            // buttonAddBlockedAttachment
            // 
            this.buttonAddBlockedAttachment.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonAddBlockedAttachment.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.buttonAddBlockedAttachment.Location = new System.Drawing.Point(821, 39);
            this.buttonAddBlockedAttachment.Margin = new System.Windows.Forms.Padding(4);
            this.buttonAddBlockedAttachment.Name = "buttonAddBlockedAttachment";
            this.buttonAddBlockedAttachment.Size = new System.Drawing.Size(133, 31);
            this.buttonAddBlockedAttachment.TabIndex = 48;
            this.buttonAddBlockedAttachment.Text = "&Add...";
            this.buttonAddBlockedAttachment.UseVisualStyleBackColor = true;
            this.buttonAddBlockedAttachment.Click += new System.EventHandler(this.buttonAddBlockedAttachment_Click);
            // 
            // listBlockedAttachments
            // 
            this.listBlockedAttachments.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.listBlockedAttachments.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
            this.listBlockedAttachments.FullRowSelect = true;
            this.listBlockedAttachments.Location = new System.Drawing.Point(32, 39);
            this.listBlockedAttachments.Margin = new System.Windows.Forms.Padding(4);
            this.listBlockedAttachments.Name = "listBlockedAttachments";
            this.listBlockedAttachments.Size = new System.Drawing.Size(777, 501);
            this.listBlockedAttachments.TabIndex = 47;
            this.listBlockedAttachments.UseCompatibleStateImageBehavior = false;
            this.listBlockedAttachments.View = System.Windows.Forms.View.Details;
            this.listBlockedAttachments.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listBlockedAttachments_MouseDoubleClick);
            this.listBlockedAttachments.SelectedIndexChanged += new System.EventHandler(this.listBlockedAttachments_SelectedIndexChanged);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Name";
            this.columnHeader1.Width = 100;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Description";
            this.columnHeader2.Width = 250;
            // 
            // ucAntiVirus
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.tabControl);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "ucAntiVirus";
            this.Size = new System.Drawing.Size(975, 586);
            this.tabControl.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabClamAV.ResumeLayout(false);
            this.tabClamAV.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            this.tabPage4.ResumeLayout(false);
            this.tabPage4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.TabControl tabControl;
       private System.Windows.Forms.TabPage tabPage1;
       private hMailServer.Shared.ucText textVirusScanMaxSize;
       private System.Windows.Forms.Label labelMaxSizeToScan;
       private hMailServer.Administrator.Controls.ucRadioButton radioDeleteAttachments;
       private hMailServer.Administrator.Controls.ucCheckbox checkNotifyReceiver;
       private hMailServer.Administrator.Controls.ucCheckbox checkNotifySender;
       private hMailServer.Administrator.Controls.ucRadioButton radioDeleteEmail;
       private System.Windows.Forms.Label labelWhenAVirusIsFound;
       private System.Windows.Forms.TabPage tabPage2;
       private System.Windows.Forms.TabPage tabPage3;
       private System.Windows.Forms.TabPage tabPage4;
       private hMailServer.Administrator.Controls.ucCheckbox checkUseClamWin;
       private System.Windows.Forms.Label label1;
       private hMailServer.Shared.ucText textClamScanDatabase;
       private System.Windows.Forms.Label labelClamScanExecutable;
       private hMailServer.Shared.ucText textClamScanExecutable;
       private System.Windows.Forms.Button buttonAutoDetect;
       private System.Windows.Forms.Label labelReturnValue;
       private hMailServer.Shared.ucText textCustomScannerReturnValue;
       private System.Windows.Forms.Label labelScannerExecutable;
       private hMailServer.Shared.ucText textCustomScannerExecutable;
       private hMailServer.Administrator.Controls.ucCheckbox checkUseCustomScanner;
       private System.Windows.Forms.Button buttonEditBlockedAttachment;
       private System.Windows.Forms.Button buttonDeleteBlockedAttachment;
       private System.Windows.Forms.Button buttonAddBlockedAttachment;
       private ucListView listBlockedAttachments;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.ColumnHeader columnHeader2;
       private hMailServer.Administrator.Controls.ucCheckbox checkBlockAttachmentsEnabled;
       private System.Windows.Forms.TabPage tabClamAV;
       private hMailServer.Administrator.Controls.ucCheckbox checkClamAVEnabled;
       private System.Windows.Forms.Button buttonClamAVTest;
       private System.Windows.Forms.Label labelTCPIPPort;
       private System.Windows.Forms.Label labelHostname;
       private hMailServer.Shared.ucText textClamAVPort;
       private hMailServer.Shared.ucText textClamAVHostName;
       private System.Windows.Forms.Button buttonClamWinTest;
       private System.Windows.Forms.Button buttonCustomScannerTest;

     }
}
