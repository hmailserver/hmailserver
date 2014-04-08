namespace hMailServer.Administrator
{
    partial class ucPerformance
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
           this.tabPageCache = new System.Windows.Forms.TabPage();
           this.labelDistributionListHitRate = new System.Windows.Forms.Label();
           this.labelDistributionList = new System.Windows.Forms.Label();
           this.labelAliasHitRate = new System.Windows.Forms.Label();
           this.labelAlias = new System.Windows.Forms.Label();
           this.labelAccountHitRate = new System.Windows.Forms.Label();
           this.labelDomainHitRate = new System.Windows.Forms.Label();
           this.labelAccount = new System.Windows.Forms.Label();
           this.labelDomain = new System.Windows.Forms.Label();
           this.labelHitRate = new System.Windows.Forms.Label();
           this.labelTimeToLive = new System.Windows.Forms.Label();
           this.labelType = new System.Windows.Forms.Label();
           this.tabPageIndexing = new System.Windows.Forms.TabPage();
           this.labelWorkerThreadPriority = new System.Windows.Forms.Label();
           this.labelDeliveryThreads = new System.Windows.Forms.Label();
           this.labelTCPIPThreads = new System.Windows.Forms.Label();
           this.labelMaxAsynchTasks = new System.Windows.Forms.Label();
           this.tabMessageIndexing = new System.Windows.Forms.TabPage();
           this.buttonMessageIndexingClear = new System.Windows.Forms.Button();
           this.buttonMessageIndexingStatusRefresh = new System.Windows.Forms.Button();
           this.messageIndexingStatusText = new System.Windows.Forms.Label();
           this.messageIndexingProgressBar = new System.Windows.Forms.ProgressBar();
           this.labelStatus = new System.Windows.Forms.Label();
           this.timerRefreshHitRate = new System.Windows.Forms.Timer(this.components);
           this.textCacheDistributionListTTL = new hMailServer.Shared.ucText();
           this.textCacheAliasTTL = new hMailServer.Shared.ucText();
           this.textCacheAccountTTL = new hMailServer.Shared.ucText();
           this.textCacheDomainTTL = new hMailServer.Shared.ucText();
           this.checkEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.comboWorkerThreadPriority = new hMailServer.Administrator.Controls.ucComboBox();
           this.textDeliveryThreads = new hMailServer.Shared.ucText();
           this.textTCPIPThreads = new hMailServer.Shared.ucText();
           this.textMaxAsynchronousThreads = new hMailServer.Shared.ucText();
           this.checkMessageIndexingEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
           this.tabControl.SuspendLayout();
           this.tabPageCache.SuspendLayout();
           this.tabPageIndexing.SuspendLayout();
           this.tabMessageIndexing.SuspendLayout();
           this.SuspendLayout();
           // 
           // tabControl
           // 
           this.tabControl.Controls.Add(this.tabPageCache);
           this.tabControl.Controls.Add(this.tabPageIndexing);
           this.tabControl.Controls.Add(this.tabMessageIndexing);
           this.tabControl.Dock = System.Windows.Forms.DockStyle.Fill;
           this.tabControl.Location = new System.Drawing.Point(0, 0);
           this.tabControl.Name = "tabControl";
           this.tabControl.SelectedIndex = 0;
           this.tabControl.Size = new System.Drawing.Size(722, 464);
           this.tabControl.TabIndex = 0;
           this.tabControl.SelectedIndexChanged += new System.EventHandler(this.tabControl_SelectedIndexChanged);
           // 
           // tabPageCache
           // 
           this.tabPageCache.Controls.Add(this.labelDistributionListHitRate);
           this.tabPageCache.Controls.Add(this.textCacheDistributionListTTL);
           this.tabPageCache.Controls.Add(this.labelDistributionList);
           this.tabPageCache.Controls.Add(this.labelAliasHitRate);
           this.tabPageCache.Controls.Add(this.textCacheAliasTTL);
           this.tabPageCache.Controls.Add(this.labelAlias);
           this.tabPageCache.Controls.Add(this.labelAccountHitRate);
           this.tabPageCache.Controls.Add(this.textCacheAccountTTL);
           this.tabPageCache.Controls.Add(this.labelDomainHitRate);
           this.tabPageCache.Controls.Add(this.labelAccount);
           this.tabPageCache.Controls.Add(this.labelDomain);
           this.tabPageCache.Controls.Add(this.labelHitRate);
           this.tabPageCache.Controls.Add(this.labelTimeToLive);
           this.tabPageCache.Controls.Add(this.textCacheDomainTTL);
           this.tabPageCache.Controls.Add(this.labelType);
           this.tabPageCache.Controls.Add(this.checkEnabled);
           this.tabPageCache.Location = new System.Drawing.Point(4, 22);
           this.tabPageCache.Name = "tabPageCache";
           this.tabPageCache.Padding = new System.Windows.Forms.Padding(3);
           this.tabPageCache.Size = new System.Drawing.Size(714, 438);
           this.tabPageCache.TabIndex = 0;
           this.tabPageCache.Text = "Cache";
           this.tabPageCache.UseVisualStyleBackColor = true;
           // 
           // labelDistributionListHitRate
           // 
           this.labelDistributionListHitRate.AutoSize = true;
           this.labelDistributionListHitRate.Location = new System.Drawing.Point(309, 166);
           this.labelDistributionListHitRate.Name = "labelDistributionListHitRate";
           this.labelDistributionListHitRate.Size = new System.Drawing.Size(10, 13);
           this.labelDistributionListHitRate.TabIndex = 29;
           this.labelDistributionListHitRate.Text = "-";
           // 
           // labelDistributionList
           // 
           this.labelDistributionList.AutoSize = true;
           this.labelDistributionList.Location = new System.Drawing.Point(38, 166);
           this.labelDistributionList.Name = "labelDistributionList";
           this.labelDistributionList.Size = new System.Drawing.Size(74, 13);
           this.labelDistributionList.TabIndex = 27;
           this.labelDistributionList.Text = "Distribution list";
           // 
           // labelAliasHitRate
           // 
           this.labelAliasHitRate.AutoSize = true;
           this.labelAliasHitRate.Location = new System.Drawing.Point(309, 140);
           this.labelAliasHitRate.Name = "labelAliasHitRate";
           this.labelAliasHitRate.Size = new System.Drawing.Size(10, 13);
           this.labelAliasHitRate.TabIndex = 26;
           this.labelAliasHitRate.Text = "-";
           // 
           // labelAlias
           // 
           this.labelAlias.AutoSize = true;
           this.labelAlias.Location = new System.Drawing.Point(38, 140);
           this.labelAlias.Name = "labelAlias";
           this.labelAlias.Size = new System.Drawing.Size(29, 13);
           this.labelAlias.TabIndex = 24;
           this.labelAlias.Text = "Alias";
           // 
           // labelAccountHitRate
           // 
           this.labelAccountHitRate.AutoSize = true;
           this.labelAccountHitRate.Location = new System.Drawing.Point(309, 114);
           this.labelAccountHitRate.Name = "labelAccountHitRate";
           this.labelAccountHitRate.Size = new System.Drawing.Size(10, 13);
           this.labelAccountHitRate.TabIndex = 23;
           this.labelAccountHitRate.Text = "-";
           // 
           // labelDomainHitRate
           // 
           this.labelDomainHitRate.AutoSize = true;
           this.labelDomainHitRate.Location = new System.Drawing.Point(309, 87);
           this.labelDomainHitRate.Name = "labelDomainHitRate";
           this.labelDomainHitRate.Size = new System.Drawing.Size(10, 13);
           this.labelDomainHitRate.TabIndex = 21;
           this.labelDomainHitRate.Text = "-";
           // 
           // labelAccount
           // 
           this.labelAccount.AutoSize = true;
           this.labelAccount.Location = new System.Drawing.Point(38, 114);
           this.labelAccount.Name = "labelAccount";
           this.labelAccount.Size = new System.Drawing.Size(47, 13);
           this.labelAccount.TabIndex = 20;
           this.labelAccount.Text = "Account";
           // 
           // labelDomain
           // 
           this.labelDomain.AutoSize = true;
           this.labelDomain.Location = new System.Drawing.Point(38, 87);
           this.labelDomain.Name = "labelDomain";
           this.labelDomain.Size = new System.Drawing.Size(43, 13);
           this.labelDomain.TabIndex = 19;
           this.labelDomain.Text = "Domain";
           // 
           // labelHitRate
           // 
           this.labelHitRate.AutoSize = true;
           this.labelHitRate.Location = new System.Drawing.Point(309, 55);
           this.labelHitRate.Name = "labelHitRate";
           this.labelHitRate.Size = new System.Drawing.Size(41, 13);
           this.labelHitRate.TabIndex = 18;
           this.labelHitRate.Text = "Hit rate";
           // 
           // labelTimeToLive
           // 
           this.labelTimeToLive.AutoSize = true;
           this.labelTimeToLive.Location = new System.Drawing.Point(155, 55);
           this.labelTimeToLive.Name = "labelTimeToLive";
           this.labelTimeToLive.Size = new System.Drawing.Size(110, 13);
           this.labelTimeToLive.TabIndex = 17;
           this.labelTimeToLive.Text = "Time to live (seconds)";
           // 
           // labelType
           // 
           this.labelType.AutoSize = true;
           this.labelType.Location = new System.Drawing.Point(38, 55);
           this.labelType.Name = "labelType";
           this.labelType.Size = new System.Drawing.Size(31, 13);
           this.labelType.TabIndex = 15;
           this.labelType.Text = "Type";
           // 
           // tabPageIndexing
           // 
           this.tabPageIndexing.Controls.Add(this.comboWorkerThreadPriority);
           this.tabPageIndexing.Controls.Add(this.labelWorkerThreadPriority);
           this.tabPageIndexing.Controls.Add(this.labelDeliveryThreads);
           this.tabPageIndexing.Controls.Add(this.labelTCPIPThreads);
           this.tabPageIndexing.Controls.Add(this.labelMaxAsynchTasks);
           this.tabPageIndexing.Controls.Add(this.textDeliveryThreads);
           this.tabPageIndexing.Controls.Add(this.textTCPIPThreads);
           this.tabPageIndexing.Controls.Add(this.textMaxAsynchronousThreads);
           this.tabPageIndexing.Location = new System.Drawing.Point(4, 22);
           this.tabPageIndexing.Name = "tabPageIndexing";
           this.tabPageIndexing.Padding = new System.Windows.Forms.Padding(3);
           this.tabPageIndexing.Size = new System.Drawing.Size(714, 438);
           this.tabPageIndexing.TabIndex = 1;
           this.tabPageIndexing.Text = "Threading";
           this.tabPageIndexing.UseVisualStyleBackColor = true;
           // 
           // labelWorkerThreadPriority
           // 
           this.labelWorkerThreadPriority.AutoSize = true;
           this.labelWorkerThreadPriority.Location = new System.Drawing.Point(18, 148);
           this.labelWorkerThreadPriority.Name = "labelWorkerThreadPriority";
           this.labelWorkerThreadPriority.Size = new System.Drawing.Size(108, 13);
           this.labelWorkerThreadPriority.TabIndex = 24;
           this.labelWorkerThreadPriority.Text = "Worker thread priority";
           // 
           // labelDeliveryThreads
           // 
           this.labelDeliveryThreads.AutoSize = true;
           this.labelDeliveryThreads.Location = new System.Drawing.Point(18, 61);
           this.labelDeliveryThreads.Name = "labelDeliveryThreads";
           this.labelDeliveryThreads.Size = new System.Drawing.Size(83, 13);
           this.labelDeliveryThreads.TabIndex = 23;
           this.labelDeliveryThreads.Text = "Delivery threads";
           // 
           // labelTCPIPThreads
           // 
           this.labelTCPIPThreads.AutoSize = true;
           this.labelTCPIPThreads.Location = new System.Drawing.Point(18, 13);
           this.labelTCPIPThreads.Name = "labelTCPIPThreads";
           this.labelTCPIPThreads.Size = new System.Drawing.Size(164, 13);
           this.labelTCPIPThreads.TabIndex = 21;
           this.labelTCPIPThreads.Text = "Max number of command threads";
           // 
           // labelMaxAsynchTasks
           // 
           this.labelMaxAsynchTasks.AutoSize = true;
           this.labelMaxAsynchTasks.Location = new System.Drawing.Point(18, 105);
           this.labelMaxAsynchTasks.Name = "labelMaxAsynchTasks";
           this.labelMaxAsynchTasks.Size = new System.Drawing.Size(207, 13);
           this.labelMaxAsynchTasks.TabIndex = 27;
           this.labelMaxAsynchTasks.Text = "Max number of asynchronous task threads";
           // 
           // tabMessageIndexing
           // 
           this.tabMessageIndexing.Controls.Add(this.buttonMessageIndexingClear);
           this.tabMessageIndexing.Controls.Add(this.buttonMessageIndexingStatusRefresh);
           this.tabMessageIndexing.Controls.Add(this.messageIndexingStatusText);
           this.tabMessageIndexing.Controls.Add(this.messageIndexingProgressBar);
           this.tabMessageIndexing.Controls.Add(this.labelStatus);
           this.tabMessageIndexing.Controls.Add(this.checkMessageIndexingEnabled);
           this.tabMessageIndexing.Location = new System.Drawing.Point(4, 22);
           this.tabMessageIndexing.Name = "tabMessageIndexing";
           this.tabMessageIndexing.Size = new System.Drawing.Size(714, 438);
           this.tabMessageIndexing.TabIndex = 2;
           this.tabMessageIndexing.Text = "Message indexing";
           this.tabMessageIndexing.UseVisualStyleBackColor = true;
           // 
           // buttonMessageIndexingClear
           // 
           this.buttonMessageIndexingClear.Location = new System.Drawing.Point(20, 122);
           this.buttonMessageIndexingClear.Name = "buttonMessageIndexingClear";
           this.buttonMessageIndexingClear.Size = new System.Drawing.Size(84, 23);
           this.buttonMessageIndexingClear.TabIndex = 20;
           this.buttonMessageIndexingClear.Text = "Clear";
           this.buttonMessageIndexingClear.UseVisualStyleBackColor = true;
           this.buttonMessageIndexingClear.Click += new System.EventHandler(this.buttonMessageIndexingClear_Click);
           // 
           // buttonMessageIndexingStatusRefresh
           // 
           this.buttonMessageIndexingStatusRefresh.Location = new System.Drawing.Point(366, 60);
           this.buttonMessageIndexingStatusRefresh.Name = "buttonMessageIndexingStatusRefresh";
           this.buttonMessageIndexingStatusRefresh.Size = new System.Drawing.Size(84, 23);
           this.buttonMessageIndexingStatusRefresh.TabIndex = 19;
           this.buttonMessageIndexingStatusRefresh.Text = "Refresh";
           this.buttonMessageIndexingStatusRefresh.UseVisualStyleBackColor = true;
           this.buttonMessageIndexingStatusRefresh.Click += new System.EventHandler(this.buttonMessageIndexingStatusRefresh_Click);
           // 
           // messageIndexingStatusText
           // 
           this.messageIndexingStatusText.AutoSize = true;
           this.messageIndexingStatusText.Location = new System.Drawing.Point(17, 88);
           this.messageIndexingStatusText.Name = "messageIndexingStatusText";
           this.messageIndexingStatusText.Size = new System.Drawing.Size(10, 13);
           this.messageIndexingStatusText.TabIndex = 18;
           this.messageIndexingStatusText.Text = "-";
           // 
           // messageIndexingProgressBar
           // 
           this.messageIndexingProgressBar.Location = new System.Drawing.Point(18, 60);
           this.messageIndexingProgressBar.Name = "messageIndexingProgressBar";
           this.messageIndexingProgressBar.Size = new System.Drawing.Size(342, 23);
           this.messageIndexingProgressBar.TabIndex = 17;
           // 
           // labelStatus
           // 
           this.labelStatus.AutoSize = true;
           this.labelStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
           this.labelStatus.Location = new System.Drawing.Point(15, 42);
           this.labelStatus.Name = "labelStatus";
           this.labelStatus.Size = new System.Drawing.Size(43, 13);
           this.labelStatus.TabIndex = 16;
           this.labelStatus.Text = "Status";
           // 
           // timerRefreshHitRate
           // 
           this.timerRefreshHitRate.Enabled = true;
           this.timerRefreshHitRate.Interval = 5000;
           this.timerRefreshHitRate.Tick += new System.EventHandler(this.timerRefreshHitRate_Tick);
           // 
           // textCacheDistributionListTTL
           // 
           this.textCacheDistributionListTTL.Location = new System.Drawing.Point(158, 163);
           this.textCacheDistributionListTTL.Name = "textCacheDistributionListTTL";
           this.textCacheDistributionListTTL.Number = 0;
           this.textCacheDistributionListTTL.Numeric = true;
           this.textCacheDistributionListTTL.Size = new System.Drawing.Size(81, 20);
           this.textCacheDistributionListTTL.TabIndex = 28;
           // 
           // textCacheAliasTTL
           // 
           this.textCacheAliasTTL.Location = new System.Drawing.Point(158, 137);
           this.textCacheAliasTTL.Name = "textCacheAliasTTL";
           this.textCacheAliasTTL.Number = 0;
           this.textCacheAliasTTL.Numeric = true;
           this.textCacheAliasTTL.Size = new System.Drawing.Size(81, 20);
           this.textCacheAliasTTL.TabIndex = 25;
           // 
           // textCacheAccountTTL
           // 
           this.textCacheAccountTTL.Location = new System.Drawing.Point(158, 111);
           this.textCacheAccountTTL.Name = "textCacheAccountTTL";
           this.textCacheAccountTTL.Number = 0;
           this.textCacheAccountTTL.Numeric = true;
           this.textCacheAccountTTL.Size = new System.Drawing.Size(81, 20);
           this.textCacheAccountTTL.TabIndex = 22;
           // 
           // textCacheDomainTTL
           // 
           this.textCacheDomainTTL.Location = new System.Drawing.Point(158, 84);
           this.textCacheDomainTTL.Name = "textCacheDomainTTL";
           this.textCacheDomainTTL.Number = 0;
           this.textCacheDomainTTL.Numeric = true;
           this.textCacheDomainTTL.Size = new System.Drawing.Size(81, 20);
           this.textCacheDomainTTL.TabIndex = 16;
           // 
           // checkEnabled
           // 
           this.checkEnabled.AutoSize = true;
           this.checkEnabled.Location = new System.Drawing.Point(18, 17);
           this.checkEnabled.Name = "checkEnabled";
           this.checkEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkEnabled.TabIndex = 14;
           this.checkEnabled.Text = "Enabled";
           this.checkEnabled.UseVisualStyleBackColor = true;
           this.checkEnabled.CheckedChanged += new System.EventHandler(this.checkEnabled_CheckedChanged);
           // 
           // comboWorkerThreadPriority
           // 
           this.comboWorkerThreadPriority.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
           this.comboWorkerThreadPriority.FormattingEnabled = true;
           this.comboWorkerThreadPriority.Location = new System.Drawing.Point(21, 164);
           this.comboWorkerThreadPriority.Name = "comboWorkerThreadPriority";
           this.comboWorkerThreadPriority.Size = new System.Drawing.Size(148, 21);
           this.comboWorkerThreadPriority.TabIndex = 25;
           // 
           // textDeliveryThreads
           // 
           this.textDeliveryThreads.Location = new System.Drawing.Point(21, 77);
           this.textDeliveryThreads.Name = "textDeliveryThreads";
           this.textDeliveryThreads.Number = 0;
           this.textDeliveryThreads.Numeric = true;
           this.textDeliveryThreads.Size = new System.Drawing.Size(119, 20);
           this.textDeliveryThreads.TabIndex = 22;
           // 
           // textTCPIPThreads
           // 
           this.textTCPIPThreads.Location = new System.Drawing.Point(21, 29);
           this.textTCPIPThreads.Name = "textTCPIPThreads";
           this.textTCPIPThreads.Number = 0;
           this.textTCPIPThreads.Numeric = true;
           this.textTCPIPThreads.Size = new System.Drawing.Size(119, 20);
           this.textTCPIPThreads.TabIndex = 20;
           // 
           // textMaxAsynchronousThreads
           // 
           this.textMaxAsynchronousThreads.Location = new System.Drawing.Point(21, 121);
           this.textMaxAsynchronousThreads.Name = "textMaxAsynchronousThreads";
           this.textMaxAsynchronousThreads.Number = 0;
           this.textMaxAsynchronousThreads.Numeric = true;
           this.textMaxAsynchronousThreads.Size = new System.Drawing.Size(119, 20);
           this.textMaxAsynchronousThreads.TabIndex = 26;
           // 
           // checkMessageIndexingEnabled
           // 
           this.checkMessageIndexingEnabled.AutoSize = true;
           this.checkMessageIndexingEnabled.Location = new System.Drawing.Point(18, 17);
           this.checkMessageIndexingEnabled.Name = "checkMessageIndexingEnabled";
           this.checkMessageIndexingEnabled.Size = new System.Drawing.Size(65, 17);
           this.checkMessageIndexingEnabled.TabIndex = 15;
           this.checkMessageIndexingEnabled.Text = "Enabled";
           this.checkMessageIndexingEnabled.UseVisualStyleBackColor = true;
           // 
           // ucPerformance
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.tabControl);
           this.Name = "ucPerformance";
           this.Size = new System.Drawing.Size(722, 464);
           this.tabControl.ResumeLayout(false);
           this.tabPageCache.ResumeLayout(false);
           this.tabPageCache.PerformLayout();
           this.tabPageIndexing.ResumeLayout(false);
           this.tabPageIndexing.PerformLayout();
           this.tabMessageIndexing.ResumeLayout(false);
           this.tabMessageIndexing.PerformLayout();
           this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.TabControl tabControl;
       private System.Windows.Forms.TabPage tabPageCache;
       private System.Windows.Forms.TabPage tabPageIndexing;
       private hMailServer.Administrator.Controls.ucCheckbox checkEnabled;
       private System.Windows.Forms.Label labelAccount;
       private System.Windows.Forms.Label labelDomain;
       private System.Windows.Forms.Label labelHitRate;
       private System.Windows.Forms.Label labelTimeToLive;
       private hMailServer.Shared.ucText textCacheDomainTTL;
       private System.Windows.Forms.Label labelType;
       private System.Windows.Forms.Label labelDomainHitRate;
       private System.Windows.Forms.Label labelDistributionListHitRate;
       private hMailServer.Shared.ucText textCacheDistributionListTTL;
       private System.Windows.Forms.Label labelDistributionList;
       private System.Windows.Forms.Label labelAliasHitRate;
       private hMailServer.Shared.ucText textCacheAliasTTL;
       private System.Windows.Forms.Label labelAlias;
       private System.Windows.Forms.Label labelAccountHitRate;
       private hMailServer.Shared.ucText textCacheAccountTTL;
       private System.Windows.Forms.Label labelDeliveryThreads;
       private hMailServer.Shared.ucText textDeliveryThreads;
       private System.Windows.Forms.Label labelTCPIPThreads;
       private hMailServer.Shared.ucText textTCPIPThreads;
       private System.Windows.Forms.Label labelWorkerThreadPriority;
       private hMailServer.Administrator.Controls.ucComboBox comboWorkerThreadPriority;
       private System.Windows.Forms.Timer timerRefreshHitRate;
       private System.Windows.Forms.TabPage tabMessageIndexing;
       private hMailServer.Administrator.Controls.ucCheckbox checkMessageIndexingEnabled;
       private System.Windows.Forms.Label labelStatus;
       private System.Windows.Forms.Label messageIndexingStatusText;
       private System.Windows.Forms.ProgressBar messageIndexingProgressBar;
       private System.Windows.Forms.Button buttonMessageIndexingStatusRefresh;
       private System.Windows.Forms.Button buttonMessageIndexingClear;
       private System.Windows.Forms.Label labelMaxAsynchTasks;
       private hMailServer.Shared.ucText textMaxAsynchronousThreads;
    }
}
