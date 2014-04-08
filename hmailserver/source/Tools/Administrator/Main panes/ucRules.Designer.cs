namespace hMailServer.Administrator
{
    partial class ucRules
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
           this.buttonEditRule = new System.Windows.Forms.Button();
           this.buttonDeleteRule = new System.Windows.Forms.Button();
           this.buttonAddRule = new System.Windows.Forms.Button();
           this.listRules = new hMailServer.Administrator.ucListView();
           this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
           this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
           this.buttonMoveUp = new System.Windows.Forms.Button();
           this.buttonMoveDown = new System.Windows.Forms.Button();
           this.SuspendLayout();
           // 
           // buttonEditRule
           // 
           this.buttonEditRule.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonEditRule.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonEditRule.Location = new System.Drawing.Point(473, 39);
           this.buttonEditRule.Name = "buttonEditRule";
           this.buttonEditRule.Size = new System.Drawing.Size(100, 25);
           this.buttonEditRule.TabIndex = 54;
           this.buttonEditRule.Text = "&Edit...";
           this.buttonEditRule.UseVisualStyleBackColor = true;
           this.buttonEditRule.Click += new System.EventHandler(this.buttonEditRule_Click);
           // 
           // buttonDeleteRule
           // 
           this.buttonDeleteRule.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonDeleteRule.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonDeleteRule.Location = new System.Drawing.Point(473, 70);
           this.buttonDeleteRule.Name = "buttonDeleteRule";
           this.buttonDeleteRule.Size = new System.Drawing.Size(100, 25);
           this.buttonDeleteRule.TabIndex = 53;
           this.buttonDeleteRule.Text = "Remove";
           this.buttonDeleteRule.UseVisualStyleBackColor = true;
           this.buttonDeleteRule.Click += new System.EventHandler(this.buttonDeleteRule_Click);
           // 
           // buttonAddRule
           // 
           this.buttonAddRule.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonAddRule.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonAddRule.Location = new System.Drawing.Point(473, 8);
           this.buttonAddRule.Name = "buttonAddRule";
           this.buttonAddRule.Size = new System.Drawing.Size(100, 25);
           this.buttonAddRule.TabIndex = 52;
           this.buttonAddRule.Text = "&Add...";
           this.buttonAddRule.UseVisualStyleBackColor = true;
           this.buttonAddRule.Click += new System.EventHandler(this.buttonAddRule_Click);
           // 
           // listRules
           // 
           this.listRules.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                       | System.Windows.Forms.AnchorStyles.Left)
                       | System.Windows.Forms.AnchorStyles.Right)));
           this.listRules.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
           this.listRules.FullRowSelect = true;
           this.listRules.Location = new System.Drawing.Point(0, 0);
           this.listRules.Name = "listRules";
           this.listRules.Size = new System.Drawing.Size(464, 352);
           this.listRules.TabIndex = 55;
           this.listRules.UseCompatibleStateImageBehavior = false;
           this.listRules.View = System.Windows.Forms.View.Details;
           this.listRules.SelectedIndexChanged += new System.EventHandler(this.listRules_SelectedIndexChanged);
           this.listRules.DoubleClick += new System.EventHandler(this.listRules_DoubleClick);
           // 
           // columnHeader1
           // 
           this.columnHeader1.Text = "Name";
           this.columnHeader1.Width = 150;
           // 
           // columnHeader2
           // 
           this.columnHeader2.Text = "Enabled";
           this.columnHeader2.Width = 100;
           // 
           // buttonMoveUp
           // 
           this.buttonMoveUp.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonMoveUp.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonMoveUp.Location = new System.Drawing.Point(473, 112);
           this.buttonMoveUp.Name = "buttonMoveUp";
           this.buttonMoveUp.Size = new System.Drawing.Size(100, 25);
           this.buttonMoveUp.TabIndex = 57;
           this.buttonMoveUp.Text = "Move up";
           this.buttonMoveUp.UseVisualStyleBackColor = true;
           this.buttonMoveUp.Click += new System.EventHandler(this.buttonMoveUp_Click);
           // 
           // buttonMoveDown
           // 
           this.buttonMoveDown.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
           this.buttonMoveDown.DialogResult = System.Windows.Forms.DialogResult.OK;
           this.buttonMoveDown.Location = new System.Drawing.Point(473, 143);
           this.buttonMoveDown.Name = "buttonMoveDown";
           this.buttonMoveDown.Size = new System.Drawing.Size(100, 25);
           this.buttonMoveDown.TabIndex = 56;
           this.buttonMoveDown.Text = "Move down";
           this.buttonMoveDown.UseVisualStyleBackColor = true;
           this.buttonMoveDown.Click += new System.EventHandler(this.buttonMoveDown_Click);
           // 
           // ucRules
           // 
           this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
           this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
           this.Controls.Add(this.buttonMoveUp);
           this.Controls.Add(this.buttonMoveDown);
           this.Controls.Add(this.listRules);
           this.Controls.Add(this.buttonEditRule);
           this.Controls.Add(this.buttonDeleteRule);
           this.Controls.Add(this.buttonAddRule);
           this.Name = "ucRules";
           this.Size = new System.Drawing.Size(584, 355);
           this.ResumeLayout(false);

        }

        #endregion

       private System.Windows.Forms.Button buttonEditRule;
       private System.Windows.Forms.Button buttonDeleteRule;
       private System.Windows.Forms.Button buttonAddRule;
       private ucListView listRules;
       private System.Windows.Forms.ColumnHeader columnHeader1;
       private System.Windows.Forms.ColumnHeader columnHeader2;
       private System.Windows.Forms.Button buttonMoveUp;
       private System.Windows.Forms.Button buttonMoveDown;

     }
}
