namespace DataDirectorySynchronizer.Pages
{
   partial class ucProgress
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
         this.labelStatusLabel = new System.Windows.Forms.Label();
         this.labelStatus = new System.Windows.Forms.Label();
         this.labelExecutionTimeLabel = new System.Windows.Forms.Label();
         this.labelExecutionTime = new System.Windows.Forms.Label();
         this.timer = new System.Windows.Forms.Timer(this.components);
         this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
         this.listProcess = new System.Windows.Forms.ListView();
         this.labelSkipped = new System.Windows.Forms.Label();
         this.SuspendLayout();
         // 
         // labelStatusLabel
         // 
         this.labelStatusLabel.AutoSize = true;
         this.labelStatusLabel.Location = new System.Drawing.Point(8, 8);
         this.labelStatusLabel.Name = "labelStatusLabel";
         this.labelStatusLabel.Size = new System.Drawing.Size(37, 13);
         this.labelStatusLabel.TabIndex = 1;
         this.labelStatusLabel.Text = "Status";
         // 
         // labelStatus
         // 
         this.labelStatus.AutoSize = true;
         this.labelStatus.Location = new System.Drawing.Point(8, 32);
         this.labelStatus.Name = "labelStatus";
         this.labelStatus.Size = new System.Drawing.Size(10, 13);
         this.labelStatus.TabIndex = 2;
         this.labelStatus.Text = "-";
         // 
         // labelExecutionTimeLabel
         // 
         this.labelExecutionTimeLabel.AutoSize = true;
         this.labelExecutionTimeLabel.Location = new System.Drawing.Point(8, 56);
         this.labelExecutionTimeLabel.Name = "labelExecutionTimeLabel";
         this.labelExecutionTimeLabel.Size = new System.Drawing.Size(76, 13);
         this.labelExecutionTimeLabel.TabIndex = 4;
         this.labelExecutionTimeLabel.Text = "Execution time";
         // 
         // labelExecutionTime
         // 
         this.labelExecutionTime.AutoSize = true;
         this.labelExecutionTime.Location = new System.Drawing.Point(8, 72);
         this.labelExecutionTime.Name = "labelExecutionTime";
         this.labelExecutionTime.Size = new System.Drawing.Size(10, 13);
         this.labelExecutionTime.TabIndex = 5;
         this.labelExecutionTime.Text = "-";
         this.labelExecutionTime.Click += new System.EventHandler(this.labelExecutionTime_Click);
         // 
         // timer
         // 
         this.timer.Interval = 1000;
         this.timer.Tick += new System.EventHandler(this.timer_Tick);
         // 
         // columnHeader1
         // 
         this.columnHeader1.Text = "File name";
         this.columnHeader1.Width = 600;
         // 
         // listProcess
         // 
         this.listProcess.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                     | System.Windows.Forms.AnchorStyles.Left)
                     | System.Windows.Forms.AnchorStyles.Right)));
         this.listProcess.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1});
         this.listProcess.Location = new System.Drawing.Point(8, 112);
         this.listProcess.Name = "listProcess";
         this.listProcess.Size = new System.Drawing.Size(785, 144);
         this.listProcess.TabIndex = 0;
         this.listProcess.UseCompatibleStateImageBehavior = false;
         this.listProcess.View = System.Windows.Forms.View.Details;
         // 
         // labelSkipped
         // 
         this.labelSkipped.AutoSize = true;
         this.labelSkipped.Location = new System.Drawing.Point(8, 96);
         this.labelSkipped.Name = "labelSkipped";
         this.labelSkipped.Size = new System.Drawing.Size(194, 13);
         this.labelSkipped.TabIndex = 7;
         this.labelSkipped.Text = "The following items have been skipped.";
         // 
         // ucProgress
         // 
         this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
         this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
         this.Controls.Add(this.labelSkipped);
         this.Controls.Add(this.labelExecutionTime);
         this.Controls.Add(this.labelExecutionTimeLabel);
         this.Controls.Add(this.labelStatus);
         this.Controls.Add(this.labelStatusLabel);
         this.Controls.Add(this.listProcess);
         this.Name = "ucProgress";
         this.Size = new System.Drawing.Size(800, 262);
         this.ResumeLayout(false);
         this.PerformLayout();

      }

      #endregion

      private System.Windows.Forms.Label labelStatusLabel;
      private System.Windows.Forms.Label labelStatus;
      private System.Windows.Forms.Label labelExecutionTimeLabel;
      private System.Windows.Forms.Label labelExecutionTime;
      private System.Windows.Forms.Timer timer;
      private System.Windows.Forms.ColumnHeader columnHeader1;
      private System.Windows.Forms.ListView listProcess;
      private System.Windows.Forms.Label labelSkipped;
   }
}
