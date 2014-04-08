// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using System.Threading;
using System.Windows.Forms;
using Builder.Common;

namespace hMailServer_builder
{
   public delegate void DelegateBuildStepStarted(int iIndex);

   public delegate void DelegateBuildStepCompleted(int iIndex);

   public delegate void DelegateBuildStepError(int iIndex, string sError);

   public delegate void DelegateThreadFinished();

   public delegate void DelegateAddToLog(string sMessage, bool timestamp);

   public partial class frmMain : Form
   {
      private readonly ManualResetEvent m_EventStopThread;
      private readonly ManualResetEvent m_EventThreadStopped;
      public DelegateAddToLog m_AddToLog;
      public DelegateBuildStepCompleted m_DelegateBuildStepCompleted;
      public DelegateBuildStepError m_DelegateBuildStepError;
      public DelegateBuildStepStarted m_DelegateBuildStepStarted;
      public DelegateThreadFinished m_DelegateThreadFinished;
      private Thread m_WorkerThread;

      private DateTime m_dtStartTime;
      private Builder.Common.Builder m_oBuilder;

      public frmMain()
      {
         InitializeComponent();

         m_EventStopThread = new ManualResetEvent(false);
         m_EventThreadStopped = new ManualResetEvent(false);

         m_DelegateBuildStepStarted = OnBuildStepStarted;
         m_DelegateBuildStepError = OnBuildStepError;
         m_DelegateThreadFinished = OnThreadFinished;
         m_DelegateBuildStepCompleted = OnBuildStepCompleted;
         m_AddToLog = OnAddToLog;
      }

      private bool IsRunning
      {
         get { return cmdStart.Enabled == false; }
      }

      private void SaveSettings()
      {
         var settings = new Settings();
         settings.LoadSettings();

         settings.SourcePath = txtPathSource.Text;
         settings.BuildNumber = Convert.ToInt32(txtBuildNumber.Text);
         settings.Version = txtVersion.Text;
         settings.VSPath = txtPathVS8.Text;
         settings.InnoSetupPath = txtPathInnoSetup.Text;
         settings.GitPath = txtPathGit.Text;

         settings.SaveSettings();
      }

      private void LoadSettings()
      {
         var settings = new Settings();
         settings.LoadSettings();

         txtPathSource.Text = settings.SourcePath;
         txtPathVS8.Text = settings.VSPath;
         txtBuildNumber.Text = settings.BuildNumber.ToString();
         txtPathInnoSetup.Text = settings.InnoSetupPath;
         txtVersion.Text = settings.Version;
         txtPathGit.Text = settings.GitPath;

         string s = settings.BuildInstructions;

         var oBuildLoader = new BuildLoader();
         m_oBuilder = oBuildLoader.Load(s);

         m_oBuilder.MessageLog += m_oBuilder_MessageLog;

         string s2 = s;
      }

      private void m_oBuilder_MessageLog(bool timestamp, string message)
      {
         Invoke(m_AddToLog, new object[] {message, timestamp});
      }

      private void VisualizeBuild()
      {
         for (int i = 0; i < m_oBuilder.Count; i++)
         {
            BuildStep oStep = m_oBuilder.Get(i);

            ListViewItem lvwItem = lvwBuildSteps.Items.Add(oStep.Name);

            lvwItem.SubItems.Add("");
         }
      }

      private void frmMain_Load(object sender, EventArgs e)
      {
         if (!File.Exists("hMailServer builder.exe.config"))
         {
            MessageBox.Show("Could not load settings file: hMailServer builder.exe.config", "Initialization error");
            Close();
            return;
         }
         LoadSettings();


         VisualizeBuild();
      }

      private void cmdCLose_Click(object sender, EventArgs e)
      {
         if (cmdStart.Enabled == false)
         {
            // Stop the running thread and then quit.
            StopThread();
         }

         SaveSettings();
         Close();
      }


      private void cmdRun_Click(object sender, EventArgs e)
      {
         SaveSettings();

         txtLog.Text = "";
         // Run all steps from the start
         Run(-1, -1);
      }

      private void Run(int iStartIndex, int iStopIndex)
      {
         if (IsRunning)
            return;

         var settings = new Settings();
         settings.LoadSettings();


         for (int i = 0; i < lvwBuildSteps.Items.Count; i++)
            lvwBuildSteps.Items[i].SubItems[1].Text = "";

         timerBuildTime.Enabled = true;
         m_dtStartTime = DateTime.Now;

         cmdStart.Enabled = false;
         cmdStop.Enabled = true;

         // Set parameters to builder
         m_oBuilder.StepStart = iStartIndex;
         m_oBuilder.StepEnd = iStopIndex;

         m_oBuilder.ParameterSourcePath = txtPathSource.Text;
         m_oBuilder.ParameterVS8Path = txtPathVS8.Text;
         m_oBuilder.ParameterInnoSetupPath = txtPathInnoSetup.Text;
         m_oBuilder.ParameterGitPath = txtPathGit.Text;

         // Create macros
         m_oBuilder.LoadMacros(txtPathSource.Text, txtVersion.Text, txtBuildNumber.Text);

         string result;
         if (!settings.ValidateSettings(m_oBuilder, out result))
         {
            OnAddToLog(result, true);
            return;
         }

         m_EventStopThread.Reset();
         // create worker thread instance
         m_WorkerThread = new Thread(ThreadEntryPoint);
         m_WorkerThread.Name = "Build thread"; // looks nice in Output window
         m_WorkerThread.Start();
      }


