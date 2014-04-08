// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace MercuryMigrate
{
   static class Globals
   {
      static hMailServer.Application _application;

      static public List<string> ValidDomains { get; set; }
      static public List<MercuryUser> ValidUsers { get; set; }

      static public string PrimaryDomain { get; set; }
      static public List<MercuryUser> SelectedUsers { get; set; }

      static Globals()
      {
         ValidDomains = new List<string>();
         ValidUsers = new List<MercuryUser>();

         PrimaryDomain = "";
         SelectedUsers = new List<MercuryUser>();
      }

      static public List<KeyValuePair<string, string>> GetSettingsFromMercuryIni(string file, string section)
      {
         List<KeyValuePair<string, string>> result = new List<KeyValuePair<string, string>>();

         string content = File.ReadAllText(file);

         string [] lines = Microsoft.VisualBasic.Strings.Split(content, Environment.NewLine, -1, Microsoft.VisualBasic.CompareMethod.Text);

         bool correctSection = false;
         foreach (string line in lines)
         {
            string trimmedLine = line.Trim();

            if (trimmedLine.StartsWith("#"))
               continue;

            if (trimmedLine.StartsWith("["))
            {
               if (result.Count > 0)
               {
                  // We're done.
                  break;
               }

               if (trimmedLine.StartsWith("[" + section + "]"))
               {
                  correctSection = true;
               }
            }

            if (!correctSection)
               continue;

            int colonPos = trimmedLine.IndexOf(":");
            if (colonPos < 0)
            {
               // Not a setting line.
               continue;
            }

            string settingName = trimmedLine.Substring(0, colonPos).Trim();
            string settingValue = trimmedLine.Substring(colonPos + 1).Trim();
            
            int hashPos = settingValue.IndexOf("#");
            if (hashPos > 0)
               settingValue = settingValue.Substring(0, hashPos);

            result.Add(new KeyValuePair<string, string>(settingName, settingValue));
         }

         return result;
      }

      static public List<MercuryUser> GetUsers(string mercuryDir)
      {
         string mailDir = Path.Combine(mercuryDir, "MAIL");
         string usersFile = Path.Combine(mailDir, "PMAIL.USR");
         
         List<MercuryUser> result = new List<MercuryUser>();

         string content = File.ReadAllText(usersFile);
         string[] lines = Microsoft.VisualBasic.Strings.Split(content, Environment.NewLine, -1, Microsoft.VisualBasic.CompareMethod.Text);

         foreach (string line in lines)
         {
            string trimmedLine = line.Trim();
            if (trimmedLine.Length <= 3)
               continue;

            string[] userProperties = trimmedLine.Split(';');

            if (userProperties.Length > 1)
            {
               MercuryUser user = new MercuryUser();
               user.Name = userProperties[1];
               user.Password = GetMercuryUserPassword(mercuryDir, user.Name);
               result.Add(user);
            }
         }

         return result;
      }

      static public string GetMercuryUserPassword(string mercuryDir, string userName)
      {
         string mailDir = Path.Combine(mercuryDir, "MAIL");
         string userDir = Path.Combine(mailDir, userName);
         string passwordFile = Path.Combine(userDir, "PASSWD.PM");

         if (!File.Exists(passwordFile))
         {
            string message = string.Format("The password file {0} for user {1} does not exist.", passwordFile, userName);
            throw new Exception(message);
         }

         string contents = File.ReadAllText(passwordFile);

         string[] lines = Microsoft.VisualBasic.Strings.Split(contents, Environment.NewLine, -1, Microsoft.VisualBasic.CompareMethod.Text);

         foreach (string line in lines)
         {
            string trimmedLine = line.Trim();
            if (trimmedLine.Length <= 3)
               continue;

            string[] passwordProperties = trimmedLine.Split(':');

            if (passwordProperties.Length < 2)
               continue;

            if (String.Compare(passwordProperties[0], "POP3_access", true) == 0)
            {
               string password = passwordProperties[1].Trim();
               return password;
            }
         }

         string passwordNotFoundMessage = string.Format("The password for user user {0} could not be determined.", userName);
         throw new Exception(passwordNotFoundMessage);
      }


      public static void SetApp(hMailServer.Application application)
      {
         _application = application;
      }

      public static hMailServer.Application GetApp()
      {
         return _application;
      }


      
   }
}
