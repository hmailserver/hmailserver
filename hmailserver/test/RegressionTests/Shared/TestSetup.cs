// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Mail;
using System.Net.Sockets;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.ServiceProcess;
using System.Text;
using System.Threading;
using NUnit.Framework;
using hMailServer;
using RegressionTests.Infrastructure;

namespace RegressionTests.Shared
{
   public class TestSetup
   {
      private Application application;
      private Settings _settings;

      private static int _freePort = 20000;

      public TestSetup()
      {

      }

      public void Authenticate()
      {
         application = new Application();

         Account account = application.Authenticate("Administrator", "testar");

         if (account == null)
            account = application.Authenticate("Administrator", "");

         if (account == null)
            Assert.Fail("hMailServer API authentication failed");

         _settings = application.Settings;
      }

      public void RemoveAllRoutes()
      {
         Routes routes = _settings.Routes;
         while (routes.Count > 0)
            routes[0].Delete();
      }

      public static string GethMailServerCOMIPaddress()
      {
         IPAddress[] addresses = Dns.GetHostEntry("mail.hmailserver.com").AddressList;

         return addresses[0].ToString();
      }


      public Domain PerformBasicSetup()
      {
         bool restartRequired = false;

         Domain domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         _settings.TCPIPPorts.SetDefault();

         _settings.SecurityRanges.SetDefault();

         SetupBlockedAttachments();
         DisableSpamProtection();
         DisableVirusProtection();
         RemoveAllRoutes();
         RemoveAllRules();
         RemoveAllSharedFolders();
         RemoveAllGroups();
         ClearGreyListingWhiteAddresses();
         EnableLogging(true);

         _settings.SSLCertificates.Clear();
         
         if (_settings.AutoBanOnLogonFailure)
            _settings.AutoBanOnLogonFailure = false;

         if (_settings.SMTPNoOfTries != 0)
            _settings.SMTPNoOfTries = 0;

         if (_settings.SMTPMinutesBetweenTry != 60)
            _settings.SMTPMinutesBetweenTry = 60;

         if (_settings.Scripting.Enabled)
            _settings.Scripting.Enabled = false;

         if (_settings.MirrorEMailAddress != "")
            _settings.MirrorEMailAddress = "";

         if (_settings.SMTPRelayer != "")
            _settings.SMTPRelayer = "";

         if (_settings.SMTPRelayerConnectionSecurity != eConnectionSecurity.eCSNone)
            _settings.SMTPRelayerConnectionSecurity = eConnectionSecurity.eCSNone;

         if (_settings.MaxDeliveryThreads != 50)
            _settings.MaxDeliveryThreads = 50;

         if (_settings.Scripting.Language != "VBScript")
            _settings.Scripting.Language = "VBScript";

         if (_settings.IMAPPublicFolderName != "#Public")
            _settings.IMAPPublicFolderName = "#Public";

         if (_settings.MaxNumberOfInvalidCommands != 3)
            _settings.MaxNumberOfInvalidCommands = 3;

         if (_settings.DisconnectInvalidClients)
            _settings.DisconnectInvalidClients = false;

         if (_settings.MaxSMTPRecipientsInBatch != 100)
            _settings.MaxSMTPRecipientsInBatch = 100;

         if (_settings.IMAPHierarchyDelimiter != ".")
            _settings.IMAPHierarchyDelimiter = ".";

         if (_settings.IMAPACLEnabled != true)
            _settings.IMAPACLEnabled = true;

         if (_settings.MaxMessageSize != 20480)
            _settings.MaxMessageSize = 20480;

         if (_settings.MaxNumberOfMXHosts != 15)
            _settings.MaxNumberOfMXHosts = 15;

         if (_settings.VerifyRemoteSslCertificate)
            _settings.VerifyRemoteSslCertificate = false;

         var expectedCipherList = GetCipherList();

         if (_settings.SslCipherList != expectedCipherList)
         {
            restartRequired = true;
            _settings.SslCipherList = expectedCipherList;
         }

         if (_settings.MaxSMTPConnections > 0)
            _settings.MaxSMTPConnections = 0;
         if (_settings.MaxIMAPConnections > 0)
            _settings.MaxIMAPConnections = 0;
         if (_settings.MaxPOP3Connections > 0)
            _settings.MaxPOP3Connections = 0;

         if (!_settings.TlsVersion10Enabled)
         {
            _settings.TlsVersion10Enabled = true;
            restartRequired = true;
         }

         if (!_settings.TlsVersion11Enabled)
         {
            _settings.TlsVersion11Enabled = true;
            restartRequired = true;
         }

         if (!_settings.TlsVersion12Enabled)
         {
            _settings.TlsVersion12Enabled = true;
            restartRequired = true;
         }

         if (!_settings.TlsVersion13Enabled)
         {
             _settings.TlsVersion13Enabled = true;
             restartRequired = true;
         }


         hMailServer.AntiVirus antiVirus = _settings.AntiVirus;

         if (antiVirus.ClamAVEnabled)
            antiVirus.ClamAVEnabled = false;

         if (antiVirus.ClamAVPort != 3310)
            antiVirus.ClamAVPort = 3310;

         if (antiVirus.ClamAVHost != "localhost")
            antiVirus.ClamAVHost = "localhost";

         EnableLogging(true);

         CustomAsserts.AssertNoReportedError();

         if (File.Exists(LogHandler.GetEventLogFileName()))
            File.Delete(LogHandler.GetEventLogFileName());

         if (application.ServerState == eServerState.hStateStopped)
            application.Start();
         else if (application.ServerState == eServerState.hStateRunning)
         {
            if (restartRequired)
            {
               application.Stop();
               application.Start();
            }
         }
         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         return domain;
      }

