using System;
using System.Threading;

namespace RegressionTests.Infrastructure
{
   public class RetryHelper
   {
      public static void TryAction(TimeSpan timeout, Action action)
      {
         TryAction(action, TimeSpan.FromMilliseconds(500), timeout);
      }

      public static void TryAction(Action action, TimeSpan retryInterval, TimeSpan timeout)
      {
         DateTime endTime = DateTime.Now + timeout;

         while (true)
         {
            try
            {
               action();
               return;
            }
            catch
            {
               if (DateTime.Now > endTime)
                  throw;

               Thread.Sleep(retryInterval);
            }
         }
      }
   }
}
