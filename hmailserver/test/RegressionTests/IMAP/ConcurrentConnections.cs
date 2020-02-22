using System;
using System.Collections.Generic;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class ConcurrentConnections : TestFixtureBase
   {
      [Test]
      public void ChangingFlagShouldAffectAllConnections()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var sim1 = new ImapClientSimulator();
         Assert.IsTrue(sim1.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(sim1.SelectFolder("Inbox"));

         var sim2 = new ImapClientSimulator();
         Assert.IsTrue(sim2.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(sim2.SelectFolder("Inbox"));

         sim1.SetFlagOnMessage(1, true, "\\Deleted");

         var flags1 = sim1.GetFlags(1);
         var flags2 = sim2.GetFlags(1);

         Assert.IsTrue(flags2.Contains(@"* 1 FETCH (FLAGS (\Deleted))"), flags2);
         Assert.IsTrue(flags2.Contains(@"* 1 FETCH (FLAGS (\Deleted) UID 1)"), flags2);
         
      }
   }
}
