using System;
using System.IO;
using System.Management;
using System.Threading;
using RegressionTests.Infrastructure;

namespace VMTestRunner.Console
{
   /// <summary>
   /// Talks to the guest over the network: files go through the administrative
   /// shares and programs are started with WMI. Used for guests which are too old
   /// for PowerShell Direct and Copy-VMFile (anything before Windows 10).
   ///
   /// The address of the guest is looked up while it boots, unless the environment
   /// names one.
   ///
   /// The guest must allow file sharing and WMI through its firewall, and must have
   /// LocalAccountTokenFilterPolicy set to 1 so a local administrator account isn't
   /// stripped of its privileges when connecting remotely.
   /// </summary>
   public class NetworkGuestSession : GuestSession
   {
      private static readonly TimeSpan ReadyTimeout = TimeSpan.FromMinutes(10);
      private static readonly TimeSpan ProgramTimeout = TimeSpan.FromHours(4);
      private static readonly TimeSpan PollInterval = TimeSpan.FromSeconds(2);

      private const string WorkFolder = @"C:\Temp";

      private readonly GuestAddressResolver _addressResolver;
      private readonly string _configuredAddress;
      private readonly string _username;
      private readonly string _password;

      private string _address;
      private NetworkShare _share;

      public NetworkGuestSession(GuestAddressResolver addressResolver, string configuredAddress, string username,
         string password, int testIndex)
         : base(testIndex)
      {
         _addressResolver = addressResolver;
         _configuredAddress = configuredAddress;
         _username = username;
         _password = password;
      }

