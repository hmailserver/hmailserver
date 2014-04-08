
namespace hMailServer.Administrator
{
    partial class ucStatus
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
           this.components = new System.ComponentModel.Container();
           this.tabControl = new System.Windows.Forms.TabControl();
           this.tabServer = new System.Windows.Forms.TabPage();
           this.label1 = new System.Windows.Forms.Label();
           this.labelDBVersion = new System.Windows.Forms.Label();
           this.labelDatabaseVersion = new System.Windows.Forms.Label();
           this.labelDatabaseInformation = new System.Windows.Forms.Label();
           this.labelName = new System.Windows.Forms.Label();
           this.labelNameTitle = new System.Windows.Forms.Label();
           this.labelServerHost = new System.Windows.Forms.Label();
           this.labelServerHostTitle = new System.Windows.Forms.Label();
           this.labelServerType = new System.Windows.Forms.Label();
           this.labelServerTypeTitle = new System.Windows.Forms.Label();
           this.buttonShowWarning = new System.Windows.Forms.Button();
           this.listWarnings = new hMailServer.Administrator.ucListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
           this.labelWarnings = new System.Windows.Forms.Label();
           this.labelVersion = new System.Windows.Forms.Label();
           this.labelVersionTitle = new System.Windows.Forms.Label();
           this.labelServerInformation = new System.Windows.Forms.Label();
           this.buttonStartStop = new System.Windows.Forms.Button();
           this.labelCurrentStatus = new System.Windows.Forms.Label();
           this.labelAction = new System.Windows.Forms.Label();
           this.labelCurrentStatusTitle = new System.Windows.Forms.Label();
           this.tabStatus = new System.Windows.Forms.TabPage();
           this.textIMAPSessions = new hMailServer.Shared.ucText();
           this.textPOP3Sessions = new hMailServer.Shared.ucText();
           this.textSMTPSessions = new hMailServer.Shared.ucText();
           this.textMessagesContainingSpam = new hMailServer.Shared.ucText();
           this.textMessagesContainingViruses = new hMailServer.Shared.ucText();
           this.textProcessedMessages = new hMailServer.Shared.ucText();
           this.labelIMAP = new System.Windows.Forms.Label();
           this.labelPOP3 = new System.Windows.Forms.Label();
           this.labelSMTP = new System.Windows.Forms.Label();
           this.labelMessagesContainingSpam = new System.Windows.Forms.Label();
           this.labelMessagesContainingVirus = new System.Windows.Forms.Label();
           this.labelProcessedMessages = new System.Windows.Forms.Label();
           this.labelCurrentSessions = new System.Windows.Forms.Label();
           this.labelNumberOf = new System.Windows.Forms.Label();
           this.labelStartTime = new System.Windows.Forms.Label();
           this.labelStartTimeTitle = new System.Windows.Forms.Label();
           this.tabLogging = new System.Windows.Forms.TabPage();
           this.btnStartLiveLog = new System.Windows.Forms.Button();
           this.listLiveLog = new hMailServer.Administrator.ucListView();
           this.columnType = new System.Windows.Forms.ColumnHeader();
           this.columnSession = new System.Windows.Forms.ColumnHeader();
           this.columnThread = new System.Windows.Forms.ColumnHeader();
           this.columnTime = new System.Windows.Forms.ColumnHeader();
           this.columnIP = new System.Windows.Forms.ColumnHeader();
           this.columnText = new System.Windows.Forms.ColumnHeader();
           this.contextMenuStripLiveLog = new System.Windows.Forms.ContextMenuStrip(this.components);
           this.menuItemOnlyIncludeThisSessionToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
           this.btnClear = new System.Windows.Forms.Button();
           this.tabDeliveryQueue = new System.Windows.Forms.TabPage();
           this.labelNumberOfMessages = new System.Windows.Forms.Label();
           this.buttonRefreshDeliveryQueue = new System.Windows.Forms.Button();
           this.listDeliveryQueue = new hMailServer.Administrator.ucListView();
           this.columnID = new System.Windows.Forms.ColumnHeader();
           this.columnCreated = new System.Windows.Forms.ColumnHeader();
           this.columnFrom = new System.Windows.Forms.ColumnHeader();
           this.columnTo = new System.Windows.Forms.ColumnHeader();
           this.columnNextTry = new System.Windows.Forms.ColumnHeader();
           this.columnNoOfTries = new System.Windows.Forms.ColumnHeader();
           this.columnFilename = new System.Windows.Forms.ColumnHeader();
           this.contextMenuDeliveryQueue = new System.Windows.Forms.ContextMenuStrip(this.components);
           this.menuItemShow = new System.Windows.Forms.ToolStripMenuItem();
           this.menuItemSendNow = new System.Windows.Forms.ToolStripMenuItem();
           this.menuItemDelete = new System.Windows.Forms.ToolStripMenuItem();
           this.btnClearQueue = new System.Windows.Forms.Button();
           this.timerLiveLog = new System.Windows.Forms.Timer(this.components);
           this.timerServerStats = new System.Windows.Forms.Timer(this.components);
           this.tabControl.SuspendLayout();
           this.tabServer.SuspendLayout();
           this.tabStatus.SuspendLayout();
           this.tabLogging.SuspendLayout();
           this.contextMenuStripLiveLog.SuspendLayout();
           this.tabDeliveryQueue.SuspendLayout();
           this.contextMenuDeliveryQueue.SuspendLayout();
           this.SuspendLayout();
           // 
           // tabControl
           // 
           this.tabControl.Controls.Add(this.tabServer);
           this.tabControl.Controls.Add(this.tabStatus);
           this.tabControl.Controls.Add(this.tabLogging);
           this.tabControl.Controls.Add(this.tabDeliveryQueue);
           this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
           this.tabControl.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.tabControl.Location = new System.Drawing.Point(0, 0);
           this.tabControl.Name = "tabControl";
           this.tabControl.SelectedIndex = 0;
           this.tabControl.Size = new System.Drawing.Size(733, 404);
           this.tabControl.TabIndex = 0;
           // 
           // tabServer
           // 
           this.tabServer.Controls.Add(this.label1);
           this.tabServer.Controls.Add(this.labelDBVersion);
           this.tabServer.Controls.Add(this.labelDatabaseVersion);
           this.tabServer.Controls.Add(this.labelDatabaseInformation);
           this.tabServer.Controls.Add(this.labelName);
           this.tabServer.Controls.Add(this.labelNameTitle);
           this.tabServer.Controls.Add(this.labelServerHost);
           this.tabServer.Controls.Add(this.labelServerHostTitle);
           this.tabServer.Controls.Add(this.labelServerType);
           this.tabServer.Controls.Add(this.labelServerTypeTitle);
           this.tabServer.Controls.Add(this.buttonShowWarning);
           this.tabServer.Controls.Add(this.listWarnings);
           this.tabServer.Controls.Add(this.labelWarnings);
           this.tabServer.Controls.Add(this.labelVersion);
           this.tabServer.Controls.Add(this.labelVersionTitle);
           this.tabServer.Controls.Add(this.labelServerInformation);
           this.tabServer.Controls.Add(this.buttonStartStop);
           this.tabServer.Controls.Add(this.labelCurrentStatus);
           this.tabServer.Controls.Add(this.labelAction);
           this.tabServer.Controls.Add(this.labelCurrentStatusTitle);
           this.tabServer.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.tabServer.Location = new System.Drawing.Point(4, 22);
           this.tabServer.Name = "tabServer";
           this.tabServer.Padding = new System.Windows.Forms.Padding(3);
           this.tabServer.Size = new System.Drawing.Size(725, 378);
           this.tabServer.TabIndex = 0;
           this.tabServer.Text = "Server";
           this.tabServer.UseVisualStyleBackColor = true;
           // 
           // label1
           // 
           this.label1.AutoSize = true;
           this.label1.Location = new System.Drawing.Point(139, 68);
           this.label1.Name = "label1";
           this.label1.Size = new System.Drawing.Size(183, 13);
           this.label1.TabIndex = 19;
           this.label1.Text = "(Use Windows to Start/Stop Service)";
           // 
           // labelDBVersion
           // 
           this.labelDBVersion.AutoSize = true;
           this.labelDBVersion.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelDBVersion.Location = new System.Drawing.Point(602, 136);
           this.labelDBVersion.Name = "labelDBVersion";
           this.labelDBVersion.Size = new System.Drawing.Size(15, 13);
           this.labelDBVersion.TabIndex = 18;
           this.labelDBVersion.Text = "--";
           // 
           // labelDatabaseVersion
           // 
           this.labelDatabaseVersion.AutoSize = true;
           this.labelDatabaseVersion.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelDatabaseVersion.Location = new System.Drawing.Point(602, 112);
           this.labelDatabaseVersion.Name = "labelDatabaseVersion";
           this.labelDatabaseVersion.Size = new System.Drawing.Size(91, 13);
           this.labelDatabaseVersion.TabIndex = 17;
           this.labelDatabaseVersion.Text = "Database version";
           // 
           // labelDatabaseInformation
           // 
           this.labelDatabaseInformation.AutoSize = true;
           this.labelDatabaseInformation.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelDatabaseInformation.Location = new System.Drawing.Point(192, 88);
           this.labelDatabaseInformation.Name = "labelDatabaseInformation";
           this.labelDatabaseInformation.Size = new System.Drawing.Size(130, 13);
           this.labelDatabaseInformation.TabIndex = 16;
           this.labelDatabaseInformation.Text = "Database information";
           // 
           // labelName
           // 
           this.labelName.AutoSize = true;
           this.labelName.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelName.Location = new System.Drawing.Point(508, 136);
           this.labelName.Name = "labelName";
           this.labelName.Size = new System.Drawing.Size(15, 13);
           this.labelName.TabIndex = 15;
           this.labelName.Text = "--";
           // 
           // labelNameTitle
           // 
           this.labelNameTitle.AutoSize = true;
           this.labelNameTitle.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelNameTitle.Location = new System.Drawing.Point(508, 112);
           this.labelNameTitle.Name = "labelNameTitle";
           this.labelNameTitle.Size = new System.Drawing.Size(34, 13);
           this.labelNameTitle.TabIndex = 14;
           this.labelNameTitle.Text = "Name";
           // 
           // labelServerHost
           // 
           this.labelServerHost.AutoSize = true;
           this.labelServerHost.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelServerHost.Location = new System.Drawing.Point(358, 136);
           this.labelServerHost.Name = "labelServerHost";
           this.labelServerHost.Size = new System.Drawing.Size(15, 13);
           this.labelServerHost.TabIndex = 13;
           this.labelServerHost.Text = "--";
           // 
           // labelServerHostTitle
           // 
           this.labelServerHostTitle.AutoSize = true;
           this.labelServerHostTitle.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelServerHostTitle.Location = new System.Drawing.Point(358, 112);
           this.labelServerHostTitle.Name = "labelServerHostTitle";
           this.labelServerHostTitle.Size = new System.Drawing.Size(80, 13);
           this.labelServerHostTitle.TabIndex = 12;
           this.labelServerHostTitle.Text = "Server address";
           // 
           // labelServerType
           // 
           this.labelServerType.AutoSize = true;
           this.labelServerType.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelServerType.Location = new System.Drawing.Point(244, 136);
           this.labelServerType.Name = "labelServerType";
           this.labelServerType.Size = new System.Drawing.Size(15, 13);
           this.labelServerType.TabIndex = 11;
           this.labelServerType.Text = "--";
           // 
           // labelServerTypeTitle
           // 
           this.labelServerTypeTitle.AutoSize = true;
           this.labelServerTypeTitle.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelServerTypeTitle.Location = new System.Drawing.Point(244, 112);
           this.labelServerTypeTitle.Name = "labelServerTypeTitle";
           this.labelServerTypeTitle.Size = new System.Drawing.Size(31, 13);
           this.labelServerTypeTitle.TabIndex = 10;
           this.labelServerTypeTitle.Text = "Type";
           // 
           // buttonShowWarning
           // 
           this.buttonShowWarning.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonShowWarning.Enabled = false;
           this.buttonShowWarning.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.buttonShowWarning.Location = new System.Drawing.Point(616, 192);
           this.buttonShowWarning.Name = "buttonShowWarning";
           this.buttonShowWarning.Size = new System.Drawing.Size(100, 25);
           this.buttonShowWarning.TabIndex = 9;
           this.buttonShowWarning.Text = "Show";
           this.buttonShowWarning.UseVisualStyleBackColor = true;
           this.buttonShowWarning.Click += new System.EventHandler(this.buttonShowWarning_Click);
           // 
           // listWarnings
           // 
           this.listWarnings.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listWarnings.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader3});
           this.listWarnings.FullRowSelect = true;
           this.listWarnings.Location = new System.Drawing.Point(16, 192);
           this.listWarnings.Name = "listWarnings";
           this.listWarnings.ShowItemToolTips = true;
           this.listWarnings.Size = new System.Drawing.Size(592, 173);
           this.listWarnings.TabIndex = 8;
           this.listWarnings.UseCompatibleStateImageBehavior = false;
           this.listWarnings.View = System.Windows.Forms.View.Details;
           this.listWarnings.SelectedIndexChanged += new System.EventHandler(this.listWarnings_SelectedIndexChanged);
           this.listWarnings.DoubleClick += new System.EventHandler(this.listWarnings_DoubleClick);
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "ID";
           // 
           // columnHeader2
           // 
           this.columnHeader2.Text = "Severity";
           // 
           // columnHeader3
           // 
           this.columnHeader3.Text = "Description";
           this.columnHeader3.Width = 300;
           // 
           // labelWarnings
           // 
           this.labelWarnings.AutoSize = true;
           this.labelWarnings.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelWarnings.ForeColor = System.Drawing.Color.DarkRed;
           this.labelWarnings.Location = new System.Drawing.Point(8, 168);
           this.labelWarnings.Name = "labelWarnings";
           this.labelWarnings.Size = new System.Drawing.Size(137, 13);
           this.labelWarnings.TabIndex = 7;
           this.labelWarnings.Text = "Configuration warnings";
           // 
           // labelVersion
           // 
           this.labelVersion.AutoSize = true;
           this.labelVersion.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelVersion.Location = new System.Drawing.Point(32, 136);
           this.labelVersion.Name = "labelVersion";
           this.labelVersion.Size = new System.Drawing.Size(15, 13);
           this.labelVersion.TabIndex = 6;
           this.labelVersion.Text = "--";
           // 
           // labelVersionTitle
           // 
           this.labelVersionTitle.AutoSize = true;
           this.labelVersionTitle.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelVersionTitle.Location = new System.Drawing.Point(32, 112);
           this.labelVersionTitle.Name = "labelVersionTitle";
           this.labelVersionTitle.Size = new System.Drawing.Size(42, 13);
           this.labelVersionTitle.TabIndex = 5;
           this.labelVersionTitle.Text = "Version";
           // 
           // labelServerInformation
           // 
           this.labelServerInformation.AutoSize = true;
           this.labelServerInformation.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelServerInformation.Location = new System.Drawing.Point(6, 88);
           this.labelServerInformation.Name = "labelServerInformation";
           this.labelServerInformation.Size = new System.Drawing.Size(114, 13);
           this.labelServerInformation.TabIndex = 4;
           this.labelServerInformation.Text = "Server information";
           // 
           // buttonStartStop
           // 
           this.buttonStartStop.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.buttonStartStop.Location = new System.Drawing.Point(176, 40);
           this.buttonStartStop.Name = "buttonStartStop";
           this.buttonStartStop.Size = new System.Drawing.Size(100, 25);
           this.buttonStartStop.TabIndex = 3;
           this.buttonStartStop.Text = "Start";
           this.buttonStartStop.UseVisualStyleBackColor = true;
           this.buttonStartStop.Click += new System.EventHandler(this.buttonStartStop_Click);
           // 
           // labelCurrentStatus
           // 
           this.labelCurrentStatus.AutoSize = true;
           this.labelCurrentStatus.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelCurrentStatus.Location = new System.Drawing.Point(6, 42);
           this.labelCurrentStatus.Name = "labelCurrentStatus";
           this.labelCurrentStatus.Size = new System.Drawing.Size(38, 13);
           this.labelCurrentStatus.TabIndex = 2;
           this.labelCurrentStatus.Text = "Status";
           // 
           // labelAction
           // 
           this.labelAction.AutoSize = true;
           this.labelAction.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelAction.Location = new System.Drawing.Point(176, 16);
           this.labelAction.Name = "labelAction";
           this.labelAction.Size = new System.Drawing.Size(37, 13);
           this.labelAction.TabIndex = 1;
           this.labelAction.Text = "Action";
           // 
           // labelCurrentStatusTitle
           // 
           this.labelCurrentStatusTitle.AutoSize = true;
           this.labelCurrentStatusTitle.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelCurrentStatusTitle.Location = new System.Drawing.Point(6, 13);
           this.labelCurrentStatusTitle.Name = "labelCurrentStatusTitle";
           this.labelCurrentStatusTitle.Size = new System.Drawing.Size(77, 13);
           this.labelCurrentStatusTitle.TabIndex = 0;
           this.labelCurrentStatusTitle.Text = "Current status";
           // 
           // tabStatus
           // 
           this.tabStatus.Controls.Add(this.textIMAPSessions);
           this.tabStatus.Controls.Add(this.textPOP3Sessions);
           this.tabStatus.Controls.Add(this.textSMTPSessions);
           this.tabStatus.Controls.Add(this.textMessagesContainingSpam);
           this.tabStatus.Controls.Add(this.textMessagesContainingViruses);
           this.tabStatus.Controls.Add(this.textProcessedMessages);
           this.tabStatus.Controls.Add(this.labelIMAP);
           this.tabStatus.Controls.Add(this.labelPOP3);
           this.tabStatus.Controls.Add(this.labelSMTP);
           this.tabStatus.Controls.Add(this.labelMessagesContainingSpam);
           this.tabStatus.Controls.Add(this.labelMessagesContainingVirus);
           this.tabStatus.Controls.Add(this.labelProcessedMessages);
           this.tabStatus.Controls.Add(this.labelCurrentSessions);
           this.tabStatus.Controls.Add(this.labelNumberOf);
           this.tabStatus.Controls.Add(this.labelStartTime);
           this.tabStatus.Controls.Add(this.labelStartTimeTitle);
           this.tabStatus.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.tabStatus.Location = new System.Drawing.Point(4, 22);
           this.tabStatus.Name = "tabStatus";
           this.tabStatus.Padding = new System.Windows.Forms.Padding(3);
           this.tabStatus.Size = new System.Drawing.Size(725, 378);
           this.tabStatus.TabIndex = 1;
           this.tabStatus.Text = "Status";
           this.tabStatus.UseVisualStyleBackColor = true;
           // 
           // textIMAPSessions
           // 
           this.textIMAPSessions.Location = new System.Drawing.Point(262, 200);
           this.textIMAPSessions.Name = "textIMAPSessions";
           this.textIMAPSessions.Number = 0;
           this.textIMAPSessions.Number64 = ((long)(0));
           this.textIMAPSessions.Numeric = false;
           this.textIMAPSessions.ReadOnly = true;
           this.textIMAPSessions.Size = new System.Drawing.Size(91, 21);
           this.textIMAPSessions.TabIndex = 15;
           // 
           // textPOP3Sessions
           // 
           this.textPOP3Sessions.Location = new System.Drawing.Point(262, 155);
           this.textPOP3Sessions.Name = "textPOP3Sessions";
           this.textPOP3Sessions.Number = 0;
           this.textPOP3Sessions.Number64 = ((long)(0));
           this.textPOP3Sessions.Numeric = false;
           this.textPOP3Sessions.ReadOnly = true;
           this.textPOP3Sessions.Size = new System.Drawing.Size(91, 21);
           this.textPOP3Sessions.TabIndex = 14;
           // 
           // textSMTPSessions
           // 
           this.textSMTPSessions.Location = new System.Drawing.Point(262, 107);
           this.textSMTPSessions.Name = "textSMTPSessions";
           this.textSMTPSessions.Number = 0;
           this.textSMTPSessions.Number64 = ((long)(0));
           this.textSMTPSessions.Numeric = false;
           this.textSMTPSessions.ReadOnly = true;
           this.textSMTPSessions.Size = new System.Drawing.Size(91, 21);
           this.textSMTPSessions.TabIndex = 13;
           // 
           // textMessagesContainingSpam
           // 
           this.textMessagesContainingSpam.Location = new System.Drawing.Point(37, 200);
           this.textMessagesContainingSpam.Name = "textMessagesContainingSpam";
           this.textMessagesContainingSpam.Number = 0;
           this.textMessagesContainingSpam.Number64 = ((long)(0));
           this.textMessagesContainingSpam.Numeric = false;
           this.textMessagesContainingSpam.ReadOnly = true;
           this.textMessagesContainingSpam.Size = new System.Drawing.Size(132, 21);
           this.textMessagesContainingSpam.TabIndex = 12;
           // 
           // textMessagesContainingViruses
           // 
           this.textMessagesContainingViruses.Location = new System.Drawing.Point(36, 155);
           this.textMessagesContainingViruses.Name = "textMessagesContainingViruses";
           this.textMessagesContainingViruses.Number = 0;
           this.textMessagesContainingViruses.Number64 = ((long)(0));
           this.textMessagesContainingViruses.Numeric = false;
           this.textMessagesContainingViruses.ReadOnly = true;
           this.textMessagesContainingViruses.Size = new System.Drawing.Size(132, 21);
           this.textMessagesContainingViruses.TabIndex = 11;
           // 
           // textProcessedMessages
           // 
           this.textProcessedMessages.Location = new System.Drawing.Point(37, 107);
           this.textProcessedMessages.Name = "textProcessedMessages";
           this.textProcessedMessages.Number = 0;
           this.textProcessedMessages.Number64 = ((long)(0));
           this.textProcessedMessages.Numeric = false;
           this.textProcessedMessages.ReadOnly = true;
           this.textProcessedMessages.Size = new System.Drawing.Size(132, 21);
           this.textProcessedMessages.TabIndex = 10;
           // 
           // labelIMAP
           // 
           this.labelIMAP.AutoSize = true;
           this.labelIMAP.Location = new System.Drawing.Point(261, 184);
           this.labelIMAP.Name = "labelIMAP";
           this.labelIMAP.Size = new System.Drawing.Size(32, 13);
           this.labelIMAP.TabIndex = 9;
           this.labelIMAP.Text = "IMAP";
           // 
           // labelPOP3
           // 
           this.labelPOP3.AutoSize = true;
           this.labelPOP3.Location = new System.Drawing.Point(259, 139);
           this.labelPOP3.Name = "labelPOP3";
           this.labelPOP3.Size = new System.Drawing.Size(33, 13);
           this.labelPOP3.TabIndex = 8;
           this.labelPOP3.Text = "POP3";
           // 
           // labelSMTP
           // 
           this.labelSMTP.AutoSize = true;
           this.labelSMTP.Location = new System.Drawing.Point(259, 91);
           this.labelSMTP.Name = "labelSMTP";
           this.labelSMTP.Size = new System.Drawing.Size(33, 13);
           this.labelSMTP.TabIndex = 7;
           this.labelSMTP.Text = "SMTP";
           // 
           // labelMessagesContainingSpam
           // 
           this.labelMessagesContainingSpam.AutoSize = true;
           this.labelMessagesContainingSpam.Location = new System.Drawing.Point(34, 184);
           this.labelMessagesContainingSpam.Name = "labelMessagesContainingSpam";
           this.labelMessagesContainingSpam.Size = new System.Drawing.Size(83, 13);
           this.labelMessagesContainingSpam.TabIndex = 6;
           this.labelMessagesContainingSpam.Text = "Spam messages";
           // 
           // labelMessagesContainingVirus
           // 
           this.labelMessagesContainingVirus.AutoSize = true;
           this.labelMessagesContainingVirus.Location = new System.Drawing.Point(34, 139);
           this.labelMessagesContainingVirus.Name = "labelMessagesContainingVirus";
           this.labelMessagesContainingVirus.Size = new System.Drawing.Size(87, 13);
           this.labelMessagesContainingVirus.TabIndex = 5;
           this.labelMessagesContainingVirus.Text = "Viruses detected";
           // 
           // labelProcessedMessages
           // 
           this.labelProcessedMessages.AutoSize = true;
           this.labelProcessedMessages.Location = new System.Drawing.Point(34, 91);
           this.labelProcessedMessages.Name = "labelProcessedMessages";
           this.labelProcessedMessages.Size = new System.Drawing.Size(106, 13);
           this.labelProcessedMessages.TabIndex = 4;
           this.labelProcessedMessages.Text = "Processed messages";
           // 
           // labelCurrentSessions
           // 
           this.labelCurrentSessions.AutoSize = true;
           this.labelCurrentSessions.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelCurrentSessions.Location = new System.Drawing.Point(229, 60);
           this.labelCurrentSessions.Name = "labelCurrentSessions";
           this.labelCurrentSessions.Size = new System.Drawing.Size(147, 13);
           this.labelCurrentSessions.TabIndex = 3;
           this.labelCurrentSessions.Text = "Number of open sessions";
           // 
           // labelNumberOf
           // 
           this.labelNumberOf.AutoSize = true;
           this.labelNumberOf.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelNumberOf.Location = new System.Drawing.Point(10, 60);
           this.labelNumberOf.Name = "labelNumberOf";
           this.labelNumberOf.Size = new System.Drawing.Size(65, 13);
           this.labelNumberOf.TabIndex = 2;
           this.labelNumberOf.Text = "Number of";
           // 
           // labelStartTime
           // 
           this.labelStartTime.AutoSize = true;
           this.labelStartTime.Location = new System.Drawing.Point(10, 34);
           this.labelStartTime.Name = "labelStartTime";
           this.labelStartTime.Size = new System.Drawing.Size(15, 13);
           this.labelStartTime.TabIndex = 1;
           this.labelStartTime.Text = "--";
           // 
           // labelStartTimeTitle
           // 
           this.labelStartTimeTitle.AutoSize = true;
           this.labelStartTimeTitle.Location = new System.Drawing.Point(10, 12);
           this.labelStartTimeTitle.Name = "labelStartTimeTitle";
           this.labelStartTimeTitle.Size = new System.Drawing.Size(81, 13);
           this.labelStartTimeTitle.TabIndex = 0;
           this.labelStartTimeTitle.Text = "Server up since";
           // 
           // tabLogging
           // 
           this.tabLogging.Controls.Add(this.btnStartLiveLog);
           this.tabLogging.Controls.Add(this.listLiveLog);
           this.tabLogging.Controls.Add(this.btnClear);
           this.tabLogging.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.tabLogging.Location = new System.Drawing.Point(4, 22);
           this.tabLogging.Name = "tabLogging";
           this.tabLogging.Size = new System.Drawing.Size(725, 378);
           this.tabLogging.TabIndex = 2;
           this.tabLogging.Text = "Logging";
           this.tabLogging.UseVisualStyleBackColor = true;
           // 
           // btnStartLiveLog
           // 
           this.btnStartLiveLog.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.btnStartLiveLog.Location = new System.Drawing.Point(16, 8);
           this.btnStartLiveLog.Name = "btnStartLiveLog";
           this.btnStartLiveLog.Size = new System.Drawing.Size(100, 25);
           this.btnStartLiveLog.TabIndex = 4;
           this.btnStartLiveLog.Text = "Start";
           this.btnStartLiveLog.UseVisualStyleBackColor = true;
           this.btnStartLiveLog.Click += new System.EventHandler(this.btnStartLiveLog_Click);
           // 
           // listLiveLog
           // 
           this.listLiveLog.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listLiveLog.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnType,
            this.columnSession,
            this.columnThread,
            this.columnTime,
            this.columnIP,
            this.columnText});
           this.listLiveLog.ContextMenuStrip = this.contextMenuStripLiveLog;
           this.listLiveLog.FullRowSelect = true;
           this.listLiveLog.Location = new System.Drawing.Point(16, 40);
           this.listLiveLog.Name = "listLiveLog";
           this.listLiveLog.Size = new System.Drawing.Size(699, 324);
           this.listLiveLog.TabIndex = 6;
           this.listLiveLog.UseCompatibleStateImageBehavior = false;
           this.listLiveLog.View = System.Windows.Forms.View.Details;
           // 
           // columnType
           // 
           this.columnType.Text = "Type";
           this.columnType.Width = 64;
           // 
           // columnSession
           // 
           this.columnSession.Text = "Session";
           // 
           // columnThread
           // 
           this.columnThread.Text = "Thread";
           // 
           // columnTime
           // 
           this.columnTime.Text = "Time";
           this.columnTime.Width = 100;
           // 
           // columnIP
           // 
           this.columnIP.Text = "IP";
           // 
           // columnText
           // 
           this.columnText.Text = "Text";
           this.columnText.Width = 250;
           // 
           // contextMenuStripLiveLog
           // 
           this.contextMenuStripLiveLog.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuItemOnlyIncludeThisSessionToolStripMenuItem});
           this.contextMenuStripLiveLog.Name = "contextMenuStripLiveLog";
           this.contextMenuStripLiveLog.Size = new System.Drawing.Size(205, 26);
           // 
           // menuItemOnlyIncludeThisSessionToolStripMenuItem
           // 
           this.menuItemOnlyIncludeThisSessionToolStripMenuItem.Name = "menuItemOnlyIncludeThisSessionToolStripMenuItem";
           this.menuItemOnlyIncludeThisSessionToolStripMenuItem.Size = new System.Drawing.Size(204, 22);
           this.menuItemOnlyIncludeThisSessionToolStripMenuItem.Text = "Only include this session";
           this.menuItemOnlyIncludeThisSessionToolStripMenuItem.Click += new System.EventHandler(this.menuItemOnlyIncludeThisSessionToolStripMenuItem_Click);
           // 
           // btnClear
           // 
           this.btnClear.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.btnClear.Location = new System.Drawing.Point(128, 8);
           this.btnClear.Name = "btnClear";
           this.btnClear.Size = new System.Drawing.Size(100, 25);
           this.btnClear.TabIndex = 5;
           this.btnClear.Text = "Clear";
           this.btnClear.UseVisualStyleBackColor = true;
           this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
           // 
           // tabDeliveryQueue
           // 
           this.tabDeliveryQueue.Controls.Add(this.labelNumberOfMessages);
           this.tabDeliveryQueue.Controls.Add(this.buttonRefreshDeliveryQueue);
           this.tabDeliveryQueue.Controls.Add(this.listDeliveryQueue);
           this.tabDeliveryQueue.Controls.Add(this.btnClearQueue);
           this.tabDeliveryQueue.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.tabDeliveryQueue.Location = new System.Drawing.Point(4, 22);
           this.tabDeliveryQueue.Name = "tabDeliveryQueue";
           this.tabDeliveryQueue.Size = new System.Drawing.Size(725, 378);
           this.tabDeliveryQueue.TabIndex = 3;
           this.tabDeliveryQueue.Text = "Delivery queue";
           this.tabDeliveryQueue.UseVisualStyleBackColor = true;
           // 
           // labelNumberOfMessages
           // 
           this.labelNumberOfMessages.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
           this.labelNumberOfMessages.AutoSize = true;
           this.labelNumberOfMessages.Location = new System.Drawing.Point(16, 360);
           this.labelNumberOfMessages.Name = "labelNumberOfMessages";
           this.labelNumberOfMessages.Size = new System.Drawing.Size(0, 13);
           this.labelNumberOfMessages.TabIndex = 10;
           // 
           // buttonRefreshDeliveryQueue
           // 
           this.buttonRefreshDeliveryQueue.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.buttonRefreshDeliveryQueue.Location = new System.Drawing.Point(16, 8);
           this.buttonRefreshDeliveryQueue.Name = "buttonRefreshDeliveryQueue";
           this.buttonRefreshDeliveryQueue.Size = new System.Drawing.Size(100, 25);
           this.buttonRefreshDeliveryQueue.TabIndex = 7;
           this.buttonRefreshDeliveryQueue.Text = "Refresh";
           this.buttonRefreshDeliveryQueue.UseVisualStyleBackColor = true;
           this.buttonRefreshDeliveryQueue.Click += new System.EventHandler(this.buttonRefreshDeliveryQueue_Click);
           // 
           // listDeliveryQueue
           // 
           this.listDeliveryQueue.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listDeliveryQueue.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnID,
            this.columnCreated,
            this.columnFrom,
            this.columnTo,
            this.columnNextTry,
            this.columnNoOfTries,
            this.columnFilename});
           this.listDeliveryQueue.ContextMenuStrip = this.contextMenuDeliveryQueue;
           this.listDeliveryQueue.FullRowSelect = true;
           this.listDeliveryQueue.Location = new System.Drawing.Point(16, 40);
           this.listDeliveryQueue.Name = "listDeliveryQueue";
           this.listDeliveryQueue.Size = new System.Drawing.Size(706, 312);
           this.listDeliveryQueue.TabIndex = 9;
           this.listDeliveryQueue.UseCompatibleStateImageBehavior = false;
           this.listDeliveryQueue.View = System.Windows.Forms.View.Details;
           this.listDeliveryQueue.SelectedIndexChanged += new System.EventHandler(this.listDeliveryQueue_SelectedIndexChanged);
           this.listDeliveryQueue.DoubleClick += new System.EventHandler(this.listDeliveryQueue_DoubleClick);
           // 
           // columnID
           // 
           this.columnID.Tag = "Numeric";
           this.columnID.Text = "ID";
           this.columnID.Width = 40;
           // 
           // columnCreated
           // 
           this.columnCreated.Text = "Created";
           this.columnCreated.Width = 100;
           // 
           // columnFrom
           // 
           this.columnFrom.Text = "From";
           this.columnFrom.Width = 100;
           // 
           // columnTo
           // 
           this.columnTo.Text = "To";
           this.columnTo.Width = 100;
           // 
           // columnNextTry
           // 
           this.columnNextTry.Text = "Next try";
           this.columnNextTry.Width = 140;
           // 
           // columnNoOfTries
           // 
           this.columnNoOfTries.Tag = "Numeric";
           this.columnNoOfTries.Text = "Number of retries";
           this.columnNoOfTries.Width = 70;
           // 
           // columnFilename
           // 
           this.columnFilename.Text = "File name";
           this.columnFilename.Width = 150;
           // 
           // contextMenuDeliveryQueue
           // 
           this.contextMenuDeliveryQueue.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuItemShow,
            this.menuItemSendNow,
            this.menuItemDelete});
           this.contextMenuDeliveryQueue.Name = "contextMenuDeliveryQueue";
           this.contextMenuDeliveryQueue.Size = new System.Drawing.Size(127, 70);
           // 
           // menuItemShow
           // 
           this.menuItemShow.Name = "menuItemShow";
           this.menuItemShow.Size = new System.Drawing.Size(126, 22);
           this.menuItemShow.Text = "Show";
           this.menuItemShow.Click += new System.EventHandler(this.menuItemShow_Click);
           // 
           // menuItemSendNow
           // 
           this.menuItemSendNow.Name = "menuItemSendNow";
           this.menuItemSendNow.Size = new System.Drawing.Size(126, 22);
           this.menuItemSendNow.Text = "Send now";
           this.menuItemSendNow.Click += new System.EventHandler(this.menuItemSendNow_Click);
           // 
           // menuItemDelete
           // 
           this.menuItemDelete.Name = "menuItemDelete";
           this.menuItemDelete.Size = new System.Drawing.Size(126, 22);
           this.menuItemDelete.Text = "Remove";
           this.menuItemDelete.Click += new System.EventHandler(this.menuItemDelete_Click);
           // 
           // btnClearQueue
           // 
           this.btnClearQueue.Font = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.btnClearQueue.Location = new System.Drawing.Point(128, 8);
           this.btnClearQueue.Name = "btnClearQueue";
           this.btnClearQueue.Size = new System.Drawing.Size(100, 25);
           this.btnClearQueue.TabIndex = 8;
           this.btnClearQueue.Text = "Clear queue";
           this.btnClearQueue.UseVisualStyleBackColor = true;
           this.btnClearQueue.Click += new System.EventHandler(this.btnClearQueue_Click);
           // 
           // timerLiveLog
           // 
           this.timerLiveLog.Interval = 1000;
           this.timerLiveLog.Tick += new System.EventHandler(this.timerLiveLog_Tick);
           // 
           // timerServerStats
           // 
           this.timerServerStats.Enabled = true;
           this.timerServerStats.Interval = 2000;
           this.timerServerStats.Tick += new System.EventHandler(this.timerServerStats_Tick);
           // 
           // ucStatus
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.tabControl);
           this.Name = "ucStatus";
           this.Size = new System.Drawing.Size(733, 404);
           this.tabControl.ResumeLayout(false);
           this.tabServer.ResumeLayout(false);
           this.tabServer.PerformLayout();
           this.tabStatus.ResumeLayout(false);
           this.tabStatus.PerformLayout();
           this.tabLogging.ResumeLayout(false);
           this.contextMenuStripLiveLog.ResumeLayout(false);
           this.tabDeliveryQueue.ResumeLayout(false);
           this.tabDeliveryQueue.PerformLayout();
           this.contextMenuDeliveryQueue.ResumeLayout(false);
           this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage tabServer;
        private System.Windows.Forms.Label labelCurrentStatusTitle;
        private System.Windows.Forms.TabPage tabStatus;
        private System.Windows.Forms.TabPage tabLogging;
        private System.Windows.Forms.TabPage tabDeliveryQueue;
        private System.Windows.Forms.Label labelCurrentStatus;
        private System.Windows.Forms.Label labelAction;
       private System.Windows.Forms.Button buttonStartStop;
        private System.Windows.Forms.Label labelWarnings;
        private System.Windows.Forms.Label labelVersion;
        private System.Windows.Forms.Label labelVersionTitle;
        private System.Windows.Forms.Label labelServerInformation;
        private hMailServer.Shared.ucText textIMAPSessions;
        private hMailServer.Shared.ucText textPOP3Sessions;
        private hMailServer.Shared.ucText textSMTPSessions;
        private hMailServer.Shared.ucText textMessagesContainingSpam;
        private hMailServer.Shared.ucText textMessagesContainingViruses;
        private hMailServer.Shared.ucText textProcessedMessages;
        private System.Windows.Forms.Label labelIMAP;
        private System.Windows.Forms.Label labelPOP3;
        private System.Windows.Forms.Label labelSMTP;
        private System.Windows.Forms.Label labelMessagesContainingSpam;
        private System.Windows.Forms.Label labelMessagesContainingVirus;
        private System.Windows.Forms.Label labelProcessedMessages;
        private System.Windows.Forms.Label labelCurrentSessions;
        private System.Windows.Forms.Label labelNumberOf;
        private System.Windows.Forms.Label labelStartTime;
        private System.Windows.Forms.Label labelStartTimeTitle;
        private hMailServer.Administrator.ucListView listLiveLog;
        private System.Windows.Forms.ColumnHeader columnType;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.Button btnStartLiveLog;
        private System.Windows.Forms.ColumnHeader columnSession;
        private System.Windows.Forms.ColumnHeader columnThread;
        private System.Windows.Forms.ColumnHeader columnTime;
        private System.Windows.Forms.ColumnHeader columnIP;
        private System.Windows.Forms.ColumnHeader columnText;
        private System.Windows.Forms.Timer timerLiveLog;
        private System.Windows.Forms.Button buttonRefreshDeliveryQueue;
        private hMailServer.Administrator.ucListView listDeliveryQueue;
        private System.Windows.Forms.ColumnHeader columnID;
        private System.Windows.Forms.ColumnHeader columnCreated;
        private System.Windows.Forms.ColumnHeader columnFrom;
        private System.Windows.Forms.ColumnHeader columnTo;
        private System.Windows.Forms.ColumnHeader columnNextTry;
        private System.Windows.Forms.ColumnHeader columnNoOfTries;
        private System.Windows.Forms.Button btnClearQueue;
        private System.Windows.Forms.ColumnHeader columnFilename;
        private System.Windows.Forms.ContextMenuStrip contextMenuDeliveryQueue;
        private System.Windows.Forms.ToolStripMenuItem menuItemSendNow;
        private System.Windows.Forms.ToolStripMenuItem menuItemDelete;
       private ucListView listWarnings;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.ColumnHeader columnHeader2;
       private System.Windows.Forms.ColumnHeader columnHeader3;
       private System.Windows.Forms.Button buttonShowWarning;
       private System.Windows.Forms.Label labelNumberOfMessages;
       private System.Windows.Forms.ContextMenuStrip contextMenuStripLiveLog;
       private System.Windows.Forms.ToolStripMenuItem menuItemOnlyIncludeThisSessionToolStripMenuItem;
       private System.Windows.Forms.Label labelServerType;
       private System.Windows.Forms.Label labelServerTypeTitle;
       private System.Windows.Forms.Label labelName;
       private System.Windows.Forms.Label labelNameTitle;
       private System.Windows.Forms.Label labelServerHost;
       private System.Windows.Forms.Label labelServerHostTitle;
       private System.Windows.Forms.Label labelDatabaseInformation;
       private System.Windows.Forms.ToolStripMenuItem menuItemShow;
       private System.Windows.Forms.Timer timerServerStats;
       private System.Windows.Forms.Label labelDBVersion;
       private System.Windows.Forms.Label labelDatabaseVersion;
       private System.Windows.Forms.Label label1;
    }
}
