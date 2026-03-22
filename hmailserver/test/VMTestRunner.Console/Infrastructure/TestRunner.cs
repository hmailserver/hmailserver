using System;
using System.IO;
using System.Reflection;
using System.Threading;
using System.Xml;

namespace VMTestRunner.Console
{
   public class TestRunner
   {
      private const string NuGetPackagesRelativePath = @"..\..\..\..\packages\";
      private const string RegressionTestsBinRelativePath = @"..\..\..\..\RegressionTests\bin\x64\Debug\";
      private const string NUnitConsoleRunnerPackagePath = @"NUnit.ConsoleRunner.3.16.3\tools";
      private const string NUnitPackagePath = @"NUnit.3.13.3\lib\net45";
      private readonly string _nUnitPath;
      private readonly string _nUnitConsolePath;

      private const string Username = "vmware";
      private const string Password = "Secret123";

      private const string RunTestScriptName = "RunTestsInHyperV.bat";

      private readonly TestEnvironment _environment;

      private readonly string _softwareUnderTest;

      private readonly int _testIndex;

      private static readonly NLog.Logger Logger = NLog.LogManager.GetCurrentClassLogger();

      public TestRunner(TestEnvironment environment, string softwareUnderTest, int testIndex)
      {
         _environment = environment;
         _softwareUnderTest = softwareUnderTest;
         _testIndex = testIndex;

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
         var vm = new HyperV(_testIndex);

         var currentDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
         var testAssemblyDirectory = Path.Combine(currentDirectory, RegressionTestsBinRelativePath);

         var testAssemblyNames = new string[]
            {
               "RegressionTests.dll",
               "Interop.hMailServer.dll"
            };

         string guestTestPath = @"C:\Nunit";

         string softwareUnderTestFullPath = _softwareUnderTest;
         string softwareUnderTestName = Path.GetFileName(softwareUnderTestFullPath);

         string softwareUnderTestSilentParmas = "/SILENT /LOG=\"C:\\setup.log\"";

         string sslFolder = Path.Combine(TestSettings.GetTestFolder(), "SSL examples");

         vm.OpenVM(_environment.VMName);

         try
         {
            vm.RevertToSnapshot(_environment.SnapshotName);
            vm.SetCredentials(Username, Password);

            // Make sure we have an IP address.
            EnsureNetworkAccess(vm);

            // Set up test paths.
            vm.CreateDirectory(guestTestPath);
            vm.CreateDirectory(@"C:\Temp");

            foreach (var command in _environment.PreInstallCommands)
               vm.RunProgramInGuest(command.Executable, command.Parameters);

            foreach (var copyOperation in _environment.PreInstallFileCopy)
               vm.CopyFileToGuest(copyOperation.From, copyOperation.To);

            vm.CopyFolderToGuest(_nUnitConsolePath, guestTestPath);
            vm.CopyFolderToGuest(_nUnitPath, guestTestPath);

            foreach (var testAssemblyName in testAssemblyNames)
               vm.CopyFileToGuest(Path.Combine(testAssemblyDirectory, testAssemblyName), Path.Combine(guestTestPath, testAssemblyName));

            vm.CopyFileToGuest(Path.Combine(currentDirectory, RunTestScriptName), Path.Combine(guestTestPath, RunTestScriptName));

            // Other required stuff.
            vm.CopyFolderToGuest(sslFolder, @"C:\SSL examples");

            vm.CopyFileToGuest(softwareUnderTestFullPath, Path.Combine(guestTestPath, softwareUnderTestName));
            vm.RunProgramInGuest(Path.Combine(guestTestPath, softwareUnderTestName), softwareUnderTestSilentParmas);

            foreach (var copyOperation in _environment.PostInstallFileCopy)
               vm.CopyFileToGuest(copyOperation.From, copyOperation.To);

            foreach (var command in _environment.PostInstallCommands)
               vm.RunProgramInGuest(command.Executable, command.Parameters);

            bool useLocalVersion = false;

            if (useLocalVersion)
            {
               CopyLocalVersion(vm);
            }

            // Run NUnit
            vm.RunProgramInGuest(Path.Combine(guestTestPath, RunTestScriptName), "");

            // Collect results.
            string localResultFile = Path.GetTempFileName() + ".xml";
            string localLogFile = Path.GetTempFileName() + ".log";
            vm.CopyFileToHost(Path.Combine(guestTestPath, "TestResult.xml"), localResultFile);
            vm.CopyFileToHost(Path.Combine(guestTestPath, "TestResult.log"), localLogFile);

            var doc = new XmlDocument();
            doc.Load(localResultFile);

            var failedAttribute = doc.LastChild?.Attributes?["failed"]?.Value;
            int failedCount = failedAttribute != null ? Convert.ToInt32(failedAttribute) : 0;

            if (failedCount == 0)
               return;

            string resultContent = File.ReadAllText(localResultFile);
            string logContent = File.ReadAllText(localLogFile);
            throw new Exception($"{resultContent}\r\n\r\n{logContent}");
         }
         finally
         {
            try
            {
               vm.PowerOff();
            }
            catch (Exception ex)
            {
               Logger.Error(ex, "Unable to power off VM. Maybe it's not powered on?");
            }
         }
      }

      private void CopyLocalVersion(HyperV vm)
      {
         string currentDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

         var localExecutable = Path.Combine(currentDir,
            @"..\..\..\..\..\..\source\Server\hMailServer\x64\Release\hMailServer.exe");

         if (!File.Exists(localExecutable))
         {
            throw new Exception($"The executable {localExecutable} could not be found.");
         }

         vm.RunScriptInGuest("NET STOP HMAILSERVER");
         vm.CopyFileToGuest(localExecutable, @"C:\Program Files\hMailServer\Bin\hMailServer.exe");
         vm.RunScriptInGuest("NET START HMAILSERVER");
      }

      private void Debug(string message) => Logger.Debug($"[Test {_testIndex}] {message}");

      private void EnsureNetworkAccess(HyperV vm)
      {
         Debug("Ensuring network access...");

         string pingResultData = string.Empty;

         var timeoutTime = DateTime.UtcNow.AddSeconds(60);

         while (DateTime.UtcNow < timeoutTime)
         {
            try
            {
               pingResultData = vm.RunScriptInGuest("ipconfig /renew; ping www.google.com -n 1");

               if (pingResultData.Contains("Reply from "))
                  return;
            }
            catch (Exception)
            {
            }

            Thread.Sleep(TimeSpan.FromSeconds(2));
         }

         throw new Exception($"No network access. Ping result: {pingResultData}");
      }
   }
}
