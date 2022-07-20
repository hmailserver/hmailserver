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
      private const string NUnitPath = @"..\..\..\..\..\..\libraries\nunit-2.6.3";

      private const string Username = "vmware";
      private const string Password = "Secret123";

      private const string RunTestScriptName = "RunTestsInVmware.bat";

      private TestEnvironment _environment;

      private string _softwareUnderTest;

      public TestRunner(TestEnvironment environment, string softwareUnderTest)
      {
         _environment = environment;
         _softwareUnderTest = softwareUnderTest;
      }

      public void Run()
      {
         RunInternal();
      }

      private void RunInternal()
      {
         VMware vm = new VMware();

         if (!File.Exists(ExpandVariables(NUnitPath) + "\\nunit-console.exe"))
            throw new Exception("Incorrect path to NUnit.");

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

            int failureCount = Convert.ToInt32(doc.LastChild.Attributes["failures"].Value);
            int errorCount = Convert.ToInt32(doc.LastChild.Attributes["errors"].Value);

            if (failureCount == 0 && errorCount == 0)
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
               Console.WriteLine("Unable to power off VM. Maybe it's not powered on?");
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
