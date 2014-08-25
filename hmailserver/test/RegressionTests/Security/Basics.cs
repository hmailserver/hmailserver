// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Security
{
   [TestFixture]
   public class Basics : TestFixtureBase
   {
      [Test]
      public void TestEmptyPassword()
      {
         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "");

         string message;
         var sim = new POP3ClientSimulator();
         CustomAssert.IsFalse(sim.ConnectAndLogon(account1.Address, "", out message));


         var simIMAP = new IMAPClientSimulator();
         CustomAssert.IsFalse(simIMAP.ConnectAndLogon(account1.Address, "", out message));
         CustomAssert.AreEqual("A01 NO Invalid user name or password.\r\n", message);

         var simSMTP = new SMTPClientSimulator();
         CustomAssert.IsFalse(simSMTP.ConnectAndLogon("dGVzdEB0ZXN0LmNvbQ==", "", out message));
         CustomAssert.AreEqual("535 Authentication failed. Restarting authentication process.\r\n", message);
      }
   }
}