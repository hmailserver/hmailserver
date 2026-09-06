using System;
using System.Linq;
using System.Management.Automation;
using System.Threading;

namespace VMTestRunner.Console
{
   /// <summary>
   /// Controls the virtual machine itself. Everything which happens inside the
   /// guest goes through an IGuestSession instead.
   /// </summary>
   public class HyperV
   {
      private string _vmName;
      private readonly int _testIndex;

      private static readonly NLog.Logger Logger = NLog.LogManager.GetCurrentClassLogger();

      public HyperV(int testIndex)
      {
         _testIndex = testIndex;
      }

      public void OpenVM(string vmName)
      {
         _vmName = vmName;

         using (var ps = PowerShell.Create())
         {
            ps.AddCommand("Get-VM")
              .AddParameter("Name", _vmName);

            var results = ps.Invoke();
            HandleErrors(ps, "OpenVM");

            if (results.Count == 0)
               throw new Exception($"OpenVM: VM '{_vmName}' not found.");
         }
      }

      public void RevertToSnapshot(string snapshotName)
      {
         Debug($"Reverting '{_vmName}' to snapshot '{snapshotName}'...");

         using (var ps = PowerShell.Create())
         {
            ps.AddCommand("Restore-VMSnapshot")
              .AddParameter("VMName", _vmName)
              .AddParameter("Name", snapshotName)
              .AddParameter("Confirm", false);

            ps.Invoke();
            HandleErrors(ps, "RevertToSnapshot");
         }

         PowerOn();
      }

      public void PowerOn()
      {
         Debug($"Powering on '{_vmName}'...");

         using (var ps = PowerShell.Create())
         {
            ps.AddCommand("Start-VM")
              .AddParameter("Name", _vmName);

            ps.Invoke();

            var errors = ps.Streams.Error.Where(e => !e.ToString().Contains("already running")).ToList();
            if (errors.Any())
               throw new Exception($"PowerOn: {string.Join(Environment.NewLine, errors)}");
         }
      }

      public void PowerOff()
      {
         Debug($"Powering off '{_vmName}'...");

         using (var ps = PowerShell.Create())
         {
            ps.AddCommand("Stop-VM")
              .AddParameter("Name", _vmName)
              .AddParameter("TurnOff", true);

            ps.Invoke();
            HandleErrors(ps, "PowerOff");
         }
      }

      /// <summary>
      /// Waits for the heartbeat integration service. Guests without integration
      /// services never report one - those are waited for over the network instead.
      /// </summary>
      public void WaitForHeartbeat()
      {
         Debug($"Waiting for heartbeat from '{_vmName}'...");

         DateTime timeout = DateTime.UtcNow.AddSeconds(120);

         while (DateTime.UtcNow < timeout)
         {
            using (var ps = PowerShell.Create())
            {
               ps.AddCommand("Get-VM")
                 .AddParameter("Name", _vmName);

               var results = ps.Invoke();

               if (results.Count > 0)
               {
                  var heartbeat = results[0].Properties["Heartbeat"]?.Value?.ToString();

                  if (heartbeat == "OkApplicationsHealthy" ||
                      heartbeat == "OkApplicationsUnknown" ||
                      heartbeat == "Ok")
                  {
                     Debug($"Heartbeat received: {heartbeat}");
                     return;
                  }
               }
            }

            Thread.Sleep(TimeSpan.FromSeconds(2));
         }

         throw new Exception($"WaitForHeartbeat: Timed out waiting for heartbeat from '{_vmName}'.");
      }

      private void Debug(string message)
      {
         // The test index tells the status board which row the message belongs to.
         var logEvent = new NLog.LogEventInfo(NLog.LogLevel.Debug, Logger.Name, $"[Test {_testIndex}] {message}");
         logEvent.Properties[TestBoardConsoleTarget.TestIndexProperty] = _testIndex;
         Logger.Log(logEvent);
      }

      private void HandleErrors(PowerShell ps, string operation)
      {
         if (ps.HadErrors)
            throw new Exception($"{operation}: {string.Join(Environment.NewLine, ps.Streams.Error)}");
      }
   }
}
