// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Diagnostics;
using UnitTest;

namespace PerformanceTest
{
    class Program
    {
        static void Main(string[] args)
        {
            Stopwatch stopwatch = new Stopwatch();
            stopwatch.Start();

            // Determine message count
            POP3Simulator pop3sim = new POP3Simulator();
            int count = pop3sim.GetMessageCount("test@test.com", "test");

            // Fetch them..
            pop3sim.ConnectAndLogon("test@test.com","test");

            for (int i = 1; i <= count; i++)
            {
                pop3sim.RETR(i);
            }

            for (int i = 1; i <= count; i++)
            {
                pop3sim.DELE(i);
            }

            pop3sim.QUIT();

            System.Threading.Thread.Sleep(1000 * 60 * 60);

            stopwatch.Stop();

            Console.WriteLine("Passed time: " + stopwatch.Elapsed.TotalSeconds.ToString());


            

        }
    }
}
