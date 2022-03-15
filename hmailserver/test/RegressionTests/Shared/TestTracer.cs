using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace RegressionTests.Shared
{
   public class TestTracer
   {
      public static void WriteTraceInfo(string format, params object[] args)
      {
         string data = string.Format(format, args);
         string completeMessage = string.Format("{0} - {1}", DateTime.Now, data);

         Console.WriteLine(completeMessage);
      }
   }
}
