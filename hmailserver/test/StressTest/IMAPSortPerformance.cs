// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using NUnit.Framework;
using RegressionTests;
using System.Diagnostics;
using RegressionTests.Shared;

namespace StressTest
{

   [TestFixture]
   public class IMAPSortPerformance : TestFixtureBase
   {
      [Test]
      [Ignore]
      public void Test()
      {
         IMAPSimulator sim = new IMAPSimulator();
         sim.ConnectAndLogon("test@test.com", "test");
         sim.SelectFolder("Inbox");

         Stopwatch watch = new Stopwatch();

         watch.Start();
         string result = sim.SendSingleCommand("A282 SORT (SIZE) UTF-8");
         watch.Stop();
         
         long sortSizeTime = watch.ElapsedMilliseconds;
         watch.Reset();

         watch.Start();
         result = sim.SendSingleCommand("A282 SORT (FROM) UTF-8");
         watch.Stop();
         
         long sortFromTime = watch.ElapsedMilliseconds;
         watch.Reset();

         watch.Start();
         result = sim.SendSingleCommand("A282 SORT (FROM) UTF-8 1:15");
         watch.Stop();
         
         long sortFromTimeLimit15 = watch.ElapsedMilliseconds;
         watch.Reset();

         watch.Start();
         result = sim.SendSingleCommand("A282 SORT (DATE) UTF-8");
         watch.Stop();

         long sortDateTime = watch.ElapsedMilliseconds;
         watch.Reset();

         System.Threading.Thread.Sleep(1);
      }
   }
}
