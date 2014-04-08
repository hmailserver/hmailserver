// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using hMailServer.Shared;

namespace hMailServer.Administrator.Controls
{
   public partial class ucIPAddress : TextBox, IPropertyEditor
   {
      private string internalText;

      public ucIPAddress()
      {
         internalText = "";
      }

      public new string Text
      {
         get
         {
            return base.Text;
         }

         set
         {
            internalText = value;
            base.Text = value;
         }
      }

      public void Validate()
      {
         if (internalText.Length == 0)
         {
            // TODO: Show message.  
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

      
   }
}
