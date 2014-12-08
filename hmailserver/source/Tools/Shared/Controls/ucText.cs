// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace hMailServer.Shared
{
   public partial class ucText : TextBox, IPropertyEditor
   {
      private string internalText;
      private bool _numeric;
      public ucText()
      {
         internalText = "";
         _numeric = false;

      }

      public new string Text
      {
         get
         {
            return base.Text.Trim();
         }

         set
         {
            base.Text = value;
            internalText = value;
         }
      }

      public int Number
      {
         get
         {
            if (_numeric == false)
               return 0;

            if (Text == "")
               return 0;

            return Convert.ToInt32(Text);
         }
         set
         {
            if (_numeric == false)
               return;

            Text = value.ToString();
         }
      }

      public Int64 Number64
      {
         get
         {
            if (_numeric == false)
               return 0;

            if (Text == "")
               return 0;

            return Convert.ToInt64(Text);
         }
         set
         {
            if (_numeric == false)
               return;

            Text = value.ToString();
         }
      }

      public bool Dirty
      {
         get
         {
            if (base.Text != internalText)
               return true;
            else
               return false;

         }
      }

      public void SetClean()
      {
         internalText = base.Text;
      }

      public bool Numeric
      {
         get
         {
            return _numeric;
         }
         set
         {
            _numeric = value;
         }
      }

      protected override void OnKeyPress(KeyPressEventArgs e)
      {
         if (_numeric)
         {
            if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar))
            {
               e.Handled = true;
            }
         }

         base.OnKeyPress(e);
      }

   }
}
