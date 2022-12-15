// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Xml;
using System.Threading;

namespace VMwareIntegration.Common
{
   public class TestRunner
   {
      private const string NuGetPackagesRelativePath = @"..\..\..\..\..\packages\";
      private const string NUnitConsoleRunnerPackagePath = @"NUnit.ConsoleRunner.3.11.1\tools";
      private const string NUnitPackagePath = @"NUnit.3.12.0\lib\net45";
      private string _nUnitPath;
      private string _nUnitConsolePath;

      private const string Username = "vmware";
      private const string Password = "Secret123";

      private const string RunTestScriptName = "RunTestsInVmware.bat";

      private TestEnvironment _environment;

      private string _softwareUnderTest;

      private static readonly NLog.Logger Logger = NLog.LogManager.GetCurrentClassLogger();

      public TestRunner(TestEnvironment environment, string softwareUnderTest)
      {
         _environment = environment;
         _softwareUnderTest = softwareUnderTest;

         var packagePath = Path.Combine(Environment.CurrentDirectory, NuGetPackagesRelativePath);

         _nUnitConsolePath = Path.Combine(packagePath, NUnitConsoleRunnerPackagePath);

         if (!Directory.Exists(_nUnitConsolePath))
            throw new InvalidOperationException($"NUnit console not found in {_nUnitConsolePath}");

         _nUnitPath = Path.Combine(packagePath, NUnitPackagePath);

         if (!Directory.Exists(_nUnitPath))
            throw new InvalidOperationException($"NUnit not found in {_nUnitPath}");
      }

      public void Run()
      {
         RunInternal();
      }

      private void RunInternal()
      {
         VMware vm = new VMware();

         var currentDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

         var testAssemblyNames = new string[]
            {
               "RegressionTests.dll",
               "Interop.hMailServer.dll"
            };

         string guestTestPath = @"C:\Nunit";

         string softwareUnderTestFullPath = _softwareUnderTest;
         string softwareUnderTestName = Path.GetFileName(softwareUnderTestFullPath);

         string softwareUnderTestSilentParmas = "/SILENT";

         string sslFolder = Path.Combine(TestSettings.GetTestFolder(), "SSL examples");

         vm.Connect();

         vm.OpenVM(_environment.VMwarePath);

         try
         {
            vm.RevertToSnapshot(_environment.SnapshotName);
            vm.LoginInGuest(Username, Password);

            // Make sure we have an IP address.
            EnsureNetworkAccess(vm);

            // Set up test paths.
            vm.CreateDirectory(guestTestPath);
            vm.CreateDirectory(@"C:\Temp");

            foreach (var command in _environment.PreInstallCommands)
               vm.RunProgramInGuest(command.Executable, command.Parameters);

            foreach (var copyOperation in _environment.PreInstallFileCopy)
               vm.CopyFileToGuest(copyOperation.From, copyOperation.To);

            // Install
            vm.CopyFileToGuest(softwareUnderTestFullPath, guestTestPath + "\\" + softwareUnderTestName);
            vm.RunProgramInGuest(guestTestPath + "\\" + softwareUnderTestName, softwareUnderTestSilentParmas);

            foreach (var copyOperation in _environment.PostInstallFileCopy)
               vm.CopyFileToGuest(copyOperation.From, copyOperation.To);

            foreach (var command in _environment.PostInstallCommands)
               vm.RunProgramInGuest(command.Executable, command.Parameters);

            vm.CopyFolderToGuest(_nUnitConsolePath, guestTestPath);
            vm.CopyFolderToGuest(_nUnitPath, guestTestPath);

            foreach (var testAssemblyName in testAssemblyNames)
               vm.CopyFileToGuest(Path.Combine(currentDirectory, testAssemblyName), guestTestPath + "\\" + testAssemblyName);

            vm.CopyFileToGuest(Path.Combine(currentDirectory, RunTestScriptName), guestTestPath + "\\" + RunTestScriptName);

            // Other required stuff.
            vm.CopyFolderToGuest(sslFolder, @"C:\SSL examples");
            vm.CopyFolderToGuest(Path.Combine(sslFolder, "WithPassword"), @"C:\SSL examples\WithPassword");

            bool useLocalVersion = false;

            if (useLocalVersion)
            {
               CopyLocalVersion(vm);
            }


            // Run NUnit
            vm.RunProgramInGuest(guestTestPath + "\\" + RunTestScriptName, "");

            // Collect results.
            string localResultFile = Path.GetTempFileName() + ".xml";
            string localLogFile = Path.GetTempFileName() + ".log";
            vm.CopyFileToHost(guestTestPath + "\\TestResult.xml", localResultFile);
            vm.CopyFileToHost(guestTestPath + "\\TestResult.log", localLogFile);

            XmlDocument doc = new XmlDocument();
            doc.Load(localResultFile);

            int failedCount = Convert.ToInt32(doc.LastChild.Attributes["failed"].Value);

            if (failedCount == 0)
               return;

            string resultContent = File.ReadAllText(localResultFile);
            string logContent = File.ReadAllText(localLogFile);
            throw new Exception(resultContent + "\r\n\r\n" + logContent);
         }
         finally
         {
            try
            {
               vm.PowerOff();
            }
            catch
            {
               Logger.Error("Unable to power off VM. Maybe it's not powered on?");
            }

         }
      }

