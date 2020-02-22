using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;

namespace RegressionTests.Infrastructure
{
   class RetryHelper
   {
      public delegate void ActionDelegate();

      public static void TryAction(TimeSpan duration, ActionDelegate action)
      {
         DateTime timeout = DateTime.Now + duration;

         while (DateTime.Now < timeout)
         {
            try
            {
               action();
               return;
            }
            catch 
            {
               // Will retry.
            }

            Thread.Sleep(TimeSpan.FromMilliseconds(500));
         }

         action();
      }
      
   }
}
