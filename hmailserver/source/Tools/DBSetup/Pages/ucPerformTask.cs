// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using hMailServer.Shared;

namespace DBSetup.Pages
{
   public partial class ucPerformTask : UserControl, IWizardPage
   {
      private Dictionary<string, string> _state;

      public ucPerformTask()
      {
         InitializeComponent();
      }

      public string Title
      {
         get
         {
            return "Finish";
         }
      }

      public void OnShowPage(Dictionary<string, string> state)
      {
         textLog.Text = "";

         _state = state;
      }

      private void AddToLog(string text)
      {
         textLog.AppendText(text + Environment.NewLine);
         textLog.SelectionStart = textLog.Text.Length;
         textLog.ScrollToCaret();
         textLog.Refresh();

         Application.DoEvents();
      }

      public bool OnLeavePage(bool next)
      {
         if (next == false)
            return true;
         try
         {
            AddToLog("Starting task...");


            // Perform the task...
            hMailServer.eDBtype dbType = Globals.GetDatabaseType(_state["ServerType"]);
            string serverName = _state["ServerAddress"];
            string portString = _state["ServerPort"];
            int port = 0;
            int.TryParse(portString, out port);
            string databaseName = _state["DatabaseName"];

            string userName = _state["Username"];
            string passWord = _state["Password"];

            string serviceDependency = _state["ServiceDependency"];
            SetDependency(serviceDependency);

            if (_state["Authentication"] == "Windows")
            {
               userName = "";
               passWord = "";
            }

            hMailServer.Database database = Globals.GetApp().Database;

            if (_state["CreateNew"] == "Yes")
            {
               AddToLog("Please wait while creating database...");
               database.CreateExternalDatabase(dbType, serverName, port, databaseName, userName, passWord);
               AddToLog("Database created.");
            }
            else
            {
               AddToLog("Please wait while updating database settings...");
               database.SetDefaultDatabase(dbType, serverName, port, databaseName, userName, passWord);
               AddToLog("Settings updated.");
            }

            AddToLog("Restarting server...");
            Globals.GetApp().Reinitialize();
            AddToLog("Server restarted.");
            AddToLog("");
            AddToLog("Task completed.");

            
         }
         catch (Exception ex)
         {
            AddToLog(ex.Message);
            return false;
         }

         return true;
      }

      private void SetDependency(string service)
      {
         try
         {
            if (service.Length == 0)
               return;

            int paranthesisStart = service.LastIndexOf("(")+1;
            int paranthesisEnd = service.IndexOf(")", paranthesisStart);
            int len = paranthesisEnd - paranthesisStart;

            string serviceName = service.Substring(paranthesisStart, len);

            Globals.GetApp().Utilities.MakeDependent(serviceName);
         }
         catch (Exception)
         {
            MessageBox.Show("The set-up failed to set the service dependency.", "hMailServer");
         }

      }
   }
}
