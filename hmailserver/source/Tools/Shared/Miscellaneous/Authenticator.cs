// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System;
using System.Windows.Forms;

namespace hMailServer.Shared
{
   public static class Authenticator
   {
      public static bool AuthenticateUser(hMailServer.Application application, string password)
      {
         hMailServer.Account account = application.Authenticate("Administrator", password);

         if (account != null)
            return true;

         return false;
      }

      public static bool AuthenticateUser(hMailServer.Application application)
      {
         return AuthenticateUser(application, null, true);
      }

      /// <summary>
      /// Authenticates the Administrator account. If allowPrompt is false, no dialog is
      /// shown - needed when running unattended, where there may be no desktop available.
      /// </summary>
      public static bool AuthenticateUser(hMailServer.Application application, string password, bool allowPrompt)
      {
         // Try the password we've been given, if any.
         if (!string.IsNullOrEmpty(password) && AuthenticateUser(application, password))
            return true;

         // Then try to authenticate using an empty password.
         if (AuthenticateUser(application, ""))
            return true;

         // Try to authenticate using password on command line...
         string [] args = Environment.GetCommandLineArgs();
         foreach (string commandLinePassword in args)
         {
            if (AuthenticateUser(application, commandLinePassword))
               return true;
         }

         if (!allowPrompt)
            return false;

         while (true)
         {
            formEnterPassword passwordDlg = new formEnterPassword();

            if (passwordDlg.ShowDialog() != System.Windows.Forms.DialogResult.OK)
               return false;

            string enteredPassword = passwordDlg.Password;

            if (AuthenticateUser(application, enteredPassword))
               return true;

            MessageBox.Show("Invalid user name or password.", "hMailServer");
         }
      }
   }
}
