using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Mail;
using System.Reflection;
using System.Text;
using NUnit.Framework;
using RegressionTests.Shared;
using Attachment = hMailServer.Attachment;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class BlockedAttachmentTests : TestFixtureBase
   {
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
      }

      [Test]
      public void TestSingleBlockedAttachment()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
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
            string message = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
            CustomAssert.IsTrue(message.Contains(string.Format("The attachment {0} was blocked for delivery by the e-mail server.", attachmentName)));

         }
         finally
         {
            File.Delete(tempFile);
         }


      }

      [Test]
      public void TestTwoBlockedAttachments()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
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
            string message = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
            CustomAssert.IsTrue(message.Contains(string.Format("The attachment {0} was blocked for delivery by the e-mail server.", attachment1Name)));
            CustomAssert.IsTrue(message.Contains(string.Format("The attachment {0} was blocked for delivery by the e-mail server.", attachment2Name)));

         }
         finally
         {
            File.Delete(tempFile1);
            File.Delete(tempFile2);
         }


      }
   }
}
