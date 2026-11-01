// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.ServiceProcess;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure.Persistence
{
   [TestFixture]
   public class CrashRecovery : TestFixtureBase
   {
      [Test]
      [Description("Issue 692: SQL Compact loses committed changes when hMailServer terminates abnormally.")]
      public void DeliveredMessagesShouldSurviveProcessTermination()
      {
         if (_application.Database.DatabaseType != eDBtype.hDBTypeMSSQLCE)
            Assert.Ignore("Only applies to SQL Server Compact.");

         if (!new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator))
            Assert.Ignore("Terminating hMailServer.exe requires running the tests elevated.");

         const int messageCount = 5;

         const string address = "test@example.test";

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, address, "test");

         for (var i = 0; i < messageCount; i++)
            SmtpClientSimulator.StaticSend("sender@example.test", address, "Message " + i, "Body");

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders.get_ItemByName("INBOX"), messageCount);

         KillAndRestartService();

         Account accountAfterRestart = null;
         try
         {
            accountAfterRestart = _application.Domains.get_ItemByName("example.test").Accounts.get_ItemByAddress(address);
         }
         catch (COMException)
         {
            Assert.Fail("The account was lost when hMailServer.exe terminated.");
         }

         Assert.AreEqual(messageCount, accountAfterRestart.IMAPFolders.get_ItemByName("INBOX").Messages.Count);
      }

      private void KillAndRestartService()
      {
         var processes = Process.GetProcessesByName("hMailServer");
         Assert.AreEqual(1, processes.Length);

         using (var process = processes[0])
         {
            process.Kill();
            process.WaitForExit();
         }

         using (var service = new ServiceController("hMailServer"))
         {
            service.WaitForStatus(ServiceControllerStatus.Stopped, TimeSpan.FromSeconds(30));
            service.Start();
            service.WaitForStatus(ServiceControllerStatus.Running, TimeSpan.FromSeconds(60));
         }

         // The restart is intended, so the following tests should expect the new process.
         ServiceRestartDetector.ExpectedProcessId = null;
         ServiceRestartDetector.ValidateProcessId();

         SingletonProvider<TestSetup>.Instance.Authenticate();
         _application = SingletonProvider<TestSetup>.Instance.GetApp();
         _settings = _application.Settings;
      }
   }
}
