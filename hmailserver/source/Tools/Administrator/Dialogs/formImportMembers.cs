// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Security.Cryptography.X509Certificates;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator.Dialogs
{
   public partial class formImportMembers : Form
   {
      private hMailServer.DistributionList _list;
      private BackgroundWorker _worker;
      private WaitCursor _waitCursor;

      public formImportMembers(hMailServer.DistributionList list)
      {
         InitializeComponent();

         _list = list;
      }

      private void btnCancel_Click(object sender, EventArgs e)
      {
         if (_worker != null && _worker.IsBusy)
         {
            _worker.CancelAsync();
         }

         HideWaitCursor();
         
         this.Close();
      }

      private void buttonSelectImportFile_Click(object sender, EventArgs e)
      {
         openFileDialog1.Filter = "All files (*.*)|*.*";
         openFileDialog1.FileName = string.Empty;
         openFileDialog1.Title = this.Text;

         listItems.Items.Clear();
         if (openFileDialog1.ShowDialog() == DialogResult.OK)
         {
            ucImportFile.Text = openFileDialog1.FileName;

            var fileLines = File.ReadAllLines(ucImportFile.Text);
            var itemsListed = new Dictionary<string, bool>();

            var listViewItems = new List<ListViewItem>();

            foreach (var line in fileLines)
            {
               var normalizedLine = line.Trim(' ', '"', '\t');

               if (!normalizedLine.Contains("@"))
                  continue;
               if (string.IsNullOrEmpty(normalizedLine))
                  continue;

               if (itemsListed.ContainsKey(normalizedLine.ToLowerInvariant()))
                  continue;

               itemsListed.Add(normalizedLine, true);

               listViewItems.Add(new ListViewItem(normalizedLine));
            }

            listItems.Items.AddRange(listViewItems.ToArray());
         }

         buttonImport.Enabled = listItems.Items.Count > 0;

      }

      private void buttonImport_Click(object sender, EventArgs e)
      {
         if (_worker != null && _worker.IsBusy)
         {
            return;
         }

         _waitCursor = new WaitCursor();
         
         EnableDisable(false);

         progressBar1.Minimum = 0;
         progressBar1.Maximum = 100;
         progressBar1.Value = 0;

         _worker = new BackgroundWorker
            {
               WorkerReportsProgress = true,
               WorkerSupportsCancellation = true
               
            };
         _worker.ProgressChanged += (o, args) => progressBar1.Value = args.ProgressPercentage;
         _worker.DoWork += backgroundWorker_DoWork;
         _worker.RunWorkerCompleted += backgroundWorker_Completed;
         
         var addresses = new List<string>();
         foreach (ListViewItem item in listItems.Items)
         {
            addresses.Add(item.Text);
         }

         var data = new ImportData()
            {
               Addresses = addresses,
               DeleteRecipientsNotInImportFile = ucDeleteRecipientsNotInList.Checked
            };

         _worker.RunWorkerAsync(data);
      }

      private void backgroundWorker_Completed(object sender, RunWorkerCompletedEventArgs e)
      {
         if (!e.Cancelled)
         {
            MessageBox.Show(Strings.Localize("Import completed."), this.Text);
         }

         EnableDisable(true);
      }

      private void EnableDisable(bool enable)
      {
         buttonImport.Enabled = enable;
         listItems.Enabled = enable;
         buttonSelectImportFile.Enabled = enable;
      }

      private void HideWaitCursor()
      {
         if (_waitCursor != null)
         {
            _waitCursor.Dispose();
            _waitCursor = null;            
         }
      }

      private void backgroundWorker_DoWork(object sender, DoWorkEventArgs e)
      {
         var importData = (ImportData)e.Argument;

         var itemsInInputList = new Dictionary<string, bool>();
         foreach (var address in importData.Addresses)
            itemsInInputList.Add(address.ToLowerInvariant(), true);

         var itemsToDelete = new List<int>();
         
         var existingItems = new Dictionary<string, bool>();
         
         var recipients = _list.Recipients;

         for (int i = 0; i < recipients.Count; i++)
         {
            if (_worker.CancellationPending)
            {
               e.Cancel = true;
               return;
            }

            var recipient = recipients[i];
            var existingAddress = recipient.RecipientAddress.ToLowerInvariant();
            existingItems.Add(existingAddress, true);

            if (importData.DeleteRecipientsNotInImportFile)
            {
               if (!itemsInInputList.ContainsKey(existingAddress))
                  itemsToDelete.Add(recipient.ID);
            }

            Marshal.ReleaseComObject(recipient);
         }

         int totalCount = importData.Addresses.Count + itemsToDelete.Count;

         if (totalCount == 0)
            return;

         int processedCount = 0;

         foreach (var address in importData.Addresses)
         {
            if (_worker.CancellationPending)
            {
               e.Cancel = true;
               return;
            }

            if (!existingItems.ContainsKey(address.ToLowerInvariant()))
            {
               var newRecipient = recipients.Add();
               newRecipient.RecipientAddress = address;
               newRecipient.Save();
            }

            processedCount++;

            int percentageDone = (int) (((double)processedCount / (double)totalCount) * 100);
            _worker.ReportProgress(percentageDone);
         }

         foreach (var item in itemsToDelete)
         {
            if (_worker.CancellationPending)
            {
               e.Cancel = true;
               return;
            }

            recipients.DeleteByDBID(item);

            processedCount++;

            int percentageDone = (int)(((double)processedCount / (double)totalCount) * 100);
            _worker.ReportProgress(percentageDone);
         }

         Marshal.ReleaseComObject(recipients);
      }

      private void listItems_DoubleClick(object sender, EventArgs e)
      {

      }

      private void formImportMembers_Load(object sender, EventArgs e)
      {

      }

      private void ucDeleteRecipientsNotInList_CheckedChanged(object sender, EventArgs e)
      {

      }

      class ImportData
      {
         public List<string> Addresses { get; set; }
         public bool DeleteRecipientsNotInImportFile { get; set; }
      }
   }
}