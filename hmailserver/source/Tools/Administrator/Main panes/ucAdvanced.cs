// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucAdvanced : UserControl, ISettingsControl
    {
        public ucAdvanced()
        {
            InitializeComponent();

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }

        public void OnLeavePage()
        {

        }

        private void OnContentChanged()
        {
            Instances.MainForm.OnContentChanged();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
            OnContentChanged();
        }

        public bool Dirty
        {
            get
            {
                return DirtyChecker.IsDirty(this);
            }
        }

        public void LoadData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;

            textDefaultDomain.Text = settings.DefaultDomain;

            Marshal.ReleaseComObject(settings);

        }

        public bool SaveData()
        {
            hMailServer.Settings settings = APICreator.Application.Settings;

            settings.DefaultDomain = textDefaultDomain.Text;

            if (textAdministratorPassword.Dirty)
            {
                string newPassword = textAdministratorPassword.Text;

                settings.SetAdministratorPassword(newPassword);
            }

            DirtyChecker.SetClean(this);

            Marshal.ReleaseComObject(settings);

            return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }
    }
}
