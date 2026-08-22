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
       private bool _loading;

       public ucPasswordHashing()
        {
            InitializeComponent();

            comboAlgorithm.AddItem("Argon2id", hMailServer.ePasswordHashAlgorithm.ePWHashArgon2id);
            comboAlgorithm.AddItem("PBKDF2-SHA256", hMailServer.ePasswordHashAlgorithm.ePWHashPBKDF2SHA256);

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

           // Selecting the algorithm normally replaces the cost with the defaults for
           // that algorithm. While loading, the stored cost is what we want to show.
           _loading = true;

           try
           {
              comboAlgorithm.SelectedValue = settings.PasswordHashAlgorithm;
              textMemoryCost.Number = settings.PasswordHashMemoryCost;
              textIterations.Number = settings.PasswordHashIterations;
              checkAutoUpgrade.Checked = settings.PasswordHashAutoUpgradeEnabled;
           }
           finally
           {
              _loading = false;
           }

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

        private hMailServer.ePasswordHashAlgorithm GetSelectedAlgorithm()
        {
           object selected = comboAlgorithm.SelectedValue;

           if (selected == null)
              return hMailServer.ePasswordHashAlgorithm.ePWHashArgon2id;

           return (hMailServer.ePasswordHashAlgorithm) selected;
        }

        private void EnableDisable()
        {
           // The memory cost only means something to Argon2id.
           textMemoryCost.Enabled = GetSelectedAlgorithm() == hMailServer.ePasswordHashAlgorithm.ePWHashArgon2id;
        }

        private void comboAlgorithm_SelectedIndexChanged(object sender, EventArgs e)
        {
           if (_loading)
              return;

           // The cost parameters mean entirely different things to the two algorithms,
           // so whatever suited the previous one no longer applies to the one now
           // selected. Reset both to 0, which the server resolves to the recommended
           // default for whichever algorithm ends up configured.
           textMemoryCost.Number = 0;
           textIterations.Number = 0;
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
