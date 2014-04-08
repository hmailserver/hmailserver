// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;
using System.Threading;

namespace StressTest
{
    class Program
    {
        static void Main(string[] args)
        {
            ArrayList arr = new ArrayList();
            for (int i = 0; i < 20; i++)
            {
                Thread thd1 = new Thread(new ThreadStart(Worker.DoWork));

                thd1.Start();
                arr.Add(thd1);

                Thread.Sleep(1000);
            }
            
            Thread thd2 = new Thread(new ThreadStart(Worker.DoWork));

            ((Thread)arr[0]).Join();
      


       }


    }
}
