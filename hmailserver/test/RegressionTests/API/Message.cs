// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Net.Mail;
using System.Reflection;
using System.Text;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;
using Attachment = System.Net.Mail.Attachment;

namespace RegressionTests.API
{
   [TestFixture]
   public class Message : TestFixtureBase
   {
      [Test]
      [Description("Test to add a body after an attachment has been added.")]
      public void TestAddBodyAfterAttachment()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@test.com", "test");

         string filename = Path.Combine(Path.GetTempPath(), "dummy.txt");
         File.WriteAllText(filename, "tjena moss");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.Attachments.Add(filename);
         message.Body = "Hello";
         message.Save();

         string messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int headerEnd = messageText.IndexOf("\r\n\r\n");
         string header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: multipart/mixed;"));
         Assert.IsTrue(messageText.Contains("Hello"));
         Assert.IsTrue(messageText.Contains("dummy.txt"));

         File.Delete(filename);
      }

      [Test]
      public void TestAddTextDuringSending()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         // Send a message to the account.
         var oMessage = new hMailServer.Message();
         Assert.AreEqual(0, oMessage.State);

         Scripting scripting = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Scripting;

         string signature = "MySignature";

         string script = "Sub OnAcceptMessage(oClient, oMessage) " + Environment.NewLine +
                         " Call EventLog.Write(\"Subject:\" +oMessage.Subject)" + Environment.NewLine +
                         " Call EventLog.Write(\"Date:\" +oMessage.Date)" + Environment.NewLine +
                         " Call EventLog.Write(\"Body:\" +oMessage.Body)" + Environment.NewLine +
                         " oMessage.Body = oMessage.Body & \"" + signature + "\" " + Environment.NewLine +
                         " oMessage.Save() " + Environment.NewLine +
                         "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();
         Assert.IsEmpty(scripting.CheckSyntax());

         // Send the message.
         var recipients = new List<string>();
         recipients.Add("test@test.com");
         SmtpClientSimulator.StaticSend("test@test.com", recipients, "Hej", "Välkommen till verkligheten");

