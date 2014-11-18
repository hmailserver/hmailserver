using System;

namespace RegressionTests.Shared
{
   public class DeliveryFailedException : Exception
   {
      public DeliveryFailedException(string message) :
         base(message)
      {
         
      }
   }
}
