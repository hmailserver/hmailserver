using System.Threading;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.POP3.Fetching
{
   class LockHelper
   {
      public static void WaitForUnlock(FetchAccount fetchAccount)
      {
         // 10 seconds
         for (int i = 0; i < 100; i++)
         {
            if (!fetchAccount.IsLocked)
               return;

            Thread.Sleep(100);
         }

         string defaultLog = TestSetup.ReadCurrentDefaultLog();
         Assert.Fail("Fetch account was not unlocked. Log follows:\r\n" + defaultLog);
      }
   }
}
