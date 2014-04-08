namespace hMailServer.Administrator
{
   partial class ucAutoBan
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
         this.checkAutoBanOnLogonFailure = new hMailServer.Administrator.Controls.ucCheckbox();
         this.labelMaxInvalidLogonAttempts = new System.Windows.Forms.Label();
         this.textMaxInvalidLogonAttempts = new hMailServer.Shared.ucText();
         this.labelMaxInvalidLogonAttemptsWithin = new System.Windows.Forms.Label();
         this.textMaxInvalidLogonAttemptsWithin = new hMailServer.Shared.ucText();
         this.labelAutoBanMinutes = new System.Windows.Forms.Label();
         this.textAutoBanMinutes = new hMailServer.Shared.ucText();
         this.labelPleaseNote = new System.Windows.Forms.Label();
         this.labelAutobanhelpHext = new System.Windows.Forms.Label();
         this.label1 = new System.Windows.Forms.Label();
         this.linkMoreDetails = new System.Windows.Forms.LinkLabel();
         this.SuspendLayout();
         // 
         // checkAutoBanOnLogonFailure
         // 
         this.checkAutoBanOnLogonFailure.AutoSize = true;
         this.checkAutoBanOnLogonFailure.Location = new System.Drawing.Point(12, 14);
         this.checkAutoBanOnLogonFailure.Name = "checkAutoBanOnLogonFailure";
         this.checkAutoBanOnLogonFailure.Size = new System.Drawing.Size(65, 17);
         this.checkAutoBanOnLogonFailure.TabIndex = 31;
         this.checkAutoBanOnLogonFailure.Text = "Enabled";
         this.checkAutoBanOnLogonFailure.UseVisualStyleBackColor = true;
         this.checkAutoBanOnLogonFailure.CheckedChanged += new System.EventHandler(this.checkAutoBanOnLogonFailure_CheckedChanged);
         // 
         // labelMaxInvalidLogonAttempts
         // 
         this.labelMaxInvalidLogonAttempts.AutoSize = true;
         this.labelMaxInvalidLogonAttempts.Location = new System.Drawing.Point(29, 45);
         this.labelMaxInvalidLogonAttempts.Name = "labelMaxInvalidLogonAttempts";
         this.labelMaxInvalidLogonAttempts.Size = new System.Drawing.Size(132, 13);
         this.labelMaxInvalidLogonAttempts.TabIndex = 33;
         this.labelMaxInvalidLogonAttempts.Text = "Max invalid logon attempts";
         // 
         // textMaxInvalidLogonAttempts
         // 
         this.textMaxInvalidLogonAttempts.Location = new System.Drawing.Point(32, 61);
         this.textMaxInvalidLogonAttempts.MaxLength = 3;
         this.textMaxInvalidLogonAttempts.Name = "textMaxInvalidLogonAttempts";
         this.textMaxInvalidLogonAttempts.Number = 0;
         this.textMaxInvalidLogonAttempts.Numeric = true;
         this.textMaxInvalidLogonAttempts.Size = new System.Drawing.Size(90, 20);
         this.textMaxInvalidLogonAttempts.TabIndex = 32;
         // 
         // labelMaxInvalidLogonAttemptsWithin
         // 
         this.labelMaxInvalidLogonAttemptsWithin.AutoSize = true;
         this.labelMaxInvalidLogonAttemptsWithin.Location = new System.Drawing.Point(29, 94);
         this.labelMaxInvalidLogonAttemptsWithin.Name = "labelMaxInvalidLogonAttemptsWithin";
         this.labelMaxInvalidLogonAttemptsWithin.Size = new System.Drawing.Size(103, 13);
         this.labelMaxInvalidLogonAttemptsWithin.TabIndex = 35;
         this.labelMaxInvalidLogonAttemptsWithin.Text = "Minutes before reset";
         // 
         // textMaxInvalidLogonAttemptsWithin
         // 
         this.textMaxInvalidLogonAttemptsWithin.Location = new System.Drawing.Point(32, 110);
         this.textMaxInvalidLogonAttemptsWithin.Name = "textMaxInvalidLogonAttemptsWithin";
         this.textMaxInvalidLogonAttemptsWithin.Number = 0;
         this.textMaxInvalidLogonAttemptsWithin.Numeric = true;
         this.textMaxInvalidLogonAttemptsWithin.Size = new System.Drawing.Size(90, 20);
         this.textMaxInvalidLogonAttemptsWithin.TabIndex = 34;
         // 
         // labelAutoBanMinutes
         // 
         this.labelAutoBanMinutes.AutoSize = true;
         this.labelAutoBanMinutes.Location = new System.Drawing.Point(29, 141);
         this.labelAutoBanMinutes.Name = "labelAutoBanMinutes";
         this.labelAutoBanMinutes.Size = new System.Drawing.Size(101, 13);
         this.labelAutoBanMinutes.TabIndex = 37;
         this.labelAutoBanMinutes.Text = "Minutes to auto-ban";
         // 
         // textAutoBanMinutes
         // 
         this.textAutoBanMinutes.Location = new System.Drawing.Point(32, 157);
         this.textAutoBanMinutes.Name = "textAutoBanMinutes";
         this.textAutoBanMinutes.Number = 0;
         this.textAutoBanMinutes.Numeric = true;
         this.textAutoBanMinutes.Size = new System.Drawing.Size(90, 20);
         this.textAutoBanMinutes.TabIndex = 36;
         // 
         // labelPleaseNote
         // 
         this.labelPleaseNote.AutoSize = true;
         this.labelPleaseNote.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
         this.labelPleaseNote.Location = new System.Drawing.Point(18, 217);
         this.labelPleaseNote.Name = "labelPleaseNote";
         this.labelPleaseNote.Size = new System.Drawing.Size(86, 13);
         this.labelPleaseNote.TabIndex = 38;
         this.labelPleaseNote.Text = "Please note...";
         // 
         // labelAutobanhelpHext
         // 
         this.labelAutobanhelpHext.Location = new System.Drawing.Point(29, 239);
         this.labelAutobanhelpHext.Name = "labelAutobanhelpHext";
         this.labelAutobanhelpHext.Size = new System.Drawing.Size(471, 33);
         this.labelAutobanhelpHext.TabIndex = 39;
         this.labelAutobanhelpHext.Text = "The auto-ban functionality blocks IP addresses. If too many invalid logon attempt" +
             "s have been made from a specific IP address, that IP address will be banned.";
         // 
         // label1
         // 
         this.label1.Location = new System.Drawing.Point(29, 272);
         this.label1.Name = "label1";
         this.label1.Size = new System.Drawing.Size(471, 33);
         this.label1.TabIndex = 40;
         this.label1.Text = "If you are running a web mail system, this means that the webmail system may be b" +
             "locked, if too many invalid logon attempts are made on it.";
         // 
         // linkMoreDetails
         // 
         this.linkMoreDetails.AutoSize = true;
         this.linkMoreDetails.Location = new System.Drawing.Point(29, 305);
         this.linkMoreDetails.Name = "linkMoreDetails";
         this.linkMoreDetails.Size = new System.Drawing.Size(73, 13);
         this.linkMoreDetails.TabIndex = 42;
         this.linkMoreDetails.TabStop = true;
         this.linkMoreDetails.Text = "More details...";
         this.linkMoreDetails.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkMoreDetails_LinkClicked);
         // 
         // ucAutoBan
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.linkMoreDetails);
         this.Controls.Add(this.label1);
         this.Controls.Add(this.labelAutobanhelpHext);
         this.Controls.Add(this.labelPleaseNote);
         this.Controls.Add(this.labelAutoBanMinutes);
         this.Controls.Add(this.textAutoBanMinutes);
         this.Controls.Add(this.labelMaxInvalidLogonAttemptsWithin);
         this.Controls.Add(this.textMaxInvalidLogonAttemptsWithin);
         this.Controls.Add(this.labelMaxInvalidLogonAttempts);
         this.Controls.Add(this.textMaxInvalidLogonAttempts);
         this.Controls.Add(this.checkAutoBanOnLogonFailure);
         this.Name = "ucAutoBan";
         this.Size = new System.Drawing.Size(569, 424);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private hMailServer.Administrator.Controls.ucCheckbox checkAutoBanOnLogonFailure;
      private System.Windows.Forms.Label labelMaxInvalidLogonAttempts;
      private hMailServer.Shared.ucText textMaxInvalidLogonAttempts;
      private System.Windows.Forms.Label labelMaxInvalidLogonAttemptsWithin;
      private hMailServer.Shared.ucText textMaxInvalidLogonAttemptsWithin;
      private System.Windows.Forms.Label labelAutoBanMinutes;
      private hMailServer.Shared.ucText textAutoBanMinutes;
      private System.Windows.Forms.Label labelPleaseNote;
      private System.Windows.Forms.Label labelAutobanhelpHext;
      private System.Windows.Forms.Label label1;
      private System.Windows.Forms.LinkLabel linkMoreDetails;
   }
}
