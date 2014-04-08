namespace hMailServer.Administrator.Dialogs
{
   partial class formRule
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
          System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(formRule));
          this.labelGeneral = new System.Windows.Forms.Label();
          this.labelName = new System.Windows.Forms.Label();
          this.labelCriteria = new System.Windows.Forms.Label();
          this.buttonAddCriteria = new System.Windows.Forms.Button();
          this.buttonDeleteCriteria = new System.Windows.Forms.Button();
          this.buttonEditCriteria = new System.Windows.Forms.Button();
          this.labelActions = new System.Windows.Forms.Label();
          this.buttonEditAction = new System.Windows.Forms.Button();
          this.buttonDeleteAction = new System.Windows.Forms.Button();
          this.buttonAddAction = new System.Windows.Forms.Button();
          this.buttonMoveUp = new System.Windows.Forms.Button();
          this.buttonMoveDown = new System.Windows.Forms.Button();
          this.groupBox1 = new System.Windows.Forms.GroupBox();
          this.btnCancel = new System.Windows.Forms.Button();
          this.btnOK = new System.Windows.Forms.Button();
          this.listActions = new hMailServer.Administrator.ucListView();
          this.columnHeader3 = new System.Windows.Forms.ColumnHeader();
          this.radioUseOR = new hMailServer.Administrator.Controls.ucRadioButton();
          this.radioUseAND = new hMailServer.Administrator.Controls.ucRadioButton();
          this.listCriterias = new hMailServer.Administrator.ucListView();
          this.columnHeader = new System.Windows.Forms.ColumnHeader();
          this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
          this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
          this.checkEnabled = new hMailServer.Administrator.Controls.ucCheckbox();
          this.textName = new hMailServer.Shared.ucText();
          this.SuspendLayout();
          // 
          // labelGeneral
          // 
          this.labelGeneral.AutoSize = true;
          this.labelGeneral.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelGeneral.Location = new System.Drawing.Point(12, 9);
          this.labelGeneral.Name = "labelGeneral";
          this.labelGeneral.Size = new System.Drawing.Size(51, 13);
          this.labelGeneral.TabIndex = 13;
          this.labelGeneral.Text = "General";
          // 
          // labelName
          // 
          this.labelName.AutoSize = true;
          this.labelName.Location = new System.Drawing.Point(26, 29);
          this.labelName.Name = "labelName";
          this.labelName.Size = new System.Drawing.Size(35, 13);
          this.labelName.TabIndex = 15;
          this.labelName.Text = "Name";
          // 
          // labelCriteria
          // 
          this.labelCriteria.AutoSize = true;
          this.labelCriteria.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelCriteria.Location = new System.Drawing.Point(12, 104);
          this.labelCriteria.Name = "labelCriteria";
          this.labelCriteria.Size = new System.Drawing.Size(47, 13);
          this.labelCriteria.TabIndex = 30;
          this.labelCriteria.Text = "Criteria";
          // 
          // buttonAddCriteria
          // 
          this.buttonAddCriteria.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.buttonAddCriteria.Location = new System.Drawing.Point(344, 149);
          this.buttonAddCriteria.Name = "buttonAddCriteria";
          this.buttonAddCriteria.Size = new System.Drawing.Size(89, 25);
          this.buttonAddCriteria.TabIndex = 33;
          this.buttonAddCriteria.Text = "&Add...";
          this.buttonAddCriteria.UseVisualStyleBackColor = true;
          this.buttonAddCriteria.Click += new System.EventHandler(this.buttonAddCriteria_Click);
          // 
          // buttonDeleteCriteria
          // 
          this.buttonDeleteCriteria.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.buttonDeleteCriteria.Location = new System.Drawing.Point(344, 211);
          this.buttonDeleteCriteria.Name = "buttonDeleteCriteria";
          this.buttonDeleteCriteria.Size = new System.Drawing.Size(89, 25);
          this.buttonDeleteCriteria.TabIndex = 34;
          this.buttonDeleteCriteria.Text = "Remove";
          this.buttonDeleteCriteria.UseVisualStyleBackColor = true;
          this.buttonDeleteCriteria.Click += new System.EventHandler(this.buttonDeleteCriteria_Click);
          // 
          // buttonEditCriteria
          // 
          this.buttonEditCriteria.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
          this.buttonEditCriteria.Location = new System.Drawing.Point(344, 180);
          this.buttonEditCriteria.Name = "buttonEditCriteria";
          this.buttonEditCriteria.Size = new System.Drawing.Size(89, 25);
          this.buttonEditCriteria.TabIndex = 35;
          this.buttonEditCriteria.Text = "&Edit...";
          this.buttonEditCriteria.UseVisualStyleBackColor = true;
          this.buttonEditCriteria.Click += new System.EventHandler(this.buttonEditCriteria_Click);
          // 
          // labelActions
          // 
          this.labelActions.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
          this.labelActions.AutoSize = true;
          this.labelActions.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.labelActions.Location = new System.Drawing.Point(14, 266);
          this.labelActions.Name = "labelActions";
          this.labelActions.Size = new System.Drawing.Size(49, 13);
          this.labelActions.TabIndex = 36;
          this.labelActions.Text = "Actions";
          // 
          // buttonEditAction
          // 
          this.buttonEditAction.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.buttonEditAction.Location = new System.Drawing.Point(344, 323);
          this.buttonEditAction.Name = "buttonEditAction";
          this.buttonEditAction.Size = new System.Drawing.Size(89, 25);
          this.buttonEditAction.TabIndex = 42;
          this.buttonEditAction.Text = "&Edit...";
          this.buttonEditAction.UseVisualStyleBackColor = true;
          this.buttonEditAction.Click += new System.EventHandler(this.buttonEditAction_Click);
          // 
          // buttonDeleteAction
          // 
          this.buttonDeleteAction.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.buttonDeleteAction.Location = new System.Drawing.Point(344, 354);
          this.buttonDeleteAction.Name = "buttonDeleteAction";
          this.buttonDeleteAction.Size = new System.Drawing.Size(89, 25);
          this.buttonDeleteAction.TabIndex = 41;
          this.buttonDeleteAction.Text = "Remove";
          this.buttonDeleteAction.UseVisualStyleBackColor = true;
          this.buttonDeleteAction.Click += new System.EventHandler(this.buttonDeleteAction_Click);
          // 
          // buttonAddAction
          // 
          this.buttonAddAction.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.buttonAddAction.Location = new System.Drawing.Point(344, 292);
          this.buttonAddAction.Name = "buttonAddAction";
          this.buttonAddAction.Size = new System.Drawing.Size(89, 25);
          this.buttonAddAction.TabIndex = 40;
          this.buttonAddAction.Text = "&Add...";
          this.buttonAddAction.UseVisualStyleBackColor = true;
          this.buttonAddAction.Click += new System.EventHandler(this.buttonAddAction_Click);
          // 
          // buttonMoveUp
          // 
          this.buttonMoveUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.buttonMoveUp.Location = new System.Drawing.Point(344, 385);
          this.buttonMoveUp.Name = "buttonMoveUp";
          this.buttonMoveUp.Size = new System.Drawing.Size(89, 25);
          this.buttonMoveUp.TabIndex = 43;
          this.buttonMoveUp.Text = "Move up";
          this.buttonMoveUp.UseVisualStyleBackColor = true;
          this.buttonMoveUp.Click += new System.EventHandler(this.buttonMoveUp_Click);
          // 
          // buttonMoveDown
          // 
          this.buttonMoveDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.buttonMoveDown.Location = new System.Drawing.Point(344, 416);
          this.buttonMoveDown.Name = "buttonMoveDown";
          this.buttonMoveDown.Size = new System.Drawing.Size(89, 25);
          this.buttonMoveDown.TabIndex = 44;
          this.buttonMoveDown.Text = "Move down";
          this.buttonMoveDown.UseVisualStyleBackColor = true;
          this.buttonMoveDown.Click += new System.EventHandler(this.buttonMoveDown_Click);
          // 
          // groupBox1
          // 
          this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.groupBox1.Location = new System.Drawing.Point(17, 449);
          this.groupBox1.Name = "groupBox1";
          this.groupBox1.Size = new System.Drawing.Size(401, 4);
          this.groupBox1.TabIndex = 45;
          this.groupBox1.TabStop = false;
          // 
          // btnCancel
          // 
          this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
          this.btnCancel.Location = new System.Drawing.Point(250, 459);
          this.btnCancel.Name = "btnCancel";
          this.btnCancel.Size = new System.Drawing.Size(89, 25);
          this.btnCancel.TabIndex = 47;
          this.btnCancel.Text = "&Cancel";
          this.btnCancel.UseVisualStyleBackColor = true;
          // 
          // btnOK
          // 
          this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
          this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
          this.btnOK.Location = new System.Drawing.Point(156, 459);
          this.btnOK.Name = "btnOK";
          this.btnOK.Size = new System.Drawing.Size(89, 25);
          this.btnOK.TabIndex = 46;
          this.btnOK.Text = "&OK";
          this.btnOK.UseVisualStyleBackColor = true;
          this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
          // 
          // listActions
          // 
          this.listActions.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.listActions.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader3});
          this.listActions.FullRowSelect = true;
          this.listActions.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
          this.listActions.Location = new System.Drawing.Point(29, 288);
          this.listActions.Name = "listActions";
          this.listActions.Size = new System.Drawing.Size(309, 153);
          this.listActions.TabIndex = 39;
          this.listActions.UseCompatibleStateImageBehavior = false;
          this.listActions.View = System.Windows.Forms.View.Details;
          this.listActions.SelectedIndexChanged += new System.EventHandler(this.listActions_SelectedIndexChanged);
          this.listActions.DoubleClick += new System.EventHandler(this.listActions_DoubleClick);
          // 
          // columnHeader3
          // 
          this.columnHeader3.Text = "Action";
          this.columnHeader3.Width = 250;
          // 
          // radioUseOR
          // 
          this.radioUseOR.AutoSize = true;
          this.radioUseOR.Location = new System.Drawing.Point(143, 126);
          this.radioUseOR.Name = "radioUseOR";
          this.radioUseOR.Size = new System.Drawing.Size(63, 17);
          this.radioUseOR.TabIndex = 38;
          this.radioUseOR.TabStop = true;
          this.radioUseOR.Text = "Use OR";
          this.radioUseOR.UseVisualStyleBackColor = true;
          // 
          // radioUseAND
          // 
          this.radioUseAND.AutoSize = true;
          this.radioUseAND.Location = new System.Drawing.Point(29, 126);
          this.radioUseAND.Name = "radioUseAND";
          this.radioUseAND.Size = new System.Drawing.Size(70, 17);
          this.radioUseAND.TabIndex = 37;
          this.radioUseAND.TabStop = true;
          this.radioUseAND.Text = "Use AND";
          this.radioUseAND.UseVisualStyleBackColor = true;
          // 
          // listCriterias
          // 
          this.listCriterias.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                      | System.Windows.Forms.AnchorStyles.Left)
                      | System.Windows.Forms.AnchorStyles.Right)));
          this.listCriterias.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader,
            this.columnHeader1,
            this.columnHeader2});
          this.listCriterias.FullRowSelect = true;
          this.listCriterias.Location = new System.Drawing.Point(29, 149);
          this.listCriterias.Name = "listCriterias";
          this.listCriterias.Size = new System.Drawing.Size(309, 104);
          this.listCriterias.TabIndex = 31;
          this.listCriterias.UseCompatibleStateImageBehavior = false;
          this.listCriterias.View = System.Windows.Forms.View.Details;
          this.listCriterias.SelectedIndexChanged += new System.EventHandler(this.listCriterias_SelectedIndexChanged);
          this.listCriterias.DoubleClick += new System.EventHandler(this.listCriterias_DoubleClick);
          // 
          // columnHeader
          // 
          this.columnHeader.Text = "Field";
          this.columnHeader.Width = 100;
          // 
          // columnHeader1
          // 
          this.columnHeader1.Text = "Comparison";
          this.columnHeader1.Width = 100;
          // 
          // columnHeader2
          // 
          this.columnHeader2.Text = "Value";
          this.columnHeader2.Width = 100;
          // 
          // checkEnabled
          // 
          this.checkEnabled.AutoSize = true;
          this.checkEnabled.Location = new System.Drawing.Point(29, 71);
          this.checkEnabled.Name = "checkEnabled";
          this.checkEnabled.Size = new System.Drawing.Size(65, 17);
          this.checkEnabled.TabIndex = 29;
          this.checkEnabled.Text = "Enabled";
          this.checkEnabled.UseVisualStyleBackColor = true;
          // 
          // textName
          // 
          this.textName.Location = new System.Drawing.Point(29, 45);
          this.textName.Name = "textName";
          this.textName.Number = 0;
          this.textName.Number64 = ((long)(0));
          this.textName.Numeric = false;
          this.textName.Size = new System.Drawing.Size(129, 20);
          this.textName.TabIndex = 14;
          // 
          // formRule
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.ClientSize = new System.Drawing.Size(438, 489);
          this.Controls.Add(this.btnCancel);
          this.Controls.Add(this.btnOK);
          this.Controls.Add(this.groupBox1);
          this.Controls.Add(this.buttonMoveDown);
          this.Controls.Add(this.buttonMoveUp);
          this.Controls.Add(this.buttonEditAction);
          this.Controls.Add(this.buttonDeleteAction);
          this.Controls.Add(this.buttonAddAction);
          this.Controls.Add(this.listActions);
          this.Controls.Add(this.radioUseOR);
          this.Controls.Add(this.radioUseAND);
          this.Controls.Add(this.labelActions);
          this.Controls.Add(this.buttonEditCriteria);
          this.Controls.Add(this.buttonDeleteCriteria);
          this.Controls.Add(this.buttonAddCriteria);
          this.Controls.Add(this.listCriterias);
          this.Controls.Add(this.labelCriteria);
          this.Controls.Add(this.checkEnabled);
          this.Controls.Add(this.labelName);
          this.Controls.Add(this.textName);
          this.Controls.Add(this.labelGeneral);
          this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
          this.MaximizeBox = false;
          this.MinimizeBox = false;
          this.MinimumSize = new System.Drawing.Size(446, 516);
          this.Name = "formRule";
          this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
          this.Text = "Rule";
          this.Load += new System.EventHandler(this.formRule_Load);
          this.ResumeLayout(false);
          this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Label labelGeneral;
      private System.Windows.Forms.Label labelName;
      private hMailServer.Shared.ucText textName;
      private hMailServer.Administrator.Controls.ucCheckbox checkEnabled;
      private System.Windows.Forms.Label labelCriteria;
      private ucListView listCriterias;
      private System.Windows.Forms.Button buttonAddCriteria;
      private System.Windows.Forms.Button buttonDeleteCriteria;
      private System.Windows.Forms.Button buttonEditCriteria;
      private System.Windows.Forms.Label labelActions;
      private hMailServer.Administrator.Controls.ucRadioButton radioUseAND;
      private hMailServer.Administrator.Controls.ucRadioButton radioUseOR;
      private ucListView listActions;
      private System.Windows.Forms.Button buttonEditAction;
      private System.Windows.Forms.Button buttonDeleteAction;
      private System.Windows.Forms.Button buttonAddAction;
      private System.Windows.Forms.Button buttonMoveUp;
      private System.Windows.Forms.Button buttonMoveDown;
      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Button btnCancel;
      private System.Windows.Forms.Button btnOK;
      private System.Windows.Forms.ColumnHeader columnHeader;
      private System.Windows.Forms.ColumnHeader columnHeader1;
      private System.Windows.Forms.ColumnHeader columnHeader2;
      private System.Windows.Forms.ColumnHeader columnHeader3;
   }
}