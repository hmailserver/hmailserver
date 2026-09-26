// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System.Text.RegularExpressions;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.IMAP
{
   /// <summary>
   ///    UIDVALIDITY must be higher for a folder re-created under the same name (RFC 3501 2.3.1.1).
   /// </summary>
   [TestFixture]
   public class UidValidity : TestFixtureBase
   {
      private const string Password = "test";

      [Test]
      [Description("A folder deleted and re-created, even within the same second, gets a higher UIDVALIDITY.")]
      public void RecreatedFolderGetsHigherUidValidity()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "uidvalidity@example.test", Password);
         var simulator = ConnectAndLogon(account);

         long previous = 0;

         for (var i = 0; i < 5; i++)
         {
            Assert.IsTrue(simulator.CreateFolder("Recreated"));
            var current = GetUidValidity(simulator, "Recreated");
            Assert.Greater(current, previous);
            Assert.IsTrue(simulator.DeleteFolder("Recreated"));

            previous = current;
         }

         simulator.Disconnect();
      }

      [Test]
      [Description("After a restart, a re-created folder still gets a higher UIDVALIDITY.")]
      public void UidValidityKeepsIncreasingAfterRestart()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "uidvalidity@example.test", Password);

         var simulator = ConnectAndLogon(account);
         Assert.IsTrue(simulator.CreateFolder("Restart"));
         var before = GetUidValidity(simulator, "Restart");
         Assert.IsTrue(simulator.DeleteFolder("Restart"));
         simulator.Disconnect();

         var application = SingletonProvider<TestSetup>.Instance.GetApp();
         application.Stop();
         application.Start();

         simulator = ConnectAndLogon(account);
         Assert.IsTrue(simulator.CreateFolder("Restart"));
         Assert.Greater(GetUidValidity(simulator, "Restart"), before);
         simulator.Disconnect();
      }

      [Test]
      [Description("Each account has its own counter, so a user can't learn how many folders other accounts create.")]
      public void OtherAccountsDoNotAffectUidValidity()
      {
         var observer = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "observer@example.test", Password);
         var other = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "other@example.test", Password);

         var observerSimulator = ConnectAndLogon(observer);
         Assert.IsTrue(observerSimulator.CreateFolder("First"));
         var first = GetUidValidity(observerSimulator, "First");

         var otherSimulator = ConnectAndLogon(other);
         for (var i = 0; i < 5; i++)
            Assert.IsTrue(otherSimulator.CreateFolder("Folder" + i));
         otherSimulator.Disconnect();

         Assert.IsTrue(observerSimulator.CreateFolder("Second"));
         Assert.AreEqual(first + 1, GetUidValidity(observerSimulator, "Second"));

         observerSimulator.Disconnect();
      }

      private static ImapClientSimulator ConnectAndLogon(Account account)
      {
         var simulator = new ImapClientSimulator();
         Assert.IsTrue(simulator.ConnectAndLogon(account.Address, Password));
         return simulator;
      }

      private static long GetUidValidity(ImapClientSimulator simulator, string folderName)
      {
         var response = simulator.Status(folderName, "UIDVALIDITY");
         var match = Regex.Match(response, @"UIDVALIDITY (\d+)");
         Assert.IsTrue(match.Success, response);
         return long.Parse(match.Groups[1].Value);
      }
   }
}
