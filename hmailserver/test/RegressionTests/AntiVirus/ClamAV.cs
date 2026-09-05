using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.AntiVirus
{
   [TestFixture]
   public class ClamAV : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         CustomAsserts.AssertClamDRunning();

         _antiVirus = _application.Settings.AntiVirus;

         _antiVirus.Action = eAntivirusAction.hDeleteEmail;
      }

      private hMailServer.AntiVirus _antiVirus;

      [Test]
      public void TestIncorrectPort()
      {
         _antiVirus.ClamAVEnabled = true;
         _antiVirus.ClamAVPort = 110;

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "DummyBody");
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);

         // +OK POP3, since we are connecting to POP3 port
         var defaultLog = LogHandler.ReadCurrentDefaultLog();
         Assert.IsTrue(defaultLog.Contains("No virus detected: +OK POP3"));
      }

      [Test]
      public void TestNoVirus()
      {
         _antiVirus.ClamAVEnabled = true;

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "Mail 1");
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);
      }

      [Test]
      public void TestNotEnabled()
      {
         LogHandler.DeleteCurrentDefaultLog();
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "Mail 1");
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);
         var defaultLog = LogHandler.ReadCurrentDefaultLog();
         Assert.IsFalse(defaultLog.Contains("Connecting to ClamAV"));
      }

      [Test]
      public void TestUnusedPort()
      {
         _antiVirus.ClamAVEnabled = true;
         _antiVirus.ClamAVPort = 54391;

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "DummyBody");
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);
         CustomAsserts.AssertReportedError("Unable to connect to ClamAV server at localhost:54391.");
      }

      [Test]
      public void TestWithVirus()
      {
         _antiVirus.ClamAVEnabled = true;
         _antiVirus.NotifySender = true;
         LogHandler.DeleteCurrentDefaultLog();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         var firstPart = @"X5O!P%@AP[4\PZX54(P^)7CC)7}";
         var secondPart = @"$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";
         const string messageID = "<virus-threading@example.test>";
         const string previousReference = "<previous-virus-message@example.test>";
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address,
            "Message-ID: " + messageID + "\r\n" +
            "References: " + previousReference + "\r\n" +
            "Subject: Mail 1\r\n" +
            "\r\n" +
            firstPart + secondPart);

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         var notification = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         Assert.IsTrue(notification.Contains("In-Reply-To: " + messageID));
         Assert.IsTrue(notification.Contains("References: " + previousReference + " " + messageID));

         var defaultLog = LogHandler.ReadCurrentDefaultLog();
         Assert.IsTrue(defaultLog.Contains("Connecting to ClamAV"));
         Assert.IsTrue(defaultLog.Contains("Message will be deleted (contained virus Eicar-Signature)."));
      }
   }
}