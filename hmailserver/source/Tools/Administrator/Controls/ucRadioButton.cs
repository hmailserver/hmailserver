// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Windows.Forms;
using hMailServer.Shared;

namespace hMailServer.Administrator.Controls
{
    public partial class ucRadioButton : RadioButton, IPropertyEditor
    {
        private bool internalChecked;

        public ucRadioButton()
        {
           
            internalChecked = false;
        }

        public new bool Checked
        {
            get
            {
                return base.Checked;
            }

            set
            {
                base.Checked = value;
                internalChecked = value;
            }
        }

        public bool Dirty
        {
            get
            {
                if (base.Checked != internalChecked)
                    return true;
                else
                    return false;

            }
        }

        public void SetClean()
        {
            internalChecked = base.Checked;
        }

    }
}

