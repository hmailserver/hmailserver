using System;

namespace RegressionTests.Shared
{
   public static class RetryableAssert
   {
      public static void AreEqual(int actual, int expected)
      {
         if (actual != expected) throw new Exception(string.Format("{0} does not equal {1}", actual, expected));
      }

      public static void IsTrue(bool value)
      {
         if (!value) throw new Exception("The value is false.");
      }

      public static void StringContains(string expected, string actual)
      {
         if (!actual.Contains(expected))
            throw new Exception(string.Format("{0} does not contain {1}", actual, expected));
      }

      public static void LessOrEqual(long max, long actual)
      {
         if (actual > max)
            throw new Exception(string.Format("{0} is greater than {1}", actual, max));
      }
   }
}