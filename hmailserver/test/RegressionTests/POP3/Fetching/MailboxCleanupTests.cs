using System.Collections.Generic;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.POP3.Fetching
{
   [TestFixture]
   public class MailboxCleanupTests : TestFixtureBase
   {
      private const string MessageText = "From: Martin@example.com\r\n" +
                                         "To: Martin@example.com\r\n" +
                                         "Subject: Test\r\n" +
                                         "\r\n" +
                                         "Hello!";

      private Account _account;
      private int _serverPort;

      [SetUp]
      public void SetUpTest()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");
         _serverPort = TestSetup.GetNextFreePort();

         LogHandler.DeleteErrorLog();
      }

      private FetchAccount CreateFetchAccount()
      {
         var fa = _account.FetchAccounts.Add();

         fa.Enabled = true;
         fa.MinutesBetweenFetch = 10;
         fa.Name = "Test";
         fa.Username = "test@example.com";
         fa.Password = "test";
         fa.UseSSL = false;
         fa.ServerAddress = "localhost";
         fa.Port = _serverPort;
         fa.ProcessMIMERecipients = false;
         // Delete messages from the server once they are a day old. This is what makes the
         // cleanup look up the creation date of the stored UID.
         fa.DaysToKeepMessages = 1;
         fa.UseAntiSpam = false;
         fa.UseAntiVirus = false;
         fa.Save();

         return fa;
      }

      [Test]
      [Description(
         "The external server lists the same UID twice. Deleting the first copy removes the UID from the list of downloaded UIDs, so the lookup for the second copy finds nothing.")]
      public void DuplicateUidInUidlResponseShouldNotCrashMailboxCleanup()
      {
         FetchAccount fetchAccount;

         // Download the message once, so that its UID is stored.
         using (var pop3Server = new Pop3ServerSimulator(1, _serverPort, new List<string> {MessageText}))
         {
            pop3Server.StartListen();

            fetchAccount = CreateFetchAccount();
            fetchAccount.DownloadNow();

            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);
         }

         Pop3ClientSimulator.AssertMessageCount(_account.Address, "test", 1);

         // Make the stored UID old enough for the message to be deleted from the server.
         SingletonProvider<TestSetup>.Instance.GetApp().Database.ExecuteSQL(
            string.Format("update hm_fetchaccounts_uids set uidtime = '2000-01-01 00:00:00' where uidfaid = {0}",
               fetchAccount.ID));

         // The server now reports the same message, and therefore the same UID, twice.
         using (var pop3Server = new Pop3ServerSimulator(1, _serverPort, new List<string> {MessageText, MessageText}))
         {
            pop3Server.StartListen();

            fetchAccount.DownloadNow();

            pop3Server.WaitForCompletion();
            LockHelper.WaitForUnlock(fetchAccount);
         }

         CustomAsserts.AssertNoReportedError();
      }
   }
}
