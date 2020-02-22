using System;
using System.IO;
using System.Net.Mail;
using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class BlockedAttachmentTests : TestFixtureBase
   {
      private hMailServer.Account _account;

      [SetUp]
      public new void SetUp()
      {
         bool attachmentSetUp = false;
         var blockedAttachments = _settings.AntiVirus.BlockedAttachments;

         for (int i = 0; i < blockedAttachments.Count; i++)
         {
            var ba = blockedAttachments[i];

            if (ba.Wildcard == "*.dll")
            {
               attachmentSetUp = true;
               break;
            }
         }

         if (!attachmentSetUp)
         {
            var blockedAttachment = blockedAttachments.Add();
            blockedAttachment.Wildcard = "*.dll";
            blockedAttachment.Save();
         }

         _settings.AntiVirus.EnableAttachmentBlocking = true;

         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
      }

      [Test]
      public void TestAttachmentRemoval()
      {
         // Send a message to the account.
         string messageText = @"Date: Thu, 03 Jul 2008 22:01:53 +0200\r\n" +
                              "From: Test <test@test.com>\r\n" +
                              "MIME-Version: 1.0\r\n" +
                              "To: test@test.com\r\n" +
                              "Subject: test\r\n" +
                              "Content-Type: multipart/mixed;\r\n" +
                              "  boundary=\"------------050908050500020808050006\"\r\n" +
                              "\r\n" +
                              "This is a multi-part message in MIME format.\r\n" +
                              "--------------050908050500020808050006\r\n" +
                              "Content-Type: text/plain; charset=ISO-8859-1; format=flowed\r\n" +
                              "Content-Transfer-Encoding: 7bit\r\n" +
                              "\r\n" +
                              "Test\r\n" +
                              "\r\n" +
                              "--------------050908050500020808050006\r\n" +
                              "Content-Type: text/plain;\r\n" +
                              " name=\"AUTOEXEC.dll\"\r\n" +
                              "Content-Transfer-Encoding: base64\r\n" +
                              "Content-Disposition: inline;\r\n" +
                              " filename=\"AUTOEXEC.dll\"\r\n" +
                              "\r\n" +
                              "\r\n" +
                              "--------------050908050500020808050006--\r\n";

         SmtpClientSimulator.StaticSendRaw("test@test.com", "test@test.com", messageText);

         ImapClientSimulator.AssertMessageCount("test@test.com", "test", "Inbox", 1);

         Message message = CustomAsserts.AssertRetrieveFirstMessage(_account.IMAPFolders.get_ItemByName("INBOX"));
         Assert.AreEqual(1, message.Attachments.Count);
         Assert.AreEqual("AUTOEXEC.dll.txt", message.Attachments[0].Filename);

         string tempFile = Path.GetTempFileName();
         message.Attachments[0].SaveAs(tempFile);
         string contents = File.ReadAllText(tempFile);

         string removedMessage =
            SingletonProvider<TestSetup>.Instance.GetApp().Settings.ServerMessages.get_ItemByName(
               "ATTACHMENT_REMOVED").Text;
         removedMessage = removedMessage.Replace("%MACRO_FILE%",
                                                 message.Attachments[0].Filename.Substring(0,
                                                                                           message.Attachments[0].
                                                                                              Filename.Length - 4));

         Assert.IsTrue(contents.Contains(removedMessage));
         File.Delete(tempFile);
      }


      [Test]
      public void TestSingleBlockedAttachment()
      {
         var attachmentName = Guid.NewGuid().ToString() + ".dll";
         var tempFile = Path.Combine(Path.GetTempPath(), attachmentName);
         File.WriteAllText(tempFile, "A");

         try
         {
            using (var attachment = new System.Net.Mail.Attachment(tempFile))
            {
               var mail = new MailMessage();
               mail.From = new MailAddress("test@test.com");
               mail.To.Add("test@test.com");
               mail.Subject = "Test";
               mail.Body = "Test";
               mail.BodyEncoding = Encoding.GetEncoding(1252);
               mail.SubjectEncoding = Encoding.GetEncoding(1252);
               mail.Attachments.Add(attachment);

               var smtpClient = new SmtpClient("localhost", 25);
               smtpClient.Send(mail);
            }

            // Check that the message exists
            string message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
            Assert.IsTrue(message.Contains(string.Format("The attachment {0} was blocked for delivery by the e-mail server.", attachmentName)));

         }
         finally
         {
            File.Delete(tempFile);
         }


      }

      [Test]
      public void TestTwoBlockedAttachments()
      {
         var attachment1Name = Guid.NewGuid().ToString() + ".dll";
         var attachment2Name = Guid.NewGuid().ToString() + ".dll";
         var tempFile1 = Path.Combine(Path.GetTempPath(), attachment1Name);
         var tempFile2 = Path.Combine(Path.GetTempPath(), attachment2Name);
         File.WriteAllText(tempFile1, "A");
         File.WriteAllText(tempFile2, "A");

         try
         {
            using (var attachment1 = new System.Net.Mail.Attachment(tempFile1))
            using (var attachment2 = new System.Net.Mail.Attachment(tempFile2))
            {
               var mail = new MailMessage();
               mail.From = new MailAddress("test@test.com");
               mail.To.Add("test@test.com");
               mail.Subject = "Test";
               mail.Body = "Test";
               mail.BodyEncoding = Encoding.GetEncoding(1252);
               mail.SubjectEncoding = Encoding.GetEncoding(1252);
               mail.Attachments.Add(attachment1);
               mail.Attachments.Add(attachment2);

               var smtpClient = new SmtpClient("localhost", 25);
               smtpClient.Send(mail);
            }

            // Check that the message exists
            string message = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
            Assert.IsTrue(message.Contains(string.Format("The attachment {0} was blocked for delivery by the e-mail server.", attachment1Name)));
            Assert.IsTrue(message.Contains(string.Format("The attachment {0} was blocked for delivery by the e-mail server.", attachment2Name)));

         }
         finally
         {
            File.Delete(tempFile1);
            File.Delete(tempFile2);
         }


      }

      [Test]
      public void TestBlockedAttachmentWithUnicodeInName()
      {
         var attachmentName = Guid.NewGuid().ToString() + "漢語.dll";
         var tempFile = Path.Combine(Path.GetTempPath(), attachmentName);
         File.WriteAllText(tempFile, "A");

         try
         {
            using (var attachment = new System.Net.Mail.Attachment(tempFile))
            {
               var mail = new MailMessage();
               mail.From = new MailAddress("test@test.com");
               mail.To.Add("test@test.com");
               mail.Subject = "Test";
               mail.Body = "Test";
               mail.BodyEncoding = Encoding.GetEncoding(1252);
               mail.SubjectEncoding = Encoding.GetEncoding(1252);
               mail.Attachments.Add(attachment);

               var smtpClient = new SmtpClient("localhost", 25);
               smtpClient.Send(mail);
            }

            string expectedNewAttachmentName = attachmentName + ".txt";

            ImapClientSimulator.AssertMessageCount("test@test.com", "test", "Inbox", 1);

            Message message = CustomAsserts.AssertRetrieveFirstMessage(_account.IMAPFolders.get_ItemByName("INBOX"));
            Assert.AreEqual(1, message.Attachments.Count);
            Assert.AreEqual(expectedNewAttachmentName, message.Attachments[0].Filename);

            string attachmentOnDisk = Path.GetTempFileName();
            message.Attachments[0].SaveAs(attachmentOnDisk);
            string contents = File.ReadAllText(attachmentOnDisk);

            Assert.IsTrue(
               contents.Contains(string.Format("The attachment {0} was blocked for delivery by the e-mail server.",
                  attachmentName)), contents);

            File.Delete(attachmentOnDisk);

         }
         finally
         {
            File.Delete(tempFile);
         }


      }
   }
}
