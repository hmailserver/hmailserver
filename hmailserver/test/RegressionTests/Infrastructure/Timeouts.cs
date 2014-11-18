// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.Threading;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class Timeouts : TestFixtureBase
   {
      

      [Test]
      public void TestImproperDisconnect()
      {
         CustomAsserts.AssertSessionCount(eSessionType.eSTPOP3, 0);

         var application = SingletonProvider<TestSetup>.Instance.GetApp();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "TimeoutTest@test.com", "test");
         int iCount = application.Status.get_SessionCount(eSessionType.eSTPOP3);

         var pop3ClientSimulator = new Pop3ClientSimulator();
         pop3ClientSimulator.ConnectAndLogon(account.Address, "test");
         CustomAsserts.AssertSessionCount(eSessionType.eSTPOP3, iCount + 1);
         pop3ClientSimulator.Disconnect(); // Disconnect without sending quit

         CustomAsserts.AssertSessionCount(eSessionType.eSTPOP3, iCount);
      }
   }
}