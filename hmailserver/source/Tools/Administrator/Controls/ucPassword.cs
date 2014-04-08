// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Windows.Forms;
using hMailServer.Shared;

namespace hMailServer.Administrator.Controls
{
   public partial class ucPassword : TextBox, IPropertyEditor
   {
      private string _enteredPassword;

      private const string EncryptedMarker = "<< Encrypted >>";
      private const char passwordChar = '*';

      private bool _dirty;

      public ucPassword()
      {
         _enteredPassword = "";

         base.Text = EncryptedMarker;

         _dirty = false;
      }

      public bool Dirty
      {
         get
         {
            return _dirty;
         }
      }

      public void SetClean()
      {
         _enteredPassword = "";

         _dirty = false;
      }

      public string Password
      {
         get
         {
            string retVal = "";
            if (this.PasswordChar == '*')
            {
               // Currently in editing mode. Fetch value
               // from text property.
               retVal = this.Text;
            }
            else
            {
               // Not in editing mode. Fetch value from the
               // previously set text.
               retVal = _enteredPassword;
            }

            return retVal;

         }
      }

      protected override void OnLeave(EventArgs e)
      {
         _enteredPassword = this.Text;

         this.Text = EncryptedMarker;
         this.PasswordChar = '\0';
      }

      protected override void OnEnter(EventArgs e)
      {
         _dirty = true;

         this.Text = "";
         this.PasswordChar = passwordChar;
      }

      protected override void  OnTextChanged(EventArgs e)
 	   {
         _dirty = true;
         base.OnTextChanged(e);
      }
  }
}
