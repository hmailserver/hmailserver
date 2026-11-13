using System;
using System.IO;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.AntiVirus
{
   [TestFixture]
   public class ClamWin : TestFixtureBase
   {
      private const string ClamScanExecutable = @"C:\Program Files\ClamAV\clamscan.exe";

      [Test]
      public void TestWithVirus()
      {
         if (!File.Exists(ClamScanExecutable))
            Assert.Ignore("clamscan.exe not found at " + ClamScanExecutable);

         var antiVirus = _application.Settings.AntiVirus;
         antiVirus.Action = eAntivirusAction.hDeleteEmail;
         antiVirus.NotifySender = true;
         antiVirus.ClamWinEnabled = true;
         antiVirus.ClamWinExecutable = "\"" + ClamScanExecutable + "\"";

         var firstPart = @"X5O!P%@AP[4\PZX54(P^)7CC)7}";
         var secondPart = @"$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";

         // The full ClamAV database takes ~20 seconds to load, so use one that only matches EICAR.
         var databaseFolder = Path.Combine(_application.Settings.Directories.TempDirectory, "ClamWinTestDatabase");
         Directory.CreateDirectory(databaseFolder);
         var eicarHex = BitConverter.ToString(Encoding.ASCII.GetBytes(firstPart + secondPart)).Replace("-", "").ToLower();
         File.WriteAllText(Path.Combine(databaseFolder, "eicar.ndb"), "Eicar-Test:0:*:" + eicarHex + "\n");
         antiVirus.ClamWinDBFolder = databaseFolder;

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address,
            "Subject: Mail 1\r\n" +
            "\r\n" +
            firstPart + secondPart);

         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);

         // The infected message should have been deleted, so the notification is the only message.
         var notification = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         Assert.IsFalse(notification.Contains(secondPart));

         Directory.Delete(databaseFolder, true);
      }

      [Test]
      public void TestScannerErrorIsReportedAsError()
      {
         // clamscan exits with 2 when an error occurs. That should not be treated as a clean result.
         var antiVirus = _application.Settings.AntiVirus;
         antiVirus.ClamWinEnabled = true;
         antiVirus.ClamWinExecutable = "cmd.exe /c exit 2";

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "DummyBody");
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);

         CustomAsserts.AssertReportedError("ClamWinVirusScanner::Scan", "Return code: 2");
      }
   }
}
