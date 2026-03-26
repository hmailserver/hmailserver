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

namespace RegressionTests.API
{
   [TestFixture]
   public class Message : TestFixtureBase
   {
      [Test]
      [Description("Test to add a body after an attachment has been added.")]
      public void TestAddBodyAfterAttachment()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var filename = Path.Combine(Path.GetTempPath(), "dummy.txt");
         File.WriteAllText(filename, "tjena moss");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.Attachments.Add(filename);
         message.Body = "Hello";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var headerEnd = messageText.IndexOf("\r\n\r\n");
         var header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: multipart/mixed;"));
         Assert.IsTrue(messageText.Contains("Hello"));
         Assert.IsTrue(messageText.Contains("dummy.txt"));

         File.Delete(filename);
      }

      [Test]
      public void TestAddTextDuringSending()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // Send a message to the account.
         var message = new hMailServer.Message();
         Assert.AreEqual(0, message.State);

         var scripting = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Scripting;

         var signature = "MySignature";

         var script = "Sub OnAcceptMessage(oClient, message) " + Environment.NewLine +
                      " Call EventLog.Write(\"Subject:\" +message.Subject)" + Environment.NewLine +
                      " Call EventLog.Write(\"Date:\" +message.Date)" + Environment.NewLine +
                      " Call EventLog.Write(\"Body:\" +message.Body)" + Environment.NewLine +
                      " message.Body = message.Body & \"" + signature + "\" " + Environment.NewLine +
                      " message.Save() " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();
         Assert.IsEmpty(scripting.CheckSyntax());

         // Send the message.
         var recipients = new List<string>();
         recipients.Add("test@example.test");
         SmtpClientSimulator.StaticSend("test@example.test", recipients, "Hej", "Välkommen till verkligheten");

         // Check that the message exists
         var firstMessageText = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");

