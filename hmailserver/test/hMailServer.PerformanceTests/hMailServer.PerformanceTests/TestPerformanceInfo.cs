using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace hMailServer.PerformanceTests
{
   public static class TestPerformanceInfo
   {
      public static Dictionary<string, TimeSpan> Timings = new Dictionary<string, TimeSpan>(); 
   }
}
