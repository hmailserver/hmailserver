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

       // Must match PasswordHasher::Constants in the server.
       private const int DefaultArgon2idMemoryCost = 19456;
       private const int DefaultArgon2idIterations = 2;
       private const int DefaultPbkdf2Iterations = 600000;

       private bool _loading;

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

        private void comboAlgorithm_SelectedIndexChanged(object sender, EventArgs e)
        {
           if (_loading)
              return;

           // The cost parameters mean entirely different things to the two algorithms,
           // so whatever suited the previous one is replaced by the defaults for the
           // one now selected. PBKDF2-SHA256 has no memory cost at all.
           if (GetSelectedAlgorithm() == AlgorithmPbkdf2Sha256)
           {
              textMemoryCost.Number = 0;
              textIterations.Number = DefaultPbkdf2Iterations;
           }
           else
           {
              textMemoryCost.Number = DefaultArgon2idMemoryCost;
              textIterations.Number = DefaultArgon2idIterations;
           }
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
