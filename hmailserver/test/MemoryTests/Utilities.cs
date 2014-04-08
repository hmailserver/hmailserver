// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;

namespace MemoryTests
{
    class Utilities
    {
        public static int GetMemoryUsage()
        {
            Process[] processes = Process.GetProcessesByName("hMailServer");
            if (processes.Length == 0)
                throw new Exception("hMailServer is not running");

            Process process = processes[0];

            return process.WorkingSet;            
        }
    }
}
