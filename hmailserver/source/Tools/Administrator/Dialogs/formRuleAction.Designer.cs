namespace hMailServer.Administrator.Dialogs
{
   partial class formRuleAction
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
         this.panelForward = new System.Windows.Forms.Panel();
         this.labelForwardTo = new System.Windows.Forms.Label();
         this.panelIMAP = new System.Windows.Forms.Panel();
         this.labelIMAPFolder = new System.Windows.Forms.Label();
         this.panelScriptFunction = new System.Windows.Forms.Panel();
         this.labelScriptFunction = new System.Windows.Forms.Label();
         this.panelReply = new System.Windows.Forms.Panel();
         this.label1 = new System.Windows.Forms.Label();
         this.labelSubject = new System.Windows.Forms.Label();
         this.labelFromAddress = new System.Windows.Forms.Label();
         this.labelFromName = new System.Windows.Forms.Label();
         this.panelSetHeaderValue = new System.Windows.Forms.Panel();
         this.labelHeaderValue = new System.Windows.Forms.Label();
         this.labelHeaderName = new System.Windows.Forms.Label();
         this.groupBox1 = new System.Windows.Forms.GroupBox();
         this.btnCancel = new System.Windows.Forms.Button();
         this.btnOK = new System.Windows.Forms.Button();
         this.panelSendUsingRoute = new System.Windows.Forms.Panel();
         this.label2 = new System.Windows.Forms.Label();
         this.textHeaderValue = new hMailServer.Shared.ucText();
         this.textHeaderName = new hMailServer.Shared.ucText();
         this.textActionBody = new hMailServer.Shared.ucText();
         this.textActionSubject = new hMailServer.Shared.ucText();
         this.textActionFromAddress = new hMailServer.Shared.ucText();
         this.textActionFromName = new hMailServer.Shared.ucText();
         this.txtActionScriptFunction = new hMailServer.Shared.ucText();
         this.textIMAPFolder = new hMailServer.Shared.ucText();
         this.textForwardTo = new hMailServer.Shared.ucText();
         this.panelBindToAddress = new System.Windows.Forms.Panel();
         this.textBindToAddress = new hMailServer.Shared.ucText();
         this.labelIPAddress = new System.Windows.Forms.Label();
         this.comboRouteName = new hMailServer.Administrator.Controls.ucComboBox();
         this.comboAction = new hMailServer.Administrator.Controls.ucComboBox();
         this.panelForward.SuspendLayout();
         this.panelIMAP.SuspendLayout();
         this.panelScriptFunction.SuspendLayout();
         this.panelReply.SuspendLayout();
         this.panelSetHeaderValue.SuspendLayout();
         this.panelSendUsingRoute.SuspendLayout();
         this.panelBindToAddress.SuspendLayout();
         this.SuspendLayout();
         // 
         // panelForward
         // 
         this.panelForward.Controls.Add(this.textForwardTo);
         this.panelForward.Controls.Add(this.labelForwardTo);
         this.panelForward.Location = new System.Drawing.Point(7, 34);
         this.panelForward.Name = "panelForward";
         this.panelForward.Size = new System.Drawing.Size(274, 258);
         this.panelForward.TabIndex = 1;
         // 
         // labelForwardTo
         // 
         this.labelForwardTo.AutoSize = true;
         this.labelForwardTo.Location = new System.Drawing.Point(8, 10);
         this.labelForwardTo.Name = "labelForwardTo";
         this.labelForwardTo.Size = new System.Drawing.Size(20, 13);
         this.labelForwardTo.TabIndex = 0;
         this.labelForwardTo.Text = "To";
         // 
         // panelIMAP
         // 
         this.panelIMAP.Controls.Add(this.textIMAPFolder);
         this.panelIMAP.Controls.Add(this.labelIMAPFolder);
         this.panelIMAP.Location = new System.Drawing.Point(640, 32);
         this.panelIMAP.Name = "panelIMAP";
         this.panelIMAP.Size = new System.Drawing.Size(274, 258);
         this.panelIMAP.TabIndex = 2;
         // 
         // labelIMAPFolder
         // 
         this.labelIMAPFolder.AutoSize = true;
         this.labelIMAPFolder.Location = new System.Drawing.Point(8, 10);
         this.labelIMAPFolder.Name = "labelIMAPFolder";
         this.labelIMAPFolder.Size = new System.Drawing.Size(62, 13);
         this.labelIMAPFolder.TabIndex = 0;
         this.labelIMAPFolder.Text = "IMAP folder";
         // 
         // panelScriptFunction
         // 
         this.panelScriptFunction.Controls.Add(this.txtActionScriptFunction);
         this.panelScriptFunction.Controls.Add(this.labelScriptFunction);
         this.panelScriptFunction.Location = new System.Drawing.Point(7, 362);
         this.panelScriptFunction.Name = "panelScriptFunction";
         this.panelScriptFunction.Size = new System.Drawing.Size(274, 258);
         this.panelScriptFunction.TabIndex = 3;
         // 
         // labelScriptFunction
         // 
         this.labelScriptFunction.AutoSize = true;
         this.labelScriptFunction.Location = new System.Drawing.Point(8, 10);
         this.labelScriptFunction.Name = "labelScriptFunction";
         this.labelScriptFunction.Size = new System.Drawing.Size(75, 13);
         this.labelScriptFunction.TabIndex = 0;
         this.labelScriptFunction.Text = "Script function";
         // 
         // panelReply
         // 
         this.panelReply.Controls.Add(this.textActionBody);
         this.panelReply.Controls.Add(this.label1);
         this.panelReply.Controls.Add(this.textActionSubject);
         this.panelReply.Controls.Add(this.labelSubject);
         this.panelReply.Controls.Add(this.textActionFromAddress);
         this.panelReply.Controls.Add(this.labelFromAddress);
         this.panelReply.Controls.Add(this.textActionFromName);
         this.panelReply.Controls.Add(this.labelFromName);
         this.panelReply.Location = new System.Drawing.Point(287, 362);
         this.panelReply.Name = "panelReply";
         this.panelReply.Size = new System.Drawing.Size(274, 258);
         this.panelReply.TabIndex = 4;
         // 
         // label1
         // 
         this.label1.AutoSize = true;
         this.label1.Location = new System.Drawing.Point(13, 138);
         this.label1.Name = "label1";
         this.label1.Size = new System.Drawing.Size(31, 13);
         this.label1.TabIndex = 6;
         this.label1.Text = "Body";
         // 
         // labelSubject
         // 
         this.labelSubject.AutoSize = true;
         this.labelSubject.Location = new System.Drawing.Point(10, 95);
         this.labelSubject.Name = "labelSubject";
         this.labelSubject.Size = new System.Drawing.Size(43, 13);
         this.labelSubject.TabIndex = 4;
         this.labelSubject.Text = "Subject";
         // 
         // labelFromAddress
         // 
         this.labelFromAddress.AutoSize = true;
         this.labelFromAddress.Location = new System.Drawing.Point(10, 55);
         this.labelFromAddress.Name = "labelFromAddress";
         this.labelFromAddress.Size = new System.Drawing.Size(77, 13);
         this.labelFromAddress.TabIndex = 2;
         this.labelFromAddress.Text = "From (Address)";
         // 
         // labelFromName
         // 
         this.labelFromName.AutoSize = true;
         this.labelFromName.Location = new System.Drawing.Point(10, 11);
         this.labelFromName.Name = "labelFromName";
         this.labelFromName.Size = new System.Drawing.Size(67, 13);
         this.labelFromName.TabIndex = 0;
         this.labelFromName.Text = "From (Name)";
         // 
         // panelSetHeaderValue
         // 
         this.panelSetHeaderValue.Controls.Add(this.textHeaderValue);
         this.panelSetHeaderValue.Controls.Add(this.labelHeaderValue);
         this.panelSetHeaderValue.Controls.Add(this.textHeaderName);
         this.panelSetHeaderValue.Controls.Add(this.labelHeaderName);
         this.panelSetHeaderValue.Location = new System.Drawing.Point(573, 362);
         this.panelSetHeaderValue.Name = "panelSetHeaderValue";
         this.panelSetHeaderValue.Size = new System.Drawing.Size(274, 258);
         this.panelSetHeaderValue.TabIndex = 5;
         // 
         // labelHeaderValue
         // 
         this.labelHeaderValue.AutoSize = true;
         this.labelHeaderValue.Location = new System.Drawing.Point(10, 55);
         this.labelHeaderValue.Name = "labelHeaderValue";
         this.labelHeaderValue.Size = new System.Drawing.Size(34, 13);
         this.labelHeaderValue.TabIndex = 2;
         this.labelHeaderValue.Text = "Value";
         // 
         // labelHeaderName
         // 
         this.labelHeaderName.AutoSize = true;
         this.labelHeaderName.Location = new System.Drawing.Point(10, 11);
         this.labelHeaderName.Name = "labelHeaderName";
         this.labelHeaderName.Size = new System.Drawing.Size(71, 13);
         this.labelHeaderName.TabIndex = 0;
         this.labelHeaderName.Text = "Header name";
         // 
         // groupBox1
         // 
         this.groupBox1.Location = new System.Drawing.Point(8, 300);
         this.groupBox1.Name = "groupBox1";
         this.groupBox1.Size = new System.Drawing.Size(250, 4);
         this.groupBox1.TabIndex = 21;
         this.groupBox1.TabStop = false;
         // 
         // btnCancel
         // 
         this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
         this.btnCancel.Location = new System.Drawing.Point(165, 316);
         this.btnCancel.Name = "btnCancel";
         this.btnCancel.Size = new System.Drawing.Size(89, 25);
         this.btnCancel.TabIndex = 20;
         this.btnCancel.Text = "&Cancel";
         this.btnCancel.UseVisualStyleBackColor = true;
         // 
         // btnOK
         // 
         this.btnOK.Location = new System.Drawing.Point(70, 316);
         this.btnOK.Name = "btnOK";
         this.btnOK.Size = new System.Drawing.Size(89, 25);
         this.btnOK.TabIndex = 19;
         this.btnOK.Text = "&OK";
         this.btnOK.UseVisualStyleBackColor = true;
         this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
         // 
         // panelSendUsingRoute
         // 
         this.panelSendUsingRoute.Controls.Add(this.label2);
         this.panelSendUsingRoute.Controls.Add(this.comboRouteName);
         this.panelSendUsingRoute.Location = new System.Drawing.Point(352, 32);
         this.panelSendUsingRoute.Name = "panelSendUsingRoute";
         this.panelSendUsingRoute.Size = new System.Drawing.Size(274, 258);
         this.panelSendUsingRoute.TabIndex = 22;
         // 
         // label2
         // 
         this.label2.AutoSize = true;
         this.label2.Location = new System.Drawing.Point(8, 10);
         this.label2.Name = "label2";
         this.label2.Size = new System.Drawing.Size(20, 13);
         this.label2.TabIndex = 2;
         this.label2.Text = "To";
         this.label2.Click += new System.EventHandler(this.label2_Click);
         // 
         // textHeaderValue
         // 
         this.textHeaderValue.Location = new System.Drawing.Point(13, 71);
         this.textHeaderValue.Name = "textHeaderValue";
         this.textHeaderValue.Number = 0;
         this.textHeaderValue.Numeric = false;
         this.textHeaderValue.Size = new System.Drawing.Size(148, 20);
         this.textHeaderValue.TabIndex = 3;
         // 
         // textHeaderName
         // 
         this.textHeaderName.Location = new System.Drawing.Point(13, 27);
         this.textHeaderName.Name = "textHeaderName";
         this.textHeaderName.Number = 0;
         this.textHeaderName.Numeric = false;
         this.textHeaderName.Size = new System.Drawing.Size(148, 20);
         this.textHeaderName.TabIndex = 1;
         // 
         // textActionBody
         // 
         this.textActionBody.Location = new System.Drawing.Point(16, 154);
         this.textActionBody.Multiline = true;
         this.textActionBody.Name = "textActionBody";
         this.textActionBody.Number = 0;
         this.textActionBody.Numeric = false;
         this.textActionBody.Size = new System.Drawing.Size(245, 86);
         this.textActionBody.TabIndex = 7;
         // 
         // textActionSubject
         // 
         this.textActionSubject.Location = new System.Drawing.Point(13, 111);
         this.textActionSubject.Name = "textActionSubject";
         this.textActionSubject.Number = 0;
         this.textActionSubject.Numeric = false;
         this.textActionSubject.Size = new System.Drawing.Size(148, 20);
         this.textActionSubject.TabIndex = 5;
         // 
         // textActionFromAddress
         // 
         this.textActionFromAddress.Location = new System.Drawing.Point(13, 71);
         this.textActionFromAddress.Name = "textActionFromAddress";
         this.textActionFromAddress.Number = 0;
         this.textActionFromAddress.Numeric = false;
         this.textActionFromAddress.Size = new System.Drawing.Size(148, 20);
         this.textActionFromAddress.TabIndex = 3;
         // 
         // textActionFromName
         // 
         this.textActionFromName.Location = new System.Drawing.Point(13, 27);
         this.textActionFromName.Name = "textActionFromName";
         this.textActionFromName.Number = 0;
         this.textActionFromName.Numeric = false;
         this.textActionFromName.Size = new System.Drawing.Size(148, 20);
         this.textActionFromName.TabIndex = 1;
         // 
         // txtActionScriptFunction
         // 
         this.txtActionScriptFunction.Location = new System.Drawing.Point(13, 27);
         this.txtActionScriptFunction.Name = "txtActionScriptFunction";
         this.txtActionScriptFunction.Number = 0;
         this.txtActionScriptFunction.Numeric = false;
         this.txtActionScriptFunction.Size = new System.Drawing.Size(203, 20);
         this.txtActionScriptFunction.TabIndex = 1;
         // 
         // textIMAPFolder
         // 
         this.textIMAPFolder.Location = new System.Drawing.Point(13, 27);
         this.textIMAPFolder.Name = "textIMAPFolder";
         this.textIMAPFolder.Number = 0;
         this.textIMAPFolder.Numeric = false;
         this.textIMAPFolder.Size = new System.Drawing.Size(203, 20);
         this.textIMAPFolder.TabIndex = 1;
         // 
         // textForwardTo
         // 
         this.textForwardTo.Location = new System.Drawing.Point(13, 27);
         this.textForwardTo.Name = "textForwardTo";
         this.textForwardTo.Number = 0;
         this.textForwardTo.Numeric = false;
         this.textForwardTo.Size = new System.Drawing.Size(155, 20);
         this.textForwardTo.TabIndex = 1;
         // 
         // panelBindToAddress
         // 
         this.panelBindToAddress.Controls.Add(this.textBindToAddress);
         this.panelBindToAddress.Controls.Add(this.labelIPAddress);
         this.panelBindToAddress.Location = new System.Drawing.Point(838, 362);
         this.panelBindToAddress.Name = "panelBindToAddress";
         this.panelBindToAddress.Size = new System.Drawing.Size(274, 258);
         this.panelBindToAddress.TabIndex = 23;
         // 
         // textBindToAddress
         // 
         this.textBindToAddress.Location = new System.Drawing.Point(16, 27);
         this.textBindToAddress.Name = "textBindToAddress";
         this.textBindToAddress.Number = 0;
         this.textBindToAddress.Numeric = false;
         this.textBindToAddress.Size = new System.Drawing.Size(148, 20);
         this.textBindToAddress.TabIndex = 3;
         // 
         // labelIPAddress
         // 
         this.labelIPAddress.AutoSize = true;
         this.labelIPAddress.Location = new System.Drawing.Point(13, 10);
         this.labelIPAddress.Name = "labelIPAddress";
         this.labelIPAddress.Size = new System.Drawing.Size(57, 13);
         this.labelIPAddress.TabIndex = 2;
         this.labelIPAddress.Text = "IP address";
         // 
         // comboRouteName
         // 
         this.comboRouteName.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
         this.comboRouteName.FormattingEnabled = true;
         this.comboRouteName.Location = new System.Drawing.Point(13, 27);
         this.comboRouteName.Name = "comboRouteName";
         this.comboRouteName.Size = new System.Drawing.Size(251, 21);
         this.comboRouteName.TabIndex = 1;
         // 
         // comboAction
         // 
         this.comboAction.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
         this.comboAction.FormattingEnabled = true;
         this.comboAction.Location = new System.Drawing.Point(7, 7);
         this.comboAction.Name = "comboAction";
         this.comboAction.Size = new System.Drawing.Size(251, 21);
         this.comboAction.TabIndex = 0;
         this.comboAction.SelectedIndexChanged += new System.EventHandler(this.comboAction_SelectedIndexChanged);
         // 
         // formRuleAction
         // 
         this.AcceptButton = this.btnOK;
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.CancelButton = this.btnCancel;
         this.ClientSize = new System.Drawing.Size(285, 349);
         this.Controls.Add(this.panelBindToAddress);
         this.Controls.Add(this.panelSendUsingRoute);
         this.Controls.Add(this.groupBox1);
         this.Controls.Add(this.btnCancel);
         this.Controls.Add(this.btnOK);
         this.Controls.Add(this.panelSetHeaderValue);
         this.Controls.Add(this.panelReply);
         this.Controls.Add(this.panelScriptFunction);
         this.Controls.Add(this.panelIMAP);
         this.Controls.Add(this.panelForward);
         this.Controls.Add(this.comboAction);
         this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
         this.MaximizeBox = false;
         this.MinimizeBox = false;
         this.Name = "formRuleAction";
         this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
         this.Text = "Actions";
         this.Load += new System.EventHandler(this.formRuleAction_Load);
         this.panelForward.ResumeLayout(false);
         this.panelForward.PerformLayout();
         this.panelIMAP.ResumeLayout(false);
         this.panelIMAP.PerformLayout();
         this.panelScriptFunction.ResumeLayout(false);
         this.panelScriptFunction.PerformLayout();
         this.panelReply.ResumeLayout(false);
         this.panelReply.PerformLayout();
         this.panelSetHeaderValue.ResumeLayout(false);
         this.panelSetHeaderValue.PerformLayout();
         this.panelSendUsingRoute.ResumeLayout(false);
         this.panelSendUsingRoute.PerformLayout();
         this.panelBindToAddress.ResumeLayout(false);
         this.panelBindToAddress.PerformLayout();
         this.ResumeLayout(false);

      }

      #endregion

      private hMailServer.Administrator.Controls.ucComboBox comboAction;
      private System.Windows.Forms.Panel panelForward;
      private System.Windows.Forms.Label labelForwardTo;
      private hMailServer.Shared.ucText textForwardTo;
      private System.Windows.Forms.Panel panelIMAP;
      private hMailServer.Shared.ucText textIMAPFolder;
      private System.Windows.Forms.Label labelIMAPFolder;
      private System.Windows.Forms.Panel panelScriptFunction;
      private hMailServer.Shared.ucText txtActionScriptFunction;
      private System.Windows.Forms.Label labelScriptFunction;
      private System.Windows.Forms.Panel panelReply;
      private hMailServer.Shared.ucText textActionFromAddress;
      private System.Windows.Forms.Label labelFromAddress;
      private hMailServer.Shared.ucText textActionFromName;
      private System.Windows.Forms.Label labelFromName;
      private hMailServer.Shared.ucText textActionBody;
      private System.Windows.Forms.Label label1;
      private hMailServer.Shared.ucText textActionSubject;
      private System.Windows.Forms.Label labelSubject;
      private System.Windows.Forms.Panel panelSetHeaderValue;
      private hMailServer.Shared.ucText textHeaderValue;
      private System.Windows.Forms.Label labelHeaderValue;
      private hMailServer.Shared.ucText textHeaderName;
      private System.Windows.Forms.Label labelHeaderName;
      private System.Windows.Forms.GroupBox groupBox1;
      private System.Windows.Forms.Button btnCancel;
      private System.Windows.Forms.Button btnOK;
      private System.Windows.Forms.Panel panelSendUsingRoute;
      private System.Windows.Forms.Label label2;
      private hMailServer.Administrator.Controls.ucComboBox comboRouteName;
      private System.Windows.Forms.Panel panelBindToAddress;
      private hMailServer.Shared.ucText textBindToAddress;
      private System.Windows.Forms.Label labelIPAddress;
   }
}