// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com


using System;
using System.Runtime.Remoting;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using hMailServer.Administrator.Dialogs;
using System.Runtime.InteropServices;
using hMailServer.Shared;

namespace hMailServer.Administrator.Utilities
{
   internal class APICreator
   {
      private static hMailServer.Application application;

      public static hMailServer.Application Create(string hostName)
      {
         try
         {
            Type obj = Type.GetTypeFromProgID("hMailServer.Application", hostName);
            hMailServer.Application app = (hMailServer.Application) Activator.CreateInstance(obj);

            application = app;

            return app;
         }
         catch (COMException comException)
         {
            if (comException.ErrorCode == -2147023174)
            {
               MessageBox.Show("Unable to connect to the specified server.", EnumStrings.hMailServerAdministrator);
            }
            else
            {
               MessageBox.Show(comException.Message, EnumStrings.hMailServerAdministrator);
            }

         }
         catch (Exception e)
         {
            MessageBox.Show(e.Message, EnumStrings.hMailServerAdministrator);
         }

         return null;
      }

      public static hMailServer.Application Application
      {
         get
         {
            return application;
         }
      }

      public static bool Authenticate(hMailServer.Application app, Settings.Server server)
      {
         string password = server.encryptedPassword;

         if (password.Length > 0)
         {
            password = Encryption.Decrypt(password);
         }

         bool wrongPassword = false;

         while (true)
         {
            if (!server.savePassword || wrongPassword)
            {
               // The user must input the password.
               formEnterPassword dlg = new formEnterPassword();
               if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.Cancel)
                  return false;

               password = dlg.Password;
            }

            try
            {
               hMailServer.Account account = app.Authenticate(server.userName, password);

               if (account == null)
               {
                  // Wrong password, try again.
                  MessageBox.Show("The specified user name or password is incorrect.", EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK);

                  wrongPassword = true;
               }
               else
               {
                   try
                   {
                       if (account.AdminLevel != eAdminLevel.hAdminLevelServerAdmin)
                       {
                           // Wrong password, try again.
                           MessageBox.Show("hMailServer server administration rights are required to run hMailServer Administrator.", EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK, MessageBoxIcon.Warning);

                           return false;
                       }
                       return true;
                   }
                   finally
                   {
                       Marshal.ReleaseComObject(account);
                   }
               }

            }
            catch (Exception e)
            {
               // Wrong password, try again.
               MessageBox.Show("The specified user name or password is incorrect." + Environment.NewLine + e.Message, EnumStrings.hMailServerAdministrator, MessageBoxButtons.OK);

               wrongPassword = true;
            }


         }
      }

      public static hMailServer.Settings Settings
      {
          get
          {
              return application.Settings;
          }
      }

      public static hMailServer.AntiSpam AntiSpamSettings
      {
          get
          {
              hMailServer.Settings settings = Application.Settings;

              hMailServer.AntiSpam antiSpam = settings.AntiSpam;

              Marshal.ReleaseComObject(settings);

              return antiSpam;
          }
      }

      public static hMailServer.TCPIPPorts TCPIPPortsSettings
      {
          get
          {
              hMailServer.Settings settings = Application.Settings;

              hMailServer.TCPIPPorts tcpIPPorts = settings.TCPIPPorts;

              Marshal.ReleaseComObject(settings);

              return tcpIPPorts;
          }
      }

      public static hMailServer.DNSBlackLists DNSBlackLists
      {
          get
          {
              hMailServer.Settings settings = Application.Settings;
              hMailServer.AntiSpam antiSpam = settings.AntiSpam;
              hMailServer.DNSBlackLists dnsBlackLists = antiSpam.DNSBlackLists;

              Marshal.ReleaseComObject(settings);
              Marshal.ReleaseComObject(antiSpam);

              return dnsBlackLists;
          }
      }

      public static hMailServer.SURBLServers SURBLServers
      {
          get
          {
              hMailServer.Settings settings = Application.Settings;
              hMailServer.AntiSpam antiSpam = settings.AntiSpam;
              hMailServer.SURBLServers surblServers = antiSpam.SURBLServers;

              Marshal.ReleaseComObject(settings);
              Marshal.ReleaseComObject(antiSpam);

              return surblServers;
          }
      }

      public static hMailServer.Groups Groups
      {
          get
          {
              hMailServer.Settings settings = Application.Settings;
              hMailServer.Groups groups = settings.Groups;

              Marshal.ReleaseComObject(settings);
              
              return groups;
          }
      }

      public static hMailServer.SecurityRanges SecurityRanges
      {
          get
          {
              hMailServer.Settings settings = Application.Settings;
              hMailServer.SecurityRanges secRanges = settings.SecurityRanges;

              Marshal.ReleaseComObject(settings);

              return secRanges;
          }
      }

      public static hMailServer.Routes Routes
      {
          get
          {
              hMailServer.Settings settings = Application.Settings;
              hMailServer.Routes routes = settings.Routes;

              Marshal.ReleaseComObject(settings);

              return routes;
          }
      }

      public static hMailServer.GreyListingWhiteAddresses GreylistingWhiteAddresses
      {
          get
          {
              hMailServer.Settings settings = APICreator.Application.Settings;
              hMailServer.AntiSpam antiSpamSettings = settings.AntiSpam;
              hMailServer.GreyListingWhiteAddresses whiteAddresses = antiSpamSettings.GreyListingWhiteAddresses;

              Marshal.ReleaseComObject(settings);
              Marshal.ReleaseComObject(antiSpamSettings);

              return whiteAddresses;

          }
      }

      public static hMailServer.Links Links
      {
          get
          {
              hMailServer.Links links = APICreator.Application.Links;
              
              return links;

          }
      }

      public static hMailServer.Domain GetDomain(int domainID)
      {
          hMailServer.Links links = APICreator.Application.Links;

          hMailServer.Domain domain = links.get_Domain(domainID);

          Marshal.ReleaseComObject(links);

          return domain;
      }

   }
}