      private string GetCipherList()
      {
         return
            "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:AES128-GCM-SHA256:AES256-GCM-SHA384:ECDHE-RSA-RC4-SHA:ECDHE-ECDSA-RC4-SHA:AES128:AES256:RC4-SHA:HIGH:!aNULL:!eNULL:!EXPORT:!DES:!3DES:!MD5:!PSK;";
      }

      private void SetupBlockedAttachments()
      {
         var antiVirusSettings = _settings.AntiVirus;

         bool blockExists = false;
         for (int i = 0; i < antiVirusSettings.BlockedAttachments.Count; i++)
         {
            var item = antiVirusSettings.BlockedAttachments[i];

            if (item.Wildcard == "*.bat")
            {
               blockExists = true;
               break;
            }
         }

         if (blockExists == false)
         {
            var item = antiVirusSettings.BlockedAttachments.Add();
            item.Description = "Batch scripts";
            item.Wildcard = "*.bat";
            item.Save();
         }
      }


      private void EnableLogging(bool enable)
      {
         Logging logging = _settings.Logging;

         if (logging.AWStatsEnabled != enable)
            logging.AWStatsEnabled = enable;

         if (logging.Enabled != enable)
            logging.Enabled = enable;

         if (logging.LogApplication != enable)
            logging.LogApplication = enable;

         if (logging.LogDebug != enable)
            logging.LogDebug = enable;

         if (logging.LogIMAP != enable)
            logging.LogIMAP = enable;

         if (logging.LogPOP3 != enable)
            logging.LogPOP3 = enable;

         if (logging.LogSMTP != enable)
            logging.LogSMTP = enable;

         if (logging.LogTCPIP != enable)
            logging.LogTCPIP = enable;

         logging.KeepFilesOpen = false;
      }

      private void DisableVirusProtection()
      {
         _settings.AntiVirus.EnableAttachmentBlocking = false;
      }

      private void RemoveAllSharedFolders()
      {
         IMAPFolders folders = _settings.PublicFolders;
         while (folders.Count > 0)
            folders.DeleteByDBID(folders[0].ID);

         string publicFolderPath = Path.Combine(_settings.Directories.DataDirectory, "#Public");
         if (Directory.Exists(publicFolderPath))
            Directory.Delete(publicFolderPath, true);
      }

