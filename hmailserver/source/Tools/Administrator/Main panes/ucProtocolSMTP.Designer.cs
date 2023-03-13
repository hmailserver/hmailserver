namespace hMailServer.Administrator
{
    partial class ucProtocolSMTP
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
         this.textMaxMessageSize = new hMailServer.Shared.ucText();
         this.labelMaxMessageSize = new System.Windows.Forms.Label();
         this.textWelcomeMessage = new hMailServer.Shared.ucText();
         this.labelWelcome = new System.Windows.Forms.Label();
         this.labelOther = new System.Windows.Forms.Label();
         this.textMaxSMTPConnections = new hMailServer.Shared.ucText();
         this.labelMaxPOP3Connections = new System.Windows.Forms.Label();
         this.labelConnections = new System.Windows.Forms.Label();
         this.tabPage2 = new System.Windows.Forms.TabPage();
         this.labelConnectionSecurity = new System.Windows.Forms.Label();
         this.labelPassword = new System.Windows.Forms.Label();
         this.labelUsername = new System.Windows.Forms.Label();
         this.labelRemotePort = new System.Windows.Forms.Label();
         this.labelRemoteHostName = new System.Windows.Forms.Label();
         this.labelSMTPRelayer = new System.Windows.Forms.Label();
         this.labelHostName = new System.Windows.Forms.Label();
         this.labelMinutesOfRetries = new System.Windows.Forms.Label();
         this.labelNumberOfRetries = new System.Windows.Forms.Label();
         this.labelDeliveryOfEmail = new System.Windows.Forms.Label();
         this.comboConnectionSecurity = new hMailServer.Administrator.Controls.ucComboBox();
         this.textSMTPRelayerPassword = new hMailServer.Administrator.Controls.ucPassword();
         this.textSMTPRelayerUsername = new hMailServer.Shared.ucText();
         this.chkSMTPRelayerRequiresAuth = new hMailServer.Administrator.Controls.ucCheckbox();
         this.textSMTPRelayerPort = new hMailServer.Shared.ucText();
         this.textSMTPRelayer = new hMailServer.Shared.ucText();
         this.textHostName = new hMailServer.Shared.ucText();
         this.textMinutesBetween = new hMailServer.Shared.ucText();
         this.textNoOfRetries = new hMailServer.Shared.ucText();
         this.tabPage4 = new System.Windows.Forms.TabPage();
         this.textMaxNumberOfInvalidCommands = new hMailServer.Shared.ucText();
         this.labelMaximumNumberOfInvalidCommands = new System.Windows.Forms.Label();
         this.checkDisconnectInvalidClients = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkAllowIncorrectLineEndings = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkAllowMailFromNull = new hMailServer.Administrator.Controls.ucCheckbox();
         this.checkAllowPlainTextAuthentication = new hMailServer.Administrator.Controls.ucCheckbox();
         this.tabPage5 = new System.Windows.Forms.TabPage();
         this.chkSmtpDeliveryConnectionSecurity = new hMailServer.Administrator.Controls.ucCheckbox();
         this.textMaxNumberOfMXHosts = new hMailServer.Shared.ucText();
         this.labelMaxNumberOfMXHosts = new System.Windows.Forms.Label();
         this.labelRuleLoopLimit = new System.Windows.Forms.Label();
         this.lableOther2 = new System.Windows.Forms.Label();
         this.lblMaxRecipientsInBatch = new System.Windows.Forms.Label();
         this.labelBindToLocalIP = new System.Windows.Forms.Label();
         this.labelDeliveryOfMail2 = new System.Windows.Forms.Label();
         this.textRuleLoopLimit = new hMailServer.Shared.ucText();
         this.checkAddDeliveredToHeader = new hMailServer.Administrator.Controls.ucCheckbox();
         this.textMaxSMTPRecipientsInBatch = new hMailServer.Shared.ucText();
         this.textBindToLocalIP = new hMailServer.Shared.ucText();
         this.tabControl.SuspendLayout();
         this.tabPage1.SuspendLayout();
         this.tabPage2.SuspendLayout();
         this.tabPage4.SuspendLayout();
         this.tabPage5.SuspendLayout();
         this.SuspendLayout();
         // 
         // tabControl
         // 
         this.tabControl.Controls.Add(this.tabPage1);
         this.tabControl.Controls.Add(this.tabPage2);
         this.tabControl.Controls.Add(this.tabPage4);
         this.tabControl.Controls.Add(this.tabPage5);
         this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
         this.tabControl.Location = new System.Drawing.Point(0, 0);
         this.tabControl.Name = "tabControl";
         this.tabControl.SelectedIndex = 0;
         this.tabControl.Size = new System.Drawing.Size(534, 435);
         this.tabControl.TabIndex = 0;
         // 
         // tabPage1
         // 
         this.tabPage1.Controls.Add(this.textMaxMessageSize);
         this.tabPage1.Controls.Add(this.labelMaxMessageSize);
         this.tabPage1.Controls.Add(this.textWelcomeMessage);
         this.tabPage1.Controls.Add(this.labelWelcome);
         this.tabPage1.Controls.Add(this.labelOther);
         this.tabPage1.Controls.Add(this.textMaxSMTPConnections);
         this.tabPage1.Controls.Add(this.labelMaxPOP3Connections);
         this.tabPage1.Controls.Add(this.labelConnections);
         this.tabPage1.Location = new System.Drawing.Point(4, 22);
         this.tabPage1.Name = "tabPage1";
         this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
         this.tabPage1.Size = new System.Drawing.Size(526, 409);
         this.tabPage1.TabIndex = 0;
         this.tabPage1.Text = "General";
         this.tabPage1.UseVisualStyleBackColor = true;
         // 
         // textMaxMessageSize
         // 
         this.textMaxMessageSize.Location = new System.Drawing.Point(28, 173);
         this.textMaxMessageSize.Name = "textMaxMessageSize";
         this.textMaxMessageSize.Number = 0;
         this.textMaxMessageSize.Number64 = ((long)(0));
         this.textMaxMessageSize.Numeric = true;
         this.textMaxMessageSize.Size = new System.Drawing.Size(117, 20);
         this.textMaxMessageSize.TabIndex = 18;
         // 
         // labelMaxMessageSize
         // 
         this.labelMaxMessageSize.AutoSize = true;
         this.labelMaxMessageSize.Location = new System.Drawing.Point(26, 154);
         this.labelMaxMessageSize.Name = "labelMaxMessageSize";
         this.labelMaxMessageSize.Size = new System.Drawing.Size(116, 13);
         this.labelMaxMessageSize.TabIndex = 17;
         this.labelMaxMessageSize.Text = "Max message size (KB)";
         // 
         // textWelcomeMessage
         // 
         this.textWelcomeMessage.Location = new System.Drawing.Point(27, 128);
         this.textWelcomeMessage.Name = "textWelcomeMessage";
         this.textWelcomeMessage.Number = 0;
         this.textWelcomeMessage.Number64 = ((long)(0));
         this.textWelcomeMessage.Numeric = false;
         this.textWelcomeMessage.Size = new System.Drawing.Size(178, 20);
         this.textWelcomeMessage.TabIndex = 16;
         // 
         // labelWelcome
         // 
         this.labelWelcome.AutoSize = true;
         this.labelWelcome.Location = new System.Drawing.Point(25, 109);
         this.labelWelcome.Name = "labelWelcome";
         this.labelWelcome.Size = new System.Drawing.Size(97, 13);
         this.labelWelcome.TabIndex = 15;
         this.labelWelcome.Text = "Welcome message";
         // 
         // labelOther
         // 
         this.labelOther.AutoSize = true;
         this.labelOther.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelOther.Location = new System.Drawing.Point(8, 87);
         this.labelOther.Name = "labelOther";
         this.labelOther.Size = new System.Drawing.Size(38, 13);
         this.labelOther.TabIndex = 14;
         this.labelOther.Text = "Other";
         // 
         // textMaxSMTPConnections
         // 
         this.textMaxSMTPConnections.Location = new System.Drawing.Point(26, 53);
         this.textMaxSMTPConnections.Name = "textMaxSMTPConnections";
         this.textMaxSMTPConnections.Number = 0;
         this.textMaxSMTPConnections.Number64 = ((long)(0));
         this.textMaxSMTPConnections.Numeric = true;
         this.textMaxSMTPConnections.Size = new System.Drawing.Size(117, 20);
         this.textMaxSMTPConnections.TabIndex = 13;
         // 
         // labelMaxPOP3Connections
         // 
         this.labelMaxPOP3Connections.AutoSize = true;
         this.labelMaxPOP3Connections.Location = new System.Drawing.Point(24, 34);
         this.labelMaxPOP3Connections.Name = "labelMaxPOP3Connections";
         this.labelMaxPOP3Connections.Size = new System.Drawing.Size(300, 13);
         this.labelMaxPOP3Connections.TabIndex = 12;
         this.labelMaxPOP3Connections.Text = "Maximum number of simultaneous connections (0 for unlimited)";
         // 
         // labelConnections
         // 
         this.labelConnections.AutoSize = true;
         this.labelConnections.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelConnections.Location = new System.Drawing.Point(8, 9);
         this.labelConnections.Name = "labelConnections";
         this.labelConnections.Size = new System.Drawing.Size(77, 13);
         this.labelConnections.TabIndex = 11;
         this.labelConnections.Text = "Connections";
         // 
         // tabPage2
         // 
         this.tabPage2.Controls.Add(this.labelConnectionSecurity);
         this.tabPage2.Controls.Add(this.labelPassword);
         this.tabPage2.Controls.Add(this.labelUsername);
         this.tabPage2.Controls.Add(this.labelRemotePort);
         this.tabPage2.Controls.Add(this.labelRemoteHostName);
         this.tabPage2.Controls.Add(this.labelSMTPRelayer);
         this.tabPage2.Controls.Add(this.labelHostName);
         this.tabPage2.Controls.Add(this.labelMinutesOfRetries);
         this.tabPage2.Controls.Add(this.labelNumberOfRetries);
         this.tabPage2.Controls.Add(this.labelDeliveryOfEmail);
         this.tabPage2.Controls.Add(this.comboConnectionSecurity);
         this.tabPage2.Controls.Add(this.textSMTPRelayerPassword);
         this.tabPage2.Controls.Add(this.textSMTPRelayerUsername);
         this.tabPage2.Controls.Add(this.chkSMTPRelayerRequiresAuth);
         this.tabPage2.Controls.Add(this.textSMTPRelayerPort);
         this.tabPage2.Controls.Add(this.textSMTPRelayer);
         this.tabPage2.Controls.Add(this.textHostName);
         this.tabPage2.Controls.Add(this.textMinutesBetween);
         this.tabPage2.Controls.Add(this.textNoOfRetries);
         this.tabPage2.Location = new System.Drawing.Point(4, 22);
         this.tabPage2.Name = "tabPage2";
         this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
         this.tabPage2.Size = new System.Drawing.Size(526, 409);
         this.tabPage2.TabIndex = 1;
         this.tabPage2.Text = "Delivery of e-mail";
         this.tabPage2.UseVisualStyleBackColor = true;
         // 
         // labelConnectionSecurity
         // 
         this.labelConnectionSecurity.AutoSize = true;
         this.labelConnectionSecurity.Location = new System.Drawing.Point(32, 323);
         this.labelConnectionSecurity.Name = "labelConnectionSecurity";
         this.labelConnectionSecurity.Size = new System.Drawing.Size(100, 13);
         this.labelConnectionSecurity.TabIndex = 39;
         this.labelConnectionSecurity.Text = "Connection security";
         // 
         // labelPassword
         // 
         this.labelPassword.AutoSize = true;
         this.labelPassword.Location = new System.Drawing.Point(61, 278);
         this.labelPassword.Name = "labelPassword";
         this.labelPassword.Size = new System.Drawing.Size(53, 13);
         this.labelPassword.TabIndex = 28;
         this.labelPassword.Text = "Password";
         // 
         // labelUsername
         // 
         this.labelUsername.AutoSize = true;
         this.labelUsername.Location = new System.Drawing.Point(60, 234);
         this.labelUsername.Name = "labelUsername";
         this.labelUsername.Size = new System.Drawing.Size(58, 13);
         this.labelUsername.TabIndex = 26;
         this.labelUsername.Text = "User name";
         // 
         // labelRemotePort
         // 
         this.labelRemotePort.AutoSize = true;
         this.labelRemotePort.Location = new System.Drawing.Point(186, 165);
         this.labelRemotePort.Name = "labelRemotePort";
         this.labelRemotePort.Size = new System.Drawing.Size(104, 13);
         this.labelRemotePort.TabIndex = 23;
         this.labelRemotePort.Text = "Remote TCP/IP port";
         // 
         // labelRemoteHostName
         // 
         this.labelRemoteHostName.AutoSize = true;
         this.labelRemoteHostName.Location = new System.Drawing.Point(31, 165);
         this.labelRemoteHostName.Name = "labelRemoteHostName";
         this.labelRemoteHostName.Size = new System.Drawing.Size(96, 13);
         this.labelRemoteHostName.TabIndex = 21;
         this.labelRemoteHostName.Text = "Remote host name";
         // 
         // labelSMTPRelayer
         // 
         this.labelSMTPRelayer.AutoSize = true;
         this.labelSMTPRelayer.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelSMTPRelayer.Location = new System.Drawing.Point(15, 142);
         this.labelSMTPRelayer.Name = "labelSMTPRelayer";
         this.labelSMTPRelayer.Size = new System.Drawing.Size(88, 13);
         this.labelSMTPRelayer.TabIndex = 20;
         this.labelSMTPRelayer.Text = "SMTP Relayer";
         // 
         // labelHostName
         // 
         this.labelHostName.AutoSize = true;
         this.labelHostName.Location = new System.Drawing.Point(33, 82);
         this.labelHostName.Name = "labelHostName";
         this.labelHostName.Size = new System.Drawing.Size(85, 13);
         this.labelHostName.TabIndex = 18;
         this.labelHostName.Text = "Local host name";
         // 
         // labelMinutesOfRetries
         // 
         this.labelMinutesOfRetries.AutoSize = true;
         this.labelMinutesOfRetries.Location = new System.Drawing.Point(186, 34);
         this.labelMinutesOfRetries.Name = "labelMinutesOfRetries";
         this.labelMinutesOfRetries.Size = new System.Drawing.Size(140, 13);
         this.labelMinutesOfRetries.TabIndex = 16;
         this.labelMinutesOfRetries.Text = "Minutes between every retry";
         // 
         // labelNumberOfRetries
         // 
         this.labelNumberOfRetries.AutoSize = true;
         this.labelNumberOfRetries.Location = new System.Drawing.Point(32, 34);
         this.labelNumberOfRetries.Name = "labelNumberOfRetries";
         this.labelNumberOfRetries.Size = new System.Drawing.Size(87, 13);
         this.labelNumberOfRetries.TabIndex = 14;
         this.labelNumberOfRetries.Text = "Number of retries";
         // 
         // labelDeliveryOfEmail
         // 
         this.labelDeliveryOfEmail.AutoSize = true;
         this.labelDeliveryOfEmail.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelDeliveryOfEmail.Location = new System.Drawing.Point(16, 12);
         this.labelDeliveryOfEmail.Name = "labelDeliveryOfEmail";
         this.labelDeliveryOfEmail.Size = new System.Drawing.Size(105, 13);
         this.labelDeliveryOfEmail.TabIndex = 12;
         this.labelDeliveryOfEmail.Text = "Delivery of e-mail";
         // 
         // comboConnectionSecurity
         // 
         this.comboConnectionSecurity.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
         this.comboConnectionSecurity.FormattingEnabled = true;
         this.comboConnectionSecurity.Location = new System.Drawing.Point(35, 339);
         this.comboConnectionSecurity.Name = "comboConnectionSecurity";
         this.comboConnectionSecurity.Size = new System.Drawing.Size(171, 21);
         this.comboConnectionSecurity.TabIndex = 38;
         // 
         // textSMTPRelayerPassword
         // 
         this.textSMTPRelayerPassword.Location = new System.Drawing.Point(63, 293);
         this.textSMTPRelayerPassword.Name = "textSMTPRelayerPassword";
         this.textSMTPRelayerPassword.Size = new System.Drawing.Size(120, 20);
         this.textSMTPRelayerPassword.TabIndex = 31;
         this.textSMTPRelayerPassword.Text = "<< Encrypted >>";
         // 
         // textSMTPRelayerUsername
         // 
         this.textSMTPRelayerUsername.Location = new System.Drawing.Point(62, 250);
         this.textSMTPRelayerUsername.Name = "textSMTPRelayerUsername";
         this.textSMTPRelayerUsername.Number = 0;
         this.textSMTPRelayerUsername.Number64 = ((long)(0));
         this.textSMTPRelayerUsername.Numeric = false;
         this.textSMTPRelayerUsername.Size = new System.Drawing.Size(121, 20);
         this.textSMTPRelayerUsername.TabIndex = 27;
         // 
         // chkSMTPRelayerRequiresAuth
         // 
         this.chkSMTPRelayerRequiresAuth.AutoSize = true;
         this.chkSMTPRelayerRequiresAuth.Location = new System.Drawing.Point(33, 210);
         this.chkSMTPRelayerRequiresAuth.Name = "chkSMTPRelayerRequiresAuth";
         this.chkSMTPRelayerRequiresAuth.Size = new System.Drawing.Size(167, 17);
         this.chkSMTPRelayerRequiresAuth.TabIndex = 25;
         this.chkSMTPRelayerRequiresAuth.Text = "Server requires authentication";
         this.chkSMTPRelayerRequiresAuth.UseVisualStyleBackColor = true;
         this.chkSMTPRelayerRequiresAuth.CheckedChanged += new System.EventHandler(this.chkSMTPRelayerRequiresAuth_CheckedChanged);
         // 
         // textSMTPRelayerPort
         // 
         this.textSMTPRelayerPort.Location = new System.Drawing.Point(188, 184);
         this.textSMTPRelayerPort.Name = "textSMTPRelayerPort";
         this.textSMTPRelayerPort.Number = 0;
         this.textSMTPRelayerPort.Number64 = ((long)(0));
         this.textSMTPRelayerPort.Numeric = true;
         this.textSMTPRelayerPort.Size = new System.Drawing.Size(84, 20);
         this.textSMTPRelayerPort.TabIndex = 24;
         // 
         // textSMTPRelayer
         // 
         this.textSMTPRelayer.Location = new System.Drawing.Point(33, 184);
         this.textSMTPRelayer.Name = "textSMTPRelayer";
         this.textSMTPRelayer.Number = 0;
         this.textSMTPRelayer.Number64 = ((long)(0));
         this.textSMTPRelayer.Numeric = false;
         this.textSMTPRelayer.Size = new System.Drawing.Size(146, 20);
         this.textSMTPRelayer.TabIndex = 22;
         // 
         // textHostName
         // 
         this.textHostName.Location = new System.Drawing.Point(35, 101);
         this.textHostName.Name = "textHostName";
         this.textHostName.Number = 0;
         this.textHostName.Number64 = ((long)(0));
         this.textHostName.Numeric = false;
         this.textHostName.Size = new System.Drawing.Size(145, 20);
         this.textHostName.TabIndex = 19;
         // 
         // textMinutesBetween
         // 
         this.textMinutesBetween.Location = new System.Drawing.Point(188, 53);
         this.textMinutesBetween.Name = "textMinutesBetween";
         this.textMinutesBetween.Number = 0;
         this.textMinutesBetween.Number64 = ((long)(0));
         this.textMinutesBetween.Numeric = true;
         this.textMinutesBetween.Size = new System.Drawing.Size(109, 20);
         this.textMinutesBetween.TabIndex = 17;
         // 
         // textNoOfRetries
         // 
         this.textNoOfRetries.Location = new System.Drawing.Point(34, 53);
         this.textNoOfRetries.Name = "textNoOfRetries";
         this.textNoOfRetries.Number = 0;
         this.textNoOfRetries.Number64 = ((long)(0));
         this.textNoOfRetries.Numeric = true;
         this.textNoOfRetries.Size = new System.Drawing.Size(109, 20);
         this.textNoOfRetries.TabIndex = 15;
         // 
         // tabPage4
         // 
         this.tabPage4.Controls.Add(this.textMaxNumberOfInvalidCommands);
         this.tabPage4.Controls.Add(this.labelMaximumNumberOfInvalidCommands);
         this.tabPage4.Controls.Add(this.checkDisconnectInvalidClients);
         this.tabPage4.Controls.Add(this.checkAllowIncorrectLineEndings);
         this.tabPage4.Controls.Add(this.checkAllowMailFromNull);
         this.tabPage4.Controls.Add(this.checkAllowPlainTextAuthentication);
         this.tabPage4.Location = new System.Drawing.Point(4, 22);
         this.tabPage4.Name = "tabPage4";
         this.tabPage4.Size = new System.Drawing.Size(526, 409);
         this.tabPage4.TabIndex = 3;
         this.tabPage4.Text = "RFC compliance";
         this.tabPage4.UseVisualStyleBackColor = true;
         // 
         // textMaxNumberOfInvalidCommands
         // 
         this.textMaxNumberOfInvalidCommands.Location = new System.Drawing.Point(37, 126);
         this.textMaxNumberOfInvalidCommands.Name = "textMaxNumberOfInvalidCommands";
         this.textMaxNumberOfInvalidCommands.Number = 0;
         this.textMaxNumberOfInvalidCommands.Number64 = ((long)(0));
         this.textMaxNumberOfInvalidCommands.Numeric = true;
         this.textMaxNumberOfInvalidCommands.Size = new System.Drawing.Size(72, 20);
         this.textMaxNumberOfInvalidCommands.TabIndex = 14;
         // 
         // labelMaximumNumberOfInvalidCommands
         // 
         this.labelMaximumNumberOfInvalidCommands.AutoSize = true;
         this.labelMaximumNumberOfInvalidCommands.Location = new System.Drawing.Point(34, 110);
         this.labelMaximumNumberOfInvalidCommands.Name = "labelMaximumNumberOfInvalidCommands";
         this.labelMaximumNumberOfInvalidCommands.Size = new System.Drawing.Size(188, 13);
         this.labelMaximumNumberOfInvalidCommands.TabIndex = 13;
         this.labelMaximumNumberOfInvalidCommands.Text = "Maximum number of invalid commands";
         // 
         // checkDisconnectInvalidClients
         // 
         this.checkDisconnectInvalidClients.AutoSize = true;
         this.checkDisconnectInvalidClients.Location = new System.Drawing.Point(14, 81);
         this.checkDisconnectInvalidClients.Name = "checkDisconnectInvalidClients";
         this.checkDisconnectInvalidClients.Size = new System.Drawing.Size(265, 17);
         this.checkDisconnectInvalidClients.TabIndex = 5;
         this.checkDisconnectInvalidClients.Text = "Disconnect client after too many invalid commands";
         this.checkDisconnectInvalidClients.UseVisualStyleBackColor = true;
         this.checkDisconnectInvalidClients.CheckedChanged += new System.EventHandler(this.checkDisconnectInvalidClients_CheckedChanged);
         // 
         // checkAllowIncorrectLineEndings
         // 
         this.checkAllowIncorrectLineEndings.AutoSize = true;
         this.checkAllowIncorrectLineEndings.Location = new System.Drawing.Point(14, 58);
         this.checkAllowIncorrectLineEndings.Name = "checkAllowIncorrectLineEndings";
         this.checkAllowIncorrectLineEndings.Size = new System.Drawing.Size(208, 17);
         this.checkAllowIncorrectLineEndings.TabIndex = 4;
         this.checkAllowIncorrectLineEndings.Text = "Allow incorrectly formatted line endings";
         this.checkAllowIncorrectLineEndings.UseVisualStyleBackColor = true;
         // 
         // checkAllowMailFromNull
         // 
         this.checkAllowMailFromNull.AutoSize = true;
         this.checkAllowMailFromNull.Location = new System.Drawing.Point(14, 35);
         this.checkAllowMailFromNull.Name = "checkAllowMailFromNull";
         this.checkAllowMailFromNull.Size = new System.Drawing.Size(157, 17);
         this.checkAllowMailFromNull.TabIndex = 3;
         this.checkAllowMailFromNull.Text = "Allow empty sender address";
         this.checkAllowMailFromNull.UseVisualStyleBackColor = true;
         // 
         // checkAllowPlainTextAuthentication
         // 
         this.checkAllowPlainTextAuthentication.AutoSize = true;
         this.checkAllowPlainTextAuthentication.Location = new System.Drawing.Point(14, 12);
         this.checkAllowPlainTextAuthentication.Name = "checkAllowPlainTextAuthentication";
         this.checkAllowPlainTextAuthentication.Size = new System.Drawing.Size(166, 17);
         this.checkAllowPlainTextAuthentication.TabIndex = 2;
         this.checkAllowPlainTextAuthentication.Text = "Allow plain text authentication";
         this.checkAllowPlainTextAuthentication.UseVisualStyleBackColor = true;
         // 
         // tabPage5
         // 
         this.tabPage5.Controls.Add(this.chkSmtpDeliveryConnectionSecurity);
         this.tabPage5.Controls.Add(this.textMaxNumberOfMXHosts);
         this.tabPage5.Controls.Add(this.labelMaxNumberOfMXHosts);
         this.tabPage5.Controls.Add(this.labelRuleLoopLimit);
         this.tabPage5.Controls.Add(this.lableOther2);
         this.tabPage5.Controls.Add(this.lblMaxRecipientsInBatch);
         this.tabPage5.Controls.Add(this.labelBindToLocalIP);
         this.tabPage5.Controls.Add(this.labelDeliveryOfMail2);
         this.tabPage5.Controls.Add(this.textRuleLoopLimit);
         this.tabPage5.Controls.Add(this.checkAddDeliveredToHeader);
         this.tabPage5.Controls.Add(this.textMaxSMTPRecipientsInBatch);
         this.tabPage5.Controls.Add(this.textBindToLocalIP);
         this.tabPage5.Location = new System.Drawing.Point(4, 22);
         this.tabPage5.Name = "tabPage5";
         this.tabPage5.Size = new System.Drawing.Size(526, 409);
         this.tabPage5.TabIndex = 4;
         this.tabPage5.Text = "Advanced";
         this.tabPage5.UseVisualStyleBackColor = true;
         // 
         // chkSmtpDeliveryConnectionSecurity
         // 
         this.chkSmtpDeliveryConnectionSecurity.AutoSize = true;
         this.chkSmtpDeliveryConnectionSecurity.Location = new System.Drawing.Point(33, 138);
         this.chkSmtpDeliveryConnectionSecurity.Name = "chkSmtpDeliveryConnectionSecurity";
         this.chkSmtpDeliveryConnectionSecurity.Size = new System.Drawing.Size(157, 17);
         this.chkSmtpDeliveryConnectionSecurity.TabIndex = 41;
         this.chkSmtpDeliveryConnectionSecurity.Text = "Use STARTTLS if available";
         this.chkSmtpDeliveryConnectionSecurity.UseVisualStyleBackColor = true;
         // 
         // textMaxNumberOfMXHosts
         // 
         this.textMaxNumberOfMXHosts.Location = new System.Drawing.Point(33, 285);
         this.textMaxNumberOfMXHosts.Name = "textMaxNumberOfMXHosts";
         this.textMaxNumberOfMXHosts.Number = 0;
         this.textMaxNumberOfMXHosts.Number64 = ((long)(0));
         this.textMaxNumberOfMXHosts.Numeric = true;
         this.textMaxNumberOfMXHosts.Size = new System.Drawing.Size(109, 20);
         this.textMaxNumberOfMXHosts.TabIndex = 36;
         // 
         // labelMaxNumberOfMXHosts
         // 
         this.labelMaxNumberOfMXHosts.AutoSize = true;
         this.labelMaxNumberOfMXHosts.Location = new System.Drawing.Point(31, 269);
         this.labelMaxNumberOfMXHosts.Name = "labelMaxNumberOfMXHosts";
         this.labelMaxNumberOfMXHosts.Size = new System.Drawing.Size(172, 13);
         this.labelMaxNumberOfMXHosts.TabIndex = 35;
         this.labelMaxNumberOfMXHosts.Text = "Maximum number of recipient hosts";
         // 
         // labelRuleLoopLimit
         // 
         this.labelRuleLoopLimit.AutoSize = true;
         this.labelRuleLoopLimit.Location = new System.Drawing.Point(31, 219);
         this.labelRuleLoopLimit.Name = "labelRuleLoopLimit";
         this.labelRuleLoopLimit.Size = new System.Drawing.Size(80, 13);
         this.labelRuleLoopLimit.TabIndex = 33;
         this.labelRuleLoopLimit.Text = "Rule Loop Limit";
         // 
         // lableOther2
         // 
         this.lableOther2.AutoSize = true;
         this.lableOther2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.lableOther2.Location = new System.Drawing.Point(12, 168);
         this.lableOther2.Name = "lableOther2";
         this.lableOther2.Size = new System.Drawing.Size(38, 13);
         this.lableOther2.TabIndex = 20;
         this.lableOther2.Text = "Other";
         // 
         // lblMaxRecipientsInBatch
         // 
         this.lblMaxRecipientsInBatch.AutoSize = true;
         this.lblMaxRecipientsInBatch.Location = new System.Drawing.Point(31, 86);
         this.lblMaxRecipientsInBatch.Name = "lblMaxRecipientsInBatch";
         this.lblMaxRecipientsInBatch.Size = new System.Drawing.Size(190, 13);
         this.lblMaxRecipientsInBatch.TabIndex = 18;
         this.lblMaxRecipientsInBatch.Text = "Maximum number of recipients in batch";
         // 
         // labelBindToLocalIP
         // 
         this.labelBindToLocalIP.AutoSize = true;
         this.labelBindToLocalIP.Location = new System.Drawing.Point(31, 35);
         this.labelBindToLocalIP.Name = "labelBindToLocalIP";
         this.labelBindToLocalIP.Size = new System.Drawing.Size(118, 13);
         this.labelBindToLocalIP.TabIndex = 16;
         this.labelBindToLocalIP.Text = "Bind to local IP address";
         // 
         // labelDeliveryOfMail2
         // 
         this.labelDeliveryOfMail2.AutoSize = true;
         this.labelDeliveryOfMail2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelDeliveryOfMail2.Location = new System.Drawing.Point(12, 12);
         this.labelDeliveryOfMail2.Name = "labelDeliveryOfMail2";
         this.labelDeliveryOfMail2.Size = new System.Drawing.Size(105, 13);
         this.labelDeliveryOfMail2.TabIndex = 13;
         this.labelDeliveryOfMail2.Text = "Delivery of e-mail";
         // 
         // textRuleLoopLimit
         // 
         this.textRuleLoopLimit.Location = new System.Drawing.Point(33, 238);
         this.textRuleLoopLimit.Name = "textRuleLoopLimit";
         this.textRuleLoopLimit.Number = 0;
         this.textRuleLoopLimit.Number64 = ((long)(0));
         this.textRuleLoopLimit.Numeric = true;
         this.textRuleLoopLimit.Size = new System.Drawing.Size(109, 20);
         this.textRuleLoopLimit.TabIndex = 34;
         // 
         // checkAddDeliveredToHeader
         // 
         this.checkAddDeliveredToHeader.AutoSize = true;
         this.checkAddDeliveredToHeader.Location = new System.Drawing.Point(33, 196);
         this.checkAddDeliveredToHeader.Name = "checkAddDeliveredToHeader";
         this.checkAddDeliveredToHeader.Size = new System.Drawing.Size(145, 17);
         this.checkAddDeliveredToHeader.TabIndex = 32;
         this.checkAddDeliveredToHeader.Text = "Add Delivered-To header";
         this.checkAddDeliveredToHeader.UseVisualStyleBackColor = true;
         // 
         // textMaxSMTPRecipientsInBatch
         // 
         this.textMaxSMTPRecipientsInBatch.Location = new System.Drawing.Point(33, 102);
         this.textMaxSMTPRecipientsInBatch.Name = "textMaxSMTPRecipientsInBatch";
         this.textMaxSMTPRecipientsInBatch.Number = 0;
         this.textMaxSMTPRecipientsInBatch.Number64 = ((long)(0));
         this.textMaxSMTPRecipientsInBatch.Numeric = true;
         this.textMaxSMTPRecipientsInBatch.Size = new System.Drawing.Size(109, 20);
         this.textMaxSMTPRecipientsInBatch.TabIndex = 19;
         // 
         // textBindToLocalIP
         // 
         this.textBindToLocalIP.Location = new System.Drawing.Point(33, 54);
         this.textBindToLocalIP.Name = "textBindToLocalIP";
         this.textBindToLocalIP.Number = 0;
         this.textBindToLocalIP.Number64 = ((long)(0));
         this.textBindToLocalIP.Numeric = false;
         this.textBindToLocalIP.Size = new System.Drawing.Size(259, 20);
         this.textBindToLocalIP.TabIndex = 17;
         // 
         // ucProtocolSMTP
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.tabControl);
         this.Name = "ucProtocolSMTP";
         this.Size = new System.Drawing.Size(534, 435);
         this.tabControl.ResumeLayout(false);
         this.tabPage1.ResumeLayout(false);
         this.tabPage1.PerformLayout();
         this.tabPage2.ResumeLayout(false);
         this.tabPage2.PerformLayout();
         this.tabPage4.ResumeLayout(false);
         this.tabPage4.PerformLayout();
         this.tabPage5.ResumeLayout(false);
         this.tabPage5.PerformLayout();
         this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.TabControl tabControl;
       private System.Windows.Forms.TabPage tabPage1;
       private System.Windows.Forms.TabPage tabPage2;
       private System.Windows.Forms.TabPage tabPage4;
       private System.Windows.Forms.TabPage tabPage5;
       private hMailServer.Shared.ucText textMaxMessageSize;
       private System.Windows.Forms.Label labelMaxMessageSize;
       private hMailServer.Shared.ucText textWelcomeMessage;
       private System.Windows.Forms.Label labelWelcome;
       private System.Windows.Forms.Label labelOther;
       private hMailServer.Shared.ucText textMaxSMTPConnections;
       private System.Windows.Forms.Label labelMaxPOP3Connections;
       private System.Windows.Forms.Label labelConnections;
       private hMailServer.Shared.ucText textHostName;
       private System.Windows.Forms.Label labelHostName;
       private hMailServer.Shared.ucText textMinutesBetween;
       private System.Windows.Forms.Label labelMinutesOfRetries;
       private hMailServer.Shared.ucText textNoOfRetries;
       private System.Windows.Forms.Label labelNumberOfRetries;
       private System.Windows.Forms.Label labelDeliveryOfEmail;
       private System.Windows.Forms.Label labelSMTPRelayer;
       private hMailServer.Shared.ucText textSMTPRelayerPort;
       private System.Windows.Forms.Label labelRemotePort;
       private hMailServer.Shared.ucText textSMTPRelayer;
       private System.Windows.Forms.Label labelRemoteHostName;
       private hMailServer.Administrator.Controls.ucCheckbox chkSMTPRelayerRequiresAuth;
       private System.Windows.Forms.Label labelPassword;
       private hMailServer.Shared.ucText textSMTPRelayerUsername;
       private System.Windows.Forms.Label labelUsername;
       private hMailServer.Administrator.Controls.ucCheckbox checkAllowPlainTextAuthentication;
       private hMailServer.Administrator.Controls.ucCheckbox checkAllowMailFromNull;
       private hMailServer.Administrator.Controls.ucCheckbox checkAllowIncorrectLineEndings;
       private hMailServer.Administrator.Controls.ucCheckbox checkDisconnectInvalidClients;
       private System.Windows.Forms.Label labelMaximumNumberOfInvalidCommands;
       private hMailServer.Shared.ucText textMaxNumberOfInvalidCommands;
       private hMailServer.Shared.ucText textBindToLocalIP;
       private System.Windows.Forms.Label labelBindToLocalIP;
       private System.Windows.Forms.Label labelDeliveryOfMail2;
       private hMailServer.Shared.ucText textMaxSMTPRecipientsInBatch;
       private System.Windows.Forms.Label lblMaxRecipientsInBatch;
       private hMailServer.Administrator.Controls.ucCheckbox checkAddDeliveredToHeader;
       private System.Windows.Forms.Label lableOther2;
       private hMailServer.Shared.ucText textRuleLoopLimit;
       private System.Windows.Forms.Label labelRuleLoopLimit;
       private hMailServer.Administrator.Controls.ucPassword textSMTPRelayerPassword;
       private hMailServer.Shared.ucText textMaxNumberOfMXHosts;
       private System.Windows.Forms.Label labelMaxNumberOfMXHosts;
       private System.Windows.Forms.Label labelConnectionSecurity;
       private hMailServer.Administrator.Controls.ucComboBox comboConnectionSecurity;
       private hMailServer.Administrator.Controls.ucCheckbox chkSmtpDeliveryConnectionSecurity;
    }
}
