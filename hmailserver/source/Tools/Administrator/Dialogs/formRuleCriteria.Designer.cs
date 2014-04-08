namespace hMailServer.Administrator.Dialogs
{
   partial class formRuleCriteria
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
          this.groupBox1 = new System.Windows.Forms.GroupBox();
          this.btnCancel = new System.Windows.Forms.Button();
          this.btnOK = new System.Windows.Forms.Button();
          this.labelSearchType = new System.Windows.Forms.Label();
          this.labelValue = new System.Windows.Forms.Label();
          this.labelTestValue = new System.Windows.Forms.Label();
          this.labelTestResult = new System.Windows.Forms.Label();
          this.label1 = new System.Windows.Forms.Label();
          this.textTestValue = new hMailServer.Shared.ucText();
          this.txtMatchValue = new hMailServer.Shared.ucText();
          this.comboSearchType = new hMailServer.Administrator.Controls.ucComboBox();
          this.txtHeaderField = new hMailServer.Shared.ucText();
          this.comboPredefinedField = new hMailServer.Administrator.Controls.ucComboBox();
          this.radioCustomField = new hMailServer.Administrator.Controls.ucRadioButton();
          this.radioPredefinedField = new hMailServer.Administrator.Controls.ucRadioButton();
          this.SuspendLayout();
          // 
          // groupBox1
          // 
          this.groupBox1.Location = new System.Drawing.Point(0, 184);
          this.groupBox1.Name = "groupBox1";
          this.groupBox1.Size = new System.Drawing.Size(368, 4);
          this.groupBox1.TabIndex = 18;
          this.groupBox1.TabStop = false;
          // 
          // btnCancel
          // 
          this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
          this.btnCancel.Location = new System.Drawing.Point(281, 203);
          this.btnCancel.Name = "btnCancel";
          this.btnCancel.Size = new System.Drawing.Size(89, 25);
          this.btnCancel.TabIndex = 17;
          this.btnCancel.Text = "&Cancel";
          this.btnCancel.UseVisualStyleBackColor = true;
          // 
          // btnOK
          // 
          this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
          this.btnOK.Location = new System.Drawing.Point(186, 203);
          this.btnOK.Name = "btnOK";
          this.btnOK.Size = new System.Drawing.Size(89, 25);
          this.btnOK.TabIndex = 16;
          this.btnOK.Text = "&OK";
          this.btnOK.UseVisualStyleBackColor = true;
          this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
          // 
          // labelSearchType
          // 
          this.labelSearchType.AutoSize = true;
          this.labelSearchType.Location = new System.Drawing.Point(5, 67);
          this.labelSearchType.Name = "labelSearchType";
          this.labelSearchType.Size = new System.Drawing.Size(64, 13);
          this.labelSearchType.TabIndex = 23;
          this.labelSearchType.Text = "Search type";
          // 
          // labelValue
          // 
          this.labelValue.AutoSize = true;
          this.labelValue.Location = new System.Drawing.Point(183, 68);
          this.labelValue.Name = "labelValue";
          this.labelValue.Size = new System.Drawing.Size(34, 13);
          this.labelValue.TabIndex = 26;
          this.labelValue.Text = "Value";
          // 
          // labelTestValue
          // 
          this.labelTestValue.AutoSize = true;
          this.labelTestValue.Location = new System.Drawing.Point(5, 137);
          this.labelTestValue.Name = "labelTestValue";
          this.labelTestValue.Size = new System.Drawing.Size(34, 13);
          this.labelTestValue.TabIndex = 27;
          this.labelTestValue.Text = "Value";
          // 
          // labelTestResult
          // 
          this.labelTestResult.AutoSize = true;
          this.labelTestResult.Location = new System.Drawing.Point(183, 156);
          this.labelTestResult.Name = "labelTestResult";
          this.labelTestResult.Size = new System.Drawing.Size(0, 13);
          this.labelTestResult.TabIndex = 30;
          // 
          // label1
          // 
          this.label1.AutoSize = true;
          this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
          this.label1.Location = new System.Drawing.Point(5, 118);
          this.label1.Name = "label1";
          this.label1.Size = new System.Drawing.Size(32, 13);
          this.label1.TabIndex = 31;
          this.label1.Text = "Test";
          // 
          // textTestValue
          // 
          this.textTestValue.Location = new System.Drawing.Point(8, 153);
          this.textTestValue.Name = "textTestValue";
          this.textTestValue.Number = 0;
          this.textTestValue.Numeric = false;
          this.textTestValue.Size = new System.Drawing.Size(172, 20);
          this.textTestValue.TabIndex = 28;
          this.textTestValue.TextChanged += new System.EventHandler(this.textTestValue_TextChanged);
          // 
          // txtMatchValue
          // 
          this.txtMatchValue.Location = new System.Drawing.Point(186, 84);
          this.txtMatchValue.Name = "txtMatchValue";
          this.txtMatchValue.Number = 0;
          this.txtMatchValue.Numeric = false;
          this.txtMatchValue.Size = new System.Drawing.Size(172, 20);
          this.txtMatchValue.TabIndex = 25;
          this.txtMatchValue.TextChanged += new System.EventHandler(this.txtMatchValue_TextChanged);
          // 
          // comboSearchType
          // 
          this.comboSearchType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboSearchType.FormattingEnabled = true;
          this.comboSearchType.Location = new System.Drawing.Point(8, 83);
          this.comboSearchType.Name = "comboSearchType";
          this.comboSearchType.Size = new System.Drawing.Size(172, 21);
          this.comboSearchType.TabIndex = 24;
          this.comboSearchType.SelectionChangeCommitted += new System.EventHandler(this.comboSearchType_SelectionChangeCommitted);
          // 
          // txtHeaderField
          // 
          this.txtHeaderField.Location = new System.Drawing.Point(186, 31);
          this.txtHeaderField.Name = "txtHeaderField";
          this.txtHeaderField.Number = 0;
          this.txtHeaderField.Numeric = false;
          this.txtHeaderField.Size = new System.Drawing.Size(175, 20);
          this.txtHeaderField.TabIndex = 22;
          // 
          // comboPredefinedField
          // 
          this.comboPredefinedField.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
          this.comboPredefinedField.FormattingEnabled = true;
          this.comboPredefinedField.Location = new System.Drawing.Point(8, 32);
          this.comboPredefinedField.Name = "comboPredefinedField";
          this.comboPredefinedField.Size = new System.Drawing.Size(172, 21);
          this.comboPredefinedField.TabIndex = 21;
          // 
          // radioCustomField
          // 
          this.radioCustomField.AutoSize = true;
          this.radioCustomField.Location = new System.Drawing.Point(184, 8);
          this.radioCustomField.Name = "radioCustomField";
          this.radioCustomField.Size = new System.Drawing.Size(118, 17);
          this.radioCustomField.TabIndex = 20;
          this.radioCustomField.TabStop = true;
          this.radioCustomField.Text = "Custom header field";
          this.radioCustomField.UseVisualStyleBackColor = true;
          this.radioCustomField.CheckedChanged += new System.EventHandler(this.radioCustomField_CheckedChanged);
          // 
          // radioPredefinedField
          // 
          this.radioPredefinedField.AutoSize = true;
          this.radioPredefinedField.Location = new System.Drawing.Point(8, 8);
          this.radioPredefinedField.Name = "radioPredefinedField";
          this.radioPredefinedField.Size = new System.Drawing.Size(98, 17);
          this.radioPredefinedField.TabIndex = 19;
          this.radioPredefinedField.TabStop = true;
          this.radioPredefinedField.Text = "Predefined field";
          this.radioPredefinedField.UseVisualStyleBackColor = true;
          this.radioPredefinedField.CheckedChanged += new System.EventHandler(this.radioPredefinedField_CheckedChanged);
          // 
          // formRuleCriteria
          // 
          this.AcceptButton = this.btnOK;
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.CancelButton = this.btnCancel;
          this.ClientSize = new System.Drawing.Size(387, 236);
          this.Controls.Add(this.label1);
          this.Controls.Add(this.labelTestResult);
          this.Controls.Add(this.textTestValue);
          this.Controls.Add(this.labelTestValue);
          this.Controls.Add(this.labelValue);
          this.Controls.Add(this.txtMatchValue);
          this.Controls.Add(this.comboSearchType);
          this.Controls.Add(this.labelSearchType);
          this.Controls.Add(this.txtHeaderField);
          this.Controls.Add(this.comboPredefinedField);
          this.Controls.Add(this.radioCustomField);
          this.Controls.Add(this.radioPredefinedField);
          this.Controls.Add(this.groupBox1);
          this.Controls.Add(this.btnCancel);
          this.Controls.Add(this.btnOK);
          this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
          this.MaximizeBox = false;
          this.MinimizeBox = false;
          this.Name = "formRuleCriteria";
          this.ShowInTaskbar = false;
          this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
          this.Text = "Criteria";
          this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.formRuleCriteria_FormClosing);
          this.ResumeLayout(false);
          this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Button btnCancel;
      private System.Windows.Forms.Button btnOK;
      private hMailServer.Administrator.Controls.ucRadioButton radioPredefinedField;
      private hMailServer.Administrator.Controls.ucRadioButton radioCustomField;
      private hMailServer.Administrator.Controls.ucComboBox comboPredefinedField;
      private hMailServer.Shared.ucText txtHeaderField;
      private System.Windows.Forms.Label labelSearchType;
      private hMailServer.Administrator.Controls.ucComboBox comboSearchType;
      private hMailServer.Shared.ucText txtMatchValue;
      private System.Windows.Forms.Label labelValue;
      private System.Windows.Forms.Label labelTestValue;
      private hMailServer.Shared.ucText textTestValue;
      private System.Windows.Forms.Label labelTestResult;
      private System.Windows.Forms.Label label1;
   }
}