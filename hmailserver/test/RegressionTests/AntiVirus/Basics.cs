// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System.IO;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.AntiVirus
{
   [TestFixture]
   public class Basics : TestFixtureBase
   {
      [Test]
      public void TestAttachmentRemoval()
      {
         SingletonProvider<TestSetup>.Instance.GetApp().Settings.AntiVirus.EnableAttachmentBlocking = true;

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

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
                              " name=\"AUTOEXEC.BAT\"\r\n" +
                              "Content-Transfer-Encoding: base64\r\n" +
                              "Content-Disposition: inline;\r\n" +
                              " filename=\"AUTOEXEC.BAT\"\r\n" +
                              "\r\n" +
                              "\r\n" +
                              "--------------050908050500020808050006--\r\n";

         SMTPClientSimulator.StaticSendRaw("test@test.com", "test@test.com", messageText);

         Message message = TestSetup.AssertRetrieveFirstMessage(account1.IMAPFolders.get_ItemByName("INBOX"));
         Assert.AreEqual(1, message.Attachments.Count);
         Assert.AreEqual("AUTOEXEC.BAT.txt", message.Attachments[0].Filename);

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
   }
}