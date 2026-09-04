// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using NUnit.Framework;
using System.IO;
using RegressionTests.Shared;


namespace VolumeTests
{
    [TestFixture]
    public class DKIM : TestFixtureBase
    {
        [SetUp]
        public new void SetUp()
        {
            SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
        }

        private void DeleteCurrentLog()
        {
           hMailServer.Logging logging = _application.Settings.Logging;
           logging.LogDebug = true;
           if (File.Exists(logging.CurrentDefaultLog))
              File.Delete(logging.CurrentDefaultLog);
        }

        private bool VerifyLoadSuccess()
        {
           string logContent = File.ReadAllText(_application.Settings.Logging.CurrentDefaultLog);
           return logContent.Contains("DKIM: Message passed validation.");
        }

        /// <summary>
        /// Test a bunch of DKIM messages.
        /// </summary>
        [Test]
        public void TestDKIMGood()
        {
            hMailServer.AntiSpam antiSpam = _application.Settings.AntiSpam;

            string folderGood = Shared.GetTestDataPath("DKIM/Good");
            string[] files = Directory.GetFiles(folderGood);

            foreach (string file in files)
            {
                DeleteCurrentLog();
                Console.WriteLine(string.Format("Testing file {0}...", file));
                hMailServer.eDKIMResult result = antiSpam.DKIMVerify(file);
                Assert.AreEqual(hMailServer.eDKIMResult.eDKPass, result, file);
                Assert.IsTrue(VerifyLoadSuccess());

            }
        }

        [Test]
        public void TestDKIMMissingBH()
        {
            hMailServer.AntiSpam antiSpam = _application.Settings.AntiSpam;

            string folderMissingBH = Shared.GetTestDataPath("DKIM/Neutral - Missing bodyhash");
            string[] files = Directory.GetFiles(folderMissingBH);

            foreach (string file in files)
            {
                hMailServer.eDKIMResult result = antiSpam.DKIMVerify(file);
                Assert.AreEqual(hMailServer.eDKIMResult.eDKNeutral, result, file);
            }
        }

        [Test]
        public void TestDKIMUnsupported()
        {
            hMailServer.AntiSpam antiSpam = _application.Settings.AntiSpam;

            string folder = Shared.GetTestDataPath("DKIM/Unsupported");
            string[] files = Directory.GetFiles(folder);

            foreach (string file in files)
            {
                DeleteCurrentLog();
                hMailServer.eDKIMResult result = antiSpam.DKIMVerify(file);
                Assert.AreEqual(hMailServer.eDKIMResult.eDKNeutral, result, file);
                Assert.IsFalse(VerifyLoadSuccess());
            }
        }

        [Test]
        public void TestDKIMBadSignature()
        {
            hMailServer.AntiSpam antiSpam = _application.Settings.AntiSpam;
           

           string folder = Shared.GetTestDataPath("DKIM/PermFail");
           string[] files = Directory.GetFiles(folder);

           foreach (string file in files)
           {
              DeleteCurrentLog();
              hMailServer.eDKIMResult result = antiSpam.DKIMVerify(file);
              Assert.AreEqual(hMailServer.eDKIMResult.eDKPermFail, result, file);
              Assert.IsFalse(VerifyLoadSuccess());
           }
        }

        [Test]
        public void TestDKIMMassTest()
        {
            int verificationCount = 5000;

            hMailServer.AntiSpam antiSpam = _application.Settings.AntiSpam;

            string folderGood = Shared.GetTestDataPath("DKIM/Good");
            string goodFile = Directory.GetFiles(folderGood)[0];

            for (int i = 0; i < verificationCount; i++)
            {
                DeleteCurrentLog();
                hMailServer.eDKIMResult result = antiSpam.DKIMVerify(goodFile);
                Assert.AreEqual(hMailServer.eDKIMResult.eDKPass, result, goodFile);
                Assert.IsTrue(VerifyLoadSuccess());
            }
        }
    }
}
