// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace hMailServer.Administrator.Utilities
{
   internal class FocusChanger
   {
      public static void SetFocus(Control.ControlCollection controls)
      {
         if (controls.Count == 0)
            return;
         
         Control topMost = controls[0];
         foreach(Control control in controls)
         {
            if (control.GetType().ToString() == "System.Windows.Forms.Label")
               continue;

            if (control.Top < topMost.Top)  
            {
               topMost = control;
            }
         }

         topMost.Focus();

      }
   }
}
