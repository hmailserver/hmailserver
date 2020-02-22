using System;
using System.Collections.Generic;
using System.Text;

namespace RegressionTests.Shared
{
   static class StringExtensions
   {
      public static int Occurences(string haystack, string needle)
      {
         int count = 0;
         int n = 0;

         if (needle != "")
         {
            while ((n = haystack.IndexOf(needle, n, StringComparison.InvariantCulture)) != -1)
            {
               n += needle.Length;
               count++;
            }
         }

         return count;
      }
   }
}
