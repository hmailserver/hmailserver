using System;
using System.IO;
using System.Linq;
using System.Management.Automation;
using System.Security;
using System.Threading;
using hMailServer.Test.Infrastructure;

namespace VMTestRunner.Console
{
   class HyperV
   {
      private string _vmName;
      private PSCredential _credential;
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

         WaitForHeartbeat();
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

      private void WaitForHeartbeat()
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

      public void SetCredentials(string username, string password)
      {
         var securePassword = new SecureString();

         foreach (char c in password)
            securePassword.AppendChar(c);

         _credential = new PSCredential(username, securePassword);
      }

      public void CopyFileToGuest(string hostPath, string guestPath)
      {
         Debug($"Copying file {hostPath} to guest ({guestPath})...");

         RetryHelper.TryAction(() =>
         {
            if (!File.Exists(hostPath))
               throw new Exception($"CopyFileToGuest: The source file {hostPath} does not exist.");

            using (var ps = PowerShell.Create())
            {
               ps.AddCommand("Copy-VMFile")
                 .AddParameter("Name", _vmName)
                 .AddParameter("SourcePath", hostPath)
                 .AddParameter("DestinationPath", guestPath)
                 .AddParameter("FileSource", "Host")
                 .AddParameter("CreateFullPath", true)
                 .AddParameter("Force", true);

               ps.Invoke();
               HandleErrors(ps, "CopyFileToGuest");
            }
         }, TimeSpan.FromSeconds(1), TimeSpan.FromSeconds(10));
      }

      public void CopyFileToHost(string guestPath, string hostPath)
      {
         Debug($"Copying file {guestPath} from guest to host...");

         using (var ps = PowerShell.Create())
         {
            ps.AddCommand("Invoke-Command")
              .AddParameter("VMName", _vmName)
              .AddParameter("Credential", _credential)
              .AddParameter("ScriptBlock",
                  ScriptBlock.Create("param($src) [Convert]::ToBase64String([IO.File]::ReadAllBytes($src))"))
              .AddParameter("ArgumentList", new object[] { guestPath });

            var results = ps.Invoke();
            HandleErrors(ps, "CopyFileToHost");

            byte[] bytes = Convert.FromBase64String((string)results[0].BaseObject);
            File.WriteAllBytes(hostPath, bytes);
         }
      }

      public void CopyFolderToGuest(string source, string destination)
      {
         if (!Directory.Exists(source))
            throw new Exception($"CopyFolderToGuest: The source directory {source} does not exist.");

         CreateDirectory(destination);

         foreach (string fileName in Directory.GetFiles(source))
         {
            var fileInfo = new FileInfo(fileName);
            CopyFileToGuest(fileInfo.FullName, Path.Combine(destination, fileInfo.Name));
         }

         foreach (string subDir in Directory.GetDirectories(source))
         {
            var dirInfo = new DirectoryInfo(subDir);
            CopyFolderToGuest(subDir, Path.Combine(destination, dirInfo.Name));
         }
      }

      public void RunProgramInGuest(string fullPath, string param)
      {
         Debug($"Executing {fullPath} {param}...");

         using (var ps = PowerShell.Create())
         {
            ps.AddCommand("Invoke-Command")
              .AddParameter("VMName", _vmName)
              .AddParameter("Credential", _credential)
              .AddParameter("ScriptBlock",
                  ScriptBlock.Create("param($exe, $argList) if ($argList) { Start-Process -FilePath $exe -ArgumentList $argList -Wait -PassThru } else { Start-Process -FilePath $exe -Wait -PassThru }"))
              .AddParameter("ArgumentList", new object[] { fullPath, param });

            ps.Invoke();
            HandleErrors(ps, "RunProgramInGuest");
         }
      }

      public void CreateDirectory(string name)
      {
         using (var ps = PowerShell.Create())
         {
            ps.AddCommand("Invoke-Command")
              .AddParameter("VMName", _vmName)
              .AddParameter("Credential", _credential)
              .AddParameter("ScriptBlock",
                  ScriptBlock.Create("param($path) New-Item -ItemType Directory -Path $path -Force"))
              .AddParameter("ArgumentList", new object[] { name });

            ps.Invoke();
            HandleErrors(ps, "CreateDirectory");
         }
      }

      public string RunScriptInGuest(string script)
      {
         using (var ps = PowerShell.Create())
         {
            ps.AddCommand("Invoke-Command")
              .AddParameter("VMName", _vmName)
              .AddParameter("Credential", _credential)
              .AddParameter("ScriptBlock", ScriptBlock.Create(script));

            var results = ps.Invoke();
            HandleErrors(ps, "RunScriptInGuest");

            return string.Join(Environment.NewLine, results.Select(r => r.ToString()));
         }
      }

      private void Debug(string message) => Logger.Debug($"[Test {_testIndex}] {message}");

      private void HandleErrors(PowerShell ps, string operation)
      {
         if (ps.HadErrors)
            throw new Exception($"{operation}: {string.Join(Environment.NewLine, ps.Streams.Error)}");
      }
   }
}
