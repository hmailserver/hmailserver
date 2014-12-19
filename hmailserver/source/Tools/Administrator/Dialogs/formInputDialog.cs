// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Shared;
namespace hMailServer.Administrator
{
   public partial class formInputDialog : Form
   {
      public formInputDialog()
      {
         InitializeComponent();

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
         Strings.Localize(this);

         EnableDisable();
      }

      public string Title
      {
         set
         {
            base.Text = value;
         }
         get
         {
            return base.Text;
         }
      }

      public new string Text
      {
         set
         {
            labelText.Text = value;
         }
         get
         {
            return labelText.Text;
         }
      }

      public string Value
      {
         set
         {
            textValue.Text = value;

            EnableDisable();
         }
         get
         {
            return textValue.Text.Trim();
         }
      }

      private void btnOK_Click(object sender, EventArgs e)
      {
         
      }

      private void textValue_TextChanged(object sender, EventArgs e)
      {
         EnableDisable();
      }

      private void EnableDisable()
      {
         btnOK.Enabled = textValue.Text.Trim().Length > 0;
      }
  

   }
}