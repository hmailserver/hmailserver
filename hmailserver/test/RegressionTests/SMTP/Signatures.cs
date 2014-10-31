// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.SMTP
{
   [TestFixture]
   public class Signatures : TestFixtureBase
   {
      private Account _account;

      [SetUp]
      public new void SetUp()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "signature@test.com", "test");
      }

      [Test]
      public void TestAccountSignatureMacro()
      {
         _domain.SignatureEnabled = true;
         _domain.AddSignaturesToLocalMail = true;

         _account.PersonFirstName = "Martin";
         _account.PersonLastName = "Knafve";
         _account.SignatureEnabled = true;
         _account.SignaturePlainText = "Regards %User.FirstName% %User.Lastname%";
         _account.Save();

         SmtpClientSimulator.StaticSend(_account.Address, _account.Address, "Test of signature, 2",
                                        "Test of signature - Body");

         string sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

         Assert.IsTrue(sMessageData.Contains("Regards Martin Knafve"));
      }

      /// <summary>
      ///  Test different setups for domain signature.
      /// </summary>
      [Test]
      public void TestDomainSignature()
      {
         var pop3ClientSimulator = new Pop3ClientSimulator();

         _domain.SignatureEnabled = true;
         _domain.AddSignaturesToLocalMail = true;

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "domainsignature@test.com",
                                                                              "test");
         oAccount1.SignatureEnabled = true;
         oAccount1.SignaturePlainText = "PlainTextSignature";
         oAccount1.Save();


         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send(oAccount1.Address, oAccount1.Address, "Test of signature, 1", "Test of signature - Body");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.IndexOf("PlainTextSignature") <= 0)
            throw new Exception("Did not find signature");


         _domain.SignatureEnabled = true;
         _domain.SignaturePlainText = "DomainSignature";
         _domain.SignatureMethod = eDomainSignatureMethod.eSMAppendToAccountSignature;
         _domain.Save();
         smtpClientSimulator.Send(oAccount1.Address, oAccount1.Address, "Test of signature, 1", "Test of signature - Body");

         sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.IndexOf("PlainTextSignature") <= 0 ||
             sMessageContents.IndexOf("DomainSignature") <= 0)
            throw new Exception("Did not find signature");


         _domain.SignatureEnabled = true;
         _domain.SignaturePlainText = "DomainSignature";
         _domain.SignatureMethod = eDomainSignatureMethod.eSMOverwriteAccountSignature;
         _domain.Save();
         smtpClientSimulator.Send(oAccount1.Address, oAccount1.Address, "Test of signature, 1", "Test of signature - Body");

         sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.IndexOf("PlainTextSignature") >= 0 ||
             sMessageContents.IndexOf("DomainSignature") <= 0)
            throw new Exception("Did not find signature");


         _domain.SignatureEnabled = true;
         _domain.SignaturePlainText = "DomainSignature";
         _domain.SignatureMethod = eDomainSignatureMethod.eSMSetIfNotSpecifiedInAccount;
         _domain.Save();
         smtpClientSimulator.Send(oAccount1.Address, oAccount1.Address, "Test of signature, 1", "Test of signature - Body");

         sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.IndexOf("DomainSignature") >= 0)
            throw new Exception("Found incorrect signature.");


         _domain.SignatureEnabled = true;
         _domain.SignaturePlainText = "DomainSignature";
         _domain.SignatureMethod = eDomainSignatureMethod.eSMSetIfNotSpecifiedInAccount;
         _domain.Save();
         oAccount1.SignaturePlainText = "";
         oAccount1.Save();

         smtpClientSimulator.Send(oAccount1.Address, oAccount1.Address, "Test of signature, 1", "Test of signature - Body");

         sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.IndexOf("DomainSignature") <= 0)
            throw new Exception("Found incorrect signature.");


         _domain.AddSignaturesToLocalMail = false;
         _domain.Save();
         oAccount1.SignaturePlainText = "PlainTextSignature";
         oAccount1.Save();
         smtpClientSimulator.Send(oAccount1.Address, oAccount1.Address, "Test of signature, 1", "Test of signature - Body");

         sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.IndexOf("PlainTextSignature") > 0)
            throw new Exception("Found incorrect signature.");

         _domain.AddSignaturesToLocalMail = true;
         _domain.Save();

         smtpClientSimulator.Send(oAccount1.Address, oAccount1.Address, "Test of signature, 1", "Test of signature - Body");

         sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");
         if (sMessageContents.IndexOf("PlainTextSignature") <= 0)
            throw new Exception("Found incorrect signature.");
      }

      [Test]
      public void TestDomainSignatureMacro()
      {
         _domain.SignatureEnabled = true;
         _domain.AddSignaturesToLocalMail = true;
         _domain.SignaturePlainText = "Regards %User.Lastname%, %User.FirstName%";
         _domain.SignatureMethod = eDomainSignatureMethod.eSMSetIfNotSpecifiedInAccount;
         _domain.Save();

         _account.PersonFirstName = "Martin";
         _account.PersonLastName = "Knafve";
         _account.Save();

         SmtpClientSimulator.StaticSend(_account.Address, _account.Address, "Test of signature, 2",
                                        "Test of signature - Body");

         string sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

         Assert.IsTrue(sMessageData.Contains("Regards Knafve, Martin"));
      }

      [Test]
      public void TestSignature()
      {
         _domain.SignatureEnabled = true;
         _domain.AddSignaturesToLocalMail = true;

         _account.SignatureEnabled = true;
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send(_account.Address, _account.Address, "Test of signature, 1", "Test of signature - Body");

         string sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

         if (sMessageData.IndexOf("PlainTextSignature") > 0)
            throw new Exception("Found exception which should not be there");

         _account.SignaturePlainText = "PlainTextSignature";
         _account.Save();
         smtpClientSimulator.Send(_account.Address, _account.Address, "Test of signature, 2", "Test of signature - Body");


         sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

         if (sMessageData.IndexOf("PlainTextSignature") < 0)
            throw new Exception("Did not find expected signature");

         // Turn off signature again
         _account.SignatureEnabled = false;
         _account.Save();

         smtpClientSimulator.Send(_account.Address, _account.Address, "Test of signature, 2", "Test of signature - Body");
         sMessageData = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");

         if (sMessageData.IndexOf("PlainTextSignature") > 0)
            throw new Exception("Found signature even though there shouldn't be any");
      }

      /// <summary>
      /// Check that domain signature is not added to message sent from external domain.
      /// </summary>
      [Test]
      public void TestSignatureExternalDomain()
      {
         _domain.SignatureEnabled = true;
         _domain.SignaturePlainText = "MyDomainSignature";
         _domain.AddSignaturesToLocalMail = true;
         _domain.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@test.com", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("nonexistant@dummy-example.com", account.Address, "SignatureTest", "SignaturerTestBody");

         string messageData = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsFalse(messageData.Contains(_domain.SignaturePlainText));
      }

      /// <summary>
      ///  Check that domain signature is added to mail sent from local domain but non-existent account.
      ///  This may be the case if an email is sent for example from an alias address.
      /// </summary>
      [Test]
      public void TestSignatureLocalDomainNonExistantAccount()
      {
         _domain.SignatureEnabled = true;
         _domain.SignaturePlainText = "MyDomainSignature";
         _domain.AddSignaturesToLocalMail = true;
         _domain.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@test.com", "test");

         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("nonexistant@" + _domain.Name, account.Address, "SignatureTest", "SignaturerTestBody");

         string messageData = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(messageData.Contains(_domain.SignaturePlainText));
      }

      /// <summary>
      ///  Test a combination of external accounts and signature macros. If this test fails,
      ///  something is terrible broken with the signature functionality...
      /// </summary>
      [Test]
      public void TestSignatureMacroLocalDomainNonExistantAccount()
      {
         _domain.SignatureEnabled = true;
         _domain.SignaturePlainText = "MyDomainSignature %User.FirstName%";
         _domain.AddSignaturesToLocalMail = true;
         _domain.Save();

         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@test.com", "test");
         account.PersonFirstName = "Martin";

         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("nonexistant@" + _domain.Name, account.Address, "SignatureTest", "SignaturerTestBody");

         string messageData = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(messageData.Contains("%User.FirstName%"));
      }

      [Test]
      public void  TestMimeModificationEmailWithPlaintextAttachment()
      {
         var message = SendMessageWithSignature("PlainTextSignature", "HtmlSignature",
            TestResources.EmailWith_TextPlainBody_TextHtmlBody_TextPlainAttachment);

         var tempFile = Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString());
         message.Attachments[1].SaveAs(tempFile);

         Assert.IsTrue(message.Body.Contains("PlainTextBody\r\n\r\nPlainTextSignature"), message.Body);
         Assert.IsTrue(message.HTMLBody.Contains("<b>HtmlBody</b>\r\n<br/>\r\nHtmlSignature"), message.HTMLBody);

         try
         {
            var content = File.ReadAllText(tempFile);
            Assert.AreEqual("Test", content);
         }
         finally
         {
            File.Delete(tempFile);
         }
      }

      [Test]
      public void TestMimeModificationEmailWithHtmlAttachment()
      {
         var message = SendMessageWithSignature("PlainTextSignature", "HtmlSignature",
            TestResources.EmailWith_TextPlainBody_TextHtmlBody_TextHtmlAttachment);

         var tempFile = Path.Combine(Path.GetTempPath(), Guid.NewGuid().ToString());
         message.Attachments[1].SaveAs(tempFile);

         Assert.IsTrue(message.Body.Contains("PlainTextBody\r\n\r\nPlainTextSignature"), message.Body);
         Assert.IsTrue(message.HTMLBody.Contains("<b>HtmlBody</b>\r\n<br/>\r\nHtmlSignature"), message.HTMLBody);

         try
         {
            var content = File.ReadAllText(tempFile);
            Assert.AreEqual("HtmlAttachment", content);
         }
         finally
         {
            File.Delete(tempFile);
         }
      }

      [Test]
      public void TestMimeModificationEmailWithTextPlainMainPartNoContentType()
      {
         var message = SendMessageWithSignature("PlainTextSignature", "HtmlSignature",
            TestResources.EmailWith_TextPlainBody_NoContentType);

         Assert.IsTrue(message.Body.Contains("PlainTextBody\r\n\r\n\r\nPlainTextSignature"), message.Body);
         Assert.AreEqual("", message.HTMLBody);
      }

      [Test]
      public void TestMimeModificationEmailWithTextPlainMainPartAndTextPlainContentType()
      {
         var message = SendMessageWithSignature("PlainTextSignature", "HtmlSignature",
            TestResources.EmailWith_TextPlainBody_TextPlainContentType);

         Assert.IsTrue(message.Body.Contains("PlainTextBody\r\n\r\n\r\nPlainTextSignature"), message.Body);
         Assert.AreEqual("", message.HTMLBody);
      }

      [Test]
      public void TestMimeModificationEmailWithTextHtmlMainPartAndTextHtmlContentType()
      {
         var message = SendMessageWithSignature("PlainTextSignature", "HtmlSignature",
            TestResources.EmailWith_TextHtmlBody_TextHtmlContentType);

         Assert.AreEqual("", message.Body);
         Assert.IsTrue(message.HTMLBody.Contains("<b>HtmlBody</b>\r\n\r\n<br/>\r\nHtmlSignature"), message.HTMLBody);
         
      }

      [Test]
      public void TestMimeModificationEmailWithTextPlainPartAndTextHtmlPart()
      {
         var message = SendMessageWithSignature("PlainTextSignature", "HtmlSignature",
            TestResources.EmailWith_TextPlainBody_TextHtmlBody);

         Assert.IsTrue(message.Body.Contains("PlainTextBody\r\n\r\nPlainTextSignature"), message.Body);
         Assert.IsTrue(message.HTMLBody.Contains("<b>HtmlBody</b>\r\n<br/>\r\nHtmlSignature"), message.HTMLBody);

      }


      private Message SendMessageWithSignature(string plainTextSignature, string htmlSignature, string message)
      {
         _domain.SignatureEnabled = true;
         _domain.AddSignaturesToLocalMail = true;

         _account.SignatureEnabled = true;
         _account.SignaturePlainText = plainTextSignature;
         _account.SignatureHTML = htmlSignature;
         _account.Save();

         SmtpClientSimulator.StaticSendRaw(_account.Address, _account.Address, message);

         return CustomAsserts.AssertGetFirstMessage(_account, "Inbox");


      }
   }
}