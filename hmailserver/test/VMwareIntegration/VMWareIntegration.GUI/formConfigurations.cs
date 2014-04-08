// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;
using System.Diagnostics;
using VMwareIntegration.Common;


namespace VMwareIntegration
{
   public partial class formConfigurations : Form
   {
      private Thread _runThread;
      private ListViewItem _currentItem;

      private DateTime? _testStartTime = null;


      public delegate void delegateTestCompleted(bool success, string message, string failureText);
      public delegateTestCompleted TestCompleted;

      public formConfigurations()
      {
         InitializeComponent();
         _currentItem = null;

         ShowTestInfo();

         SetTestRunning(false);

         TestCompleted = new delegateTestCompleted(this.OnInternalOnTestCompleted);
      }

      private void ShowTestInfo()
      {
         ListViewItem item = listStatus.Items.Add("Tester");
         item.SubItems.Add(System.Reflection.Assembly.GetExecutingAssembly().Location);

         item = listStatus.Items.Add("Test subject");
         item.SubItems.Add(TestSettings.GetSoftwareUnderTest());

         item = listStatus.Items.Add("Test fixture");
         item.SubItems.Add(TestSettings.GetFixturePath());
      }

      private void OnTestCompleted(bool success, string message)
      {
 
      }

      private int GetStatusColumn()
      {
         return listConfigurations.Columns.Count - 1;
      }

      private void ReportStatusForCurrentItem(string result, string failureText)
      {
         if (_currentItem != null)
         {
            _currentItem.SubItems[GetStatusColumn()].Text = result;

            if (failureText.Length > 0)
                _currentItem.SubItems[GetStatusColumn()].Text += " " + failureText;
         }
      }
      

      private void formConfigurations_Load(object sender, EventArgs e)
      {
         if (Process.GetProcessesByName(Process.GetCurrentProcess().ProcessName).Length > 1)
         {
            MessageBox.Show("Only one instance of this application can be started.", "VMwareIntegration", MessageBoxButtons.OK, MessageBoxIcon.Error);

            this.Close();
         }

         try
         {
             List<TestEnvironment> listEnvironments = new List<TestEnvironment>();
             TestEnvironments.AddAll(listEnvironments);


             foreach (TestEnvironment environment in listEnvironments)
             {
                 ListViewItem item = listConfigurations.Items.Add(environment.Description);
                 item.SubItems.Add(environment.OperatingSystem);
                 item.SubItems.Add(environment.DatabaseType);
                 item.SubItems.Add(environment.SnapshotName);
                 item.SubItems.Add(""); // Status
                 item.Tag = environment;
                 item.Checked = true;
             }
         }
         catch (Exception ex)
         {
             MessageBox.Show(ex.Message + Environment.NewLine + ex.StackTrace, "VMwareIntegration", MessageBoxButtons.OK, MessageBoxIcon.Error);

             this.Close();
         }
      }

      private void buttonClose_Click(object sender, EventArgs e)
      {
         this.Close();
      }
     
      /// <summary>
      ///    Locates the next test to run and starts it.
      /// </summary>
      private bool StartNextTest()
      {
         SetTestRunning(true);

         _testStartTime = DateTime.Now;

         foreach (ListViewItem item in listConfigurations.Items)
         {
            if (item.Checked)
            {
               _currentItem = item;

               ReportStatusForCurrentItem("Started " + DateTime.Now.ToShortTimeString(), "");

               TestEnvironment environment = item.Tag as TestEnvironment;
               TestRunner runner = new TestRunner(false, environment, checkStopOnError.Checked);

               runner.TestCompleted += new TestRunner.TestCompletedDelegate(runner_TestCompleted);
               _runThread = new Thread(new ThreadStart(runner.RunThread));
               _runThread.Name = "Test run thread";
               _runThread.Start();

               return true;
            }
         }

         // No test to run was found.
         _testStartTime = null;

         SetTestRunning(false);

         return false;
      }

      void runner_TestCompleted(bool result, string message, string failureText)
      {
         this.Invoke(this.TestCompleted, new Object[] { result, message, failureText });
      }

      public void OnInternalOnTestCompleted(bool success, string message, string failureText)
      {
         _currentItem.Checked = false;

         // Disable the automatic update.
         _testStartTime = null;

         ReportStatusForCurrentItem(message, failureText);

         _currentItem = null;

         if (!success && checkStopOnError.Checked)
            return;


         // Launch the next test!
         bool started = StartNextTest();

         if (!started && success && checkShutdownOnSuccess.Checked)
         {
            // No more tests to run. Bye.
            System.Diagnostics.Process.Start("shutdown.exe", "-s -f -t 00");
         }
      }

      private void buttonRunSelected_Click(object sender, EventArgs e)
      {
         if (MessageBox.Show("Is this the correct hMailServer installation program?" + Environment.NewLine + Environment.NewLine + Path.GetFileName(TestSettings.GetSoftwareUnderTest()), "Test subject", MessageBoxButtons.YesNoCancel) != DialogResult.Yes)
            return;
         
         StartNextTest();
      }

      private void buttonSelectAll_Click(object sender, EventArgs e)
      {
         SelectAll(true);

      }

      private void SelectAll(bool select)
      {
         foreach (ListViewItem item in listConfigurations.Items)
            item.Checked = select;
      }

      private void buttonSelectNone_Click(object sender, EventArgs e)
      {
         SelectAll(false);
      }

      private void listConfigurations_ItemCheck(object sender, ItemCheckEventArgs e)
      {
         listConfigurations.Items[e.Index].SubItems[GetStatusColumn()].Text = "";
      }

      private void checkStopOnError_CheckedChanged(object sender, EventArgs e)
      {

      }

      private void listView1_SelectedIndexChanged(object sender, EventArgs e)
      {
      
      }

      private void timer_Tick(object sender, EventArgs e)
      {
         int count = 0;
         foreach (ListViewItem item in listConfigurations.Items)
         {
            if (item.Checked)
               count++;
         }

         // Calculated elapsed time.
         TimeSpan ts = new TimeSpan();
         if (_testStartTime != null)
         {
            ts = DateTime.Now - (DateTime) _testStartTime;
         }

         string timeSpan = "Running: " + ts.Minutes.ToString("00") + ":" + ts.Seconds.ToString("00");

         this.Text = string.Format("Test [{0}]", count);

         ReportStatusForCurrentItem(timeSpan, "");

         
      }

      private void buttonStop_Click(object sender, EventArgs e)
      {
         StopTest();

      }

      private void StopTest()
      {
         _runThread.Abort();

         SetTestRunning(false);
      }

      private void SetTestRunning(bool running)
      {
         buttonRunSelected.Enabled = !running;
         buttonClose.Enabled = !running;
         buttonStop.Enabled = running;
      }

      private void formConfigurations_FormClosing(object sender, FormClosingEventArgs e)
      {
         if (buttonStop.Enabled)
         {
            if (MessageBox.Show("A test is currently running. Abort and exit?", "VMwareIntegration", MessageBoxButtons.YesNo)  == DialogResult.Yes)
            {
               StopTest();
            }
            else
            {
               e.Cancel = true;
            }
         }
      }

      private void formConfigurations_Shown(object sender, EventArgs e)
      {

      }

      

   }
}
