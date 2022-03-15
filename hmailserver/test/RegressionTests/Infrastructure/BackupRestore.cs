// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class BackupRestore : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         InitializeBackupSettings();
      }

      #endregion

      private string _backupDir;
      private static string _folderCreationTime;
      private bool _backupMessages = true;
      private int _fetchAccountPort = TestSetup.GetNextFreePort();

      public void InitializeBackupSettings()
      {
         _application = SingletonProvider<TestSetup>.Instance.GetApp();
         SetBackupDir(Path.Combine(Path.GetTempPath(), TestSetup.UniqueString()));

         var dirInfo = new DirectoryInfo(_backupDir);
         dirInfo.Create();

         if (File.Exists(_application.Settings.Backup.LogFile))
            File.Delete(_application.Settings.Backup.LogFile);
      }

      public void SetBackupDir(string backupDir)
      {
         _backupDir = backupDir;
      }

      internal bool Execute()
      {
         CustomAsserts.AssertDeleteFile(_application.Settings.Backup.LogFile);

         SetupEnvironment();
         if (!BackupEnvironment())
            return false;

         DeleteEnvironment();
         RestoreEnvironment();
         ConfirmRestore();

         Directory.Delete(_backupDir, true);

         return true;
      }

      private bool WaitForBackupCompletion()
      {
         Assert.IsNotNull(_application);

         for (int i = 0; i < 40; i++)
         {
            try
            {
               string logFile = _application.Settings.Backup.LogFile;

               string contents = TestSetup.ReadExistingTextFile(logFile);

               if (contents.IndexOf("Backup completed successfully") > 0)
                  return true;

               if (contents.IndexOf("BACKUP ERROR:") > 0)
               {
                  return false;
               }
            }
            catch (Exception)
            {
               // probably a share access violation.
            }

            Thread.Sleep(250);
         }

         return false;
      }

      private void WaitForRestoreCompletion(string lastServerStartTime)
      {
         Assert.IsNotNull(_application);

         for (int i = 0; i < 600; i++)
         {
            try
            {
               string startTime = _application.Status.StartTime;

               if (startTime.Length > 0 && startTime != lastServerStartTime)
                  return;
            }
            catch (Exception)
            {
               // probably a share access violation.
            }

            Thread.Sleep(100);
         }

         throw new Exception("Timeout while waiting for backup or restore completion");
      }

      public void SetupEnvironment()
      {
         // Set up the domain. It's already set-up...
         Domain domain = _application.Domains[0];
         Assert.IsNotNull(domain);
         Assert.AreEqual("test.com", domain.Name);

         SetupDomainObject(domain);
         SetupDomainObjects(domain);
         SetupSettings();
      }

      private void SetupSettings()
      {
         SetupBlockedAttachment();
         SetupGreyListingWhiteList();
         SetupWhiteList();
         SetupRoutes();
         SetupDNSBlackLists();
         SetupSURBLServers();
         SetupSSLCertificates();
         SetupGroupObject();
         SetupPublicFolders();
         SetupIncomingRelay();
      }

      private void SetupSSLCertificates()
      {
         SSLCertificate cert = _application.Settings.SSLCertificates.Add();
         cert.CertificateFile = "file1.txt";
         cert.Name = "name1";
         cert.PrivateKeyFile = "pk1";
         cert.Save();

         cert = _application.Settings.SSLCertificates.Add();
         cert.CertificateFile = "file2.txt";
         cert.Name = "name2";
         cert.PrivateKeyFile = "pk2";
         cert.Save();
      }

      private void SetupBlockedAttachment()
      {
         BlockedAttachment ba = _application.Settings.AntiVirus.BlockedAttachments.Add();
         ba.Description = "My description";
         ba.Wildcard = "*.my";
         ba.Save();
      }

      private void SetupDNSBlackLists()
      {
         DNSBlackList black = _application.Settings.AntiSpam.DNSBlackLists.Add();
         black.Active = true;
         black.DNSHost = "127.0.0.1";
         black.ExpectedResult = "127.5.2.1";
         black.RejectMessage = "Test";
         black.Score = 4;
         black.Save();

         black = _application.Settings.AntiSpam.DNSBlackLists.Add();
         black.Active = true;
         black.DNSHost = "127.0.0.2";
         black.ExpectedResult = "127.5.2.2";
         black.RejectMessage = "Test2";
         black.Score = 5;
         black.Save();
      }

      private void SetupIncomingRelay()
      {
         IncomingRelay relay = _application.Settings.IncomingRelays.Add();
         relay.Name = "Test";
         relay.LowerIP = "1.2.3.4";
         relay.UpperIP = "4.3.2.1";
         relay.Save();
      }


      private void SetupSURBLServers()
      {
         SURBLServer black = _application.Settings.AntiSpam.SURBLServers.Add();
         black.Active = true;
         black.DNSHost = "127.0.0.1";
         black.RejectMessage = "Test";
         black.Score = 4;
         black.Save();

         black = _application.Settings.AntiSpam.SURBLServers.Add();
         black.Active = true;
         black.DNSHost = "127.0.0.2";
         black.RejectMessage = "Test2";
         black.Score = 5;
         black.Save();
      }

      private void SetupGreyListingWhiteList()
      {
         GreyListingWhiteAddresses addresses = _application.Settings.AntiSpam.GreyListingWhiteAddresses;

         GreyListingWhiteAddress address = addresses.Add();
         address.Description = "helo1";
         address.IPAddress = "1.1.1.1";
         address.Save();

         address = addresses.Add();
         address.Description = "helo2";
         address.IPAddress = "2.2.2.2";
         address.Save();
      }


      private void SetupWhiteList()
      {
         WhiteListAddresses addresses = _application.Settings.AntiSpam.WhiteListAddresses;

         WhiteListAddress address = addresses.Add();
         address.Description = "Desc1";
         address.EmailAddress = "Email1";
         address.LowerIPAddress = "1.1.1.1";
         address.UpperIPAddress = "2.2.2.2";
         address.Save();

         address = addresses.Add();
         address.Description = "Desc2";
         address.EmailAddress = "Email2";
         address.LowerIPAddress = "2.2.2.2";
         address.UpperIPAddress = "21.21.21.21";
         address.Save();
      }


      private void SetupRoutes()
      {
         Routes routes = _application.Settings.Routes;

         Route route = routes.Add();
         route.DomainName = "test1.com";
         route.Description = "description";
         route.UseSSL = true;
         route.TreatSenderAsLocalDomain = true;
         route.TreatRecipientAsLocalDomain = false;
         route.Save();

         route = routes.Add();
         route.DomainName = "test2.com";
         route.Description = "description2";
         route.UseSSL = false;
         route.TreatSenderAsLocalDomain = false;
         route.TreatRecipientAsLocalDomain = true;
         route.Save();

         RouteAddresses addresses = route.Addresses;

         RouteAddress address1 = addresses.Add();
         address1.Address = "address1@test2.com";
         address1.Save();

         RouteAddress address2 = addresses.Add();
         address2.Address = "address2@test2.com";
         address2.Save();
      }

      private void SetupDomainObject(Domain domain)
      {
         domain.Postmaster = "someone@test.com";
         domain.SignatureEnabled = true;
         domain.AddSignaturesToLocalMail = true;
         domain.AddSignaturesToReplies = true;

         domain.SignaturePlainText = "PLS";
         domain.SignatureHTML = "HTML";
         domain.Save();

         DomainAlias da = domain.DomainAliases.Add();
         da.AliasName = "test1.com";
         da.Save();

         DomainAlias da2 = domain.DomainAliases.Add();
         da2.AliasName = "test2.com";
         da2.Save();
      }

      private void SetupDomainObjects(Domain domain)
      {
         SetupAccountObject(domain);
         SetupAliasObject(domain);
         SetupDistributionListObject(domain);
      }

      private void SetupPublicFolders()
      {
         IMAPFolders folders = _application.Settings.PublicFolders;
         IMAPFolder folder = folders.Add("Test1");
         folder.Save();

         folder = folders.Add("Test2");
         folder.Save();

         folder = folders.Add("Test3");
         folder.Save();

         folder = folders.Add("ACL");
         folder.Save();

         Domain domain = _application.Domains[0];
         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "acltest-1@test.com", "test");

         IMAPFolderPermission permission = folder.Permissions.Add();
         permission.PermissionType = eACLPermissionType.ePermissionTypeUser;
         permission.PermissionAccountID = account1.ID;
         permission.set_Permission(eACLPermission.ePermissionAdminister, true);
         permission.set_Permission(eACLPermission.ePermissionInsert, true);
         permission.Save();

         folder = folder.SubFolders.Add("MySubFolder");
         folder.Save();

         Group group1 = _application.Settings.Groups.Add();
         group1.Name = "ACLTestGroup";
         group1.Save();

         permission = folder.Permissions.Add();
         permission.PermissionType = eACLPermissionType.ePermissionTypeGroup;
         permission.PermissionGroupID = group1.ID;
         permission.set_Permission(eACLPermission.ePermissionLookup, true);
         permission.set_Permission(eACLPermission.ePermissionDeleteMailbox, true);
         permission.Save();
      }

      private void ConfirmPublicFolders()
      {
         if (_backupMessages)
         {
            Domain domain = _application.Domains[0];

            IMAPFolders publicFolders = _application.Settings.PublicFolders;

            Assert.IsNotNull(publicFolders.get_ItemByName("Test1"));
            Assert.IsNotNull(publicFolders.get_ItemByName("Test2"));
            Assert.IsNotNull(publicFolders.get_ItemByName("Test3"));
            Assert.IsNotNull(publicFolders.get_ItemByName("ACL"));

            IMAPFolderPermissions permissions = publicFolders.get_ItemByName("ACL").Permissions;
            Assert.AreEqual(1, permissions.Count);

            Account account1 = domain.Accounts.get_ItemByAddress("acltest-1@test.com");

            IMAPFolderPermission permission = permissions[0];

            Assert.AreEqual(eACLPermissionType.ePermissionTypeUser, permission.PermissionType);
            Assert.AreEqual(account1.ID, permission.PermissionAccountID);

            Assert.IsTrue(permission.get_Permission(eACLPermission.ePermissionAdminister));
            Assert.IsTrue(permission.get_Permission(eACLPermission.ePermissionInsert));
            Assert.IsFalse(permission.get_Permission(eACLPermission.ePermissionRead));
            Assert.IsFalse(permission.get_Permission(eACLPermission.ePermissionWriteSeen));

            Group group1 = _application.Settings.Groups.get_ItemByName("ACLTestGroup");

            permissions = publicFolders.get_ItemByName("ACL").SubFolders.get_ItemByName("MySubFolder").Permissions;
            permission = permissions[0];

            Assert.AreEqual(eACLPermissionType.ePermissionTypeGroup, permission.PermissionType);
            Assert.AreEqual(group1.ID, permission.PermissionGroupID);

            Assert.IsTrue(permission.get_Permission(eACLPermission.ePermissionLookup));
            Assert.IsTrue(permission.get_Permission(eACLPermission.ePermissionDeleteMailbox));
            Assert.IsFalse(permission.get_Permission(eACLPermission.ePermissionRead));
            Assert.IsFalse(permission.get_Permission(eACLPermission.ePermissionWriteSeen));
         }
      }

      private void SetupDistributionListObject(Domain domain)
      {
         var listRecipients = new List<string>();
         listRecipients.Add("member1@test.com");
         listRecipients.Add("member2@test.com");
         listRecipients.Add("member3@test.com");

         DistributionList list = SingletonProvider<TestSetup>.Instance.AddDistributionList(domain, "list@test.com",
                                                                                           listRecipients);
      }

      private void SetupGroupObject()
      {
         Group group = _application.Settings.Groups.Add();
         group.Name = "TestGroup";
         group.Save();

         Account gm1 = SingletonProvider<TestSetup>.Instance.AddAccount(_application.Domains[0], "gm1@test.com",
                                                                        "test");
         Account gm2 = SingletonProvider<TestSetup>.Instance.AddAccount(_application.Domains[0], "gm2@test.com",
                                                                        "test");
         Account gm3 = SingletonProvider<TestSetup>.Instance.AddAccount(_application.Domains[0], "gm3@test.com",
                                                                        "test");

         GroupMember gm = group.Members.Add();
         gm.AccountID = gm1.ID;
         gm.Save();

         gm = group.Members.Add();
         gm.AccountID = gm2.ID;
         gm.Save();

         gm = group.Members.Add();
         gm.AccountID = gm3.ID;
         gm.Save();
      }

      private void SetupAliasObject(Domain domain)
      {
         Alias alias = SingletonProvider<TestSetup>.Instance.AddAlias(domain, "alias@test.com", "someone@test.com");
      }

      private void SetupAccountObject(Domain domain)
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(domain, "test@test.com", "test");

         // Make sure the inbox contains two messages which should be backed up.
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Message 1 Subject",
            "Message 1 Body");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Message 2 Subject",
                                                      "Message 2 Body");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 2);

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Message 3 Subject",
                                                      "Message 3 Body");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 3);

         var sim = new ImapClientSimulator();
         Assert.IsTrue(sim.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(sim.SelectFolder("Inbox"));
         Assert.IsTrue(sim.SetDeletedFlag(2));
         Assert.IsTrue(sim.Expunge());
         sim.Disconnect();

         _folderCreationTime = account.IMAPFolders.get_ItemByName("INBOX").CreationTime;

         account.Active = true;
         account.ADDomain = "AD";
         account.AdminLevel = eAdminLevel.hAdminLevelDomainAdmin;
         account.ADUsername = "AU";
         account.ForwardAddress = "FA";
         account.ForwardEnabled = false;
         account.ForwardKeepOriginal = true;
         account.IsAD = false;
         account.MaxSize = 1250;
         account.PersonFirstName = "First";
         account.PersonLastName = "Last";
         account.SignatureEnabled = true;
         account.SignatureHTML = "HTML";
         account.SignaturePlainText = "PLAIN";
         account.VacationMessage = "VAC";
         account.VacationMessageExpires = true;
         account.VacationMessageExpiresDate = "2040-01-01";
         account.VacationMessageIsOn = true;
         account.VacationSubject = "SUBJ";
         account.Password = "test";
         account.Save();

         // Set up fetch account
         FetchAccount fa = account.FetchAccounts.Add();
         fa.DaysToKeepMessages = 5;
         fa.Enabled = true;
         fa.MinutesBetweenFetch = 10;
         fa.Name = "test";
         fa.Port = _fetchAccountPort;
         fa.ProcessMIMEDate = true;
         fa.ProcessMIMERecipients = true;
         fa.ServerAddress = "127.0.0.1";
         fa.Username = "test";
         fa.UseSSL = false;
         fa.UseAntiSpam = true;
         fa.UseAntiVirus = true;
         fa.Save();

         DownloadFromExternalAccount(account, fa);

         Rule rule = account.Rules.Add();
         rule.Name = "MyRule";
         RuleCriteria criteria = rule.Criterias.Add();
         criteria.MatchType = eRuleMatchType.eMTGreaterThan;
         criteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         criteria.MatchValue = "0";
         criteria.Save();

         RuleAction action = rule.Actions.Add();
         action.Type = eRuleActionType.eRAForwardEmail;
         action.To = "someone@test.com";
         action.Body = "Test";
         action.Filename = "File";
         action.FromAddress = "T";
         action.FromName = "N";
         action.HeaderName = "H";
         action.IMAPFolder = "Folder";
         action.ScriptFunction = "Script";
         action.Subject = "Subj";
         action.Value = "Value";
         action.Save();

         rule.Save();
      }

      private void DownloadFromExternalAccount(Account account, FetchAccount fa)
      {
         if (!_backupMessages)
            return;

         // Download messages from the external account.
         var messages = new List<string>();

         messages.Add("Subject: Message 1\r\n");
         messages.Add("Subject: Message 2\r\n");
         messages.Add("Subject: Message 3\r\n");

         using (var pop3Server = new Pop3ServerSimulator(1, fa.Port, messages))
         {
            pop3Server.StartListen();
            fa.DownloadNow();
            pop3Server.WaitForCompletion();
         }

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 5);
      }

      private bool BackupEnvironment()
      {
         BackupSettings oBackupSettings = _application.Settings.Backup;
         oBackupSettings.BackupDomains = true;
         oBackupSettings.BackupMessages = _backupMessages;
         oBackupSettings.BackupSettings = true;
         oBackupSettings.CompressDestinationFiles = true;

         oBackupSettings.Destination = _backupDir;

         _application.BackupManager.StartBackup();

         return WaitForBackupCompletion();
      }

      private void DeleteEnvironment()
      {
         while (_application.Domains.Count > 0)
            _application.Domains[0].Delete();

         WhiteListAddresses addresses = _application.Settings.AntiSpam.WhiteListAddresses;
         while (addresses.Count > 0)
            addresses[0].Delete();

         Routes routes = _application.Settings.Routes;
         while (routes.Count > 0)
            routes[0].Delete();

         BlockedAttachments attachments = _application.Settings.AntiVirus.BlockedAttachments;
         while (attachments.Count > 0)
            attachments[0].Delete();

         DNSBlackLists blackLists = _application.Settings.AntiSpam.DNSBlackLists;
         while (blackLists.Count > 0)
            blackLists[0].Delete();

         SURBLServers surblServers = _application.Settings.AntiSpam.SURBLServers;
         while (surblServers.Count > 0)
            surblServers[0].Delete();

         SSLCertificates sslCertificates = _application.Settings.SSLCertificates;
         while (sslCertificates.Count > 0)
            sslCertificates[0].Delete();

         IncomingRelays incomingRelays = _application.Settings.IncomingRelays;
         while (incomingRelays.Count > 0)
            incomingRelays[0].Delete();
      }

      private void RestoreEnvironment()
      {
         string startTime = _application.Status.StartTime;

         // locate backup file
         var dirInfo = new DirectoryInfo(_backupDir);
         FileInfo[] files = dirInfo.GetFiles();

         string backupFile = files[0].FullName;

         Backup backup = _application.BackupManager.LoadBackup(backupFile);

         backup.RestoreDomains = true;
         backup.RestoreMessages = _backupMessages;
         backup.RestoreSettings = true;
         backup.StartRestore();

         WaitForRestoreCompletion(startTime);
      }

      private void ConfirmRestore()
      {
         ConfirmSettings();
         ConfirmDomainObject(_application.Domains[0]);
         ConfirmDomainObjects();
      }

      private void ConfirmSettings()
      {
         ConfirmGroupObject();
         ConfirmBlockedAttachments();
         ConfirmRoutes();
         ConfirmGreyListingWhiteList();
         ConfirmWhiteList();
         ConfirmDNSBlackLists();
         ConfirmSURBLServers();
         ConfirmSSLCertificates();
         ConfirmPublicFolders();
         ConfirmIncomingRelay();
      }

      private void ConfirmSSLCertificates()
      {
         SSLCertificate cert = _application.Settings.SSLCertificates[0];
         Assert.AreEqual("file1.txt", cert.CertificateFile);
         Assert.AreEqual("name1", cert.Name);
         Assert.AreEqual("pk1", cert.PrivateKeyFile);

         cert = _application.Settings.SSLCertificates[1];
         Assert.AreEqual("file2.txt", cert.CertificateFile);
         Assert.AreEqual("name2", cert.Name);
         Assert.AreEqual("pk2", cert.PrivateKeyFile);
      }

      private void ConfirmDNSBlackLists()
      {
         DNSBlackList black = _application.Settings.AntiSpam.DNSBlackLists.get_ItemByDNSHost("127.0.0.1");
         Assert.IsTrue(black.Active);
         Assert.AreEqual("127.5.2.1", black.ExpectedResult);
         Assert.AreEqual("Test", black.RejectMessage);
         Assert.AreEqual(4, black.Score);

         black = _application.Settings.AntiSpam.DNSBlackLists.get_ItemByDNSHost("127.0.0.2");
         Assert.IsTrue(black.Active);
         Assert.AreEqual("127.5.2.2", black.ExpectedResult);
         Assert.AreEqual("Test2", black.RejectMessage);
         Assert.AreEqual(5, black.Score);
      }

      private void ConfirmIncomingRelay()
      {
         IncomingRelays relays = _application.Settings.IncomingRelays;
         Assert.AreEqual(1, relays.Count);

         IncomingRelay relay = relays[0];
         Assert.AreEqual("Test", relay.Name);
         Assert.AreEqual("1.2.3.4", relay.LowerIP);
         Assert.AreEqual("4.3.2.1", relay.UpperIP);
      }

      private void ConfirmSURBLServers()
      {
         SURBLServer black = _application.Settings.AntiSpam.SURBLServers.get_ItemByDNSHost("127.0.0.1");
         Assert.IsTrue(black.Active);
         Assert.AreEqual("Test", black.RejectMessage);
         Assert.AreEqual(4, black.Score);

         black = _application.Settings.AntiSpam.SURBLServers.get_ItemByDNSHost("127.0.0.2");
         Assert.IsTrue(black.Active);
         Assert.AreEqual("Test2", black.RejectMessage);
         Assert.AreEqual(5, black.Score);
      }

      private void ConfirmBlockedAttachments()
      {
         BlockedAttachments attachments = _application.Settings.AntiVirus.BlockedAttachments;
         Assert.Greater(attachments.Count, 0);

         for (int i = 0; i < attachments.Count; i++)
         {
            BlockedAttachment ba = attachments[i];

            if (ba.Description == "My description" && ba.Wildcard == "*.my")
               return;
         }

         Assert.Fail("Blocked attachment not found");
      }

      private void ConfirmRoutes()
      {
         Routes routes = _application.Settings.Routes;

         Assert.AreEqual(2, routes.Count);

         Route route = routes[0];
         Assert.AreEqual("test1.com", route.DomainName);
         Assert.AreEqual("description", route.Description);
         Assert.AreEqual(true, route.UseSSL);
         Assert.AreEqual(true, route.TreatSenderAsLocalDomain);
         Assert.AreEqual(false, route.TreatRecipientAsLocalDomain);
         Assert.AreEqual(false, route.TreatSecurityAsLocalDomain);

         route = routes[1];
         Assert.AreEqual("test2.com", route.DomainName);
         Assert.AreEqual("description2", route.Description);
         Assert.AreEqual(false, route.UseSSL);
         Assert.AreEqual(false, route.TreatSenderAsLocalDomain);
         Assert.AreEqual(true, route.TreatRecipientAsLocalDomain);
         Assert.AreEqual(true, route.TreatSecurityAsLocalDomain);

         Assert.AreEqual(2, route.Addresses.Count);

         var addresses = new List<string>();
         addresses.Add(route.Addresses[0].Address);
         addresses.Add(route.Addresses[1].Address);

         Assert.IsTrue(addresses.Contains("address1@test2.com"));
         Assert.IsTrue(addresses.Contains("address2@test2.com"));
      }

      private void ConfirmGreyListingWhiteList()
      {
         GreyListingWhiteAddresses addresses = _application.Settings.AntiSpam.GreyListingWhiteAddresses;

         Assert.AreEqual(2, addresses.Count);

         GreyListingWhiteAddress address = addresses[0];
         Assert.AreEqual("helo1", address.Description);
         Assert.AreEqual("1.1.1.1", address.IPAddress);

         address = addresses[1];
         Assert.AreEqual("helo2", address.Description);
         Assert.AreEqual("2.2.2.2", address.IPAddress);
      }

      private void ConfirmWhiteList()
      {
         WhiteListAddresses addresses = _application.Settings.AntiSpam.WhiteListAddresses;

         Assert.AreEqual(2, addresses.Count);

         WhiteListAddress address = addresses[0];
         Assert.AreEqual("Desc1", address.Description);
         Assert.AreEqual("Email1", address.EmailAddress);
         Assert.AreEqual("1.1.1.1", address.LowerIPAddress);
         Assert.AreEqual("2.2.2.2", address.UpperIPAddress);

         address = addresses[1];
         Assert.AreEqual("Desc2", address.Description);
         Assert.AreEqual("Email2", address.EmailAddress);
         Assert.AreEqual("2.2.2.2", address.LowerIPAddress);
         Assert.AreEqual("21.21.21.21", address.UpperIPAddress);
      }

      private void ConfirmDomainObject(Domain domain)
      {
         Assert.AreEqual("someone@test.com", domain.Postmaster);
         Assert.IsTrue(domain.SignatureEnabled);
         Assert.IsTrue(domain.AddSignaturesToLocalMail);
         Assert.IsTrue(domain.AddSignaturesToReplies);
         Assert.AreEqual("PLS", domain.SignaturePlainText);
         Assert.AreEqual("HTML", domain.SignatureHTML);

         Assert.AreEqual("test.com", _application.Domains[0].Name);
         Assert.AreEqual(2, _application.Domains[0].DomainAliases.Count);
         Assert.AreEqual("test1.com", _application.Domains[0].DomainAliases[0].AliasName);
         Assert.AreEqual("test2.com", _application.Domains[0].DomainAliases[1].AliasName);
      }

      private void ConfirmDomainObjects()
      {
         ConfirmAccountObject();
         ConfirmAliasObject();
         ConfirmDistributionListObject();
      }

      private void ConfirmDistributionListObject()
      {
         DistributionList list = _application.Domains[0].DistributionLists[0];
         Assert.AreEqual("list@test.com", list.Address);
         Assert.AreEqual(3, list.Recipients.Count);
         Assert.AreEqual("member1@test.com", list.Recipients[0].RecipientAddress);
         Assert.AreEqual("member2@test.com", list.Recipients[1].RecipientAddress);
         Assert.AreEqual("member3@test.com", list.Recipients[2].RecipientAddress);
      }

      private void ConfirmAliasObject()
      {
         Alias alias = _application.Domains[0].Aliases[0];
         Assert.AreEqual("alias@test.com", alias.Name);
         Assert.AreEqual("someone@test.com", alias.Value);
      }

      private void ConfirmAccountObject()
      {
         Account account = _application.Domains[0].Accounts.get_ItemByAddress("test@test.com");


         Assert.IsTrue(account.Active);
         Assert.AreEqual("AD", account.ADDomain);
         Assert.AreEqual(eAdminLevel.hAdminLevelDomainAdmin, account.AdminLevel);
         Assert.AreEqual("AU", account.ADUsername);
         Assert.AreEqual("FA", account.ForwardAddress);
         Assert.IsFalse(account.ForwardEnabled);
         Assert.IsTrue(account.ForwardKeepOriginal);
         Assert.IsFalse(account.IsAD);
         Assert.AreEqual(1250, account.MaxSize);
         Assert.AreEqual("First", account.PersonFirstName);
         Assert.AreEqual("Last", account.PersonLastName);
         Assert.IsTrue(account.SignatureEnabled);
         Assert.AreEqual("HTML", account.SignatureHTML);
         Assert.AreEqual("PLAIN", account.SignaturePlainText);
         Assert.AreEqual("VAC", account.VacationMessage);
         Assert.IsTrue(account.VacationMessageExpires);
         Assert.AreEqual("2040-01-01", account.VacationMessageExpiresDate.Substring(0, 10));
         Assert.IsTrue(account.VacationMessageIsOn);
         Assert.AreEqual("SUBJ", account.VacationSubject);

         // Confirm fetch account
         FetchAccount fa = account.FetchAccounts.get_Item(0);
         Assert.AreEqual(5, fa.DaysToKeepMessages);
         Assert.IsTrue(fa.Enabled);
         Assert.AreEqual(10, fa.MinutesBetweenFetch);
         Assert.AreEqual("test", fa.Name);
         Assert.AreEqual(_fetchAccountPort, fa.Port);
         Assert.AreEqual(true, fa.ProcessMIMEDate);
         Assert.IsTrue(fa.ProcessMIMERecipients);
         Assert.AreEqual("test", fa.Username);
         Assert.IsFalse(fa.UseSSL);
         Assert.AreEqual("127.0.0.1", fa.ServerAddress);
         Assert.IsTrue(fa.UseAntiSpam);
         Assert.IsTrue(fa.UseAntiVirus);

         // Make sur no additional mail is downloaded. We have already downloaded it.
         DownloadFromExternalAccount(account, fa);

         Rule rule = account.Rules[0];
         Assert.AreEqual("MyRule", rule.Name);

         RuleCriteria criteria = rule.Criterias[0];
         Assert.AreEqual(eRuleMatchType.eMTGreaterThan, criteria.MatchType);
         Assert.AreEqual(eRulePredefinedField.eFTMessageSize, criteria.PredefinedField);
         Assert.AreEqual("0", criteria.MatchValue);

         RuleAction action = rule.Actions[0];
         Assert.AreEqual(eRuleActionType.eRAForwardEmail, action.Type);
         Assert.AreEqual("someone@test.com", action.To);
         Assert.AreEqual("Test", action.Body);
         Assert.AreEqual("File", action.Filename);
         Assert.AreEqual("T", action.FromAddress);
         Assert.AreEqual("N", action.FromName);
         Assert.AreEqual("H", action.HeaderName);
         Assert.AreEqual("Folder", action.IMAPFolder);
         Assert.AreEqual("Script", action.ScriptFunction);
         Assert.AreEqual("Subj", action.Subject);
         Assert.AreEqual("Value", action.Value);

         IMAPFolder inbox = account.IMAPFolders.get_ItemByName("INBOX");

         Messages messages = inbox.Messages;

         if (_backupMessages)
         {
            Assert.AreEqual(6, inbox.CurrentUID);
            Assert.AreEqual(_folderCreationTime, inbox.CreationTime);

            Assert.AreEqual(5, messages.Count);
            Assert.AreEqual("Message 1 Subject", messages[0].Subject);
            Assert.IsTrue(messages[1].Body.Contains("Message 3 Body"));

            Assert.AreEqual(1, messages[0].UID);
            Assert.AreEqual(3, messages[1].UID);
         }
         else
         {
            Assert.AreEqual(0, messages.Count);
         }
      }

      private void ConfirmGroupObject()
      {
         Group group = _application.Settings.Groups.get_ItemByName("TestGroup");

         Assert.AreEqual("TestGroup", group.Name);
         Assert.AreEqual(3, group.Members.Count);

         Assert.AreEqual("gm1@test.com", group.Members[0].Account.Address);
         Assert.AreEqual("gm2@test.com", group.Members[1].Account.Address);
         Assert.AreEqual("gm3@test.com", group.Members[2].Account.Address);
      }

      [Test]
      public void TestWithMessages()
      {
         _backupMessages = true;
         Assert.IsTrue(Execute());
      }

      [Test]
      public void TestWithoutMessages()
      {
         _backupMessages = false;
         Assert.IsTrue(Execute());
      }
   }
}