// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;
using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Dialogs;
using hMailServer.Administrator.Utilities.Localization;
using System.Runtime.InteropServices;
using hMailServer.Shared;
using System.Reflection;

namespace hMailServer.Administrator
{
    public partial class ucStatus : UserControl, ISettingsControl
    {
        private hMailServer.Application _application;
        private hMailServer.Logging _logging = null;

        public ucStatus()
        {
            InitializeComponent();

            new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);

            Strings.Localize(contextMenuDeliveryQueue);
            Strings.Localize(contextMenuStripLiveLog);
			
            // Enable dobule-buffering in live log to improve performance.
            PropertyInfo aProp = typeof(ListView).GetProperty("DoubleBuffered", BindingFlags.NonPublic | BindingFlags.Instance);
			   aProp.SetValue(listLiveLog, true, null);

            listLiveLog.LoadWidths("StatusLiveLog");
            listDeliveryQueue.LoadWidths("StatusDeliveryQueue");
        }

        public void OnLeavePage()
        {
            timerLiveLog.Enabled = false;
            timerServerStats.Enabled = false;

            Marshal.ReleaseComObject(_logging);

            listLiveLog.SaveWidths("StatusLiveLog");
            listDeliveryQueue.SaveWidths("StatusDeliveryQueue");
        }

        ~ucStatus()
        {

        }


        public bool Dirty
        {
            get { return false; }
        }

        public void LoadData()
        {
            _application = APICreator.Application;

            labelVersion.Text = string.Format("{0} ({1})", _application.Version, _application.VersionArchitecture);

            ShowStatistics();

            hMailServer.Database database = _application.Database;

            labelServerType.Text = InternalNames.GetDatabaseServerTypeName(database.DatabaseType);

            if (database.ServerName.Length > 0)
                labelServerHost.Text = database.ServerName;
            else
                labelServerHost.Text = "-";

            if (database.DatabaseName.Length > 0 && database.DatabaseType != eDBtype.hDBTypeMSSQLCE)
                labelName.Text = database.DatabaseName;
            else
                labelName.Text = "-";

            labelDBVersion.Text = database.CurrentVersion.ToString();

            Marshal.ReleaseComObject(database);

            hMailServer.Settings settings = _application.Settings;
            _logging = settings.Logging;
            Marshal.ReleaseComObject(settings);
            

            DisplayServerState();
            DisplayWarnings();
        }

        private void ShowStatistics()
        {
           hMailServer.Status status = _application.Status;

           textProcessedMessages.Text = status.ProcessedMessages.ToString();
           textMessagesContainingViruses.Text = status.RemovedViruses.ToString();
           textMessagesContainingSpam.Text = status.RemovedSpamMessages.ToString();
           textSMTPSessions.Text = status.get_SessionCount(eSessionType.eSTSMTP).ToString();
           textPOP3Sessions.Text = status.get_SessionCount(eSessionType.eSTPOP3).ToString();
           textIMAPSessions.Text = status.get_SessionCount(eSessionType.eSTIMAP).ToString();
           labelStartTime.Text = status.StartTime;

           Marshal.ReleaseComObject(status);
        }

        private void DisplayServerState()
        {

            labelCurrentStatus.Text = EnumStrings.GetStateString(_application.ServerState);

            switch (_application.ServerState)
            {
                case eServerState.hStateRunning:
                    buttonStartStop.Text = Strings.Localize("Pause");
                    buttonStartStop.Enabled = true;
                    break;
                case eServerState.hStateStopped:
                    buttonStartStop.Text = Strings.Localize("Resume");
                    buttonStartStop.Enabled = true;
                    break;
                default:
                    buttonStartStop.Enabled = false;
                    break;
            }


        }

