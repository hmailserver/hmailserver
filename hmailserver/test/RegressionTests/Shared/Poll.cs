using System;
using System.Threading;

namespace RegressionTests.Shared
{
   /// <summary>
   ///    Waits for a condition using a short poll interval that grows over time. A fixed
   ///    interval makes every wait cost at least one full interval, also when the server
   ///    is ready almost immediately.
   /// </summary>
   public static class Poll
   {
      private const int MaxIntervalMilliseconds = 100;

      public static bool Until(TimeSpan timeout, Func<bool> condition)
      {
         var deadline = DateTime.UtcNow + timeout;
         var interval = 2.0;

         while (true)
         {
            if (condition())
               return true;

            if (DateTime.UtcNow >= deadline)
               return false;

            Thread.Sleep((int) interval);

            if (interval < MaxIntervalMilliseconds)
               interval = Math.Min(interval * 1.6, MaxIntervalMilliseconds);
         }
      }
   }
}
