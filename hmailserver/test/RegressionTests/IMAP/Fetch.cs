// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class Fetch : TestFixtureBase
   {
      [Test]
      [Description("Issue 218, IMAP: Problem with file name containing non-latin chars")]
      public void TestBodyStructureWithNonLatinCharacter()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var attachmentName = "本本本.zip";

         var filename = Path.Combine(Path.GetTempPath(), attachmentName);
         File.WriteAllText(filename, "tjena moss");

         var message = new Message();
         message.Charset = "utf-8";
         message.AddRecipient("test", account.Address);
         message.From = "Test";
         message.FromAddress = account.Address;
         message.Body = "hejsan";
         message.Attachments.Add(filename);
         message.Save();

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         simulator.SelectFolder("INBOX");
         var result = simulator.Fetch("1 BODYSTRUCTURE");
         simulator.Disconnect();

         // utf-8 representation of 本本本.zip:
         Assert.IsTrue(result.Contains("=?utf-8?B?5pys5pys5pys?=.zip"));
      }

      [Test]
      public void TestFetch()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody1");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody2");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 2);

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody3");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 3);


         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");
         var result = sim.Fetch("1 BODY[1]");
         Assert.IsTrue(result.Contains("SampleBody1"), result);
         result = sim.Fetch("2 BODY[1]");
         Assert.IsTrue(result.Contains("SampleBody2"), result);
         result = sim.Fetch("3 BODY[1]");
         Assert.IsTrue(result.Contains("SampleBody3"), result);
      }

      [Test]
      [Description("Issue 293, IMAP: bodystructure is sent instead of body")]
      public void TestFetchBody()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var attachmentName = "本本本.zip";

         var filename = Path.Combine(Path.GetTempPath(), attachmentName);
         File.WriteAllText(filename, "tjena moss");

         var message = new Message();
         message.Charset = "utf-8";
         message.AddRecipient("test", account.Address);
         message.From = "Test";
         message.FromAddress = account.Address;
         message.Body = "hejsan";
         message.Attachments.Add(filename);
         message.Save();

         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         var simulator = new ImapClientSimulator();
         simulator.ConnectAndLogon(account.Address, "test");
         simulator.SelectFolder("INBOX");
         var bodyStructureResponse = simulator.Fetch("1 BODYSTRUCTURE");
         var bodyResponse = simulator.Fetch("1 BODY");
         simulator.Disconnect();

         Assert.IsTrue(bodyStructureResponse.Contains("BOUNDARY"));
         Assert.IsFalse(bodyResponse.Contains("BOUNDARY"));
      }

      [Test]
      [Description("Issue 209, Date containing \" doesn't show up in OE")]
      public void TestFetchEnvelopeWithDateContainingQuote()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@example.test", "test");

         var message = "From: Someone <someone@example.com>" + Environment.NewLine +
                       "To: Someoen <someone@example.com>" + Environment.NewLine +
                       "Date: Wed, 22 Apr 2009 11:05:09 \"GMT\"" + Environment.NewLine +
                       "Subject: Something" + Environment.NewLine +
                       Environment.NewLine +
                       "Hello" + Environment.NewLine;

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.SendRaw(account.Address, account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var simulator = new ImapClientSimulator();
         var sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         simulator.SelectFolder("INBOX");
         var result = simulator.Fetch("1 ENVELOPE");
         simulator.Disconnect();

         Assert.IsTrue(result.Contains("Wed, 22 Apr 2009 11:05:09 GMT"));
      }

      [Test]
      public void IfInReplyToFieldContainsQuoteThenFetchHeadersShouldEncodeIt()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@example.test", "test");

         var message = "From: Someone <someone@example.com>" + Environment.NewLine +
                       "To: Someoen <someone@example.com>" + Environment.NewLine +
                       "In-Reply-To: ShouldBeEncodedDueToQuote\"" + Environment.NewLine +
                       "Subject: Something" + Environment.NewLine +
                       Environment.NewLine +
                       "Hello" + Environment.NewLine;

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.SendRaw(account.Address, account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var simulator = new ImapClientSimulator();
         var sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         simulator.SelectFolder("INBOX");
         var result = simulator.Fetch("1 ENVELOPE");
         simulator.Disconnect();

         Assert.IsFalse(result.Contains("ShouldBeEncodedDueToQuote"));
      }


      [Test]
      [Description("Issue 282, hMailServer not working with Symbian N60 ")]
      public void TestFetchHeaderFields()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@example.test", "test");

         var message = "From: Someone <someone@example.com>" + Environment.NewLine +
                       "To: Someoen <someone@example.com>" + Environment.NewLine +
                       "Date: Wed, 22 Apr 2009 11:05:09 \"GMT\"" + Environment.NewLine +
                       "Subject: Something" + Environment.NewLine +
                       Environment.NewLine +
                       "Hello" + Environment.NewLine;

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.SendRaw(account.Address, account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var simulator = new ImapClientSimulator();
         var sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         simulator.SelectFolder("INBOX");
         var result = simulator.Fetch("1 BODY.PEEK[HEADER.FIELDS (Subject From)]");
         simulator.Disconnect();


         Assert.IsTrue(result.Contains("Subject: Something"));
         Assert.IsTrue(result.Contains("From: Someone <someone@example.com>"));
         // The feedback should end with an empty header line.
         Assert.IsTrue(result.Contains("\r\n\r\n)"));
         Assert.IsFalse(result.Contains("Received:"));
      }

      [Test]
      public void RequestingSameHeaderFieldMultipleTimesShouldReturnItOnce()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@example.test", "test");

         var message = "From: Someone <someone@example.com>" + Environment.NewLine +
                       "To: Someoen <someone@example.com>" + Environment.NewLine +
                       "Date: Wed, 22 Apr 2009 11:05:09 \"GMT\"" + Environment.NewLine +
                       "Subject: SubjectText" + Environment.NewLine +
                       Environment.NewLine +
                       "Hello" + Environment.NewLine;

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.SendRaw(account.Address, account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var simulator = new ImapClientSimulator();
         var sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         simulator.SelectFolder("INBOX");
         var result = simulator.Fetch("1 BODY.PEEK[HEADER.FIELDS (Subject Subject)]");
         simulator.Disconnect();

         Assert.AreEqual(1, StringExtensions.Occurences(result, "SubjectText"));
      }


      [Test]
      [Description("Issue 282, hMailServer not working with Symbian N60 ")]
      public void TestFetchHeaderFieldsNot()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@example.test", "test");

         var message = "From: Someone <someone@example.com>" + Environment.NewLine +
                       "To: Someoen <someone@example.com>" + Environment.NewLine +
                       "Date: Wed, 22 Apr 2009 11:05:09 \"GMT\"" + Environment.NewLine +
                       "Subject: Something" + Environment.NewLine +
                       Environment.NewLine +
                       "Hello" + Environment.NewLine;

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.SendRaw(account.Address, account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var simulator = new ImapClientSimulator();
         var sWelcomeMessage = simulator.Connect();
         simulator.Logon(account.Address, "test");
         simulator.SelectFolder("INBOX");
         var result = simulator.Fetch("1 BODY.PEEK[HEADER.FIELDS.NOT (Subject From)]");
         simulator.Disconnect();


         Assert.IsTrue(result.Contains("Received:"), result);
         Assert.IsFalse(result.Contains("Subject:"), result);
         Assert.IsFalse(result.Contains("From:"), result);
         // The feedback should end with an empty header line.
         Assert.IsTrue(result.Contains("\r\n\r\n)"), result);
      }

      [Test]
      public void TestFetchInvalid()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody1");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody2");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody3");

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 3);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");
         var result = sim.Fetch("0 BODY[1]");
         Assert.IsTrue(result.StartsWith("A17 OK FETCH completed"));
         result = sim.Fetch("-1 BODY[1]");
         Assert.IsTrue(result.StartsWith("A17 BAD"));
         result = sim.Fetch("-100 BODY[1]");
         Assert.IsTrue(result.StartsWith("A17 BAD"));
      }

      [Test]
      [Description("RFC 3501: partial fetch where start offset is beyond end of content must return empty string")]
      public void PartialFetch_StartBeyondEndReturnsEmptyString()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Body");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         var result = sim.Fetch("1 BODY[HEADER]<99999.10>");
         Assert.IsTrue(result.Contains("BODY[HEADER]<99999>"), result);
         Assert.IsTrue(result.Contains("\"\""), result);

         sim.Disconnect();
      }

      [Test]
      [Description("Issue 524: BODY[TEXT] partial fetch where offset >= part size must return empty string, not crash")]
      public void PartialFetch_BodyText_StartBeyondEndReturnsEmptyString()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Body");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         var result = sim.Fetch("1 BODY.PEEK[TEXT]<393216.393216>");
         Assert.IsTrue(result.Contains("BODY[TEXT]<393216>"), result);
         Assert.IsTrue(result.Contains("\"\""), result);

         sim.Disconnect();
      }

      [Test]
      [Description("Issue 524: BODY[1] partial fetch where offset >= part size must return empty string, not crash (Thunderbird chunked fetch scenario)")]
      public void PartialFetch_BodyPart_StartBeyondEndReturnsEmptyString()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Body");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         // Reproduces the exact Thunderbird chunked-fetch scenario from issue #524.
         var result = sim.Fetch("1 BODY.PEEK[1]<393216.393216>");
         Assert.IsTrue(result.Contains("BODY[1]<393216>"), result);
         Assert.IsTrue(result.Contains("\"\""), result);

         sim.Disconnect();
      }

      [Test]
      [Description("RFC 3501: partial fetch where requested count exceeds remaining bytes must truncate")]
      public void PartialFetch_RequestedSizeExceedingRemainderTruncates()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Body");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         var fullResult = sim.Fetch("1 BODY[HEADER]");
         var lStart = fullResult.IndexOf('{');
         var lEnd = fullResult.IndexOf('}', lStart);
         var fullSize = int.Parse(fullResult.Substring(lStart + 1, lEnd - lStart - 1));

         var partial = sim.Fetch("1 BODY[HEADER]<5.99999>");
         Assert.IsTrue(partial.Contains("BODY[HEADER]<5>"), partial);
         Assert.IsTrue(partial.Contains("{" + (fullSize - 5) + "}"), partial);

         sim.Disconnect();
      }

      [Test]
      [Description("BODY.PEEK must not set the \\Seen flag")]
      public void BodyPeekDoesNotSetSeenFlag()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Body");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         sim.Fetch("1 BODY.PEEK[TEXT]");
         var flags = sim.GetFlags(1);
         Assert.IsFalse(flags.Contains("\\Seen"), flags);

         sim.Disconnect();
      }

      [Test]
      [Description("BODY (without PEEK) must set the \\Seen flag")]
      public void BodyFetchSetsSeenFlag()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Body");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         sim.Fetch("1 BODY[TEXT]");
         var flags = sim.GetFlags(1);
         Assert.IsTrue(flags.Contains("\\Seen"), flags);

         sim.Disconnect();
      }

      [Test]
      [Description("Partial fetch of BODY[TEXT] must return correct byte slice")]
      public void PartialFetch_BodyText()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBodyContent");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         var fullResult = sim.Fetch("1 BODY.PEEK[TEXT]");
         var lStart = fullResult.IndexOf('{');
         var lEnd = fullResult.IndexOf('}', lStart);
         var fullSize = int.Parse(fullResult.Substring(lStart + 1, lEnd - lStart - 1));
         var contentStart = fullResult.IndexOf("\r\n", lEnd) + 2;
         var fullText = fullResult.Substring(contentStart, fullSize);

         Assert.IsTrue(fullSize > 5, $"Body too short ({fullSize}) for a meaningful partial test");

         var partial = sim.Fetch("1 BODY.PEEK[TEXT]<0.5>");
         Assert.IsTrue(partial.Contains("BODY[TEXT]<0>"), partial);
         Assert.IsTrue(partial.Contains("{5}"), partial);
         var partialStart = partial.IndexOf("{5}") + 5;
         Assert.AreEqual(fullText.Substring(0, 5), partial.Substring(partialStart, 5));

         sim.Disconnect();
      }

      [Test]
      [Description("Partial fetch of BODY[HEADER.FIELDS] must return correct byte slice")]
      public void PartialFetch_HeaderFields()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@example.test", "test");
         var message = "From: Someone <someone@example.com>" + Environment.NewLine +
                       "Subject: TestSubject" + Environment.NewLine +
                       Environment.NewLine +
                       "Hello" + Environment.NewLine;
         new SmtpClientSimulator().SendRaw(account.Address, account.Address, message);
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         var fullResult = sim.Fetch("1 BODY.PEEK[HEADER.FIELDS (Subject)]");
         var lStart = fullResult.IndexOf('{');
         var lEnd = fullResult.IndexOf('}', lStart);
         var fullSize = int.Parse(fullResult.Substring(lStart + 1, lEnd - lStart - 1));
         var contentStart = fullResult.IndexOf("\r\n", lEnd) + 2;
         var fullContent = fullResult.Substring(contentStart, fullSize);

         Assert.IsTrue(fullSize > 5, $"HEADER.FIELDS response too short ({fullSize}) for a meaningful partial test");

         var partial = sim.Fetch("1 BODY.PEEK[HEADER.FIELDS (Subject)]<0.5>");
         Assert.IsTrue(partial.Contains("BODY[HEADER.FIELDS (SUBJECT)]<0>") || partial.Contains("BODY[HEADER.FIELDS (Subject)]<0>"), partial);
         Assert.IsTrue(partial.Contains("{5}"), partial);
         var partialStart = partial.IndexOf("{5}") + 5;
         Assert.AreEqual(fullContent.Substring(0, 5), partial.Substring(partialStart, 5));

         sim.Disconnect();
      }

      [Test]
      [Description("Partial fetch of BODY[HEADER.FIELDS.NOT] must return correct byte slice")]
      public void PartialFetch_HeaderFieldsNot()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@example.test", "test");
         var message = "From: Someone <someone@example.com>" + Environment.NewLine +
                       "Subject: TestSubject" + Environment.NewLine +
                       Environment.NewLine +
                       "Hello" + Environment.NewLine;
         new SmtpClientSimulator().SendRaw(account.Address, account.Address, message);
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         var fullResult = sim.Fetch("1 BODY.PEEK[HEADER.FIELDS.NOT (Subject)]");
         var lStart = fullResult.IndexOf('{');
         var lEnd = fullResult.IndexOf('}', lStart);
         var fullSize = int.Parse(fullResult.Substring(lStart + 1, lEnd - lStart - 1));
         var contentStart = fullResult.IndexOf("\r\n", lEnd) + 2;
         var fullContent = fullResult.Substring(contentStart, fullSize);

         Assert.IsTrue(fullSize > 5, $"HEADER.FIELDS.NOT response too short ({fullSize}) for a meaningful partial test");

         var partial = sim.Fetch("1 BODY.PEEK[HEADER.FIELDS.NOT (Subject)]<0.5>");
         Assert.IsTrue(partial.Contains("BODY[HEADER.FIELDS.NOT (SUBJECT)]<0>") || partial.Contains("BODY[HEADER.FIELDS.NOT (Subject)]<0>"), partial);
         Assert.IsTrue(partial.Contains("{5}"), partial);
         var partialStart = partial.IndexOf("{5}") + 5;
         Assert.AreEqual(fullContent.Substring(0, 5), partial.Substring(partialStart, 5));

         sim.Disconnect();
      }

      [Test]
      [Description("RFC822.SIZE must return a positive integer reflecting message size")]
      public void FetchRfc822SizeReturnsPositiveInteger()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Body");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         var result = sim.Fetch("1 RFC822.SIZE");
         Assert.IsTrue(result.Contains("RFC822.SIZE"), result);
         var sizeStart = result.IndexOf("RFC822.SIZE ") + "RFC822.SIZE ".Length;
         var sizeEnd = result.IndexOfAny(new[] { ' ', ')' }, sizeStart);
         var size = int.Parse(result.Substring(sizeStart, sizeEnd - sizeStart));
         Assert.IsTrue(size > 0, $"RFC822.SIZE should be positive, got {size}");

         sim.Disconnect();
      }

      [Test]
      [Description("INTERNALDATE must return a non-empty quoted date string")]
      public void FetchInternaldateReturnsNonEmptyQuotedString()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Body");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         var result = sim.Fetch("1 INTERNALDATE");
         Assert.IsTrue(result.Contains("INTERNALDATE \""), result);

         sim.Disconnect();
      }

      [Test]
      [Description("UID fetch must return a positive integer")]
      public void FetchUidReturnsPositiveInteger()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Body");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         var result = sim.Fetch("1 UID");
         Assert.IsTrue(result.Contains("UID "), result);
         var uidStart = result.IndexOf("UID ") + 4;
         var uidEnd = result.IndexOfAny(new[] { ' ', ')' }, uidStart);
         var uid = int.Parse(result.Substring(uidStart, uidEnd - uidStart));
         Assert.IsTrue(uid > 0, $"UID should be positive, got {uid}");

         sim.Disconnect();
      }

      [Test]
      [Description("Issue 334, IMAP FETCH does not properly honour the <start.size> partial body clause")]
      public void TestPartialFetch_HeaderOctetRange()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test subject", "Body text");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         // Fetch full header to establish baseline size and content.
         var fullHeaderResult = sim.Fetch("1 BODY[HEADER]");
         var literalStart = fullHeaderResult.IndexOf('{');
         var literalEnd = fullHeaderResult.IndexOf('}', literalStart);
         var fullSize = int.Parse(fullHeaderResult.Substring(literalStart + 1, literalEnd - literalStart - 1));
         Assert.IsTrue(fullSize > 15, $"Header too short ({fullSize} bytes) for a meaningful partial test");

         var contentStart = fullHeaderResult.IndexOf("\r\n", literalEnd) + 2;
         var fullHeader = fullHeaderResult.Substring(contentStart, fullSize);

         // <0.10>: 10 bytes starting at offset 0.
         var result0 = sim.Fetch("1 BODY[HEADER]<0.10>");
         Assert.IsTrue(result0.Contains("BODY[HEADER]<0>"), result0);
         Assert.IsTrue(result0.Contains("{10}"), result0);
         var content0Start = result0.IndexOf("{10}") + 6;
         Assert.AreEqual(fullHeader.Substring(0, 10), result0.Substring(content0Start, 10));

         // <5.10>: 10 bytes starting at offset 5.
         var result5 = sim.Fetch("1 BODY[HEADER]<5.10>");
         Assert.IsTrue(result5.Contains("BODY[HEADER]<5>"), result5);
         Assert.IsTrue(result5.Contains("{10}"), result5);
         var content5Start = result5.IndexOf("{10}") + 6;
         Assert.AreEqual(fullHeader.Substring(5, 10), result5.Substring(content5Start, 10));

         sim.Disconnect();
      }

      [Test]
      [Description("Valid partial fetch of a numbered body part (BODY[1]) must return the correct byte slice")]
      public void PartialFetch_BodyPart_ValidRangeReturnsCorrectBytes()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test subject", "SampleBodyContent");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         // Fetch full body part to establish baseline.
         var fullResult = sim.Fetch("1 BODY.PEEK[1]");
         var literalStart = fullResult.IndexOf('{');
         var literalEnd = fullResult.IndexOf('}', literalStart);
         var fullSize = int.Parse(fullResult.Substring(literalStart + 1, literalEnd - literalStart - 1));
         Assert.IsTrue(fullSize > 5, $"Body part too short ({fullSize} bytes) for a meaningful partial test");
         var contentStart = fullResult.IndexOf("\r\n", literalEnd) + 2;
         var fullContent = fullResult.Substring(contentStart, fullSize);

         // <0.5>: 5 bytes from the start.
         var result0 = sim.Fetch("1 BODY.PEEK[1]<0.5>");
         Assert.IsTrue(result0.Contains("BODY[1]<0>"), result0);
         Assert.IsTrue(result0.Contains("{5}"), result0);
         var partial0Start = result0.IndexOf("{5}") + 5;
         Assert.AreEqual(fullContent.Substring(0, 5), result0.Substring(partial0Start, 5));

         // <5.5>: 5 bytes starting at offset 5.
         var result5 = sim.Fetch("1 BODY.PEEK[1]<5.5>");
         Assert.IsTrue(result5.Contains("BODY[1]<5>"), result5);
         Assert.IsTrue(result5.Contains("{5}"), result5);
         var partial5Start = result5.IndexOf("{5}") + 5;
         Assert.AreEqual(fullContent.Substring(5, 5), result5.Substring(partial5Start, 5));

         sim.Disconnect();
      }
   }
}