// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;

namespace MemoryTests
{
    class Tests
    {
        public void Run()
        {
            hMailServer.Application applicaiton = new hMailServer.Application();
            applicaiton.Authenticate("Administrator", "testar");

            // Run DNS query tests.
            TestDNSQueries test = new TestDNSQueries(applicaiton);
            test.Prepare();
            int iMemoryUsageBefore = Utilities.GetMemoryUsage();
            test.Run();
            int iMemoryUsageAfter = Utilities.GetMemoryUsage();
            int iBytesDiff = iMemoryUsageAfter - iMemoryUsageBefore;
            if (iBytesDiff > test.MaxIncrease)
                throw new Exception("Memory leak found: " + iBytesDiff.ToString() + " bytes leaked");
        }        
    }
}
