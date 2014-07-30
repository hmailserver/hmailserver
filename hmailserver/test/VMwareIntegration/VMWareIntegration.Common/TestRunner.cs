// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using System.Xml;
using System.Threading;

namespace VMwareIntegration.Common
{
   public class TestRunner
   {
      private const string NUnitPath = @"..\..\..\..\..\..\libraries\nunit-2.5.3";
      
      private TestEnvironment _environment;
      private bool _stopOnError;
      private bool _embedded;
      public delegate void TestCompletedDelegate(int testIndex, bool result, string message, string failureText);

      public event TestCompletedDelegate TestCompleted;

      private string _softwareUnderTest;
      private int _testIndex;

      public TestRunner(bool embedded, int testIndex, TestEnvironment environment, bool stopOnError, string softwareUnderTest)
      {
         _environment = environment;
         _stopOnError = stopOnError;
         _embedded = embedded;
         _softwareUnderTest = softwareUnderTest;
         _testIndex = testIndex;
      }

      public void RunThread()
      {
         Run();
      }

      public bool Run()
      {
         DateTime startTime = DateTime.Now;
         try
         {
            RunInternal();
            ReportStatus(true, DateTime.Now - startTime, "");
            return true;
         }
         catch (Exception ex)
         {
            ReportStatus(false, DateTime.Now - startTime, ex.Message);
            return false;
         }
      }

      private void ReportStatus(bool success, TimeSpan ts, string failureText)
      {
         try
         {
            string text = success ? "Success" : "Failure";

            text = text + " " + ts.ToString().Substring(0, 8);
            TestCompleted(_testIndex, success, text, failureText);
         }
         catch (Exception)
         {

         }

      }

      private void RunInternal()
      {
         VMware vm = new VMware();

         bool success = true;

         if (!File.Exists(ExpandVariables(NUnitPath) + "\\nunit-console.exe"))
             throw new Exception("Incorrect path to NUnit.");

         try
         {
            string fixtureSourcePath = TestSettings.GetFixturePath();
            string fixturePath = fixtureSourcePath + @"\bin\Release";

            string runTestsScriptName = "RunTestsInVmware.bat";
            string runTestScripts = fixtureSourcePath + @"\" + runTestsScriptName;
            string guestTestPath = @"C:\Nunit";

            string softwareUnderTestFullPath = _softwareUnderTest;
            string softwareUnderTestName = Path.GetFileName(softwareUnderTestFullPath);

            string softwareUnderTestSilentParmas = "/SILENT";

            string sslFolder = Path.Combine(TestSettings.GetTestFolder(), "SSL examples");

            vm.Connect();
            vm.OpenVM(_environment.VMwarePath);
            vm.RevertToSnapshot(_environment.SnapshotName);
            vm.LoginInGuest("VMware", "vmware");

            // Make sure we have an IP address.
            EnsureNetworkAccess(vm);

            // Set up test paths.
            vm.CreateDirectory(guestTestPath);
            vm.CreateDirectory(@"C:\Temp");

            // Install
            vm.CopyFileToGuest(softwareUnderTestFullPath, guestTestPath + "\\" + softwareUnderTestName);
            vm.RunProgramInGuest(guestTestPath + "\\" + softwareUnderTestName, softwareUnderTestSilentParmas);

            foreach (PostInstallFileCopy copyOperation in _environment.PostInstallFileCopy)
               vm.CopyFileToGuest(copyOperation.From, copyOperation.To);

            foreach (PostInstallCommand command in _environment.PostInstallCommands)
               vm.RunProgramInGuest(command.Executable, command.Parameters);

            // Configure Nunit
            vm.CopyFolderToGuest(ExpandVariables(NUnitPath), guestTestPath);
            vm.CopyFolderToGuest(Path.Combine(ExpandVariables(NUnitPath), "lib"), Path.Combine(guestTestPath, "lib"));
            vm.CopyFolderToGuest(fixturePath, guestTestPath);
            vm.CopyFileToGuest(runTestScripts, guestTestPath + "\\" + runTestsScriptName);

            // Other required stuff.
            vm.CopyFolderToGuest(sslFolder, @"C:\SSL examples");
            vm.CopyFolderToGuest(Path.Combine(sslFolder, "WithPassword"), @"C:\SSL examples\WithPassword");

            bool useLocalVersion = false;

            if (useLocalVersion)
            {
               CopyLocalVersion(vm);
            }


            // Run NUnit
            vm.RunProgramInGuest(guestTestPath + "\\" + runTestsScriptName, "");

            // Collect results.
            string localResultFile = System.IO.Path.GetTempFileName() + ".xml";
            vm.CopyFileToHost(guestTestPath + "\\TestResult.xml", localResultFile);

            XmlDocument doc = new XmlDocument();
            doc.Load(localResultFile);

            int failureCount = Convert.ToInt32(doc.LastChild.Attributes["failures"].Value);
            int errorCount = Convert.ToInt32(doc.LastChild.Attributes["errors"].Value);
            
            if (failureCount == 0 && errorCount == 0)
               return;

            string resultContent = File.ReadAllText(localResultFile);
            success = false;

            throw new Exception(resultContent);
         }
         catch (ThreadAbortException)
         {
            // Aborting. Power of virtual machine.
            vm.PowerOff();
            vm = null;

         }
         catch (Exception e)
         {
            success = false;
            throw e;
         }
         finally
         {
            if (success || _embedded || !_stopOnError)
            {
               if (vm != null)
                  vm.PowerOff();
            }
         }
      }

      private void CopyLocalVersion(VMware vm)
      {
         const string localPath =
            @"C:\dev\hmailserver\hmailserver\source\Server\hMailServer\Release\hMailServer.exe";

         RunScriptInGuest(vm, "NET STOP HMAILSERVER");
         RunScriptInGuest(vm, @"MKDIR ""C:\Program Files (x86)\hMailServer\Bin\");
         RunScriptInGuest(vm, @"MKDIR ""C:\Program Files\hMailServer\Bin\");
         vm.CopyFileToGuest(localPath, @"C:\Program Files (x86)\hMailServer\Bin\hMailServer.exe");
         vm.CopyFileToGuest(localPath, @"C:\Program Files\hMailServer\Bin\hMailServer.exe");
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

         for (int i = 0; i < 5; i++)
         {
            string script = "ipconfig /renew\r\nping www.google.com -n 1 > C:\\pingresult.txt";
            RunScriptInGuest(vmware, script);

            string pingResultFile = Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString());

            vmware.CopyFileToHost("C:\\pingresult.txt", pingResultFile);

            pingResultData = File.ReadAllText(pingResultFile);

            if (pingResultData.Contains("Reply from "))
               return;
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
