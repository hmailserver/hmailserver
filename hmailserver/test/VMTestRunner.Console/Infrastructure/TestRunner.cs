using System;
using System.IO;
using System.Reflection;
using System.Xml;

namespace VMTestRunner.Console
{
   public class TestRunner
   {
      private const string NuGetPackagesRelativePath = @"..\..\..\..\packages\";
      private const string RegressionTestsBinRelativePath = @"..\..\..\..\RegressionTests\bin\x64\Debug\";
      private const string NUnitConsoleRunnerPackagePath = @"NUnit.ConsoleRunner.3.15.5\tools";
      private const string NUnitPackagePath = @"NUnit.3.13.3\lib\net45";
      private readonly string _nUnitPath;
      private readonly string _nUnitConsolePath;

      private const string Username = "vmware";
      private const string Password = "Secret123";

      private const string RunTestScriptName = "RunTestsInHyperV.bat";

      private const string SetupLogPath = @"C:\setup.log";

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

         string softwareUnderTestSilentParmas = $"/SILENT /LOG=\"{SetupLogPath}\"";

         string sslFolder = Path.Combine(TestSettings.GetTestFolder(), "SSL examples");

         vm.OpenVM(_environment.VMName);

         try
         {
            vm.RevertToSnapshot(_environment.SnapshotName);

            using (var guest = CreateGuestSession(vm))
            {
               guest.WaitUntilReady();

               // Set up test paths.
               guest.CreateDirectory(guestTestPath);
               guest.CreateDirectory(@"C:\Temp");

               foreach (var command in _environment.PreInstallCommands)
                  guest.RunProgram(command.Executable, command.Parameters);

               foreach (var copyOperation in _environment.PreInstallFileCopy)
                  guest.CopyFileToGuest(copyOperation.From, copyOperation.To);

               guest.CopyFolderToGuest(_nUnitConsolePath, guestTestPath);
               guest.CopyFolderToGuest(_nUnitPath, guestTestPath);

               foreach (var testAssemblyName in testAssemblyNames)
                  guest.CopyFileToGuest(Path.Combine(testAssemblyDirectory, testAssemblyName), Path.Combine(guestTestPath, testAssemblyName));

               guest.CopyFileToGuest(Path.Combine(currentDirectory, RunTestScriptName), Path.Combine(guestTestPath, RunTestScriptName));

               // Other required stuff.
               guest.CopyFolderToGuest(sslFolder, @"C:\SSL examples");

               guest.CopyFileToGuest(softwareUnderTestFullPath, Path.Combine(guestTestPath, softwareUnderTestName));
               RunSetup(guest, Path.Combine(guestTestPath, softwareUnderTestName), softwareUnderTestSilentParmas);

               foreach (var copyOperation in _environment.PostInstallFileCopy)
                  guest.CopyFileToGuest(copyOperation.From, copyOperation.To);

               foreach (var command in _environment.PostInstallCommands)
                  guest.RunProgram(command.Executable, command.Parameters);

               bool useLocalVersion = false;

               if (useLocalVersion)
               {
                  CopyLocalVersion(guest);
               }

               // Run NUnit
               if (_environment.IncludeStressTests)
               {
                  guest.RunProgram(Path.Combine(guestTestPath, RunTestScriptName), "IncludeStress");
               }
               else
               {
                  guest.RunProgram(Path.Combine(guestTestPath, RunTestScriptName), "");
               }

               // Collect results. The NUnit result is kept next to the log file of this run.
               string localResultFile = RunContext.GetResultFilePath(_environment);
               string localLogFile = Path.GetTempFileName() + ".log";
               guest.CopyFileToHost(Path.Combine(guestTestPath, "TestResult.xml"), localResultFile);
               guest.CopyFileToHost(Path.Combine(guestTestPath, "TestResult.log"), localLogFile);

               Logger.Info($"Test {_testIndex} - NUnit result saved to {localResultFile}");

               var doc = new XmlDocument();
               doc.Load(localResultFile);

               var failedAttribute = doc.LastChild?.Attributes?["failed"]?.Value;
               int failedCount = failedAttribute != null ? Convert.ToInt32(failedAttribute) : 0;

               if (failedCount == 0)
                  return;

               string resultContent = File.ReadAllText(localResultFile);
               string logContent = File.ReadAllText(localLogFile);
               string failureSummary = NUnitResultParser.SummarizeFailures(doc);
               throw new TestFailedException($"{resultContent}\r\n\r\n{logContent}", failureSummary);
            }
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

      /// <summary>
      /// Creates the session used to reach the inside of the guest. Guests older than
      /// Windows 10 have neither PowerShell Direct nor the guest service interface.
      /// </summary>
      private IGuestSession CreateGuestSession(HyperV vm)
      {
         switch (_environment.GuestTransport)
         {
            case GuestTransport.Network:
               return new NetworkGuestSession(_environment.GuestAddress, Username, Password, _testIndex);
            default:
               return new PowerShellDirectGuestSession(vm, _environment.VMName, Username, Password, _testIndex);
         }
      }

      /// <summary>
      /// Runs the setup program. Setup returns a non-zero exit code if the installation
      /// fails, for example if the database couldn't be upgraded.
      /// </summary>
      private void RunSetup(IGuestSession guest, string setupPath, string parameters)
      {
         try
         {
            guest.RunProgram(setupPath, parameters, true);
         }
         catch (Exception ex)
         {
            throw new Exception($"{ex.Message}{Environment.NewLine}{Environment.NewLine}{GetSetupLog(guest)}", ex);
         }
      }

      private string GetSetupLog(IGuestSession guest)
      {
         try
         {
            string localSetupLog = Path.GetTempFileName() + ".log";
            guest.CopyFileToHost(SetupLogPath, localSetupLog);

            return File.ReadAllText(localSetupLog);
         }
         catch (Exception ex)
         {
            return $"The setup log {SetupLogPath} could not be read: {ex.Message}";
         }
      }

      private void CopyLocalVersion(IGuestSession guest)
      {
         string currentDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

         var localExecutable = Path.Combine(currentDir,
            @"..\..\..\..\..\..\source\Server\hMailServer\x64\Release\hMailServer.exe");

         if (!File.Exists(localExecutable))
         {
            throw new Exception($"The executable {localExecutable} could not be found.");
         }

         guest.RunProgram(@"C:\Windows\System32\net.exe", "stop hMailServer");
         guest.CopyFileToGuest(localExecutable, @"C:\Program Files\hMailServer\Bin\hMailServer.exe");
         guest.RunProgram(@"C:\Windows\System32\net.exe", "start hMailServer");
      }

   }
}
