// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using NUnit.Framework;
using System.IO;
using RegressionTests.Shared;


namespace StressTest
{
    [TestFixture]
    public class DKIM : TestFixtureBase
    {
        [SetUp]
        public new void SetUp()
        {
            SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
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

            string folderGood = Path.GetFullPath("../../../TestData/DKIM/Good");
            string path = Path.Combine(Environment.CurrentDirectory, folderGood);
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

            string folderMissingBH = Path.GetFullPath("../../../TestData/DKIM/Neutral - Missing bodyhash");
            string path = Path.Combine(Environment.CurrentDirectory, folderMissingBH);
            string[] files = Directory.GetFiles(path);

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

            string folder = Path.GetFullPath("../../../TestData/DKIM/Unsupported");
            string path = Path.Combine(Environment.CurrentDirectory, folder);
            string[] files = Directory.GetFiles(path);

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
           

           string folder = Path.GetFullPath("../../../TestData/DKIM/PermFail");
           string path = Path.Combine(Environment.CurrentDirectory, folder);
           string[] files = Directory.GetFiles(path);

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

            string folderGood = Path.GetFullPath("../../../TestData/DKIM/Good");
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