        private void DisplayWarnings()
        {
            listWarnings.Items.Clear();

            hMailServer.Settings settings = APICreator.Application.Settings;

            if (settings.HostName.Length == 0)
                AddWarning("W001", Strings.Localize("High"), Strings.Localize("You haven't specified the public host name for this computer in the SMTP settings."));

            if (settings.DenyMailFromNull)
                AddWarning("W002", Strings.Localize("High"), Strings.Localize("You have configured hMailServer not to allow email with empty sender address. Many email server will not accept email from your server with this configuration."));

            int autobanRanges = 0;

            // Check if External to external is enabled in any of the IP ranges.
            hMailServer.SecurityRanges ranges = settings.SecurityRanges;
            for (int i = 0; i < ranges.Count; i++)
            {
                hMailServer.SecurityRange range = ranges[i];

                if (range.AllowDeliveryFromRemoteToRemote && !range.RequireSMTPAuthExternalToExternal)
                {
                    string warning =
                       Strings.Localize("hMailServer is configured to allow deliveries from external to external accounts in the IP range %s. This may make the server vulnerable to spam. It is recommended that you disable this option.");

                    warning = warning.Replace("%s", range.Name);

                    AddWarning("W003", Strings.Localize("Critical"), warning);
                }

               if (range.LowerIP == "127.0.0.1" && range.UpperIP == "127.0.0.1" && range.Expires)
               {
                  string warning =
                       Strings.Localize("Localhost is currently banned in the IP ranges.");

                  AddWarning("W004", Strings.Localize("High"), warning);
               }

               if (range.Expires)
                  autobanRanges += 1;

               Marshal.ReleaseComObject(range);
            }

           if (autobanRanges > 0)
           {
              string warning =
                  Strings.Localize("There is a total of %s auto-ban IP ranges.");

              warning = warning.Replace("%s", autobanRanges.ToString());

              AddWarning("W005", Strings.Localize("Medium"), warning);
           }


            Marshal.ReleaseComObject(ranges);
            Marshal.ReleaseComObject(settings);

        }


        private void AddWarning(string id, string severity, string text)
        {
            ListViewItem item = listWarnings.Items.Add(id);
            item.SubItems.Add(severity);
            item.SubItems.Add(text);
        }

        public bool SaveData()
        {
            // nothing to save
           return true;
        }

        public void LoadResources()
        {
            // load the translated resources
        }

        private void buttonStartStop_Click(object sender, System.EventArgs e)
        {
            WaitCursor cursor = new WaitCursor();

            switch (_application.ServerState)
            {
                case eServerState.hStateRunning:
                    _application.Stop();
                    break;
                case eServerState.hStateStopped:
                    _application.Start();
                    break;
            }

            DisplayServerState();
        }

        private void btnStartLiveLog_Click(object sender, System.EventArgs e)
        {
            if (timerLiveLog.Enabled == false)
            {
                _logging.EnableLiveLogging(true);
                timerLiveLog.Enabled = true;
            }
            else
            {
                _logging.EnableLiveLogging(false);
                timerLiveLog.Enabled = false;
            }

            DisplayLiveLogButtonCaption();
        }

        private void DisplayLiveLogButtonCaption()
        {
            if (timerLiveLog.Enabled)
                btnStartLiveLog.Text = Strings.Localize("Stop");
            else
                btnStartLiveLog.Text = Strings.Localize("Start");
        }

        private void btnClear_Click(object sender, System.EventArgs e)
        {
            listLiveLog.Items.Clear();
        }

        private void timerLiveLog_Tick(object sender, System.EventArgs e)
        {
            try
            {
                RefreshLiveLog();
            }
            catch(Exception)
            {

            }
            
        }