         // Check that the message exists
         string message = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         Assert.IsNotEmpty(message);
         Assert.IsTrue(message.Contains(signature));
         Assert.Less(0, message.IndexOf("Hej"));
      }

      [Test]
      public void TestAddTextDuringSendingAttachment()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         // Send a message to the account.
         var oMessage = new hMailServer.Message();
         Assert.AreEqual(0, oMessage.State);

         Scripting scripting = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Scripting;

         string signature = "MySignature";

         string script = "Sub OnAcceptMessage(oClient, oMessage) " + Environment.NewLine +
                         " oMessage.Body = oMessage.Body & \"" + signature + "\" " + Environment.NewLine +
                         " oMessage.Save() " + Environment.NewLine +
                         "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();

         Assembly a = Assembly.GetExecutingAssembly();

         var mail = new MailMessage();
         mail.From = new MailAddress("test@test.se");
         mail.To.Add("test@test.com");
         mail.Subject = "Automatiskt servertest";
         mail.Body = "Detta är ett automatiskt test av epostservern.";
         mail.BodyEncoding = Encoding.GetEncoding(1252);
         mail.SubjectEncoding = Encoding.GetEncoding(1252);
         mail.Attachments.Add(new Attachment(a.Location));
         var oClient = new SmtpClient("localhost", 25);
         oClient.Send(mail);

         // Check that the message exists
         string message = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         Assert.IsNotEmpty(message, message);
         Assert.IsTrue(message.Contains(signature), message);
      }

      [Test]
      [Description("Add text to an empty body during sending of attachments")]
      public void TestAddTextToEmptyBody()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         // Send a message to the account.
         var oMessage = new hMailServer.Message();
         Assert.AreEqual(0, oMessage.State);

         Scripting scripting = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Scripting;

         string signature = "MySignature";

         string script = "Sub OnAcceptMessage(oClient, oMessage) " + Environment.NewLine +
                         " oMessage.Body = oMessage.Body & \"" + signature + "\" " + Environment.NewLine +
                         " oMessage.Save() " + Environment.NewLine +
                         "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();

         Assembly a = Assembly.GetExecutingAssembly();

         var mail = new MailMessage();
         mail.From = new MailAddress("test@test.se");
         mail.To.Add("test@test.com");
         mail.Subject = "Automatiskt servertest";
         mail.Body = "";
         mail.BodyEncoding = Encoding.GetEncoding(1252);
         mail.SubjectEncoding = Encoding.GetEncoding(1252);
         mail.Attachments.Add(new Attachment(a.Location));
         var oClient = new SmtpClient("localhost", 25);
         oClient.Send(mail);

         // Check that the message exists
         string message = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         Assert.IsNotEmpty(message, message);
         Assert.IsTrue(message.Contains(signature), message);
      }

      [Test]
      [Description("Test to create a simple message with a HTML body.")]
      public void TestMailCreationHTML()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@test.com", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.HTMLBody = "Hello";
         message.Save();

         string messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int headerEnd = messageText.IndexOf("\r\n\r\n");
         string header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: text/html; charset=\"utf-8\""));
         Assert.IsTrue(header.Contains("Content-Transfer-Encoding: quoted-printable"));
         Assert.IsTrue(messageText.Contains("Hello"));
      }

      [Test]
      [Description("Test to create a simple message with a HTML body and a plain text body.")]
      public void TestMailCreationHTMLAndPlainText()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@test.com", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.HTMLBody = "HTMLBody";
         message.Body = "PlainTextBody";
         message.Save();

         string messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int headerEnd = messageText.IndexOf("\r\n\r\n");
         string header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: multipart/alternative"));
         Assert.IsFalse(header.Contains("Content-Transfer-Encoding: quoted-printable"));
         Assert.IsTrue(messageText.Contains("PlainTextBody"));
         Assert.IsTrue(messageText.Contains("HTMLBody"));
      }

      [Test]
      [Description("Test to create a simple message with a HTML body and a plain text body.")]
      public void TestMailCreationHTMLAndPlainTextReverse()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@test.com", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.Body = "PlainTextBody";
         message.HTMLBody = "HTMLBody";
         message.Save();

         string messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int headerEnd = messageText.IndexOf("\r\n\r\n");
         string header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: multipart/alternative"));
         Assert.IsFalse(header.Contains("Content-Transfer-Encoding: quoted-printable"));
         Assert.IsTrue(messageText.Contains("PlainTextBody"));
         Assert.IsTrue(messageText.Contains("HTMLBody"));
      }

      [Test]
      [Description("Test to create a simple message with a body.")]
      public void TestMailCreationPlainText()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@test.com", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.Body = "Hello";
         message.Save();

         string messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int headerEnd = messageText.IndexOf("\r\n\r\n");
         string header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: text/plain; charset=\"utf-8\""));
         Assert.IsTrue(header.Contains("Content-Transfer-Encoding: quoted-printable"));
         Assert.IsTrue(messageText.Contains("Hello"));
      }

      [Test]
      [Description("Test to create a simple message with a HTML body and a plain text body.")]
      public void TestMailCreationUnicodeAndAttachment()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@test.com", "test");

         string filename = Path.Combine(Path.GetTempPath(), "dummy.txt");
         File.WriteAllText(filename, "tjena moss");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.Charset = "utf-8";
         message.Attachments.Add(filename);
         // may cause an Assert.
         message.HTMLBody = "Test of message... 日本語";
         message.Body = "Test of message... 日本語";
         message.Save();

         string messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");


         int headerEnd = messageText.IndexOf("\r\n\r\n");
         string header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: multipart/mixed; charset=\"utf-8\""));

         File.Delete(filename);
      }

      [Test]
      [Description("Test to create a message with both a plain text and HTML part")]
      public void TestMailCreationUnicodeBodyAndHtml()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@test.com", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.Charset = "utf-8";
         message.Body = "Test of message... 日本語";
         message.HTMLBody = "Test of message... 日本語";
         message.Save();

         string messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(messageText.Contains("Content-Type: text/html; charset=\"utf-8\""));
         Assert.IsTrue(messageText.Contains("Content-Type: text/plain; charset=\"utf-8\""));
      }

      [Test]
      [Description(
         "Test to update a subject in a message with no message-wide character set (should default to utf-8)")]
      public void TestUpdateSubjectOnMessageWithNoMessageWideCharacterSet()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@test.com", "test");

         string script = "Sub OnAcceptMessage(oClient, oMessage) " + Environment.NewLine +
                         " oMessage.Subject = \"[ov]\" + oMessage.Subject " + Environment.NewLine +
                         " oMessage.Save() " + Environment.NewLine +
                         "End Sub" + Environment.NewLine + Environment.NewLine;

         Scripting scripting = _settings.Scripting;
         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();
         Assert.IsEmpty(scripting.CheckSyntax());

         string body = @"From: <test@example.com>" + Environment.NewLine +
                       "Subject: =?windows-1251?B?yuDr7Pvq7uLzIMji4O3zIC0g7/Do7OXwICLy5fXt6Pfl8eru4+4g8OX4?=" +
                       Environment.NewLine +
                       "   =?windows-1251?B?5e3o/yIgW0Z3ZDog0tAg4uXw8ejoIDEuMl0=?=" + Environment.NewLine +
                       Environment.NewLine +
                       "Hej!" + Environment.NewLine;


         SmtpClientSimulator.StaticSendRaw("encode@test.com", "encode@test.com", body);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         string subject = account.IMAPFolders[0].Messages[0].Subject;
      }
   }
}