      public override void WaitUntilReady()
      {
         Debug(_configuredAddress == null
            ? "Waiting for the guest to report an address..."
            : $"Waiting for '{_configuredAddress}' to become reachable...");

         var deadline = DateTime.UtcNow.Add(ReadyTimeout);
         string lastError = "(none)";

         while (DateTime.UtcNow < deadline)
         {
            try
            {
               if (!ResolveAddress())
               {
                  lastError = "The address of the guest could not be determined.";
                  Thread.Sleep(PollInterval);
                  continue;
               }

               Connect();

               // Both transports are needed, so both are checked.
               if (Directory.Exists(ToUncPath(@"C:\")))
               {
                  CreateScope().Connect();

                  Debug($"'{_address}' is reachable.");
                  return;
               }

               lastError = $"The share {ToUncPath(@"C:\")} is not available.";
            }
            catch (Exception ex)
            {
               lastError = ex.Message;

               // The credentials are re-established on the next attempt - the guest
               // may not have been ready to accept them yet.
               Disconnect();
            }

            Thread.Sleep(PollInterval);
         }

         throw new Exception($"WaitUntilReady: The guest ({_address ?? "address unknown"}) could not be reached. " +
                             $"Last error: {lastError}");
      }

      /// <summary>
      /// Looks up the address of the guest, unless the environment names one. The
      /// lookup fails while the guest is still booting, so it is retried.
      /// </summary>
      private bool ResolveAddress()
      {
         if (_address != null)
            return true;

         _address = _configuredAddress ?? _addressResolver.Resolve();

         if (_address == null)
            return false;

         Debug($"Using address '{_address}' for the guest.");
         return true;
      }

      /// <summary>
      /// A local account on the guest has to be qualified, or the guest tries to
      /// authenticate it against its own domain.
      /// </summary>
      private string QualifiedUsername =>
         _username.Contains("\\") || _username.Contains("@")
            ? _username
            : $"{_address}\\{_username}";

      public override void CreateDirectory(string path)
      {
         Connect();

         Directory.CreateDirectory(ToUncPath(path));
      }

      public override void CopyFileToGuest(string hostPath, string guestPath)
      {
         Debug($"Copying file {hostPath} to guest ({guestPath})...");

         RetryHelper.TryAction(() =>
         {
            if (!File.Exists(hostPath))
               throw new Exception($"CopyFileToGuest: The source file {hostPath} does not exist.");

            Connect();

            string target = ToUncPath(guestPath);

            Directory.CreateDirectory(Path.GetDirectoryName(target));
            File.Copy(hostPath, target, true);
         }, TimeSpan.FromSeconds(1), TimeSpan.FromSeconds(10));
      }

      public override void CopyFileToHost(string guestPath, string hostPath)
      {
         Debug($"Copying file {guestPath} from guest to host...");

         Connect();

         File.Copy(ToUncPath(guestPath), hostPath, true);
      }

      /// <summary>
      /// Runs a program in the guest. WMI doesn't report the exit code of the process
      /// it starts, so the program is wrapped in a batch file which writes the exit
      /// code to a file we can read afterwards.
      /// </summary>
      public override void RunProgram(string fullPath, string param, bool throwOnFailure = false)
      {
         Debug($"Executing {fullPath} {param}...");

         Connect();

         string id = Guid.NewGuid().ToString("N");
         string batchPath = $@"{WorkFolder}\vmtestrunner-{id}.bat";
         string exitCodePath = $@"{WorkFolder}\vmtestrunner-{id}.exit";

         CreateDirectory(WorkFolder);

         // 'call' is needed for batch files - without it control never returns here.
         string command = param == null || param.Trim().Length == 0
            ? $"call \"{fullPath}\""
            : $"call \"{fullPath}\" {param}";

         // The redirection comes first: 'echo %ERRORLEVEL%>file' makes cmd read the exit
         // code as the handle to redirect, which writes an empty file.
         File.WriteAllText(ToUncPath(batchPath),
            "@echo off\r\n" +
            command + "\r\n" +
            $">\"{exitCodePath}\" echo %ERRORLEVEL%\r\n");

         try
         {
            var scope = CreateScope();
            scope.Connect();

            uint processId = StartProcess(scope, $"cmd.exe /c \"{batchPath}\"");

            WaitForProcess(scope, processId, exitCodePath);

            if (!throwOnFailure)
               return;

            int exitCode = ReadExitCode(exitCodePath);

            if (exitCode != 0)
               throw new Exception($"RunProgram: {fullPath} {param} failed with exit code {exitCode}.");
         }
         finally
         {
            Delete(batchPath);
            Delete(exitCodePath);
         }
      }

      private uint StartProcess(ManagementScope scope, string commandLine)
      {
         using (var processClass = new ManagementClass(scope, new ManagementPath("Win32_Process"), null))
         using (var inParameters = processClass.GetMethodParameters("Create"))
         {
            inParameters["CommandLine"] = commandLine;

            using (var outParameters = processClass.InvokeMethod("Create", inParameters, null))
            {
               uint returnValue = Convert.ToUInt32(outParameters["ReturnValue"]);

               if (returnValue != 0)
                  throw new Exception($"RunProgram: {commandLine} could not be started. Win32_Process.Create returned {returnValue}.");

               return Convert.ToUInt32(outParameters["ProcessId"]);
            }
         }
      }

      private void WaitForProcess(ManagementScope scope, uint processId, string exitCodePath)
      {
         var deadline = DateTime.UtcNow.Add(ProgramTimeout);

         while (DateTime.UtcNow < deadline)
         {
            if (!IsRunning(scope, processId))
            {
               // The batch file writes the exit code just before it ends, so give the
               // write a moment to land before giving up on it.
               for (int attempt = 0; attempt < 5; attempt++)
               {
                  if (File.Exists(ToUncPath(exitCodePath)))
                     return;

                  Thread.Sleep(PollInterval);
               }

               throw new Exception($"RunProgram: The process ended without writing {exitCodePath}.");
            }

            Thread.Sleep(PollInterval);
         }

         throw new Exception($"RunProgram: The process did not complete within {ProgramTimeout}.");
      }

      private bool IsRunning(ManagementScope scope, uint processId)
      {
         var query = new ObjectQuery($"SELECT ProcessId FROM Win32_Process WHERE ProcessId = {processId}");

         using (var searcher = new ManagementObjectSearcher(scope, query))
         using (var results = searcher.Get())
         {
            return results.Count > 0;
         }
      }

      private int ReadExitCode(string exitCodePath)
      {
         string content = File.ReadAllText(ToUncPath(exitCodePath)).Trim();

         if (!int.TryParse(content, out int exitCode))
            throw new Exception($"RunProgram: The exit code of the process could not be determined. Result: {content}");

         return exitCode;
      }

      private void Delete(string guestPath)
      {
         try
         {
            File.Delete(ToUncPath(guestPath));
         }
         catch (Exception)
         {
            // Leftovers in the temp folder are harmless - the snapshot is restored
            // before the next run anyway.
         }
      }

      private ManagementScope CreateScope()
      {
         var options = new ConnectionOptions
         {
            Username = QualifiedUsername,
            Password = _password,
            Impersonation = ImpersonationLevel.Impersonate,
            Authentication = AuthenticationLevel.PacketPrivacy,
            EnablePrivileges = true,
            Timeout = TimeSpan.FromMinutes(2),
         };

         return new ManagementScope($@"\\{_address}\root\cimv2", options);
      }

      private void Connect()
      {
         if (!ResolveAddress())
            throw new Exception("The address of the guest could not be determined.");

         if (_share == null)
            _share = new NetworkShare(_address, QualifiedUsername, _password);
      }

      private void Disconnect()
      {
         _share?.Dispose();
         _share = null;
      }

      /// <summary>
      /// Turns a path inside the guest into a path on its administrative share.
      /// </summary>
      private string ToUncPath(string guestPath)
      {
         if (guestPath.Length < 3 || guestPath[1] != ':' || guestPath[2] != '\\')
            throw new Exception($"The path {guestPath} must be a local path in the guest, such as C:\\Temp.");

         return $@"\\{_address}\{guestPath[0]}${guestPath.Substring(2)}";
      }

      public override void Dispose()
      {
         Disconnect();
      }
   }
}
