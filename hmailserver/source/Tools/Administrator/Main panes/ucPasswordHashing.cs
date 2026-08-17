// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
    public partial class ucPasswordHashing : UserControl, ISettingsControl
    {
       private const int AlgorithmArgon2id = 1;
       private const int AlgorithmPbkdf2Sha256 = 2;

       public ucPasswordHashing()
        {
            InitializeComponent();

            comboAlgorithm.AddItem("Argon2id", AlgorithmArgon2id);
            comboAlgorithm.AddItem("PBKDF2-SHA256", AlgorithmPbkdf2Sha256);

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
              return DirtyChecker.IsDirty(this);
           }
        }

        public void LoadData()
        {
           hMailServer.Application app = APICreator.Application;

           hMailServer.Settings settings = app.Settings;

           comboAlgorithm.SelectedValue = settings.PasswordHashAlgorithm;
           textMemoryCost.Number = settings.PasswordHashMemoryCost;
           textIterations.Number = settings.PasswordHashIterations;
           checkAutoUpgrade.Checked = settings.PasswordHashAutoUpgradeEnabled;

           Marshal.ReleaseComObject(settings);

           EnableDisable();
        }

        public bool SaveData()
        {
           hMailServer.Application app = APICreator.Application;

           hMailServer.Settings settings = app.Settings;

           bool restartRequired = comboAlgorithm.Dirty || textMemoryCost.Dirty || textIterations.Dirty;

           settings.PasswordHashAlgorithm = GetSelectedAlgorithm();
           settings.PasswordHashMemoryCost = textMemoryCost.Number;
           settings.PasswordHashIterations = textIterations.Number;
           settings.PasswordHashAutoUpgradeEnabled = checkAutoUpgrade.Checked;

           Marshal.ReleaseComObject(settings);

           if (restartRequired)
              Utility.AskRestartServer();

           DirtyChecker.SetClean(this);
           return true;

       }

        public void LoadResources()
        {
            // load the translated resources
        }

        private int GetSelectedAlgorithm()
        {
           object selected = comboAlgorithm.SelectedValue;

           if (selected == null)
              return AlgorithmArgon2id;

           return (int) selected;
        }

        private void EnableDisable()
        {
           // The memory cost only means something to Argon2id.
           textMemoryCost.Enabled = GetSelectedAlgorithm() == AlgorithmArgon2id;
        }

        private void OnContentChanged()
        {
           Instances.MainForm.OnContentChanged();

           EnableDisable();
        }

        private void OnContentChanged(object sender, EventArgs e)
        {
           OnContentChanged();
        }
    }
}
