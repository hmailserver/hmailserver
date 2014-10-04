using System;
using System.IO;
using System.Runtime.InteropServices;
using System.ServiceProcess;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.Infrastructure
{
   [TestFixture]
   public class ExceptionHandlerTests : TestFixtureBase
   {
      [DllImport("kernel32")]
      static extern long WritePrivateProfileString(string Section, string Key, string Value, string FilePath);

      [TestFixtureTearDown]
      public void TestFixtureTearDown()
      {
         SetCrashSimulationMode(0);
      }

      [SetUp]
      public new void SetUp()
      {
         if (Environment.OSVersion.Version.Major == 5 &&
             Environment.OSVersion.Version.Minor == 0)
         {
            Assert.Pass("This functionality is not supported on Windows 2000.");
         }

         DeleteAllMinidumps();
      }

      [TearDown]
      public void TearDown()
      {
         RetryHelper.TryAction(TimeSpan.FromSeconds(5), () =>
            {
               TestSetup.DeleteErrorLog();
               DeleteAllMinidumps();
            });
      }

      private void DeleteAllMinidumps()
      {
         var minidumps = GetMinidumps();

         foreach (var minidump in minidumps)
            File.Delete(minidump);
      }

      private string[] GetMinidumps()
      {
         var logDirectory = _settings.Directories.LogDirectory;

         var minidumps = Directory.GetFiles(logDirectory, "minidump*.dmp");
         return minidumps;
      }

      [Test]
      public void HelpShouldNotResultInErrorIfCrashSimulationDisabled()
      {
         SetCrashSimulationMode(0);

         TriggerCrashSimulationError();

         TestSetup.AssertNoReportedError();

         AssertMinidumpCount(0);
      }

      [Test]
      public void ThrowIntShouldResultInErrorDump()
      {
         SetCrashSimulationMode(1);

         TriggerCrashSimulationError();

         TestSetup.AssertReportedError("Description: An error occured while parsing data.",
                                       "An error occured while executing 'IOCPQueueWorkerTask'",
                                       "An error has been detected. A mini dump has been written");

         AssertMinidumpCount(1);
      }


      [Test]
      public void ThrowLogicErrorShouldResultInErrorDump()
      {
         SetCrashSimulationMode(2);

         TriggerCrashSimulationError();

         TestSetup.AssertReportedError("Message: Crash simulation test");

         AssertMinidumpCount(1);
      }

      [Test]
      public void AccessViolationShouldResultInErrorDump()
      {
         SetCrashSimulationMode(3);

         TriggerCrashSimulationError();

         TestSetup.AssertReportedError("An error has been detected. A mini dump has been written");

         AssertMinidumpCount(1);
      }

      [Test]
      public void DisconnectedExceptionShouldNotExitRunningThreads()
      {
         SetCrashSimulationMode(4);

         for (int i = 0; i < 20; i++)
         {
            TriggerCrashSimulationError();
            TestSetup.AssertNoReportedError();
         }

         var defaultLog = TestSetup.ReadCurrentDefaultLog();
         CustomAssert.IsTrue(defaultLog.Contains("Connection was terminated - Client is disconnected."));
      }

      [Test]
      public void AtMost10MinidumpsAreGenerated()
      {
         SetCrashSimulationMode(3);

         // 11 errors triggered, but only 10 should generate minidumps.
         for (int i = 1; i <= 11; i++)
            TriggerCrashSimulationError();

         AssertMinidumpCount(10);

         // We should log info that we skipped minidump generation.
         RetryHelper.TryAction(TimeSpan.FromSeconds(5), () =>
            {
               var log = TestSetup.ReadCurrentDefaultLog();
               CustomAssert.IsTrue(log.Contains("Minidump creation aborted. The max count (10) is reached and no log is older than 4 hours."));
            });


         // Delete one minidump, so only 9 remains.
         var minidumps = GetMinidumps();
         File.Delete(minidumps[0]);

         // Now we should be able to create another.
         TriggerCrashSimulationError();

         AssertMinidumpCount(10);

      }

      [Test]
      public void MinidumpsOlderThan4HoursMayBeDeleted()
      {
         SetCrashSimulationMode(3);

         // 11 errors triggered, but only 10 should generate minidumps.
         for (int i = 1; i <= 11; i++)
            TriggerCrashSimulationError();

         AssertMinidumpCount(10);
         TestSetup.DeleteErrorLog();

         // We should log info that we skipped minidump generation.
         RetryHelper.TryAction(TimeSpan.FromSeconds(5), () =>
         {
            var log = TestSetup.ReadCurrentDefaultLog();
            CustomAssert.IsTrue(log.Contains("Minidump creation aborted. The max count (10) is reached and no log is older than 4 hours."));
         });

         // Pretend one minidump is really old.
         var minidumps = GetMinidumps();
         var testminidump = minidumps[0];
         File.SetCreationTime(testminidump, new DateTime(2014,01,01));


         // Now we should be able to create another.
         TriggerCrashSimulationError();

         RetryHelper.TryAction(TimeSpan.FromSeconds(10), () => CustomAssert.IsFalse(File.Exists(testminidump)));

         AssertMinidumpCount(10);

      }


      private static void TriggerCrashSimulationError()
      {
         using (var tcpConnection = new TcpConnection())
         {
            tcpConnection.Connect(25);
            tcpConnection.Send("help\r\n");
            tcpConnection.Receive();
         }
      }

      private void AssertMinidumpCount(int count)
      {
         RetryHelper.TryAction(TimeSpan.FromSeconds(10), () =>
         {
            var minidumps = GetMinidumps();
            CustomAssert.AreEqual(count, minidumps.Length);
         });
      }

      private void SetCrashSimulationMode(int mode)
      {
         var iniFile = Path.Combine(Path.Combine(_settings.Directories.ProgramDirectory, "Bin"), "hMailServer.ini");

         WritePrivateProfileString("Settings", "CrashSimulationMode", mode.ToString(), iniFile);

         var serviceController = new ServiceController("hMailServer");
         serviceController.Stop();
         serviceController.WaitForStatus(ServiceControllerStatus.Stopped, TimeSpan.FromSeconds(20));

         serviceController.Start();
         serviceController.WaitForStatus(ServiceControllerStatus.Running, TimeSpan.FromSeconds(20));

         base.TestFixtureSetUp();
      }

   }
}
