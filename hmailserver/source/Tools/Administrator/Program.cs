// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Collections;
using hMailServer.Administrator.Dialogs;
using System.Threading;
using System.Runtime.InteropServices;
using hMailServer.Administrator.Utilities;
using hMailServer.Shared;

namespace hMailServer.Administrator
{
   static class Program
   {
      /// <summary>
      /// The main entry point for the application.
      /// </summary>
      [STAThread]
      static void Main()
      {
         // Hook up so that we can fetch unhandled events.
         AppDomain currentDomain = AppDomain.CurrentDomain;
         currentDomain.UnhandledException += new UnhandledExceptionEventHandler(currentDomain_UnhandledException);
         System.Windows.Forms.Application.ThreadException += new ThreadExceptionEventHandler(currentDomain_ThreadException);

         // What does this actually do...?
         System.Windows.Forms.Application.EnableVisualStyles();
         System.Windows.Forms.Application.SetCompatibleTextRenderingDefault(false);

         formConnect connectDlg = new formConnect();
         if (!connectDlg.AutoConnect())
         {
            if (connectDlg.ShowDialog() != DialogResult.OK)
            {
               // We failed to connect. Exit app again.
               return;
            }
         }

         hMailServer.Application application = connectDlg.Application;
         string serverHost = connectDlg.Server;

         System.Windows.Forms.Application.Run(new formMain(application, serverHost));

         if (application != null)
            Marshal.ReleaseComObject(application);

      }

      static void currentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
      {
         formErrorMessage dialog = new formErrorMessage(e);
         dialog.ShowDialog();

         Instances.MainForm.Repaint();

      }

      private static void currentDomain_ThreadException(object sender, ThreadExceptionEventArgs t)
      {
         System.Runtime.InteropServices.COMException comEx = t.Exception as System.Runtime.InteropServices.COMException;

         if (comEx != null)
         {
            // RPC server fault.
            if (comEx.ErrorCode == -2147417851 || comEx.ErrorCode == -2147023174)
            {
               // Connection 
               HandleConnectionLost();
               return;
            }
         }

         formErrorMessage dialog = new formErrorMessage(t);
         dialog.ShowDialog();

         Instances.MainForm.Repaint();
      }

      private static void HandleConnectionLost()
      {
         MessageBox.Show("hMailServer Administrator lost the connection to the hMailServer service.\r\n"+
                         "This may happen for example if the hMailServer service stops or restarts.\r\n" +
                         "\r\n" +
                         "If the stop or restart was unexpected, please review the hMailServer logs.\r\n" +
                         "\r\n" +
                         "Press OK to restart hMailServer Administrator.", EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Error);
         System.Windows.Forms.Application.Restart();

      }

   }
}