// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.ServiceProcess;
using System.Text;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.AntiSpam
{
   [TestFixture]
   public class SpamAssassin : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         CustomAsserts.AssertSpamAssassinIsRunning();

         // Enable spam assassin
         var antiSpam = _settings.AntiSpam;

         account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sa@example.test", "test");

         // Disallow incorrect line endings.
         antiSpam.SpamMarkThreshold = 1;
         antiSpam.SpamDeleteThreshold = 10000;
         antiSpam.AddHeaderReason = true;
         antiSpam.AddHeaderSpam = true;
         antiSpam.PrependSubject = true;
         antiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SpamAssassin
         antiSpam.SpamAssassinEnabled = true;
         antiSpam.SpamAssassinHost = "localhost";
         antiSpam.SpamAssassinPort = 783;
         antiSpam.SpamAssassinMergeScore = false;
         antiSpam.SpamAssassinScore = 5;
      }

      private Account account;


      [Test]
      [Description("Issue 533, message file kept open when the end-of-data sequence arrives in a packet of its own")]
      public void TerminatingDotInSeparatePacketShouldNotLeaveMessageFileOpen()
      {
         // The transmission buffer writes its content to the message file when it holds more
         // than 40000 bytes. The message below is sent in two chunks, where the second one
         // pushes the buffer past that limit and ends with a line break. Everything received
         // has then been written to the message file, and the buffer is empty when the
         // terminating <CRLF>.<CRLF> is received in a packet of its own.
         //
         // hMailServer used to keep the message file open for writing in that situation, which
         // made the spam tests below unable to add the Return-Path header to the message and
         // unable to replace the message file with the SpamAssassin result.
         var connection = new TcpConnection();
         Assert.IsTrue(connection.Connect(25));
         Assert.IsTrue(connection.Receive().StartsWith("220"));
         Assert.IsTrue(connection.SendAndReceive("HELO example.com\r\n").StartsWith("250"));
         Assert.IsTrue(connection.SendAndReceive("MAIL FROM:<" + account.Address + ">\r\n").StartsWith("250"));
         Assert.IsTrue(connection.SendAndReceive("RCPT TO:<" + account.Address + ">\r\n").StartsWith("250"));
         Assert.IsTrue(connection.SendAndReceive("DATA\r\n").StartsWith("354"));

         var firstChunk = new StringBuilder();
         firstChunk.Append("From: " + account.Address + "\r\n");
         firstChunk.Append("To: " + account.Address + "\r\n");
         firstChunk.Append("Subject: SA test\r\n");
         firstChunk.Append("\r\n");

         // Stay below the 40000 byte limit, so that nothing has been written to the message
         // file when the second chunk is sent.
         while (firstChunk.Length < 39800)
            firstChunk.Append("This is a test message which is sent in several packets.\r\n");

         connection.Send(firstChunk.ToString());
         Thread.Sleep(1000);

         var secondChunk = new StringBuilder();
         while (secondChunk.Length < 300)
            secondChunk.Append("Second chunk of the test message.\r\n");

         connection.Send(secondChunk.ToString());
         Thread.Sleep(1000);

         connection.Send(".\r\n");
         Assert.IsTrue(connection.Receive().StartsWith("250"));

         connection.SendAndReceive("QUIT\r\n");
         connection.Disconnect();

         var messageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(messageContents.Contains("Second chunk of the test message."), messageContents);
         Assert.IsTrue(messageContents.Contains("X-Spam-Status"), "SpamAssassin did not run");

         CustomAsserts.AssertNoReportedError();
      }

      [Test]
      public void ItShouldBePossibleToTestSAConnectionUsingAPISuccess()
      {
         var antiSpam = _settings.AntiSpam;

         string resultText;
         Assert.IsTrue(antiSpam.TestSpamAssassinConnection("localhost", 783, out resultText));
         Assert.IsTrue(resultText.Contains("Content analysis details:"));
      }

      [Test]
      public void ItShouldBePossibleToTestSAConnectionUsingAPIFailure()
      {
         var antiSpam = _settings.AntiSpam;

         string resultText;

         Assert.IsFalse(antiSpam.TestSpamAssassinConnection("localhost", 0, out resultText));
      }


      [Test]
      public void TestBasic()
      {
         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         var sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (!sMessageContents.Contains("X-Spam-Status")) Assert.Fail("SpamAssassin did not run");
      }

      [Test]
      public void TestDisabled()
      {
         var smtpClientSimulator = new SmtpClientSimulator();

         _settings.AntiSpam.SpamAssassinEnabled = false;
         _settings.AntiSpam.SpamAssassinHost = "localhost";
         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");

         var sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (sMessageContents.Contains("X-Spam-Status"))
         {
            _settings.AntiSpam.SpamAssassinEnabled = false;
            throw new Exception("Spam assassin not run");
         }
      }

      [Test]
      public void TestIncorrectHost()
      {
         var smtpClientSimulator = new SmtpClientSimulator();

         _settings.AntiSpam.SpamAssassinEnabled = true;
         _settings.AntiSpam.SpamAssassinHost = "localholst"; // <- misspelled
         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         var sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (sMessageContents.Contains("X-Spam-Status"))
         {
            _settings.AntiSpam.SpamAssassinEnabled = false;
            throw new Exception("Spam assassin not run");
         }

         CustomAsserts.AssertReportedError("The IP address for SpamAssassin could not be resolved.");
      }

      [Test]
      public void TestIncorrectPort()
      {
         var smtpClientSimulator = new SmtpClientSimulator();

         _settings.AntiSpam.SpamAssassinEnabled = true;
         _settings.AntiSpam.SpamAssassinHost = "localhost"; // <- misspelled
         _settings.AntiSpam.SpamAssassinPort = 12345;

         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         var sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (sMessageContents.Contains("X-Spam-Status"))
         {
            _settings.AntiSpam.SpamAssassinEnabled = false;
            throw new Exception("Spam assassin not run");
         }

         CustomAsserts.AssertReportedError(
            "The SpamAssassin tests did not complete. Please confirm that the configuration (host name and port) is valid and that SpamAssassin is running.");
      }

      [Test]
      public void TestIpAddressAsHostName()
      {
         var smtpClientSimulator = new SmtpClientSimulator();

         _settings.AntiSpam.SpamAssassinEnabled = true;
         _settings.AntiSpam.SpamAssassinHost = "127.0.0.1";
         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         var messageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         if (!messageContents.Contains("X-Spam-Status")) Assert.Fail("SpamAssassin did not run");
      }

      [Test]
      public void TestMessageScore()
      {
         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test",
            "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         var sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var scoreStart = sMessageContents.IndexOf("X-Spam-Status: Yes, score") + "X-Spam-Status: Yes, score".Length +
                          1;
         var scoreEnd = sMessageContents.IndexOf(".", scoreStart);
         var scoreLength = scoreEnd - scoreStart;

         var score = sMessageContents.Substring(scoreStart, scoreLength);
         var scoreValue = Convert.ToDouble(score);

         Assert.Greater(scoreValue, 500);
      }

      [Test]
      public void TestMessageScoreNotMerged()
      {
         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test",
            "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         var sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var scoreStart = sMessageContents.IndexOf("X-hMailServer-Reason-Score");
         Assert.AreNotEqual(0, scoreStart);

         scoreStart = sMessageContents.IndexOf(":", scoreStart) + 2;
         var scoreEnd = sMessageContents.IndexOf("\r\n", scoreStart);
         var scoreLength = scoreEnd - scoreStart;
         var score = sMessageContents.Substring(scoreStart, scoreLength);

         var scoreValue = Convert.ToDouble(score);
         Assert.Less(scoreValue, 10);
      }

      [Test]
      public void TestSANotRunning()
      {
         StopSpamAssassin();

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         var sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsFalse(sMessageContents.Contains("X-Spam-Status"));

         CustomAsserts.AssertReportedError("There was a communication error with SpamAssassin.",
            "The SpamAssassin tests did not complete. Please confirm that the configuration (host name and port) is valid and that SpamAssassin is running.");
      }

      [Test]
      public void TestScoreMerge()
      {
         _settings.AntiSpam.SpamAssassinMergeScore = true;

         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test",
            "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         var sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var scoreStart = sMessageContents.IndexOf("X-hMailServer-Reason-Score");
         Assert.AreNotEqual(-1, scoreStart, sMessageContents);

         try
         {
            scoreStart = sMessageContents.IndexOf(":", scoreStart) + 2;
         }
         catch (Exception)
         {
            Assert.Fail(sMessageContents);
         }

         Assert.AreNotEqual(-1, scoreStart, sMessageContents);

         var scoreEnd = sMessageContents.IndexOf("\r\n", scoreStart);
         Assert.AreNotEqual(-1, scoreEnd, sMessageContents);

         var scoreLength = scoreEnd - scoreStart;
         var score = sMessageContents.Substring(scoreStart, scoreLength);

         var scoreValue = Convert.ToDouble(score);
         Assert.Greater(scoreValue, 100);
      }

      [Test]
      public void TestSpamMessage()
      {
         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test",
            "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         var sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (!sMessageContents.Contains("X-Spam-Status: Yes"))
            Assert.Fail("Spam message not treated as spam (no X-Spam-Status-header).");

         if (!sMessageContents.Contains("X-hMailServer-Spam"))
            Assert.Fail("Spam message not treated as spam (no X-hMailServer-Spam header).");

         if (!sMessageContents.Contains("X-hMailServer-Reason"))
            Assert.Fail("Spam message not treated as spam (no X-hMailServer-Reason header).");

         if (!sMessageContents.Contains("X-hMailServer-Reason-Score"))
            Assert.Fail("Spam message not treated as spam (no X-hMailServer-Reason-Score header).");
      }

      [Test]
      [Description("Make sure that after SA has been run, the message header is still valid.")]
      public void MessageHeaderShouldBeValidAfterSAHasRun()
      {
         // Send a messages to this account.
         var smtpClient = new SmtpClientSimulator();
         smtpClient.Send(account.Address, account.Address, "SA test",
            "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         var fullMessage = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var messageHeader = fullMessage.Substring(0, fullMessage.IndexOf("\r\n\r\n"));
         Assert.IsTrue(messageHeader.Contains("Received:"));
         Assert.IsTrue(messageHeader.Contains("Return-Path:"));
         Assert.IsTrue(messageHeader.Contains("From:"));
         Assert.IsTrue(messageHeader.Contains("Subject: ThisIsSpam"));
      }

      [Test]
      public void TestWhiteList()
      {
         // First white-list the sender address
         var address = _settings.AntiSpam.WhiteListAddresses.Add();
         address.Description = "TestWhiteList";
         address.EmailAddress = "test-sender@example.test";
         address.LowerIPAddress = "0.0.0.0";
         address.UpperIPAddress = "255.255.255.255";
         address.Save();


         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("test-sender@example.test", account.Address, "SA test",
            "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         var sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsFalse(sMessageContents.Contains("X-Spam-Status: Yes"));
      }


      private static void StopSpamAssassin()
      {
         // Check if we can launch it...
         try
         {
            var serviceController = new ServiceController("SpamAssassinJAM");
            serviceController.Stop();
         }
         catch (Exception)
         {
            Assert.Inconclusive("Unable to stop SpamAssassin process. Is SpamAssassin installed?");
         }
      }
   }
}