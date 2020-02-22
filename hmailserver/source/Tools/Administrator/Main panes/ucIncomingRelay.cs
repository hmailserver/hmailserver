// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Nodes;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucIncomingRelay : UserControl, ISettingsControl
    {
        hMailServer.IncomingRelay _representedObject;

        public ucIncomingRelay(int IncomingRelayID)
        {
            InitializeComponent();

            if (IncomingRelayID > 0)
            {
                hMailServer.Settings settings = APICreator.Settings;
                hMailServer.IncomingRelays IncomingRelays = settings.IncomingRelays;
                _representedObject = IncomingRelays.get_ItemByDBID(IncomingRelayID);
                Marshal.ReleaseComObject(IncomingRelays);
                Marshal.ReleaseComObject(settings);
            }

            DirtyChecker.SubscribeToChange(this, OnContentChanged);

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
        }


        public void OnLeavePage()
        {

        }

        public bool Dirty
        {
            get
            {
                return DirtyChecker.IsDirty(this) &&
                       textName.Text.Length > 0;
            }
        }

        public void LoadData()
        {
            if (_representedObject == null)
                return;

            textName.Text = _representedObject.Name;
            textLower.Text = _representedObject.LowerIP;
            textUpper.Text = _representedObject.UpperIP;
        }

        public bool SaveData()
        {
            bool newObject = false;
            if (_representedObject == null)
            {
                hMailServer.Application app = APICreator.Application;
                hMailServer.Settings settings = app.Settings;
                hMailServer.IncomingRelays IncomingRelays = settings.IncomingRelays;
                _representedObject = IncomingRelays.Add();

                newObject = true;

                Marshal.ReleaseComObject(settings);
                Marshal.ReleaseComObject(IncomingRelays);
            }

            _representedObject.Name = textName.Text;

            _representedObject.LowerIP = textLower.Text;
            _representedObject.UpperIP = textUpper.Text;

            _representedObject.Save();

            // Refresh the node in the tree if the name has changed.
            IMainForm mainForm = Instances.MainForm;
            mainForm.RefreshCurrentNode(textName.Text);

            // Set the object to clean.
            DirtyChecker.SetClean(this);

            if (newObject)
            {
                SearchNodeText crit = new SearchNodeText(_representedObject.Name);
                mainForm.SelectNode(crit);
            }

            return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }


        private void OnContentChanged()
        {
            Instances.MainForm.OnContentChanged();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
            OnContentChanged();
        }

    }
}
