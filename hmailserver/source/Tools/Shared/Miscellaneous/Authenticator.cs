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
         // First try to authenticate using an empty password.
         if (AuthenticateUser(application, ""))
            return true;
         
         // Try to authenticate using password on command line...
         string [] args = Environment.GetCommandLineArgs();
         foreach (string password in args)
         {
            if (AuthenticateUser(application, password))
               return true;
         }

         while (true)
         {
            formEnterPassword passwordDlg = new formEnterPassword();

            if (passwordDlg.ShowDialog() != System.Windows.Forms.DialogResult.OK)
               return false;

            string password = passwordDlg.Password;

            if (AuthenticateUser(application, password))
               return true;

            MessageBox.Show("Invalid user name or password.", "hMailServer");
         }
      }
   }
}