      // Worker thread function.
      // Called indirectly from btnStartThread_Click
      private void ThreadEntryPoint()
      {
         var worker = new BuildRunner(m_EventStopThread, m_EventThreadStopped, m_oBuilder);

         worker.StepCompleted += worker_StepCompleted;
         worker.StepError += worker_StepError;
         worker.StepStarted += worker_StepStarted;
         worker.ThreadFinished += worker_ThreadFinished;
         worker.Run();
      }

      private void worker_ThreadFinished()
      {
         Invoke(m_DelegateThreadFinished);
      }

      private void worker_StepStarted(int stepIndex)
      {
         Invoke(m_DelegateBuildStepStarted, new object[] {stepIndex});
      }

      private void worker_StepError(int stepindex, string errorMessage)
      {
         Invoke(m_DelegateBuildStepError, new object[] {stepindex, errorMessage});
      }

      private void worker_StepCompleted(int stepIndex)
      {
         Invoke(m_DelegateBuildStepCompleted, new object[] {stepIndex});
      }

      // Stop worker thread if it is running.
      // Called when user presses Stop button of form is closed.
      private void StopThread()
      {
         if (m_WorkerThread != null && m_WorkerThread.IsAlive) // thread is active
         {
            // set event "Stop"
            m_EventStopThread.Set();

            // wait when thread  will stop or finish
            while (m_WorkerThread.IsAlive)
            {
               // We cannot use here infinite wait because our thread
               // makes syncronous calls to main form, this will cause deadlock.
               // Instead of this we wait for event some appropriate time
               // (and by the way give time to worker thread) and
               // process events. These events may contain Invoke calls.
               if (WaitHandle.WaitAll(
                  (new[] {m_EventThreadStopped}),
                  100,
                  true))
               {
                  break;
               }

               Application.DoEvents();
            }
         }

         OnThreadFinished(); // set initial state of buttons
      }

      private void OnBuildStepStarted(int index)
      {
         lvwBuildSteps.Items[index].SubItems[1].Text = "Started";
      }

      private void OnBuildStepCompleted(int index)
      {
         lvwBuildSteps.Items[index].SubItems[1].Text = "Completed";
      }


      private void OnBuildStepError(int index, string sError)
      {
         lvwBuildSteps.Items[index].SubItems[1].Text = "Error: " + sError;

         OnAddToLog("Build ended...\r\n", true);
      }

      private void OnAddToLog(string sMessage, bool timestamp)
      {
         string appendOutput = "";
         if (timestamp)
            appendOutput = DateTime.Now + Environment.NewLine;

         appendOutput += sMessage + Environment.NewLine;

         txtLog.SuspendLayout();
         txtLog.AppendText(appendOutput);

         txtLog.SelectionStart = txtLog.Text.Length - 1;
         txtLog.ScrollToCaret();
         txtLog.ResumeLayout();
      }

      private void OnThreadFinished()
      {
         cmdStart.Enabled = true;
         cmdStop.Enabled = false;
         timerBuildTime.Enabled = false;
      }

      private void lvwBuildSteps_DoubleClick(object sender, EventArgs e)
      {
         BuildSelected();
      }

      private void lvwBuildSteps_SelectedIndexChanged(object sender, EventArgs e)
      {
      }

      private void cmdStop_Click(object sender, EventArgs e)
      {
         StopThread();
      }

      private void cmBuildSelected_Click(object sender, EventArgs e)
      {
         BuildSelected();
      }


      private void BuildSelected()
      {
         int iCount = lvwBuildSteps.SelectedItems.Count;

         if (iCount == 0)
            return;

         int iFirstSelected = lvwBuildSteps.SelectedItems[0].Index;
         int iLastSelected = lvwBuildSteps.SelectedItems[iCount - 1].Index;

         Run(iFirstSelected, iLastSelected);
      }

      private void cmBuildFromCursor_Click(object sender, EventArgs e)
      {
         int iCount = lvwBuildSteps.SelectedItems.Count;

         if (iCount == 0)
            return;

         int iFirstToBuild = lvwBuildSteps.SelectedItems[0].Index;
         int iLastTobuild = lvwBuildSteps.Items.Count;

         Run(iFirstToBuild, iLastTobuild);
      }

      private void timerBuildTime_Tick(object sender, EventArgs e)
      {
         DateTime dtNow = DateTime.Now;

         TimeSpan dtSpan = dtNow - m_dtStartTime;

         string s = String.Format("{0:mm}{0:ss}", dtSpan.Minutes, dtSpan.Seconds);
         lblBuildTime.Text = dtSpan.ToString().Substring(0, 8);
      }

      private void label1_Click(object sender, EventArgs e)
      {
      }

      private void textBox1_TextChanged(object sender, EventArgs e)
      {
      }
   }
}