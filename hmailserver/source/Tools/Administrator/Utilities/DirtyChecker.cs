// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using hMailServer.Administrator.Controls;
using System.Windows.Forms;
using hMailServer.Shared;

namespace hMailServer.Administrator.Utilities
{
   class DirtyChecker
   {

      // Somewhat ugly but...
      private enum Action
      {
         CheckDirty = 1,
         SetClean = 2,
         SubscribeToChange = 3
      }

      public static bool IsDirty(Control parentControl)
      {
         return IterateControls(parentControl, Action.CheckDirty, null);
      }

      public static void SetClean(Control parentControl)
      {
         IterateControls(parentControl, Action.SetClean, null);
      }

      public static void SubscribeToChange(Control parentControl, EventHandler ev)
      {
         IterateControls(parentControl, Action.SubscribeToChange, ev);
      }

      private static bool IterateControls(Control parentControl, Action action, EventHandler ev)
      {
         foreach (Control c in parentControl.Controls)
         {
            if (c.GetType() == typeof(System.Windows.Forms.TabControl))
            {
               TabControl tabControl = c as TabControl;

               foreach (TabPage tp in tabControl.TabPages)
               {
                  if (IterateControls(tp, action, ev))
                     return true;
               }
            }

            if (c.GetType() == typeof(System.Windows.Forms.Panel))
            {
                Panel panel = c as Panel;

                if (IterateControls(panel, action, ev))
                    return true;
            }

            System.Type editorType = c.GetType();
            if (editorType == typeof(ucText) ||
                editorType == typeof(ucRadioButton) ||
                editorType == typeof(ucCheckbox) ||
                editorType == typeof(ucComboBox) ||
                editorType == typeof(ucEmailEdit) ||
                editorType == typeof(ucDateTimePicker) ||
                editorType == typeof(ucPassword) ||
                editorType == typeof(ucIPAddress))
            {
               IPropertyEditor editor = c as IPropertyEditor;

               if (action == Action.CheckDirty)
               {
                  if (editor.Dirty)
                  {
                     return true;
                  }
               }
               else if (action == Action.SetClean)
               {
                  editor.SetClean();
               }
               else if (action == Action.SubscribeToChange)
               {
                  
                  if (editorType == typeof(ucText))
                  {
                     ucText edit = c as ucText;
                     edit.TextChanged += ev;
                  }
                  else if (editorType == typeof(ucCheckbox))
                  {
                     ucCheckbox edit = c as ucCheckbox;
                     edit.CheckedChanged += ev;
                  }
                  else if (editorType == typeof(ucComboBox))
                  {
                     ucComboBox edit = c as ucComboBox;
                     edit.SelectedIndexChanged += ev;
                  }
                  else if (editorType == typeof(ucRadioButton))
                  {
                     ucRadioButton edit = c as ucRadioButton;
                     edit.CheckedChanged += ev;
                  }
                  else if (editorType == typeof(ucEmailEdit))
                  {
                     ucEmailEdit edit = c as ucEmailEdit;
                     edit.ContentChanged += ev;
                  }
                  else if (editorType == typeof(ucDateTimePicker))
                  {
                     ucDateTimePicker dateTimePicker = c as ucDateTimePicker;
                     dateTimePicker.ValueChanged += ev;
                  }
                  else if (editorType == typeof(ucPassword))
                  {
                     ucPassword passwordEdit = c as ucPassword;
                     passwordEdit.TextChanged += ev;
                  }
                  else if (editorType == typeof(ucIPAddress))
                  {
                     ucIPAddress passwordEdit = c as ucIPAddress;
                     passwordEdit.TextChanged += ev;
                  }
               }
            }
         }

         return false;
      }

   }
}
