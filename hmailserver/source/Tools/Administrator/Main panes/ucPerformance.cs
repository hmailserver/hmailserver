// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;
using System.Runtime.InteropServices;

using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class ucPerformance : UserControl, ISettingsControl
   {
      public ucPerformance()
      {
         InitializeComponent();

         DirtyChecker.SubscribeToChange(this, OnContentChanged);

         comboWorkerThreadPriority.AddItem("Highest", 2);
         comboWorkerThreadPriority.AddItem("Above normal", 1);
         comboWorkerThreadPriority.AddItem("Normal", 0);
         comboWorkerThreadPriority.AddItem("Below normal", -1);
         comboWorkerThreadPriority.AddItem("Lowest", -2);
         comboWorkerThreadPriority.AddItem("Idle", -15);

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
      }

      public void OnLeavePage()
      {
          timerRefreshHitRate.Enabled = false;
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
         hMailServer.Cache cache = settings.Cache;

         checkEnabled.Checked = cache.Enabled;

         textCacheDomainTTL.Number = cache.DomainCacheTTL;
         textCacheAccountTTL.Number = cache.AccountCacheTTL;
         textCacheAliasTTL.Number = cache.AliasCacheTTL;
         textCacheDistributionListTTL.Number = cache.DistributionListCacheTTL;

         textTCPIPThreads.Number = settings.TCPIPThreads;
         textDeliveryThreads.Number = settings.MaxDeliveryThreads;
         textMaxAsynchronousThreads.Number = settings.MaxAsynchronousThreads;

         comboWorkerThreadPriority.SelectedValue = settings.WorkerThreadPriority;

         hMailServer.MessageIndexing indexing = settings.MessageIndexing;

         checkMessageIndexingEnabled.Checked = indexing.Enabled;

         Marshal.ReleaseComObject(indexing);
         Marshal.ReleaseComObject(settings);
         Marshal.ReleaseComObject(cache);

         EnableDisable();
      }

      private void EnableDisable()
      {
         textCacheDomainTTL.Enabled = checkEnabled.Checked;
         textCacheAccountTTL.Enabled = checkEnabled.Checked;
         textCacheAliasTTL.Enabled = checkEnabled.Checked;
         textCacheDistributionListTTL.Enabled = checkEnabled.Checked;
      }

      public bool SaveData()
      {
         hMailServer.Settings settings = APICreator.Application.Settings;
         hMailServer.Cache cache = settings.Cache;
         hMailServer.MessageIndexing indexing = settings.MessageIndexing;

         cache.Enabled = checkEnabled.Checked;

         cache.DomainCacheTTL = textCacheDomainTTL.Number;
         cache.AccountCacheTTL = textCacheAccountTTL.Number;
         cache.AliasCacheTTL = textCacheAliasTTL.Number;
         cache.DistributionListCacheTTL = textCacheDistributionListTTL.Number;

         settings.TCPIPThreads = textTCPIPThreads.Number;
         settings.MaxDeliveryThreads = textDeliveryThreads.Number;
         settings.MaxAsynchronousThreads = textMaxAsynchronousThreads.Number;

         settings.WorkerThreadPriority = (int)comboWorkerThreadPriority.SelectedValue;

         indexing.Enabled = checkMessageIndexingEnabled.Checked;

         DirtyChecker.SetClean(this);

         Marshal.ReleaseComObject(settings);
         Marshal.ReleaseComObject(cache);
         Marshal.ReleaseComObject(indexing);

         return true;

      }

      public void LoadResources()
      {
         // load the translated resources
      }

      private void checkEnabled_CheckedChanged(object sender, EventArgs e)
      {
         EnableDisable();
      }

      private void timerRefreshHitRate_Tick(object sender, EventArgs e)
      {
         // No reason to refresh stats if we aren't looking at the cache page.
         if (tabControl.SelectedTab.Name != "tabPageCache")
            return;

         hMailServer.Settings settings = APICreator.Application.Settings;
         hMailServer.Cache cache = settings.Cache;

         labelDomainHitRate.Text = cache.DomainHitRate.ToString();
         labelAccountHitRate.Text = cache.AccountHitRate.ToString();
         labelAliasHitRate.Text = cache.AliasHitRate.ToString();
         labelDistributionListHitRate.Text = cache.DistributionListHitRate.ToString();
                   Marshal.ReleaseComObject(settings);
         Marshal.ReleaseComObject(cache);
      }

      private void tabControl_SelectedIndexChanged(object sender, EventArgs e)
      {
         if (tabControl.SelectedTab.Name == "tabMessageIndexing")
            DisplayMessageIndexingStatus();
      }

      private void DisplayMessageIndexingStatus()
      {
         using (new WaitCursor())
         {
            hMailServer.Settings settings = APICreator.Application.Settings;
            hMailServer.MessageIndexing indexing = settings.MessageIndexing;

            int totalMessageCount = indexing.TotalMessageCount;
            int totalIndexCount = indexing.TotalIndexedCount;

            if (totalIndexCount > totalMessageCount)
               totalIndexCount = totalMessageCount;

            messageIndexingProgressBar.Maximum = totalMessageCount;
            messageIndexingProgressBar.Value = totalIndexCount;

            messageIndexingStatusText.Text = string.Format("{0} / {1}", totalIndexCount, totalMessageCount);

            Marshal.ReleaseComObject(indexing);
            Marshal.ReleaseComObject(settings);
         }
      }

      private void buttonMessageIndexingStatusRefresh_Click(object sender, EventArgs e)
      {
         DisplayMessageIndexingStatus();
      }

      private void buttonMessageIndexingClear_Click(object sender, EventArgs e)
      {
         if (MessageBox.Show(Strings.Localize("Are you sure you want to clear the message index?"), 
                             EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNo, 
                             MessageBoxIcon.Question) == DialogResult.Yes)
         {
            hMailServer.Settings settings = APICreator.Application.Settings;
            hMailServer.MessageIndexing indexing = settings.MessageIndexing;

            indexing.Clear();

            Marshal.ReleaseComObject(indexing);
            Marshal.ReleaseComObject(settings);

            DisplayMessageIndexingStatus();
         }
      }
   }
}
