using System;
using System.Diagnostics;
using System.IO;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Stress.Scripting
{
   [TestFixture]
   public class Events : TestFixtureBase
   {
      [Test]
      [Description("Regression test for issue #577 - reloading event scripts leaked a script engine per event handler.")]
      public void TestReloadingScriptsDoesNotLeakMemory()
      {
         var scripting = _settings.Scripting;

         var script =
            @"Sub OnAcceptMessage(oClient, oMessage)
              End Sub";

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;

         // Warm up, so that the memory measured below isn't affected by first-time allocations.
         for (var i = 0; i < 25; i++)
            scripting.Reload();

         var memoryBefore = GetServerPrivateMemory();

         const int reloadCount = 250;
         for (var i = 0; i < reloadCount; i++)
            scripting.Reload();

         var memoryAfter = GetServerPrivateMemory();
         var increase = memoryAfter - memoryBefore;

         // Every reload checks whether each of the event handlers exists, and each of those checks
         // used to leak an entire script engine. The limit is generous enough to absorb normal
         // allocator noise, but far below what the leak produced.
         const long maxIncrease = 10 * 1024 * 1024;

         Assert.Less(increase, maxIncrease,
            string.Format("Memory grew by {0} bytes over {1} script reloads.", increase, reloadCount));
      }

      private static long GetServerPrivateMemory()
      {
         var processes = Process.GetProcessesByName("hmailserver");
         if (processes.Length != 1)
            throw new Exception(string.Format("Expected a single hMailServer.exe process, found {0}", processes.Length));

         processes[0].Refresh();
         return processes[0].PrivateMemorySize64;
      }
   }

}
