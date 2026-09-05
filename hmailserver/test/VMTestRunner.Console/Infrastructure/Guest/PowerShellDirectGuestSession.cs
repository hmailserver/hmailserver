using System;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Management.Automation;
using System.Security;
using System.Threading;
using RegressionTests.Infrastructure;

namespace VMTestRunner.Console
{
   /// <summary>
   /// Talks to the guest over PowerShell Direct and Copy-VMFile. Requires a
   /// Windows 10 / Server 2016 or later guest with integration services.
   /// </summary>
   public class PowerShellDirectGuestSession : GuestSession
   {
      private const string NetworkTestHostname = "www.google.com";

      private readonly HyperV _vm;
      private readonly string _vmName;
      private readonly PSCredential _credential;

      public PowerShellDirectGuestSession(HyperV vm, string vmName, string username, string password, int testIndex)
         : base(testIndex)
      {
         _vm = vm;
         _vmName = vmName;

         var securePassword = new SecureString();

         foreach (char c in password)
            securePassword.AppendChar(c);

         _credential = new PSCredential(username, securePassword);
      }

      public override void WaitUntilReady()
      {
         _vm.WaitForHeartbeat();

         EnsureNetworkAccess();
      }

      public override void CopyFileToGuest(string hostPath, string guestPath)
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

      public override void CopyFileToHost(string guestPath, string hostPath)
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

      // The exit code is read inside the guest - a Process object doesn't keep its
      // ExitCode when it's serialized back to us.
      private const string RunProgramScript =
         "param($exe, $argList) " +
         "if ($argList) { $process = Start-Process -FilePath $exe -ArgumentList $argList -Wait -PassThru } " +
         "else { $process = Start-Process -FilePath $exe -Wait -PassThru } " +
         "$process.ExitCode";

      /// <summary>
      /// Runs a program in the guest. throwOnFailure should only be used for programs
      /// which are known to return a meaningful exit code - 'net stop' for example
      /// fails if the service isn't running, which isn't an error to us.
      /// </summary>
      public override void RunProgram(string fullPath, string param, bool throwOnFailure = false)
      {
         Debug($"Executing {fullPath} {param}...");

         using (var ps = PowerShell.Create())
         {
            ps.AddCommand("Invoke-Command")
              .AddParameter("VMName", _vmName)
              .AddParameter("Credential", _credential)
              .AddParameter("ScriptBlock",
                  ScriptBlock.Create(RunProgramScript))
              .AddParameter("ArgumentList", new object[] { fullPath, param });

            var results = ps.Invoke();
            HandleErrors(ps, "RunProgram");

            if (!throwOnFailure)
               return;

            int exitCode = GetExitCode(results);

            if (exitCode != 0)
               throw new Exception($"RunProgram: {fullPath} {param} failed with exit code {exitCode}.");
         }
      }

      private int GetExitCode(Collection<PSObject> results)
      {
         var exitCode = results.FirstOrDefault()?.BaseObject;

         if (!(exitCode is int))
            throw new Exception($"RunProgram: The exit code of the process could not be determined. Result: {exitCode ?? "(none)"}");

         return (int) exitCode;
      }

      public override void CreateDirectory(string name)
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

      private string RunScriptInGuest(string script)
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

      private void EnsureNetworkAccess()
      {
         Debug("Ensuring network access...");

         // ICMP and general internet access are blocked on the host, so check
         // for a usable IP address and a working DNS lookup instead of pinging.
         string script = @"
            $ErrorActionPreference = 'SilentlyContinue'

            ipconfig /renew | Out-Null

            $ip = (Get-NetIPAddress -AddressFamily IPv4 |
                   Where-Object { $_.IPAddress -notlike '169.254.*' -and $_.IPAddress -ne '127.0.0.1' } |
                   Select-Object -First 1).IPAddress

            if (-not $ip) { 'NO_IP'; return }

            try
            {
               [void][System.Net.Dns]::GetHostEntry('" + NetworkTestHostname + @"')
               ""OK $ip""
            }
            catch
            {
               ""NO_DNS $ip""
            }";

         string resultData = string.Empty;

         var timeoutTime = DateTime.UtcNow.AddSeconds(60);

         while (DateTime.UtcNow < timeoutTime)
         {
            try
            {
               resultData = RunScriptInGuest(script);

               if (resultData.StartsWith("OK "))
                  return;
            }
            catch (Exception)
            {
            }

            Thread.Sleep(TimeSpan.FromSeconds(2));
         }

         throw new Exception($"No network access. Result: {resultData}");
      }

      private void HandleErrors(PowerShell ps, string operation)
      {
         if (ps.HadErrors)
            throw new Exception($"{operation}: {string.Join(Environment.NewLine, ps.Streams.Error)}");
      }
   }
}
