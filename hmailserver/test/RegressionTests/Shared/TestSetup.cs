// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Mail;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Threading;
using NUnit.Framework;
using hMailServer;

namespace RegressionTests.Shared
{
   internal class TestSetup
   {
      private readonly Application application;
      private Settings _settings;

      private static int _freePort = 20000;

      public TestSetup()
      {
         application = new Application();
      }

      public void Authenticate()
      {
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


      public Domain DoBasicSetup()
      {
         if (application.ServerState == eServerState.hStateStopped)
            application.Start();

         Domain domain = SingletonProvider<TestSetup>.Instance.AddTestDomain();

         _settings.SecurityRanges.SetDefault();

         DisableSpamProtection();
         DisableVirusProtection();
         RemoveAllRoutes();
         RemoveAllRules();
         RemoveAllSharedFolders();
         RemoveAllGroups();
         ClearGreyListingWhiteAddresses();
         EnableLogging(true);


         _settings.SSLCertificates.Clear();
         _settings.TCPIPPorts.SetDefault();

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

         hMailServer.AntiVirus antiVirus = _settings.AntiVirus;

         if (antiVirus.ClamAVEnabled)
            antiVirus.ClamAVEnabled = false;

         if (antiVirus.ClamAVPort != 3310)
            antiVirus.ClamAVPort = 3310;

         if (antiVirus.ClamAVHost != "localhost")
            antiVirus.ClamAVHost = "localhost";

         EnableLogging(true);

         if (File.Exists(GetErrorLogFileName()))
         {
            string contents = File.ReadAllText(GetErrorLogFileName());
            Assert.Fail(contents);
         }

         if (File.Exists(GetEventLogFileName()))
            File.Delete(GetEventLogFileName());

         AssertRecipientsInDeliveryQueue(0);

         return domain;
      }

      public void DeleteEventLog()
      {
         AssertDeleteFile(GetEventLogFileName());
      }

      public static void AssertDeleteFile(string file)
      {
         for (int i = 0; i <= 400; i++)
         {
            if (!File.Exists(file))
               return;

            try
            {
               File.Delete(file);
               return;
            }
            catch (Exception e)
            {
               if (i == 400)
                  throw e;
            }
            Thread.Sleep(25);
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


      private static void DeleteMessagesInQueue()
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

      private static int GetNumberOfMessagesInDeliveryQueue()
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

      public static void AssertRecipientsInDeliveryQueue(int count)
      {
         AssertRecipientsInDeliveryQueue(count, true);
      }

      public static void AssertRecipientsInDeliveryQueue(int count, bool forceSend)
      {
         if (forceSend)
            SendMessagesInQueue();

         for (int i = 1; i <= 60; i++)
         {
            if (GetNumberOfMessagesInDeliveryQueue() == count)
               return;

            if (i%10 == 0 && forceSend)
               SendMessagesInQueue();

            Thread.Sleep(100);
         }

         int currentCount = GetNumberOfMessagesInDeliveryQueue();
         if (currentCount == count)
            return;

         DeleteMessagesInQueue();

         string message = string.Format(
            "Message queue does not contain correct number of messages. Actual: {0}, Expected: {1}",
            currentCount, count);
         Assert.Fail(message);
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

         while (domains.Count > 0)
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

      public void AddSpamRule(Account oAccount)
      {
         Rule oRule = oAccount.Rules.Add();
         oRule.Name = "TestRule 1";
         oRule.Active = true;

         RuleCriteria oRuleCriteria = oRule.Criterias.Add();
         oRuleCriteria.UsePredefined = false;
         oRuleCriteria.HeaderField = "Subject";
         oRuleCriteria.MatchType = eRuleMatchType.eMTContains;
         oRuleCriteria.MatchValue = "**SPAM**";
         oRuleCriteria.Save();

         // Add action
         RuleAction oRuleAction = oRule.Actions.Add();
         oRuleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         oRuleAction.IMAPFolder = "INBOX.Spam";
         oRuleAction.Save();

         // Save the rule in the database
         oRule.Save();
      }

      public void AddCorporateRule(Account oAccount)
      {
         Rule oRule = oAccount.Rules.Add();
         oRule.Name = "TestRule 2";
         oRule.Active = true;

         RuleCriteria oRuleCriteria = oRule.Criterias.Add();
         oRuleCriteria.UsePredefined = false;
         oRuleCriteria.HeaderField = "Subject";
         oRuleCriteria.MatchType = eRuleMatchType.eMTContains;
         oRuleCriteria.MatchValue = "**CORPORATE**";
         oRuleCriteria.Save();

         // Add action
         RuleAction oRuleAction = oRule.Actions.Add();
         oRuleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         oRuleAction.IMAPFolder = "INBOX.Corporate";
         oRuleAction.Save();

         // Save the rule in the database
         oRule.Save();
      }

      public void AddExactMatchRule(Account oAccount)
      {
         Rule oRule = oAccount.Rules.Add();
         oRule.Name = "TestRule 3";
         oRule.Active = true;

         RuleCriteria oRuleCriteria = oRule.Criterias.Add();
         oRuleCriteria.UsePredefined = false;
         oRuleCriteria.HeaderField = "Subject";
         oRuleCriteria.MatchType = eRuleMatchType.eMTEquals;
         oRuleCriteria.MatchValue = "CORPORATE EXACT MATCH";
         oRuleCriteria.Save();

         // Add action
         RuleAction oRuleAction = oRule.Actions.Add();
         oRuleAction.Type = eRuleActionType.eRAMoveToImapFolder;
         oRuleAction.IMAPFolder = "INBOX.Corporate";
         oRuleAction.Save();

         // Save the rule in the database
         oRule.Save();
      }

      public static string RandomString()
      {
         string s = Guid.NewGuid().ToString();
         s = s.Replace("{", "");
         s = s.Replace("}", "");
         s = s.Replace("-", "");

         return s;
      }

      public static void WriteFile(string file, string contents)
      {
         StreamWriter streamWriter;

         streamWriter = File.CreateText(file);
         streamWriter.Write(contents);
         streamWriter.Close();
      }

      public void AssertBounceMessageExistsInQueue(string bounceTo)
      {
         Status status = application.Status;
         for (int i = 0; i < 100; i++)
         {
            if (status.UndeliveredMessages.Length == 0 || status.UndeliveredMessages.Contains("\t" + bounceTo))
               return;

            Thread.Sleep(100);
         }

         Assert.Fail("Delivery queue not empty");
      }

      public string AssertLiveLogContents()
      {
         Logging logging = _settings.Logging;
         for (int i = 0; i < 40; i++)
         {
            string contents = logging.LiveLog;
            if (contents.Length > 0)
               return contents;

            Thread.Sleep(100);
         }

         return "";
      }

      public static void AssertSpamAssassinIsRunning()
      {
         Process[] processlist = Process.GetProcesses();

         foreach (Process theprocess in processlist)
         {
            if (theprocess.ProcessName == "spamd")
               return;
         }

         // Check if we can launch it...
         string spamdExecutable = @"C:\Program Files (x86)\SpamAssassin\spamd.exe";

         try
         {
            Process.Start(spamdExecutable);
         }
         catch (Exception)
         {
            Assert.Ignore("Unable to start SpamAssassin process. Is SpamAssassin installed?");
         }
      }

      public static void AssertClamDRunning()
      {
         Process[] processlist = Process.GetProcesses();

         foreach (Process theprocess in processlist)
         {
            if (theprocess.ProcessName == "clamd")
               return;
         }

         // Check if we can launch it...
         var startInfo = new ProcessStartInfo();
         startInfo.FileName = @"C:\clamav\clamd.exe";
         startInfo.WorkingDirectory = @"C:\Clamav";
         startInfo.Arguments = "--daemon";

         try
         {
            Process.Start(startInfo);

            // Wait for clamav to start up.
            for (int i = 0; i < 10; i++)
            {
               var sock = new TcpSocket();
               if (sock.Connect(3310))
                  return;
               Thread.Sleep(1000);
            }

            Assert.Fail("ClamD process not starting up.");
         }
         catch (Exception)
         {
            Assert.Ignore("Unable to start ClamD process. Is ClamAV installed?");
         }
      }

      public static Message AssertGetFirstMessage(Account account, string folderName)
      {
         IMAPFolder folder = account.IMAPFolders.get_ItemByName(folderName);

         // Wait for message to appear.
         AssertFolderMessageCount(folder, 1);

         // return the message.
         return folder.Messages[0];
      }

      public static void AssertFolderMessageCount(IMAPFolder folder, int expectedCount)
      {
         if (expectedCount == 0)
         {
            // just in case.
            AssertRecipientsInDeliveryQueue(0);
         }

         int currentCount = 0;
         int timeout = 100;
         while (timeout > 0)
         {
            currentCount = folder.Messages.Count;

            if (currentCount == expectedCount)
               return;

            timeout--;
            Thread.Sleep(100);
         }

         string error = "Wrong number of messages in mailbox " + folder.Name;
         Assert.Fail(error);
      }

      public static Message AssertRetrieveFirstMessage(IMAPFolder folder)
      {
         int timeout = 100;
         while (timeout > 0)
         {
            if (folder.Messages.Count > 0)
            {
               return folder.Messages[0];
            }

            timeout--;
            Thread.Sleep(100);
         }

         string error = "Could not retrieve message from folder";
         Assert.Fail(error);

         return null;
      }

      public static IMAPFolder AssertFolderExists(IMAPFolders folders, string folderName)
      {
         int timeout = 100;
         while (timeout > 0)
         {
            try
            {
               return folders.get_ItemByName(folderName);
            }
            catch (Exception)
            {
            }

            timeout--;
            Thread.Sleep(100);
         }

         string error = "Folder could not be found " + folderName;
         Assert.Fail(error);
         return null;
      }

      public static string GetErrorLogFileName()
      {
         return SingletonProvider<TestSetup>.Instance.GetApp().Settings.Logging.CurrentErrorLog;
      }

      public static string GetDefaultLogFileName()
      {
         return SingletonProvider<TestSetup>.Instance.GetApp().Settings.Logging.CurrentDefaultLog;
      }

      public static void DeleteCurrentDefaultLog()
      {
         for (int i = 0; i < 50; i++)
         {
            try
            {
               string filename = GetDefaultLogFileName();
               if (File.Exists(filename))
                  File.Delete(filename);

               return;
            }
            catch (Exception)
            {
               Thread.Sleep(100);
            }
         }

         Assert.Fail("Failed to delete default log file during test");
      }

      public static string ReadCurrentDefaultLog()
      {
         string filename = GetDefaultLogFileName();
         string content = string.Empty;
         if (File.Exists(filename))
            return ReadExistingTextFile(filename);

         return content;
      }

      public static bool DefaultLogContains(string data)
      {
         string filename = GetDefaultLogFileName();

         for (int i = 0; i < 40; i++)
         {
            if (File.Exists(filename))
            {
               string content = ReadExistingTextFile(filename);
               if (content.Contains(data))
                  return true;
            }

            Thread.Sleep(250);
         }

         return false;
      }

      public static string GetEventLogFileName()
      {
         return SingletonProvider<TestSetup>.Instance.GetApp().Settings.Logging.CurrentEventLog;
      }

      public static string ReadExistingTextFile(string fileName)
      {
         AssertFileExists(fileName, false);

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

      public static void AssertFileExists(string file, bool delete)
      {
         int timeout = 100;
         while (timeout > 0)
         {
            try
            {
               if (File.Exists(file))
               {
                  if (delete)
                     File.Delete(file);

                  return;
               }
            }
            catch (Exception)
            {
            }

            timeout--;
            Thread.Sleep(100);
         }

         Assert.Fail("Expected file does not exist:" + file);
      }

      public static void AssertReportedError()
      {
         string file = GetErrorLogFileName();
         AssertFileExists(file, true);
      }

      public static void AssertReportedError(string content)
      {
         string errorLog = ReadAndDeleteErrorLog();
         Assert.IsTrue(errorLog.Contains(content), errorLog);
      }


      public static string ReadAndDeleteErrorLog()
      {
         string file = GetErrorLogFileName();
         AssertFileExists(file, false);

         string contents = File.ReadAllText(file);

         File.Delete(file);

         return contents;
      }

      public void AssertFilesInUserDirectory(Account account, int expectedFileCount)
      {
         string domain = account.Address.Substring(account.Address.IndexOf("@") + 1);
         string mailbox = account.Address.Substring(0, account.Address.IndexOf("@"));

         string domainDir = Path.Combine(_settings.Directories.DataDirectory, domain);
         string userDir = Path.Combine(domainDir, mailbox);

         AssertFilesInDirectory(userDir, expectedFileCount);
      }

      public void AssertFilesInDirectory(string directory, int expectedFileCount)
      {
         int count = 0;

         if (Directory.Exists(directory))
         {
            string[] dirs = Directory.GetDirectories(directory);

            foreach (string dir in dirs)
            {
               string[] files = Directory.GetFiles(dir);
               count += files.Length;
            }
         }

         Assert.AreEqual(expectedFileCount, count);
      }

      public static string GetCurrentMIMEDateTime()
      {
         DateTime now = DateTime.Now;

         string dayOfWeek = "";
         switch (now.DayOfWeek)
         {
            case DayOfWeek.Monday:
               dayOfWeek = "Mon";
               break;
            case DayOfWeek.Tuesday:
               dayOfWeek = "Tue";
               break;
            case DayOfWeek.Wednesday:
               dayOfWeek = "Wed";
               break;
            case DayOfWeek.Thursday:
               dayOfWeek = "Thu";
               break;
            case DayOfWeek.Friday:
               dayOfWeek = "Fri";
               break;
            case DayOfWeek.Saturday:
               dayOfWeek = "Sat";
               break;
            case DayOfWeek.Sunday:
               dayOfWeek = "Sun";
               break;
         }

         string monthName = "";
         switch (now.Month)
         {
            case 1:
               monthName = "Jan";
               break;
            case 2:
               monthName = "Feb";
               break;
            case 3:
               monthName = "Mar";
               break;
            case 4:
               monthName = "Apr";
               break;
            case 5:
               monthName = "May";
               break;
            case 6:
               monthName = "Jun";
               break;
            case 7:
               monthName = "Jul";
               break;
            case 8:
               monthName = "Aug";
               break;
            case 9:
               monthName = "Sep";
               break;
            case 10:
               monthName = "Oct";
               break;
            case 11:
               monthName = "Nov";
               break;
            case 12:
               monthName = "Dec";
               break;
         }

         string timeString = now.ToString("HH':'mm':'ss");
         string dateString = string.Format("{0}, {1} {2} {3} {4} +0100", dayOfWeek, now.Day, monthName, now.Year,
                                           timeString);

         return dateString;
      }

      internal static IPAddress GetLocalIPAddress()
      {
         // Connect to another local address.
         IPHostEntry iphostentry = Dns.GetHostEntry(Dns.GetHostName());

         foreach (IPAddress ipaddress in iphostentry.AddressList)
         {
            if (ipaddress.ToString().Contains(".") && !ipaddress.ToString().Contains("127.0.0"))
            {
               return ipaddress;
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

      public static string GetFileHash(string fileName)
      {
         byte[] bytes = File.ReadAllBytes(fileName);
         SHA1 sha = new SHA1CryptoServiceProvider();
         var hash = new StringBuilder();

         byte[] hashedData = sha.ComputeHash(bytes);

         foreach (byte b in hashedData)
         {
            hash.Append(string.Format("{0,2:X2}", b));
         }

         //return the hashed value
         return hash.ToString();
      }

      public static void SendMessage(MailMessage mailMessage)
      {
         for (int i = 0; i < 5; i++)
         {
            try
            {
               var client = new SmtpClient("localhost", 25);
               client.Send(mailMessage);

               return;
            }
            catch
            {
               if (i == 4)
                  throw;
            }
         }
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

      public string GetPublicDirectory()
      {
         string dataDir = _settings.Directories.DataDirectory;
         string publicDir = Path.Combine(dataDir, _settings.PublicFolderDiskName);
         return publicDir;
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
   }
}