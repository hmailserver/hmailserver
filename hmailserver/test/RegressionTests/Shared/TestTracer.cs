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

            Trace.WriteLine(data);
        }
    }
}
