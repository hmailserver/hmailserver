// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Shared;

namespace hMailServer.Administrator.Controls
{
   public partial class ucEmailEdit : UserControl, IPropertyEditor
   {
      private string internalText;

      public event EventHandler ContentChanged;

      public ucEmailEdit()
      {
         InitializeComponent();

         internalText = "";
      }

      public new string Text
      {
         get
         {

            if (textMailbox.Text == "")
               return "";

            return textMailbox.Text.Trim() + "@" + textDomainName.Text.Trim();
         }

         set
         {
            int atPos = value.IndexOf("@");
            if (atPos == -1)
               return;

            string before = value.Substring(0, atPos);
            string after = value.Substring(atPos+1);

            textMailbox.Text = before;
            textDomainName.Text = after;

            internalText = value;
         }
      }

      public bool Valid
      {
         get
         {
            return textMailbox.Text.Trim().Length > 0 &&
                   textDomainName.Text.Trim().Length > 0;
         }
      }

      public bool Dirty
      {
         get
         {
            if (this.Text != internalText)
               return true;
            else
               return false;

         }
      }

      public void SetClean()
      {
         internalText = this.Text;
      }

      private void textMailbox_TextChanged(object sender, EventArgs e)
      {
         if (textMailbox.Text.EndsWith("@"))
         {
            textMailbox.Text = textMailbox.Text.Substring(0, textMailbox.Text.Length - 1);
            textDomainName.Focus();
         }

         if (ContentChanged != null)
            ContentChanged(sender, e);
      }

      private void textDomainName_TextChanged(object sender, EventArgs e)
      {
         if (ContentChanged != null)
            ContentChanged(sender, e);
      }

      public bool ReadOnlyHost
      {
         set 
         {
            textDomainName.Enabled = !value;
         }
         get
         {
            return !textDomainName.Enabled;
         }
      }

   }
}
