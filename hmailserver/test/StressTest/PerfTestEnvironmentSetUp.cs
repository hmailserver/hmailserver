//using System;
//using System.Collections.Generic;
//using System.Diagnostics;
//using System.Linq;
//using System.Threading.Tasks;
//using NUnit.Framework;
//using RegressionTests.Shared;

//namespace StressTest
//{
//   [TestFixture]
//   public class PerfTestEnvironmentSetUp : TestFixtureBase
//   {
//      [Test]
//      [Ignore]
//      public void SetupEnvWith50MillionMessages()
//      {
//         const int accountCount = 1;
//         const int folderCount = 1;
//         const int messageCount = 100000;

//         var accounts = new List<string>();
//         for (int accountIdx = 0; accountIdx < accountCount; accountIdx++)
//         {
//            var account = string.Format("test-{0}@test.com", accountIdx);
//            accounts.Add(account);
            
//            TestTracer.WriteTraceInfo("Setting up {0}...", account);
//            SingletonProvider<TestSetup>.Instance.AddAccount(_domain, account, "test");
//         }

//         var folders = new List<string>();
//         for (int folderIdx = 0; folderIdx < folderCount; folderIdx++)
//            folders.Add(string.Format("Folder-{0}", folderIdx));

//         var parallelOptions = new ParallelOptions();
//         parallelOptions.MaxDegreeOfParallelism = 10;

//         var watch = new Stopwatch();
//         watch.Start();

//         Parallel.ForEach(accounts, parallelOptions, account =>
//            {
//               TestTracer.WriteTraceInfo("Processing messages for {0}...", account);

//               var sim = new ImapClientSimulator();
//               Assert.IsTrue(sim.ConnectAndLogon(account, "test"));

//               foreach (var folder in folders)
//               {
//                  Assert.IsTrue(sim.CreateFolder(folder));

//                  for (int i = 0; i < messageCount; i++)
//                  {
//                     string f = sim.SendSingleCommandWithLiteral("A01 APPEND " + folder + " {1}", "A");
//                     Assert.IsTrue(f.Contains("A01 OK APPEND completed"), f);
//                  }
//               }

//               sim.Disconnect();
//            });

//         watch.Stop();
//         TestTracer.WriteTraceInfo(watch.ElapsedMilliseconds.ToString());
//      }
//   }
//}
