// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using VixCOM;
using System.Xml;
using System.Reflection;
using System.Threading;

namespace VMwareIntegration.Common
{
   public class TestRunner
   {
       private const string NUnitPath = @"..\..\..\..\..\..\libraries\nunit-2.5.3";
      
      private TestEnvironment _environment;
      private bool _stopOnError;
      private bool _embedded;
      public delegate void TestCompletedDelegate(bool result, string message, string failureText);

      public event TestCompletedDelegate TestCompleted;

      public TestRunner(bool embedded, TestEnvironment environment, bool stopOnError)
      {
         _environment = environment;
         _stopOnError = stopOnError;
         _embedded = embedded;
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
            TestCompleted(success, text, failureText);
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

            string softwareUnderTestFullPath = TestSettings.GetSoftwareUnderTest();
            string softwareUnderTestName = Path.GetFileName(softwareUnderTestFullPath);

            string softwareUnderTestSilentParmas = "/SILENT";

            string sslFolder = Path.Combine(TestSettings.GetTestFolder(), "SSL examples");

            vm.Connect();
            vm.OpenVM(_environment.VMwarePath);
            vm.RevertToSnapshot(_environment.SnapshotName);
            vm.LoginInGuest("VMware", "vmware");

            // Make sure we have an IP address.
            vm.RunProgramInGuest("ipconfig.exe", "/renew");
            vm.RunProgramInGuest("ipconfig.exe", "/renew");

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

            // Run NUnit
            vm.RunProgramInGuest(guestTestPath + "\\" + runTestsScriptName, "");

            // Collect results.
            string localResultFile = System.IO.Path.GetTempFileName() + ".xml";
            vm.CopyFileToHost(guestTestPath + "\\TestResult.xml", localResultFile);

            XmlDocument doc = new XmlDocument();
            doc.Load(localResultFile);

            string failures = doc.LastChild.Attributes["failures"].Value;
            int failureCount = Convert.ToInt32(failures);

            if (failureCount == 0)
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
