// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.API
{
   [TestFixture]
   public class Unicode : TestFixtureBase
   {
      [Test]
      [Description("Issue 233. Test non-latin characters in list name." +
                   "The problem was that spaces weren't encoded. This had the effect that" +
                   "they were removed in the recipients email client.")]
      public void NonLatinCharactersInRecipientNameResultsInBoxesInThunderbird()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode3@example.test", "test");

         var message = new hMailServer.Message();
         //message.AddRecipient("Тестов Тест ТестостероновичТестостероновичТестостероновичТестостероновичТестостероновичТестостероновичТестостероновичТестостероновичТестостероновичТестостероновичТестостероновичТестостеронович", account.Address);
         message.AddRecipient(
            "ТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероноТестовТестТестостероно",
            account.Address);
         message.Subject = "Test";
         message.Charset = "utf-8";
         message.Body = "Test body.";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         // Important:
         //   RFC 2047: http://www.faqs.org/rfcs/rfc2047.html
         //   The notation of RFC 822 is used, with the exception that white space
         //   characters MUST NOT appear between components of an 'encoded-word'.
         //
         //   Also, there should be a space separating the encoded word with the following
         //   non-encoded word.


         var match =
            "To: \"=?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\r\n" +
            " =?utf-8?B?0KLQtdGB0YLQvtCy0KLQtdGB0YLQotC10YHRgtC+0YHRgtC10YDQvtC90L4=?=\" <encode3@example.test>";


         Assert.IsTrue(messageText.Contains(match));
      }

      [Test]
      [Description("Issue 233. Test non-latin characters in list name." +
                   "The problem was that spaces weren't encoded. This had the effect that" +
                   "they were removed in the recipients email client.")]
      public void NonLatinCharactersInRecipientNameResultsInNoSpaces()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("Tестов Тест Тестостеронович", account.Address);
         message.Subject = "Test";
         message.Charset = "utf-8";
         message.Body = "Test body.";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         // Important:
         //   RFC 2047: http://www.faqs.org/rfcs/rfc2047.html
         //   The notation of RFC 822 is used, with the exception that white space
         //   characters MUST NOT appear between components of an 'encoded-word'.
         // 
         //   Also, there should be a space separating the encoded word with the following
         //   non-encoded word.
         Assert.IsTrue(
            messageText.Contains(
               "To: \"=?utf-8?B?VNC10YHRgtC+0LIg0KLQtdGB0YIg0KLQtdGB0YLQvtGB0YLQtdGA0L7QvdC+?=\r\n =?utf-8?B?0LLQuNGH?=\" <encode@example.test>"));
      }

      [Test]
      public void SingleLatinCharacterRecipient()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode3@example.test", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("Ö", account.Address);
         message.Charset = "utf-8";
         message.Body = "Test body.";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(messageText.Contains("To: \"=?utf-8?B?w5Y=?=\" <encode3@example.test>"));
      }

      [Test]
      public void SwedishCharactersInBody()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode3@example.test", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("Ö", account.Address);
         message.Charset = "utf-8";
         message.Body = "Test ÅÄÖ Test.";

         var body = message.Body;
         Assert.IsTrue(body.Contains("Test ÅÄÖ Test."));
      }

      [Test]
      public void TestDanishSubject()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var croatianAndDanish = "ʒʥʨPiskefløde";

         var message = new hMailServer.Message();
         message.AddRecipient("test", account.Address);
         message.From = "Test";
         message.FromAddress = account.Address;
         message.Subject = croatianAndDanish;
         message.Body = "hej";
         message.Save();

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         var downloadedMessage = account.IMAPFolders[0].Messages[0];
         Assert.AreEqual(croatianAndDanish, downloadedMessage.Subject);
      }

      [Test]
      [Description("Issue 169 - Unicode subject not decoded properly.")]
      public void TestDecodeSpecificMessage()
      {
         var message = "Return-Path:" + Environment.NewLine +
                       "Received: from host ([1.2.3.4])" + Environment.NewLine +
                       "X-Facebook: from zuckmail" + Environment.NewLine +
                       "by localhost.localdomain with local (ZuckMail);" + Environment.NewLine +
                       "Date: Wed, 3 Dec 2008 06:14:37 -0800" + Environment.NewLine +
                       "To: someone@example.com" + Environment.NewLine +
                       "From: Facebook" + Environment.NewLine +
                       "Reply-to: Facebook" + Environment.NewLine +
                       "Subject: =?UTF-8?Q?V=C3=A4nligen_=C3=A5terst=C3=A4ll_dina_inst=C3=A4llningar_f=C3?=" +
                       Environment.NewLine +
                       "    =?UTF-8?Q?=B6r_meddelanden_via_e-post.?=" + Environment.NewLine +
                       "Message-ID:" + Environment.NewLine +
                       "X-Priority: 3" + Environment.NewLine +
                       "X-Mailer: ZuckMail [version 1.00]" + Environment.NewLine +
                       "Errors-To: root+monkeymonkey@facebookmail.com" + Environment.NewLine +
                       "MIME-Version: 1.0" + Environment.NewLine +
                       "Content-Type: text/plain; charset=\"UTF-8\"" + Environment.NewLine +
                       "Content-Transfer-Encoding: quoted-printable" + Environment.NewLine +
                       "" + Environment.NewLine +
                       "Dina inst=C3=A4llningar f=C3=B6r meddelanden via e-post har tyv=C3=A4rr g=" +
                       Environment.NewLine +
                       "=C3=A5tt f=C3=B6rlorade. Vi beklagar det intr=C3=A4ffade.G=C3=" + Environment.NewLine +
                       "=A5 till http://www.facebook.com/editaccount.php?notifications f=C3=B6r a=" +
                       Environment.NewLine +
                       "tt =C3=A5terst=C3=A4lla dina inst=C3=A4llningar.Tack!Facebook-grupp=" +
                       Environment.NewLine +
                       "en." + Environment.NewLine;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "decode@example.test", "test");

         SmtpClientSimulator.StaticSendRaw("test@example.test", account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var apiMessage = account.Messages[0];

         Assert.AreEqual("Vänligen återställ dina inställningar för meddelanden via e-post.", apiMessage.Subject);
         Assert.IsTrue(
            apiMessage.Body.StartsWith("Dina inställningar för meddelanden via e-post har tyvärr gått förlorade."));
      }

      [Test]
      [Description(
         "Test encode word. Test that the word is actually encoded and not just stored as unicode in the file.")]
      public void TestEncodeWord()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var croatianAndDanish = "ʒʥʨPiskefløde";
         var message = new hMailServer.Message();
         message.Subject = croatianAndDanish;
         message.AddRecipient("", account.Address);
         message.Save();

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var apiMessage = account.Messages[0];

         Assert.AreEqual(apiMessage.Subject, croatianAndDanish);
         var fileContents = File.ReadAllText(apiMessage.Filename);
         Assert.IsFalse(fileContents.Contains(croatianAndDanish));
      }

      [Test]
      public void TestEncodingOfStringIncludingSpace()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("test", account.Address);
         message.From = "Test";
         message.FromAddress = account.Address;
         message.Subject = "test mit encoding und deutc ü...";
         message.Body = "Test";
         message.Save();

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var lines = messageText.Split(Environment.NewLine.ToCharArray());

         foreach (var line in lines)
            if (line.ToLower().StartsWith("subject: "))
            {
               var subject = line.Substring("subject: ".Length + 1);

               // encoded part should not contain space.
               Assert.IsFalse(subject.Contains(" "));

               break;
            }
      }

      [Test]
      [Description("Test case for issue 164")]
      public void TestFolderAdd()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var folderName = "åäö";
         var folder = account.IMAPFolders.Add(folderName);
         var folder2 = account.IMAPFolders.get_ItemByName(folderName);
         Assert.AreEqual(folderName, folder2.Name);
      }

      [Test]
      public void TestJapaneseAttachments()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var attachmentName = "本本本.zip";

         var filename = Path.Combine(TestSetup.GetSharedTempDirectory(), attachmentName);
         File.WriteAllText(filename, "tjena moss");

         var message = new hMailServer.Message();
         message.AddRecipient("test", account.Address);
         message.From = "Test";
         message.FromAddress = account.Address;
         message.Body = "hejsan";
         message.Attachments.Add(filename);
         message.Save();

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         var downloadedMessage = account.IMAPFolders[0].Messages[0];
         Assert.AreNotEqual(0, downloadedMessage.Attachments.Count);
         var attachment = downloadedMessage.Attachments[0];
         Assert.AreEqual(attachmentName, attachment.Filename);
      }

      [Test]
      public void TestModifyBodyWithExistingAttachments()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var swedish = "abc.zip";
         var attachmentName = swedish + ".zip";
         var filename = Path.Combine(TestSetup.GetSharedTempDirectory(), attachmentName);
         File.WriteAllText(filename, swedish);


         var message = new hMailServer.Message();
         message.AddRecipient("test", account.Address);
         message.Attachments.Add(filename);
         message.From = swedish;
         message.FromAddress = account.Address;
         message.Subject = swedish;
         message.Body = "";
         message.Save();

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         var downloadedMessage = account.IMAPFolders[0].Messages[0];

         downloadedMessage.Body = "Test";
         downloadedMessage.Save();
      }

      [Test]
      [Description("Issue 233. Test non-latin characters in list name." +
                   "The problem was that spaces weren't encoded. This had the effect that" +
                   "they were removed in the recipients email client.")]
      public void TestMostlyLatinCharacters()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "encode@example.test", "test");

         var message = new hMailServer.Message();
         message.AddRecipient("Test Recipient: Тест", account.Address);
         message.Subject = "Test Subject: Тест";
         message.Charset = "utf-8";
         message.Body = "Test body.";
         message.Save();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         // Important:
         //   RFC 2047: http://www.faqs.org/rfcs/rfc2047.html
         //   The notation of RFC 822 is used, with the exception that white space
         //   characters MUST NOT appear between components of an 'encoded-word'.
         // 
         //   Also, there should be a space separating the encoded word with the following
         //   non-encoded word.
         Assert.IsTrue(
            messageText.Contains("To: \"=?utf-8?B?VGVzdCBSZWNpcGllbnQ6INCi0LXRgdGC?=\" <encode@example.test>"));
      }

      [Test]
      public void TestMultipleLanguages()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var manyLanguages = "中文åäöん文t͡ɬɪŋ";

         var message = new hMailServer.Message();
         message.AddRecipient("test", account.Address);
         message.From = manyLanguages;
         message.FromAddress = account.Address;
         message.Subject = manyLanguages;
         message.Body = manyLanguages;
         message.Save();

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         var downloadedMessage = account.IMAPFolders[0].Messages[0];
         Assert.AreEqual(manyLanguages, downloadedMessage.Subject);
         Assert.AreEqual(manyLanguages, downloadedMessage.From);
         Assert.AreEqual(manyLanguages + Environment.NewLine, downloadedMessage.Body);
      }

      [Test]
      public void TestSwedishAndChineseCombination()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var swedishAndChinese = "ÅÄÖ汉语";
         var attachmentName = swedishAndChinese + ".zip";

         var filename = Path.Combine(TestSetup.GetSharedTempDirectory(), attachmentName);
         File.WriteAllText(filename, swedishAndChinese);

         var message = new hMailServer.Message();
         message.AddRecipient("test", account.Address);
         message.From = "Test";
         message.FromAddress = account.Address;
         message.Body = swedishAndChinese;
         message.Attachments.Add(filename);
         message.Save();

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         var downloadedMessage = account.IMAPFolders[0].Messages[0];
         Assert.AreNotEqual(0, downloadedMessage.Attachments.Count);
         var attachment = downloadedMessage.Attachments[0];
         Assert.AreEqual(attachmentName, attachment.Filename);

         Assert.AreEqual(swedishAndChinese + Environment.NewLine, downloadedMessage.Body);
      }

      [Test]
      [Description("Test of issue 176. Underscore in body disappears")]
      public void TestUnderscoreInBody()
      {
         var body = "underscore_in_body\r\n";

         var message = new hMailServer.Message();
         message.Body = body;
         Assert.AreEqual(body, message.Body);
      }
   }
}