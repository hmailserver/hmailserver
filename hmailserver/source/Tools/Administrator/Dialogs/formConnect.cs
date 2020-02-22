// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Windows.Forms;

using hMailServer.Administrator.Utilities;
using hMailServer.Administrator.Utilities.Settings;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   public partial class formConnect : Form
   {
      hMailServer.Application application;
      private string _serverName;
      private UserSettings userSettings;

      public formConnect()
      {
         InitializeComponent();

         userSettings = UserSettings.Load();
         _serverName = "";

         new TabOrderManager(this).SetTabOrder(TabOrderManager.TabScheme.AcrossFirst);
      }

      public hMailServer.Application Application
      {
         get { return application; }
      }

      public string Server
      {
         get { return _serverName;  }
      }

      private void LoadSettings()
      {
         listServers.Items.Clear();

         foreach (Server server in userSettings.ServerConnections.List)
         {
            ListViewItem item = listServers.Items.Add(server.hostName);

            item.SubItems.Add(server.userName);
            item.Tag = server;

            if (server.hostName == userSettings.AutoConnectServer)
               item.Selected = true;
         }

         checkAutoConnect.Checked = userSettings.AutoConnectOnStartup;


      }

      private void SaveChanges()
      {
         userSettings.AutoConnectOnStartup = checkAutoConnect.Checked;

         if (listServers.SelectedItems.Count == 1)
         {
            ListViewItem item = listServers.SelectedItems[0];
            Server server = (Server)item.Tag;

            userSettings.AutoConnectServer = server.hostName;
         }

         UserSettings.Save(userSettings);
      }

      private void btnEdit_Click(object sender, EventArgs e)
      {
         if (listServers.SelectedItems.Count == 0)
            return;

         ListViewItem item = listServers.SelectedItems[0];
         Server server = (Server)item.Tag;

         formServerInformation serverDlg = new formServerInformation();
         serverDlg.Server = server;

         if (serverDlg.ShowDialog() == DialogResult.OK)
         {
            LoadSettings();
         }

      }

      private void btnCancel_Click(object sender, EventArgs e)
      {
         DialogResult = DialogResult.Cancel;

         this.Close();
      }

      private void btnConnect_Click(object sender, EventArgs e)
      {
         Connect();
      }

      private void Connect()
      {
         SaveChanges();

         if (listServers.SelectedItems.Count > 1)
         {
            MessageBox.Show("You can only connect to one server at a time.", EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Information);
            return;
         }

         ListViewItem item = null;

         if (listServers.SelectedItems.Count == 1)
            item = listServers.SelectedItems[0];
         else if (listServers.Items.Count == 1)
            item = listServers.Items[0];

         if (item == null)
         {
            MessageBox.Show("Please choose which server to connect to.", EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Information);
            return;
         }

         Server server = (Server)item.Tag;

         if (Connect(server))
         {

            this.Close();
         }


      }

      private bool Connect(Server server)
      {
         try
         {
            application = APICreator.Create(server.hostName);

            if (application == null)
               return false;

            if (application.Database.RequiresUpgrade)
            {
               string message = string.Format("Your database is not up to date and needs to be upgraded." + Environment.NewLine +
                                              "Please run DBUpdater.exe to update the datatabase." + Environment.NewLine +
                                              Environment.NewLine +
                                              "Current database version: {0}" + Environment.NewLine +
                                              "Required database version: {1}",
                                              application.Database.CurrentVersion,
                                              application.Database.RequiredVersion);


               MessageBox.Show(message, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
               return false;
            }

            if (APICreator.Authenticate(application, server))
            {
               DialogResult = DialogResult.OK;
               _serverName = server.hostName;

               return true;
            }

            return false;
         }
         catch (Exception ex)
         {
            MessageBox.Show(ex.Message, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            return false;
         }
      }

      private void btnAdd_Click(object sender, EventArgs e)
      {
         formServerInformation serverDlg = new formServerInformation();

         Server server = new Server()
            {
               userName = "Administrator"
            };

         serverDlg.Server = server;

         if (serverDlg.ShowDialog() == DialogResult.OK)
         {
            userSettings.ServerConnections.List.Add(server);

            LoadSettings();
         }
      }



      private void btnRemove_Click(object sender, EventArgs e)
      {
         if (listServers.SelectedItems.Count == 0)
            return;

         ListViewItem item = listServers.SelectedItems[0];
         Server server = (Server)item.Tag;

         userSettings.ServerConnections.List.Remove(server);

         listServers.Items.Remove(item);
      }

      private void listServers_DoubleClick(object sender, EventArgs e)
      {
         Connect();
      }

      private void formConnect_Load(object sender, EventArgs e)
      {
         LoadSettings();
      }

      public bool AutoConnect()
      {
         if (userSettings.AutoConnectOnStartup)
         {
            string hostName = userSettings.AutoConnectServer;
            if (string.IsNullOrEmpty(hostName))
               return false;

            foreach (Server server in userSettings.ServerConnections.List)
            {
               if (server.hostName.ToUpper() == hostName.ToUpper())
               {
                  return Connect(server);
               }
            }
         }

         return false;
      }
   }
}