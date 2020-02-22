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
   public partial class ucComboBox : ComboBox, IPropertyEditor
   {
      private string _internalText;
      List<object> _keys;
      public ucComboBox()
      {
         _internalText = "";
         _keys = new List<object>();
      }

      public void AddItem(string title, object obj)
      {
         base.Items.Add(title);
         _keys.Add(obj);
      }

      public void AddItems(Dictionary<string, object> items)
      {
         foreach (var item in items)
         {
            base.Items.Add(item.Key);
            _keys.Add(item.Value);
         }
      }

      public new object SelectedValue
      {
         set
         {
            int index = 0;
            foreach(object o in _keys)
            {
               if (o.Equals(value))
               {
                  base.SelectedIndex = index;
                  _internalText = base.Text;

                  break;
               }

               index++;
            }
         }
         get
         {
            int index = base.SelectedIndex;

            if (index == -1)
               return null;

            return _keys[index];
         }
      }

      public new string Text
      {
         get
         { 
            return base.Text;
         }

         set
         {
            base.Text = value;
            _internalText = value;
         }
      }

      public bool Dirty
      {
         get
         {
            if (base.Text != _internalText)
               return true;
            else
               return false;

         }
      }

      public void SetClean()
      {
         _internalText = base.Text;
      }
   }
}
