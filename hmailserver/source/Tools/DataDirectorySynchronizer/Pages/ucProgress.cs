// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Runtime.InteropServices;
using hMailServer.Shared;


namespace DataDirectorySynchronizer.Pages
{


   public partial class ucProgress : UserControl, IWizardPage
   {

      hMailServer.Application _application;
      hMailServer.Domains _domains;
      hMailServer.Utilities _utilities;

      private DateTime _startTime;
      private int _counter;

      public ucProgress()
      {
         InitializeComponent();

         _application = Globals.GetApp();
         _utilities = _application.Utilities;
         _domains = _application.Domains;
         _counter = 0;

      }

      private void AddProcessedFile(string file, bool processed)
      {
         if (processed == false)
         {
            listProcess.Items.Add(file);
         }

         _counter++;

         if (_counter % 10 == 0)
            Application.DoEvents();

      }

      private void StartProcess()
      {
         try
         {
            timer.Enabled = true;
            _startTime = DateTime.Now;

            hMailServer.Application application = Globals.GetApp();

            string dataDirectory = application.Settings.Directories.DataDirectory;
            DirectoryInfo dirInfo = new DirectoryInfo(dataDirectory);

            // Process the queue first.
            ProcessFilesInFolder(dirInfo, 0);

            // Process all domains
            IterateDomainFolders(dirInfo);

            timer.Enabled = false;

            application.Reinitialize();

            TimerTick();

            labelStatus.Text = "The synchronization has completed.";

         }
         catch (Exception e)
         {
            MessageBox.Show(e.Message + Environment.NewLine + e.StackTrace);
         }
      }

      private void ProcessFilesInFolder(DirectoryInfo dirInfo, int accountID)
      {
         labelStatus.Text = dirInfo.FullName;
         foreach (FileInfo file in dirInfo.GetFiles())
         {
            string fullName = file.FullName;

            bool imported = false;

            if (fullName.ToLower().EndsWith(".eml") ||
                fullName.ToLower().EndsWith(".hma"))
            {
               if (Globals.Mode == Globals.ModeType.Import)
                  imported = _utilities.ImportMessageFromFile(fullName, accountID);
               else
               {
                  // does it exist?
                  long messageID = _utilities.RetrieveMessageID(fullName);

                  if (messageID == 0)
                  {
                     // no. delete the file.
                     File.Delete(file.FullName);
                  }

                  imported = true;
               }
            }

            AddProcessedFile(fullName, imported);
         }
      }

      private void IterateDomainFolders(DirectoryInfo dirRoot)
      {
         foreach (DirectoryInfo domainFolder in dirRoot.GetDirectories())
         {

            try
            {
               // Should we process this domain?
               if (Globals.SelectedDomains.Contains(domainFolder.Name))
               {
                  hMailServer.Domain domain = _domains.get_ItemByName(domainFolder.Name);

                  DirectoryInfo accountsFolder = new DirectoryInfo(domainFolder.FullName);
                  IterateAccounts(domain, accountsFolder);

                  Marshal.ReleaseComObject(domain);
               }
            }
            catch (Exception)
            {
               AddProcessedFile(domainFolder.FullName, false);
            }
         }
      }

      private void IterateAccounts(hMailServer.Domain domain, DirectoryInfo folder)
      {
         string domainName = domain.Name;
         foreach (DirectoryInfo directory in folder.GetDirectories())
         {
            try
            {
               hMailServer.Account account =
                  domain.Accounts.get_ItemByAddress(directory.Name + "@" + domainName);

               IterateAccountFolders(account.ID, directory);

               Marshal.ReleaseComObject(account);
            }
            catch (Exception)
            {
               AddProcessedFile(directory.FullName, false);
            }
         }
      }

      private void IterateAccountFolders(int accountID, DirectoryInfo folder)
      {
         try
         {

            ProcessFilesInFolder(folder, accountID);

            foreach (DirectoryInfo accountSubFolder in folder.GetDirectories())
            {
               ProcessFilesInFolder(accountSubFolder, accountID);
            }
         }
         catch (Exception)
         {
            AddProcessedFile(folder.FullName, false);
         }
      }




      public void OnShowPage(Dictionary<string, string> _state)
      {
         StartProcess();
      }

      public bool OnLeavePage(bool next)
      {
         return true;
      }

      public string Title
      {
         get
         {
            return "Synchronization";
         }
      }

      private void timer_Tick(object sender, EventArgs e)
      {
         TimerTick();
      }

      private void TimerTick()
      {
         TimeSpan ts = DateTime.Now - _startTime;

         string hours = ts.Hours.ToString();
         string minutes = ts.Minutes.ToString();
         string seconds = ts.Seconds.ToString();

         if (hours.Length == 1) hours = "0" + hours;
         if (minutes.Length == 1) minutes = "0" + minutes;
         if (seconds.Length == 1) seconds = "0" + seconds;

         labelExecutionTime.Text = hours + ":" + minutes + ":" + seconds;
      }

      private void labelExecutionTime_Click(object sender, EventArgs e)
      {

      }


   }
}
