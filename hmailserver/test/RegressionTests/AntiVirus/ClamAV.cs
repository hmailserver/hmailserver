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
      public void TestUnexpectedResponseIsReportedAsError()
      {
         // The POP3 greeting is neither "OK" nor "FOUND", so the scan should be reported as failed.
         _antiVirus.ClamAVEnabled = true;
         _antiVirus.ClamAVPort = 110;

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account1.Address, account1.Address, "Mail 1", "DummyBody");
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);

         CustomAsserts.AssertReportedError("ClamAVVirusScanner::Scan", "+OK POP3");
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

         // The infected message should have been deleted, so the notification is the only message.
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);

         var notification = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         Assert.IsFalse(notification.Contains(secondPart));
         Assert.IsTrue(notification.Contains("In-Reply-To: " + messageID));
         Assert.IsTrue(notification.Contains("References: " + previousReference + " " + messageID));

         var defaultLog = LogHandler.ReadCurrentDefaultLog();
         Assert.IsTrue(defaultLog.Contains("Connecting to ClamAV"));
         Assert.IsTrue(defaultLog.Contains("Message will be deleted (contained virus Eicar-Signature)."));
      }

      [Test]
      public void TestStripAttachmentsKeepsTextPartWhenAttachmentIsFirst()
      {
         var message =
            "Subject: Mail 1\r\n" +
            "MIME-Version: 1.0\r\n" +
            "Content-Type: multipart/mixed; boundary=\"b1\"\r\n" +
            "\r\n" +
            "--b1\r\n" +
            "Content-Type: application/octet-stream; name=\"eicar.com\"\r\n" +
            "Content-Disposition: attachment; filename=\"eicar.com\"\r\n" +
            "\r\n" +
            EicarFirstPart + EicarSecondPart + "\r\n" +
            "--b1\r\n" +
            "Content-Type: text/plain; charset=us-ascii\r\n" +
            "\r\n" +
            "TextBodyToKeep\r\n" +
            "--b1--\r\n";

         var text = SendWithStripAttachments(message);

         Assert.IsTrue(text.Contains("TextBodyToKeep"));
      }

      [Test]
      public void TestStripAttachmentsDoesNotKeepTextAttachment()
      {
         var message =
            "Subject: Mail 1\r\n" +
            "MIME-Version: 1.0\r\n" +
            "Content-Type: multipart/mixed; boundary=\"b1\"\r\n" +
            "\r\n" +
            "--b1\r\n" +
            "Content-Type: text/plain\r\n" +
            "Content-Disposition: attachment; filename=\"eicar.com\"\r\n" +
            "\r\n" +
            EicarFirstPart + EicarSecondPart + "\r\n" +
            "--b1\r\n" +
            "Content-Type: text/plain; charset=us-ascii\r\n" +
            "\r\n" +
            "TextBodyToKeep\r\n" +
            "--b1--\r\n";

         var text = SendWithStripAttachments(message);

         Assert.IsTrue(text.Contains("TextBodyToKeep"));
      }

      [Test]
      public void TestStripAttachmentsInfectedTextBody()
      {
         var message =
            "Subject: Mail 1\r\n" +
            "MIME-Version: 1.0\r\n" +
            "Content-Type: multipart/mixed; boundary=\"b1\"\r\n" +
            "\r\n" +
            "--b1\r\n" +
            "Content-Type: text/plain; charset=us-ascii\r\n" +
            "\r\n" +
            EicarFirstPart + EicarSecondPart + "\r\n" +
            "--b1\r\n" +
            "Content-Type: application/octet-stream; name=\"harmless.bin\"\r\n" +
            "Content-Disposition: attachment; filename=\"harmless.bin\"\r\n" +
            "Content-Transfer-Encoding: base64\r\n" +
            "\r\n" +
            "SGVsbG8=\r\n" +
            "--b1--\r\n";

         SendWithStripAttachments(message);
      }

      [Test]
      public void TestStripAttachmentsSinglePartMessage()
      {
         var message =
            "Subject: Mail 1\r\n" +
            "MIME-Version: 1.0\r\n" +
            "Content-Type: application/octet-stream; name=\"eicar.com\"\r\n" +
            "\r\n" +
            EicarFirstPart + EicarSecondPart + "\r\n";

         SendWithStripAttachments(message);
      }

      [Test]
      public void TestStripAttachmentsWithoutTextPart()
      {
         var message =
            "Subject: Mail 1\r\n" +
            "MIME-Version: 1.0\r\n" +
            "Content-Type: multipart/mixed; boundary=\"b1\"\r\n" +
            "\r\n" +
            "--b1\r\n" +
            "Content-Type: application/octet-stream; name=\"eicar.com\"\r\n" +
            "Content-Disposition: attachment; filename=\"eicar.com\"\r\n" +
            "\r\n" +
            EicarFirstPart + EicarSecondPart + "\r\n" +
            "--b1--\r\n";

         SendWithStripAttachments(message);
      }

      // Split so that local virus scanners do not flag this source file.
      private const string EicarFirstPart = @"X5O!P%@AP[4\PZX54(P^)7CC)7}";
      private const string EicarSecondPart = @"$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";

      // Sends the message with the strip attachments action and asserts the virus is not delivered.
      private string SendWithStripAttachments(string message)
      {
         _antiVirus.ClamAVEnabled = true;
         _antiVirus.Action = eAntivirusAction.hDeleteAttachments;
         LogHandler.DeleteCurrentDefaultLog();

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, message);

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
         Pop3ClientSimulator.AssertMessageCount(account1.Address, "test", 1);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
         Assert.IsFalse(text.Contains(EicarSecondPart));

         var defaultLog = LogHandler.ReadCurrentDefaultLog();
         Assert.IsTrue(defaultLog.Contains("Message attachments stripped (contained virus Eicar-Signature)."));

         return text;
      }
   }
}