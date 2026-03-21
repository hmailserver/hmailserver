// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;

namespace RegressionTests.Shared
{
   public class TestSetup
   {
      private static int _freePort = 20000;
      private Settings _settings;
      private Application application;

      public void Authenticate()
      {
         application = new Application();

         var account = application.Authenticate("Administrator", "testar");

         if (account == null)
            account = application.Authenticate("Administrator", "");

         if (account == null)
            Assert.Fail("hMailServer API authentication failed");

         _settings = application.Settings;
      }

      public void RemoveAllRoutes()
      {
         var routes = _settings.Routes;
         while (routes.Count > 0)
            routes[0].Delete();
      }

      public static string GethMailServerCOMIPaddress()
      {
         var addresses = Dns.GetHostEntry("mail.hmailserver.com").AddressList;

         return addresses[0].ToString();
      }


      public Domain PerformBasicSetup()
      {
         var restartRequired = false;

         var domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

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

         if (_settings.TCPIPThreads != 15)
         {
            _settings.TCPIPThreads = 15;
            restartRequired = true;
         }

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

         if (_settings.SMTPDeliveryBindToIP != "")
            _settings.SMTPDeliveryBindToIP = "";

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

         if (_settings.IMAPSASLPlainEnabled)
            _settings.IMAPSASLPlainEnabled = false;
         if (_settings.IMAPSASLInitialResponseEnabled)
            _settings.IMAPSASLInitialResponseEnabled = false;
         if (_settings.IMAPAuthAllowPlainText)
            _settings.IMAPAuthAllowPlainText = false;
         if (!string.IsNullOrEmpty(_settings.IMAPMasterUser))
            _settings.IMAPMasterUser = string.Empty;

         if (!string.IsNullOrEmpty(_settings.SslCipherList))
         {
            restartRequired = true;
            _settings.SslCipherList = string.Empty;
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


         var antiVirus = _settings.AntiVirus;

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
            if (restartRequired)
            {
               application.Stop();
               application.Start();
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

         var blockExists = false;
         for (var i = 0; i < antiVirusSettings.BlockedAttachments.Count; i++)
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
         var logging = _settings.Logging;

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
         var folders = _settings.PublicFolders;
         while (folders.Count > 0)
            folders.DeleteByDBID(folders[0].ID);

         var publicFolderPath = Path.Combine(_settings.Directories.DataDirectory, "#Public");
         if (Directory.Exists(publicFolderPath))
            Directory.Delete(publicFolderPath, true);
      }

      private void RemoveAllGroups()
      {
         var groups = _settings.Groups;
         while (groups.Count > 0)
            groups.DeleteByDBID(groups[0].ID);
      }


      private void ClearGreyListingWhiteAddresses()
      {
         var addresses = _settings.AntiSpam.GreyListingWhiteAddresses;
         while (addresses.Count > 0)
            addresses.DeleteByDBID(addresses[0].ID);
      }


      public static void SendMessagesInQueue()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();
         var queue = application.GlobalObjects.DeliveryQueue;
         var status = application.Status;

         var messages = status.UndeliveredMessages.Split('\n');
         foreach (var message in messages)
         {
            if (message.Length < 10)
               continue;

            var info = message.Split('\t');
            var id = Convert.ToInt64(info[0]);

            queue.ResetDeliveryTime(id);
         }

         application.SubmitEMail();
      }


      public static void DeleteMessagesInQueue()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();
         var queue = application.GlobalObjects.DeliveryQueue;
         var status = application.Status;

         var messages = status.UndeliveredMessages.Split('\n');
         foreach (var message in messages)
         {
            if (message.Length < 10)
               continue;

            var info = message.Split('\t');
            var id = Convert.ToInt32(info[0]);

            queue.Remove(id);
         }

         application.SubmitEMail();
      }