      private void CopyLocalVersion(VMware vm)
      {
         string currentDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

         var localExecutable = Path.Combine(currentDir,
            @"..\..\..\..\..\..\source\Server\hMailServer\x64\Release\hMailServer.exe");

         if (!File.Exists(localExecutable))
         {
            throw new Exception($"The executable {localExecutable} could not be found.");
         }

         RunScriptInGuest(vm, "NET STOP HMAILSERVER");
         vm.CopyFileToGuest(localExecutable, @"C:\Program Files\hMailServer\Bin\hMailServer.exe");
         RunScriptInGuest(vm, "NET START HMAILSERVER");
      }

      private void RunScriptInGuest(VMware vmware, string script)
      {
         string scriptFile = Path.ChangeExtension(Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString()), ".bat");
         var guestScriptName = string.Format(@"C:\{0}.bat", Guid.NewGuid().ToString("N"));

         File.WriteAllText(scriptFile, script);

         try
         {
            vmware.CopyFileToGuest(scriptFile, guestScriptName);
         }
         finally
         {
            File.Delete(scriptFile);
         }

         vmware.RunProgramInGuest(guestScriptName, string.Empty);
      }

      private void EnsureNetworkAccess(VMware vmware)
      {
         string pingResultData = string.Empty;

         DateTime timeoutTime = DateTime.UtcNow.AddSeconds(20);

         while (DateTime.UtcNow < timeoutTime)
         {
            string script = @"echo %time% >> C:\pingresult.txt
                              ipconfig /renew >> C:\pingresult.txt
                              ping www.google.com -n 1 >> C:\pingresult.txt";

            RunScriptInGuest(vmware, script);

            string pingResultFile = Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString());

            vmware.CopyFileToHost("C:\\pingresult.txt", pingResultFile);

            pingResultData = File.ReadAllText(pingResultFile);

            if (pingResultData.Contains("Reply from "))
               return;

            Thread.Sleep(TimeSpan.FromSeconds(1));
         }

         throw new Exception("No network access. Ping result: " + pingResultData);

      }

      static string ProgramFilesx86()
      {
         if (8 == IntPtr.Size
             || (!String.IsNullOrEmpty(Environment.GetEnvironmentVariable("PROCESSOR_ARCHITEW6432"))))
         {
            return Environment.GetEnvironmentVariable("ProgramFiles(x86)");
         }

         return Environment.GetEnvironmentVariable("ProgramFiles");
      }

      static string ExpandVariables(string input)
      {
         input = input.Replace("%PROGRAM_FILES%", ProgramFilesx86());

         return input;
      }

   }
}