        private void RefreshLiveLog()
        {
            string liveLog = _logging.LiveLog;

            if (liveLog.Length == 0)
            {
               if (!_logging.LiveLoggingEnabled)
               {
                  timerLiveLog.Enabled = false;
                  DisplayLiveLogButtonCaption();

                  MessageBox.Show("The live log was automatically disabled due to too high throughput." + Environment.NewLine + 
                                  "To retrieve logging information, please read the log files.", 
                                  EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Warning);
                  return;
               }
               
            }

            string[] lines = liveLog.Split(Environment.NewLine.ToCharArray());

            List<ListViewItem> items = new List<ListViewItem>(); 

            foreach (string s in lines)
            {
                if (s.Length == 0)
                    continue;

                string temp = s.Replace("\"", "");
                string[] columns = temp.Split('\t');

                ListViewItem item = new ListViewItem();

                item.Text = columns[0];

                if (columns.Length == 4)
                {
                    item.SubItems.Add("");
                    item.SubItems.Add(columns[1]);
                    item.SubItems.Add(columns[2]);
                    item.SubItems.Add("");
                    item.SubItems.Add(columns[3]);
                }
                else
                {
                    item.SubItems.Add(columns[2]);
                    item.SubItems.Add(columns[1]);
                    item.SubItems.Add(columns[3]);
                    item.SubItems.Add(columns[4]);
                    item.SubItems.Add(columns[5]);
                }

                items.Add(item);
            }

            if (listLiveLog.Items.Count > 50000)
            {
               TruncateLiveLog();
            }

            // Check if the last item in the list is visible. If so, we should autoscroll.
            ListViewHitTestInfo info = listLiveLog.HitTest(new Point(1, listLiveLog.Height - 10));

            bool focusLastItem = false;

            if (listLiveLog.Items.Count > 0)
            {
               // Is the last item currently visible?
               ListViewItem lastItem = listLiveLog.Items[listLiveLog.Items.Count-1];
               if (lastItem.Position.Y > 0 && lastItem.Position.Y < listLiveLog.Height)
               {
                  focusLastItem = true;
               }
            }
            else
            {
               focusLastItem = true;
            }

            if (items.Count > 0)
            {
               listLiveLog.Items.AddRange(items.ToArray());

               if (focusLastItem)
               {
                  listLiveLog.EnsureVisible(listLiveLog.Items.Count - 1);
               }
            }
        }

        private void TruncateLiveLog()
        {
           listLiveLog.Items.Clear();

           ListViewItem item = listLiveLog.Items.Add("");
           item.SubItems.Add("");
           item.SubItems.Add("");
           item.SubItems.Add("");
           item.SubItems.Add("");
           item.SubItems.Add("This list was too long (> 50 000 items) and was cleared.");

           item = listLiveLog.Items.Add("");
           item.SubItems.Add("");
           item.SubItems.Add("");
           item.SubItems.Add("");
           item.SubItems.Add("");
           item.SubItems.Add("Please refer to the log on disk for log history.");
        }


        private void buttonRefreshDeliveryQueue_Click(object sender, EventArgs e)
        {
            RefreshDeliveryQueue();
        }

        private void RefreshDeliveryQueue()
        {
           using (new WaitCursor())
           {
              listDeliveryQueue.Items.Clear();

              hMailServer.Status status = _application.Status;

              string queue = status.UndeliveredMessages;

              string[] messages = queue.Split(Environment.NewLine.ToCharArray());

              List<ListViewItem> itemsToAdd = new List<ListViewItem>();

              foreach (string message in messages)
              {
                 if (message.Length == 0)
                    continue;

                 string[] columns = message.Split('\t');

                 ListViewItem item = new ListViewItem(columns[0]);
                 item.SubItems.Add(columns[1]);
                 item.SubItems.Add(columns[2]);
                 item.SubItems.Add(columns[3]);

                 if (columns[4] == "1901-01-01 00:00:00")
                    item.SubItems.Add("As soon as possible");
                 else
                    // This is added to show HOLD in Status
                    // Note ends in 1 to signify difference
                    if (columns[4] == "1901-01-01 00:00:01")
                       item.SubItems.Add("Route HOLD");
                    else
                       item.SubItems.Add(columns[4]);

                 item.SubItems.Add(columns[7]);

                 item.SubItems.Add(columns[5]);

                 item.Tag = columns[0];

                 itemsToAdd.Add(item);
              }

              listDeliveryQueue.Items.AddRange(itemsToAdd.ToArray());

              string messagesCount = "%d messages in queue";
              messagesCount = messagesCount.Replace("%d", listDeliveryQueue.Items.Count.ToString());
              labelNumberOfMessages.Text = messagesCount;
              Marshal.ReleaseComObject(status);
           }
        }