      public static int GetNumberOfMessagesInDeliveryQueue()
      {
         var application = SingletonProvider<TestSetup>.Instance.GetApp();

         var queue = application.GlobalObjects.DeliveryQueue;
         var status = application.Status;

         var messages = status.UndeliveredMessages;

         if (messages.Length < 4)
            return 0;

         var messageList = status.UndeliveredMessages.Split('\n');

         var count = 0;
         foreach (var message in messageList)
         {
            if (message.Length < 4)
               continue;

            var recipients = message.Split('\t')[3];

            var recipientList = recipients.Split(',');

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
         var domains = application.Domains;

         while (domains.Count
                > 0)
         {
            var domain = domains[0];
            domain.Delete();
         }
      }

      public void DisableSpamProtection()
      {
         var antiSpam = _settings.AntiSpam;

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

         for (var i = 0; i < antiSpam.DNSBlackLists.Count; i++)
         {
            var list = antiSpam.DNSBlackLists[i];
            if (list.Active)
            {
               list.Active = false;
               list.Save();
            }
         }

         var dnsBlackLists = antiSpam.DNSBlackLists;
         while (dnsBlackLists.Count > 0)
            dnsBlackLists.DeleteByDBID(dnsBlackLists[0].ID);

         var surblServers = antiSpam.SURBLServers;

         for (var i = surblServers.Count - 1; i >= 0; i--)
            if (surblServers[i].DNSHost != "multi.surbl.org")
               surblServers.DeleteByDBID(surblServers[i].ID);
            else
               surblServers[i].Active = false;

         if (surblServers.Count == 0)
         {
            var surblServer = surblServers.Add();
            surblServer.Active = false;
            surblServer.DNSHost = "multi.surbl.org";
            surblServer.RejectMessage = "Nope";
            surblServer.Score = 5;
            surblServer.Save();
         }

         var incomingRelays = _settings.IncomingRelays;
         while (incomingRelays.Count > 0)
            incomingRelays.DeleteByDBID(incomingRelays[0].ID);

         antiSpam.ClearGreyListingTriplets();
      }

      public Domain AddTestDomain()
      {
         ClearDomains();

         var domain = AddDomain(application.Domains, "example.test");
         return domain;
      }

      public Alias AddAlias(Domain domain, string sName, string sValue)
      {
         var alias = domain.Aliases.Add();
         alias.Name = sName;
         alias.Value = sValue;
         alias.Active = true;
         alias.Save();

         return alias;
      }

      public Group AddGroup(string sName)
      {
         var group = GetApp().Settings.Groups.Add();
         group.Name = sName;
         group.Save();
         return group;
      }

      public GroupMember AddGroupMember(Group group, Account account)
      {
         var member = group.Members.Add();
         member.AccountID = account.ID;
         member.Save();

         return member;
      }


      public Account AddAccount(Domain domain, string sAddress, string sPassword)
      {
         return AddAccount(domain.Accounts, sAddress, sPassword);
      }

      public Account AddAccount(Accounts accounts, string sAddress, string sPassword)
      {
         var account = accounts.Add();
         account.Address = sAddress;
         account.Password = sPassword;
         account.Active = true;
         account.Save();

         return account;
      }

      public Account AddAccount(Domain domain, string sAddress, string sPassword, int maxSize)
      {
         var account = domain.Accounts.Add();
         account.Address = sAddress;
         account.Password = sPassword;
         account.Active = true;
         account.MaxSize = maxSize;
         account.Save();


         return account;
      }

      public Domain AddDomain(string name)
      {
         var domains = SingletonProvider<TestSetup>.Instance.GetApp().Domains;
         return AddDomain(domains, name);
      }

      public Domain AddDomain(Domains domains, string sName)
      {
         var domain = domains.Add();
         domain.Name = sName;
         domain.Active = true;
         domain.Save();

         return domain;
      }

      public DistributionList AddDistributionList(Domain domain, string sAddress, List<string> recipients)
      {
         var list = domain.DistributionLists.Add();
         list.Active = true;
         list.Address = sAddress;
         list.Save();

         // Add recipients
         foreach (var recipientAddress in recipients)
         {
            var recipient = list.Recipients.Add();
            recipient.RecipientAddress = recipientAddress;
            recipient.Save();

            Marshal.ReleaseComObject(recipient);
         }

         return list;
      }

      public static string UniqueString()
      {
         var s = Guid.NewGuid().ToString();
         s = s.Replace("{", "");
         s = s.Replace("}", "");
         s = s.Replace("-", "");

         return s;
      }


      public static string ReadExistingTextFile(string fileName)
      {
         CustomAsserts.AssertFileExists(fileName, false);

         for (var i = 1; i <= 100; i++)
         {
            try
            {
               var stream = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
               var reader = new StreamReader(stream);
               var contents = reader.ReadToEnd();
               reader.Close();
               stream.Close();

               // Check that some data has actually been read.
               if (!string.IsNullOrEmpty(contents)) return contents;
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
         var allAddresses = new StringBuilder();

         foreach (NetworkInterface ni in NetworkInterface.GetAllNetworkInterfaces())
         {
            if (ni.OperationalStatus != OperationalStatus.Up)
               continue;

            foreach (UnicastIPAddressInformation ipInfo in ni.GetIPProperties().UnicastAddresses)
            {
               IPAddress ip = ipInfo.Address;
               allAddresses.AppendLine($"Family: {ip.AddressFamily}, Address: {ip}");

               if (ip.AddressFamily == AddressFamily.InterNetwork)
               {
                  // Example: Only private networks
                  if (IsPrivateIp(ip))
                     return ip;
               }
            }
         }

         Assert.Fail($"No local internet address found. Addresses: {allAddresses}");
         return null;
      }

      private static bool IsPrivateIp(IPAddress ip)
      {
         byte[] bytes = ip.GetAddressBytes();
         return
            (bytes[0] == 10) ||
            (bytes[0] == 172 && bytes[1] >= 16 && bytes[1] <= 31) ||
            (bytes[0] == 192 && bytes[1] == 168);
      }

      public static string GetResource(string resourceName)
      {
         var assembly = Assembly.GetExecutingAssembly();
         TextReader textReader =
            new StreamReader(assembly.GetManifestResourceStream("RegressionTests." + resourceName));
         var result = textReader.ReadToEnd();
         textReader.Close();
         return result;
      }


      public static string Escape(string input)
      {
         var escapedValue = input;

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
         for (var i = 0; i < 10000; i++)
            sb.AppendLine("0123456789012345678901234567890123456789012345678901234567890123456789");

         return sb.ToString();
      }

      public static int GetNextFreePort()
      {
         _freePort++;
         return _freePort;
      }


      internal static Route AddRoutePointingAtLocalhost(int numberOfTries, int port, bool treatSecurityAsLocal,
         eConnectionSecurity connectionSecurity)
      {
         // Add a route pointing at localhost
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;

         var route = settings.Routes.Add();
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
         var route = AddRoutePointingAtLocalhost(numberOfTries, port, false);
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