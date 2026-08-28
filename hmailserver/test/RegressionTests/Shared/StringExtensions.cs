using System;

namespace RegressionTests.Shared
{
   internal static class StringExtensions
   {
      public static int Occurrences(string haystack, string needle)
      {
         var count = 0;
         var n = 0;

         if (needle != "")
            while ((n = haystack.IndexOf(needle, n, StringComparison.InvariantCulture)) != -1)
            {
               n += needle.Length;
               count++;
            }

         return count;
      }
   }
}