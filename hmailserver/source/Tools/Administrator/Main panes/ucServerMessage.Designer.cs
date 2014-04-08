namespace hMailServer.Administrator
{
   partial class ucServerMessage
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
         this.textMessage = new hMailServer.Shared.ucText();
         this.SuspendLayout();
         // 
         // textMessage
         // 
         this.textMessage.Location = new System.Drawing.Point(8, 8);
         this.textMessage.Multiline = true;
         this.textMessage.Name = "textMessage";
         this.textMessage.Number = 0;
         this.textMessage.Numeric = false;
         this.textMessage.Size = new System.Drawing.Size(520, 272);
         this.textMessage.TabIndex = 1;
         // 
         // ucServerMessage
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.textMessage);
         this.Name = "ucServerMessage";
         this.Size = new System.Drawing.Size(539, 292);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private hMailServer.Shared.ucText textMessage;
   }
}
