// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.IO;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class Fetch : TestFixtureBase
   {
      [Test]
      [Description("Issue 218, IMAP: Problem with file name containing non-latin chars")]
      public void TestBodyStructureWithNonLatinCharacter()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         string attachmentName = "本本本.zip";

         string filename = Path.Combine(Path.GetTempPath(), attachmentName);
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

         var oSimulator = new ImapClientSimulator();
         oSimulator.ConnectAndLogon(account.Address, "test");
         oSimulator.SelectFolder("INBOX");
         string result = oSimulator.Fetch("1 BODYSTRUCTURE");
         oSimulator.Disconnect();

         // utf-8 representation of 本本本.zip:
         Assert.IsTrue(result.Contains("=?utf-8?B?5pys5pys5pys?=.zip"));
      }

      [Test]
      public void TestFetch()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody1");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody2");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 2);

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody3");
         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 3);


         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");
         string result = sim.Fetch("1 BODY[1]");
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
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");

         string attachmentName = "本本本.zip";

         string filename = Path.Combine(Path.GetTempPath(), attachmentName);
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

         var oSimulator = new ImapClientSimulator();
         oSimulator.ConnectAndLogon(account.Address, "test");
         oSimulator.SelectFolder("INBOX");
         string bodyStructureResponse = oSimulator.Fetch("1 BODYSTRUCTURE");
         string bodyResponse = oSimulator.Fetch("1 BODY");
         oSimulator.Disconnect();

         Assert.IsTrue(bodyStructureResponse.Contains("BOUNDARY"));
         Assert.IsFalse(bodyResponse.Contains("BOUNDARY"));
      }

      [Test]
      [Description("Issue 209, Date containing \" doesn't show up in OE")]
      public void TestFetchEnvelopeWithDateContainingQuote()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@test.com", "test");

         string message = "From: Someone <someone@example.com>" + Environment.NewLine +
                          "To: Someoen <someone@example.com>" + Environment.NewLine +
                          "Date: Wed, 22 Apr 2009 11:05:09 \"GMT\"" + Environment.NewLine +
                          "Subject: Something" + Environment.NewLine +
                          Environment.NewLine +
                          "Hello" + Environment.NewLine;

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.SendRaw(account.Address, account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var oSimulator = new ImapClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         oSimulator.SelectFolder("INBOX");
         string result = oSimulator.Fetch("1 ENVELOPE");
         oSimulator.Disconnect();

         Assert.IsTrue(result.Contains("Wed, 22 Apr 2009 11:05:09 GMT"));
      }

      [Test]
      public void IfInReplyToFieldContainsQuoteThenFetchHeadersShouldEncodeIt()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@test.com", "test");

         string message = "From: Someone <someone@example.com>" + Environment.NewLine +
                          "To: Someoen <someone@example.com>" + Environment.NewLine +
                          "In-Reply-To: ShouldBeEncodedDueToQuote\"" + Environment.NewLine +
                          "Subject: Something" + Environment.NewLine +
                          Environment.NewLine +
                          "Hello" + Environment.NewLine;

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.SendRaw(account.Address, account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var oSimulator = new ImapClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         oSimulator.SelectFolder("INBOX");
         string result = oSimulator.Fetch("1 ENVELOPE");
         oSimulator.Disconnect();

         Assert.IsFalse(result.Contains("ShouldBeEncodedDueToQuote"));
      }


      [Test]
      [Description("Issue 282, hMailServer not working with Symbian N60 ")]
      public void TestFetchHeaderFields()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@test.com", "test");

         string message = "From: Someone <someone@example.com>" + Environment.NewLine +
                          "To: Someoen <someone@example.com>" + Environment.NewLine +
                          "Date: Wed, 22 Apr 2009 11:05:09 \"GMT\"" + Environment.NewLine +
                          "Subject: Something" + Environment.NewLine +
                          Environment.NewLine +
                          "Hello" + Environment.NewLine;

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.SendRaw(account.Address, account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var oSimulator = new ImapClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         oSimulator.SelectFolder("INBOX");
         string result = oSimulator.Fetch("1 BODY.PEEK[HEADER.FIELDS (Subject From)]");
         oSimulator.Disconnect();


         Assert.IsTrue(result.Contains("Subject: Something"));
         Assert.IsTrue(result.Contains("From: Someone <someone@example.com>"));
         // The feedback should end with an empty header line.
         Assert.IsTrue(result.Contains("\r\n\r\n)"));
         Assert.IsFalse(result.Contains("Received:"));
      }

      [Test]
      public void RequestingSameHeaderFieldMultipleTimesShouldReturnItOnce()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@test.com", "test");

         string message = "From: Someone <someone@example.com>" + Environment.NewLine +
                          "To: Someoen <someone@example.com>" + Environment.NewLine +
                          "Date: Wed, 22 Apr 2009 11:05:09 \"GMT\"" + Environment.NewLine +
                          "Subject: SubjectText" + Environment.NewLine +
                          Environment.NewLine +
                          "Hello" + Environment.NewLine;

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.SendRaw(account.Address, account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var oSimulator = new ImapClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         oSimulator.SelectFolder("INBOX");
         string result = oSimulator.Fetch("1 BODY.PEEK[HEADER.FIELDS (Subject Subject)]");
         oSimulator.Disconnect();

         Assert.AreEqual(1, StringExtensions.Occurences(result, "SubjectText"));
      }


      [Test]
      [Description("Issue 282, hMailServer not working with Symbian N60 ")]
      public void TestFetchHeaderFieldsNot()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mimetest@test.com", "test");

         string message = "From: Someone <someone@example.com>" + Environment.NewLine +
                          "To: Someoen <someone@example.com>" + Environment.NewLine +
                          "Date: Wed, 22 Apr 2009 11:05:09 \"GMT\"" + Environment.NewLine +
                          "Subject: Something" + Environment.NewLine +
                          Environment.NewLine +
                          "Hello" + Environment.NewLine;

         var smtpSimulator = new SmtpClientSimulator();
         smtpSimulator.SendRaw(account.Address, account.Address, message);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var oSimulator = new ImapClientSimulator();
         string sWelcomeMessage = oSimulator.Connect();
         oSimulator.Logon(account.Address, "test");
         oSimulator.SelectFolder("INBOX");
         string result = oSimulator.Fetch("1 BODY.PEEK[HEADER.FIELDS.NOT (Subject From)]");
         oSimulator.Disconnect();


         Assert.IsTrue(result.Contains("Received:"), result);
         Assert.IsFalse(result.Contains("Subject:"), result);
         Assert.IsFalse(result.Contains("From:"), result);
         // The feedback should end with an empty header line.
         Assert.IsTrue(result.Contains("\r\n\r\n)"), result);
      }

      [Test]
      public void TestFetchInvalid()
      {
         Account account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody1");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody2");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "SampleBody3");

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 3);

         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");
         string result = sim.Fetch("0 BODY[1]");
         Assert.IsTrue(result.StartsWith("A17 OK FETCH completed"));
         result = sim.Fetch("-1 BODY[1]");
         Assert.IsTrue(result.StartsWith("A17 BAD"));
         result = sim.Fetch("-100 BODY[1]");
         Assert.IsTrue(result.StartsWith("A17 BAD"));
      }
   }
}