        private void btnClearQueue_Click(object sender, EventArgs e)
        {
            string message = "Are you sure you want to clear the delivery queue?";

            if (MessageBox.Show(message, EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                WaitCursor waitCursor = new WaitCursor();

                hMailServer.DeliveryQueue deliveryQueue = GetDeliveryQueue();
                deliveryQueue.Clear();
                Marshal.ReleaseComObject(deliveryQueue);
            }

            RefreshDeliveryQueue();
        }

        private void menuItemSendNow_Click(object sender, EventArgs e)
        {
            WaitCursor waitCursor = new WaitCursor();

            hMailServer.DeliveryQueue deliveryQueue = GetDeliveryQueue();
            foreach (ListViewItem item in listDeliveryQueue.SelectedItems)
            {
                string messageID = (string)item.Tag;

                deliveryQueue.ResetDeliveryTime(Convert.ToInt32(messageID));
            }

            deliveryQueue.StartDelivery();
            Marshal.ReleaseComObject(deliveryQueue);
        }

        private hMailServer.DeliveryQueue GetDeliveryQueue()
        {
            hMailServer.GlobalObjects globalObjects = _application.GlobalObjects;

            hMailServer.DeliveryQueue deliveryQueue = globalObjects.DeliveryQueue;

            Marshal.ReleaseComObject(globalObjects);

            return deliveryQueue;
        }

        private void menuItemDelete_Click(object sender, EventArgs e)
        {
            string message = "Are you sure you want to delete selected message(s) from queue?";
            if (MessageBox.Show(Strings.Localize(message), EnumStrings.hMailServerAdministrator, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                WaitCursor waitCursor = new WaitCursor();

                hMailServer.DeliveryQueue deliveryQueue = GetDeliveryQueue();
                foreach (ListViewItem item in listDeliveryQueue.SelectedItems)
                {
                    string messageID = (string)item.Tag;

                    deliveryQueue.Remove(Convert.ToInt64(messageID));
                }

                Marshal.ReleaseComObject(deliveryQueue);
            }

            RefreshDeliveryQueue();
        }

        private void listWarnings_DoubleClick(object sender, EventArgs e)
        {
            ShowSelectedWarning();
        }

        private void ShowSelectedWarning()
        {
            if (listWarnings.SelectedItems.Count != 1)
                return;

            formInfoDialog infoDialog = new formInfoDialog();

            ListViewItem item = listWarnings.SelectedItems[0];

            infoDialog.Title = "Configuration warnings";
            infoDialog.Message = item.Text + " - " + item.SubItems[1].Text + Environment.NewLine + item.SubItems[2].Text;

            infoDialog.ShowDialog();
        }

        private void buttonShowWarning_Click(object sender, EventArgs e)
        {
            ShowSelectedWarning();
        }

        private void menuItemOnlyIncludeThisSessionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            List<string> validSessionIds = new List<string>();

            foreach (ListViewItem item in listLiveLog.SelectedItems)
            {
                if (!validSessionIds.Contains(item.SubItems[1].Text))
                    validSessionIds.Add(item.SubItems[1].Text);
            }

            foreach (ListViewItem item in listLiveLog.Items)
            {
                string sessionID = item.SubItems[1].Text;

                if (!validSessionIds.Contains(sessionID))
                    listLiveLog.Items.Remove(item);
            }

        }

        private void listWarnings_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listWarnings.SelectedItems.Count == 0)
                buttonShowWarning.Enabled = false;
            else
                buttonShowWarning.Enabled = true;
        }

        private void listDeliveryQueue_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void listDeliveryQueue_DoubleClick(object sender, EventArgs e)
        {
            ShowSelectedMessage();

        }

        private void ShowSelectedMessage()
        {
            if (listDeliveryQueue.SelectedItems.Count != 1)
                return;

            ListViewItem item = listDeliveryQueue.SelectedItems[0];
            string fileName = item.SubItems[item.SubItems.Count - 1].Text;

            formMessageViewer messageViewer = new formMessageViewer(fileName);

            messageViewer.ShowDialog();
        }

        private void menuItemShow_Click(object sender, EventArgs e)
        {
            ShowSelectedMessage();
        }

        private void timerServerStats_Tick(object sender, EventArgs e)
        {
           ShowStatistics();
        }
    }
}