      private void RemoveAllGroups()
      {
         Groups groups = _settings.Groups;
         while (groups.Count > 0)
            groups.DeleteByDBID(groups[0].ID);
      }


      private void ClearGreyListingWhiteAddresses()
      {
         GreyListingWhiteAddresses addresses = _settings.AntiSpam.GreyListingWhiteAddresses;
         while (addresses.Count > 0)
            addresses.DeleteByDBID(addresses[0].ID);
      }


      public static void SendMessagesInQueue()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         DeliveryQueue queue = application.GlobalObjects.DeliveryQueue;
         Status status = application.Status;

         string[] messages = status.UndeliveredMessages.Split('\n');
         foreach (string message in messages)
         {
            if (message.Length < 10)
               continue;

            string[] info = message.Split('\t');
            Int64 id = Convert.ToInt64(info[0]);

            queue.ResetDeliveryTime(id);
         }

         application.SubmitEMail();
      }


      public static void DeleteMessagesInQueue()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();
         DeliveryQueue queue = application.GlobalObjects.DeliveryQueue;
         Status status = application.Status;

         string[] messages = status.UndeliveredMessages.Split('\n');
         foreach (string message in messages)
         {
            if (message.Length < 10)
               continue;

            string[] info = message.Split('\t');
            int id = Convert.ToInt32(info[0]);

            queue.Remove(id);
         }