         Assert.IsNotEmpty(firstMessageText);
         Assert.IsTrue(firstMessageText.Contains(signature));
         Assert.Less(0, firstMessageText.IndexOf("Hej"));
      }

      [Test]
      public void TestAddTextDuringSendingAttachment()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // Send a message to the account.
         var message = new hMailServer.Message();
         Assert.AreEqual(0, message.State);

         var scripting = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Scripting;

         var signature = "MySignature";

         var script = "Sub OnAcceptMessage(oClient, message) " + Environment.NewLine +
                      " message.Body = message.Body & \"" + signature + "\" " + Environment.NewLine +
                      " message.Save() " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();

         var a = Assembly.GetExecutingAssembly();

         var mail = new MailMessage();
         mail.From = new MailAddress("test@test.se");
         mail.To.Add("test@example.test");
         mail.Subject = "Automatiskt servertest";
         mail.Body = "Detta är ett automatiskt test av epostservern.";
         mail.BodyEncoding = Encoding.GetEncoding(1252);
         mail.SubjectEncoding = Encoding.GetEncoding(1252);
         mail.Attachments.Add(new Attachment(a.Location));
         var oClient = new SmtpClient("localhost", 25);
         oClient.Send(mail);

         // Check that the message exists
         var firstMessageText = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");

         Assert.IsNotEmpty(firstMessageText, firstMessageText);
         StringAssert.Contains(signature, firstMessageText, "The message retrieved from POP3 server does not contain signature.");
      }

      [Test]
      [Description("Add text to an empty body during sending of attachments")]
      public void TestAddTextToEmptyBody()
      {
         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // Send a message to the account.
         var message = new hMailServer.Message();
         Assert.AreEqual(0, message.State);

         var scripting = SingletonProvider<TestSetup>.Instance.GetApp().Settings.Scripting;

         var signature = "MySignature";

         var script = "Sub OnAcceptMessage(oClient, message) " + Environment.NewLine +
                      " message.Body = message.Body & \"" + signature + "\" " + Environment.NewLine +
                      " message.Save() " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();

         var a = Assembly.GetExecutingAssembly();

         var mail = new MailMessage();
         mail.From = new MailAddress("test@test.se");
         mail.To.Add("test@example.test");
         mail.Subject = "Automatiskt servertest";
         mail.Body = "";
         mail.BodyEncoding = Encoding.GetEncoding(1252);
         mail.SubjectEncoding = Encoding.GetEncoding(1252);
         mail.Attachments.Add(new Attachment(a.Location));
         var oClient = new SmtpClient("localhost", 25);
         oClient.Send(mail);

         // Check that the message exists
         var firstMessageText = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");

         Assert.IsNotEmpty(firstMessageText, firstMessageText);
         StringAssert.Contains(signature, firstMessageText, "The message retrieved from POP3 server does not contain signature.");
      }

      [Test]
      [Description("Test to create a simple message with a HTML body.")]
      public void TestMailCreationHTML()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.HTMLBody = "Hello";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var headerEnd = messageText.IndexOf("\r\n\r\n");
         var header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: text/html; charset=\"utf-8\""));
         Assert.IsTrue(header.Contains("Content-Transfer-Encoding: quoted-printable"));
         Assert.IsTrue(messageText.Contains("Hello"));
      }

      [Test]
      [Description("Test to create a simple message with a HTML body and a plain text body.")]
      public void TestMailCreationHTMLAndPlainText()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.HTMLBody = "HTMLBody";
         message.Body = "PlainTextBody";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var headerEnd = messageText.IndexOf("\r\n\r\n");
         var header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: multipart/alternative"));
         Assert.IsFalse(header.Contains("Content-Transfer-Encoding: quoted-printable"));
         Assert.IsTrue(messageText.Contains("PlainTextBody"));
         Assert.IsTrue(messageText.Contains("HTMLBody"));
      }

      [Test]
      [Description("Test to create a simple message with a HTML body and a plain text body.")]
      public void TestMailCreationHTMLAndPlainTextReverse()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.Body = "PlainTextBody";
         message.HTMLBody = "HTMLBody";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var headerEnd = messageText.IndexOf("\r\n\r\n");
         var header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: multipart/alternative"));
         Assert.IsFalse(header.Contains("Content-Transfer-Encoding: quoted-printable"));
         Assert.IsTrue(messageText.Contains("PlainTextBody"));
         Assert.IsTrue(messageText.Contains("HTMLBody"));
      }

      [Test]
      [Description("Test to create a simple message with a body.")]
      public void TestMailCreationPlainText()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.Body = "Hello";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var headerEnd = messageText.IndexOf("\r\n\r\n");
         var header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: text/plain; charset=\"utf-8\""));
         Assert.IsTrue(header.Contains("Content-Transfer-Encoding: quoted-printable"));
         Assert.IsTrue(messageText.Contains("Hello"));
      }

      [Test]
      [Description("Test to create a simple message with a HTML body and a plain text body.")]
      public void TestMailCreationUnicodeAndAttachment()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var filename = Path.Combine(Path.GetTempPath(), "dummy.txt");
         File.WriteAllText(filename, "tjena moss");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.Charset = "utf-8";
         message.Attachments.Add(filename);
         // may cause an Assert.
         message.HTMLBody = "Test of message... 日本語";
         message.Body = "Test of message... 日本語";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");


         var headerEnd = messageText.IndexOf("\r\n\r\n");
         var header = messageText.Substring(0, headerEnd);

         Assert.IsTrue(header.Contains("Content-Type: multipart/mixed; charset=\"utf-8\""));

         File.Delete(filename);
      }

      [Test]
      [Description("Test to create a message with both a plain text and HTML part")]
      public void TestMailCreationUnicodeBodyAndHtml()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("", account.Address);
         message.Charset = "utf-8";
         message.Body = "Test of message... 日本語";
         message.HTMLBody = "Test of message... 日本語";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(messageText.Contains("Content-Type: text/html; charset=\"utf-8\""));
         Assert.IsTrue(messageText.Contains("Content-Type: text/plain; charset=\"utf-8\""));
      }

      [Test]
      [Description(
         "Test to update a subject in a message with no message-wide character set (should default to utf-8)")]
      public void TestUpdateSubjectOnMessageWithNoMessageWideCharacterSet()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var script = "Sub OnAcceptMessage(oClient, message) " + Environment.NewLine +
                      " message.Subject = \"[ov]\" + message.Subject " + Environment.NewLine +
                      " message.Save() " + Environment.NewLine +
                      "End Sub" + Environment.NewLine + Environment.NewLine;

         var scripting = _settings.Scripting;
         File.WriteAllText(scripting.CurrentScriptFile, script);
         scripting.Enabled = true;
         scripting.Reload();
         Assert.IsEmpty(scripting.CheckSyntax());

         var body = @"From: <test@example.com>" + Environment.NewLine +
                    "Subject: =?windows-1251?B?yuDr7Pvq7uLzIMji4O3zIC0g7/Do7OXwICLy5fXt6Pfl8eru4+4g8OX4?=" +
                    Environment.NewLine +
                    "   =?windows-1251?B?5e3o/yIgW0Z3ZDog0tAg4uXw8ejoIDEuMl0=?=" + Environment.NewLine +
                    Environment.NewLine +
                    "Hej!" + Environment.NewLine;


         SmtpClientSimulator.StaticSendRaw("encode@example.test", "encode@example.test", body);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         var subject = account.IMAPFolders[0].Messages[0].Subject;
      }
   }
}