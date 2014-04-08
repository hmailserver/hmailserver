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
      private void AssertSessionCount(eSessionType sessionType, int expectedCount)
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         int timeout = 150;
         while (timeout > 0)
         {
            int count = application.Status.get_SessionCount(eSessionType.eSTPOP3);

            if (count == expectedCount)
               return;

            timeout--;
            Thread.Sleep(100);
         }

         Assert.Fail("Session count incorrect");
      }

      [Test]
      public void TestImproperDisconnect()
      {
         Application application = SingletonProvider<TestSetup>.Instance.GetApp();

         Account oAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "TimeoutTest@test.com", "test");
         int iCount = application.Status.get_SessionCount(eSessionType.eSTPOP3);

         var oPOP3 = new POP3Simulator();
         oPOP3.Connect(110);
         AssertSessionCount(eSessionType.eSTPOP3, iCount + 1);
         oPOP3.Disconnect(); // Disconnect without sending quit

         AssertSessionCount(eSessionType.eSTPOP3, iCount);
      }
   }
}