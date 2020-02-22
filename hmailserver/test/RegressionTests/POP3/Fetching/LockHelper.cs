using System;
using System.Threading;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.POP3.Fetching
{
   class LockHelper
   {
      public static void WaitForUnlock(FetchAccount fetchAccount)
      {
         var timeoutTime = DateTime.Now.Add(TimeSpan.FromSeconds(30));

         while (DateTime.Now < timeoutTime)
         {
            if (!fetchAccount.IsLocked)
               return;

            Thread.Sleep(100);
         }

         string defaultLog = LogHandler.ReadCurrentDefaultLog();
         Assert.Fail(string.Format("At {0}, fetch account was not unlocked.", DateTime.Now));
      }
   }
}