         application.SubmitEMail();
      }

      public static int GetNumberOfMessagesInDeliveryQueue()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         DeliveryQueue queue = application.GlobalObjects.DeliveryQueue;
         Status status = application.Status;

         string messages = status.UndeliveredMessages;

         if (messages.Length < 4)
            return 0;

         string[] messageList = status.UndeliveredMessages.Split('\n');

         int count = 0;
         foreach (string message in messageList)
         {
            if (message.Length < 4)
               continue;

            string recipients = message.Split('\t')[3];

            string[] recipientList = recipients.Split(',');

            count += recipientList.Length;
         }

         return count;
      }

      private void RemoveAllRules()
      {
         while (application.Rules.Count > 0)
            application.Rules[0].Delete();
      }

      public Application GetApp()
      {
         return application;
      }

      public void ClearDomains()
      {
         Domains domains = application.Domains;

         while (domains.Count 
            > 0)
         {
            Domain domain = domains[0];
            domain.Delete();
         }
      }

      public void DisableSpamProtection()
      {
         hMailServer.AntiSpam antiSpam = _settings.AntiSpam;

         if (antiSpam.SpamMarkThreshold != 10000)
            antiSpam.SpamMarkThreshold = 10000;

         if (antiSpam.SpamDeleteThreshold != 10000)
            antiSpam.SpamDeleteThreshold = 10000;

         if (antiSpam.CheckHostInHelo)
            antiSpam.CheckHostInHelo = false;

         if (antiSpam.GreyListingEnabled)
            antiSpam.GreyListingEnabled = false;

         if (antiSpam.BypassGreylistingOnMailFromMX)
            antiSpam.BypassGreylistingOnMailFromMX = false;

         if (antiSpam.SpamAssassinEnabled)
            antiSpam.SpamAssassinEnabled = false;

         if (antiSpam.TarpitCount != 0)
            antiSpam.TarpitCount = 0;

         if (antiSpam.TarpitDelay != 0)
            antiSpam.TarpitDelay = 0;

         if (antiSpam.UseMXChecks)
            antiSpam.UseMXChecks = false;

         if (antiSpam.UseSPF)
            antiSpam.UseSPF = false;

         if (antiSpam.MaximumMessageSize != 1024)
            antiSpam.MaximumMessageSize = 1024;

         if (antiSpam.DKIMVerificationEnabled)
            antiSpam.DKIMVerificationEnabled = false;

         antiSpam.WhiteListAddresses.Clear();

         for (int i = 0; i < antiSpam.DNSBlackLists.Count; i++)
         {
            DNSBlackList list = antiSpam.DNSBlackLists[i];
            if (list.Active)
            {
               list.Active = false;
               list.Save();
            }
         }

         DNSBlackLists dnsBlackLists = antiSpam.DNSBlackLists;
         while (dnsBlackLists.Count > 0)
            dnsBlackLists.DeleteByDBID(dnsBlackLists[0].ID);

         SURBLServers surblServers = antiSpam.SURBLServers;

         for (int i = surblServers.Count - 1; i >= 0; i--)
         {
            if (surblServers[i].DNSHost != "multi.surbl.org")
               surblServers.DeleteByDBID(surblServers[i].ID);
            else
               surblServers[i].Active = false;
         }

         if (surblServers.Count == 0)
         {
            SURBLServer surblServer = surblServers.Add();
            surblServer.Active = false;
            surblServer.DNSHost = "multi.surbl.org";
            surblServer.RejectMessage = "Nope";
            surblServer.Score = 5;
            surblServer.Save();
         }

         IncomingRelays incomingRelays = _settings.IncomingRelays;
         while (incomingRelays.Count > 0)
            incomingRelays.DeleteByDBID(incomingRelays[0].ID);

         antiSpam.ClearGreyListingTriplets();
      }

      public Domain AddTestDomain()
      {
         ClearDomains();

         Domain domain = AddDomain(application.Domains, "test.com");
         return domain;
      }

      public Alias AddAlias(Domain domain, string sName, string sValue)
      {
         Alias oAlias = domain.Aliases.Add();
         oAlias.Name = sName;
         oAlias.Value = sValue;
         oAlias.Active = true;
         oAlias.Save();

         return oAlias;
      }

      public Group AddGroup(string sName)
      {
         Group group = GetApp().Settings.Groups.Add();
         group.Name = sName;
         group.Save();
         return group;
      }

      public GroupMember AddGroupMember(Group group, Account account)
      {
         GroupMember member = group.Members.Add();
         member.AccountID = account.ID;
         member.Save();

         return member;
      }


      public Account AddAccount(Domain oDomain, string sAddress, string sPassword)
      {
         return AddAccount(oDomain.Accounts, sAddress, sPassword);
      }

      public Account AddAccount(Accounts accounts, string sAddress, string sPassword)
      {
         Account oAccount = accounts.Add();
         oAccount.Address = sAddress;
         oAccount.Password = sPassword;
         oAccount.Active = true;
         oAccount.Save();

         return oAccount;
      }

      public Account AddAccount(Domain oDomain, string sAddress, string sPassword, int maxSize)
      {
         Account oAccount = oDomain.Accounts.Add();
         oAccount.Address = sAddress;
         oAccount.Password = sPassword;
         oAccount.Active = true;
         oAccount.MaxSize = maxSize;
         oAccount.Save();


         return oAccount;
      }

      public Domain AddDomain(string name)
      {
         Domains domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;
         return AddDomain(domains, name);
      }

      public Domain AddDomain(Domains oDomains, string sName)
      {
         Domain oDomain = oDomains.Add();
         oDomain.Name = sName;
         oDomain.Active = true;
         oDomain.Save();

         return oDomain;
      }

      public DistributionList AddDistributionList(Domain oDomain, string sAddress, List<string> recipients)
      {
         DistributionList oList = oDomain.DistributionLists.Add();
         oList.Active = true;
         oList.Address = sAddress;
         oList.Save();

         // Add recipients
         foreach (string recipient in recipients)
         {
            DistributionListRecipient oRecipient = oList.Recipients.Add();
            oRecipient.RecipientAddress = recipient;
            oRecipient.Save();

            Marshal.ReleaseComObject(oRecipient);
         }

         return oList;
      }

      public static string UniqueString()
      {
         string s = Guid.NewGuid().ToString();
         s = s.Replace("{", "");
         s = s.Replace("}", "");
         s = s.Replace("-", "");

         return s;
      }

      
      public static string ReadExistingTextFile(string fileName)
      {
         CustomAsserts.AssertFileExists(fileName, false);

         for (int i = 1; i <= 100; i++)
         {
            try
            {
               var stream = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
               var reader = new StreamReader(stream);
               string contents = reader.ReadToEnd();
               reader.Close();
               stream.Close();

               // Check that some data has actually been read.
               if (!string.IsNullOrEmpty(contents))
               {
                  return contents;
               }
            }
            catch (Exception ex)
            {
               if (i == 100)
                  throw ex;
            }

            Thread.Sleep(100);
         }

         return "";
      }

      internal static IPAddress GetLocalIpAddress()
      {
         // Connect to another local address.
         IPHostEntry iphostentry = Dns.GetHostEntry(Dns.GetHostName());

         foreach (IPAddress ipaddress in iphostentry.AddressList)
         {
            if (ipaddress.AddressFamily == AddressFamily.InterNetwork)
            {
               if (ipaddress.ToString().Contains("192.168."))
               {
                  return ipaddress;
               }
            }
         }

         Assert.Fail("No local internet address found.");
         return null;
      }

      public static string GetResource(string resourceName)
      {
         Assembly assembly = Assembly.GetExecutingAssembly();
         TextReader textReader =
            new StreamReader(assembly.GetManifestResourceStream("RegressionTests." + resourceName));
         string result = textReader.ReadToEnd();
         textReader.Close();
         return result;
      }


      public static string Escape(string input)
      {
         string escapedValue = input;

         switch (SingletonProvider<TestSetup>.Instance.GetApp().Database.DatabaseType)
         {
            case eDBtype.hDBTypeMSSQL:
            case eDBtype.hDBTypeMSSQLCE:
               break;
            case eDBtype.hDBTypeMySQL:
            case eDBtype.hDBTypePostgreSQL:
               escapedValue = escapedValue.Replace("\\", "\\\\");
               break;
            default:
               throw new Exception("Unknown database type");
         }

         return escapedValue;
      }


      public static string CreateLargeDummyMailBody()
      {
         var sb = new StringBuilder();
         for (int i = 0; i < 10000; i++)
            sb.AppendLine("0123456789012345678901234567890123456789012345678901234567890123456789");

         return sb.ToString();
      }

      public static int GetNextFreePort()
      {
         _freePort++;
         return _freePort;
      }


      internal static Route AddRoutePointingAtLocalhost(int numberOfTries, int port, bool treatSecurityAsLocal, eConnectionSecurity connectionSecurity)
      {
         // Add a route pointing at localhost
         Settings oSettings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;

         Route route = oSettings.Routes.Add();
         route.DomainName = "dummy-example.com";
         route.TargetSMTPHost = "127.0.0.1";
         route.TargetSMTPPort = port;
         route.NumberOfTries = numberOfTries;
         route.MinutesBetweenTry = 5;
         route.TreatRecipientAsLocalDomain = treatSecurityAsLocal;
         route.TreatSenderAsLocalDomain = treatSecurityAsLocal;
         route.ConnectionSecurity = connectionSecurity;
         route.Save();

         return route;
      }

      internal static Route AddRoutePointingAtLocalhost(int numberOfTries, int port, bool treatSecurityAsLocal)
      {
         return AddRoutePointingAtLocalhost(numberOfTries, port, treatSecurityAsLocal, eConnectionSecurity.eCSNone);
      }

      public static Route AddRoutePointingAtLocalhostMultipleHosts(int numberOfTries, int port)
      {
         // Add a route pointing at localhost
         Route route = AddRoutePointingAtLocalhost(numberOfTries, port, false);
         route.DomainName = "dummy-example.com";
         route.TargetSMTPHost = "127.0.0.1|127.0.0.1";
         route.TargetSMTPPort = port;
         route.NumberOfTries = numberOfTries;
         route.MinutesBetweenTry = 5;
         route.Save();

         return route;
      }

   }
}