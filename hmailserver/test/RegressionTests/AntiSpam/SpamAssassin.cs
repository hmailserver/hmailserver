// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Diagnostics;
using NUnit.Framework;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.AntiSpam
{
   [TestFixture]
   public class SpamAssassin : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         TestSetup.AssertSpamAssassinIsRunning();

         // Enable spam assassin
         application = SingletonProvider<TestSetup>.Instance.GetApp();
         hMailServer.AntiSpam antiSpam = _settings.AntiSpam;

         account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sa@test.com", "test");

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

      #endregion

      private Account account;
      private Application application;

      [Test]
      public void ItShouldBePossibleToTestSAConnectionUsingAPISuccess()
      {
         hMailServer.AntiSpam antiSpam = _settings.AntiSpam;

         string resultText;
         CustomAssert.IsTrue(antiSpam.TestSpamAssassinConnection("localhost", 783, out resultText));
         CustomAssert.IsTrue(resultText.Contains("Content analysis details:"));
      }

      [Test]
      public void ItShouldBePossibleToTestSAConnectionUsingAPIFailure()
      {
         hMailServer.AntiSpam antiSpam = _settings.AntiSpam;

         string resultText;

         CustomAssert.IsFalse(antiSpam.TestSpamAssassinConnection("localhost", 0, out resultText));
      }


      [Test]
      public void TestBasic()
      {
         // Send a messages to this account.
         var oSMTP = new SMTPClientSimulator();

         CustomAssert.IsTrue(oSMTP.Send(account.Address, account.Address, "SA test", "This is a test message."));
         string sMessageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (!sMessageContents.Contains("X-Spam-Status"))
         {
            CustomAssert.Fail("SpamAssassin did not run");
         }
      }

      [Test]
      public void TestDisabled()
      {
         var oSMTP = new SMTPClientSimulator();

         _settings.AntiSpam.SpamAssassinEnabled = false;
         _settings.AntiSpam.SpamAssassinHost = "localhost";
         oSMTP.Send(account.Address, account.Address, "SA test", "This is a test message.");

         string sMessageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (sMessageContents.Contains("X-Spam-Status"))
         {
            _settings.AntiSpam.SpamAssassinEnabled = false;
            throw new Exception("Spam assassin not run");
         }
      }

      [Test]
      public void TestIncorrectHost()
      {
         var oSMTP = new SMTPClientSimulator();

         _settings.AntiSpam.SpamAssassinEnabled = true;
         _settings.AntiSpam.SpamAssassinHost = "localholst"; // <- mispelled
         oSMTP.Send(account.Address, account.Address, "SA test", "This is a test message.");
         string sMessageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (sMessageContents.Contains("X-Spam-Status"))
         {
            _settings.AntiSpam.SpamAssassinEnabled = false;
            throw new Exception("Spam assassin not run");
         }
         
         TestSetup.AssertReportedError("The IP address for SpamAssassin could not be resolved.");
      }

      [Test]
      public void TestIncorrectPort()
      {
         var oSMTP = new SMTPClientSimulator();

         _settings.AntiSpam.SpamAssassinEnabled = true;
         _settings.AntiSpam.SpamAssassinHost = "localhost"; // <- mispelled
         _settings.AntiSpam.SpamAssassinPort = 12345;

         oSMTP.Send(account.Address, account.Address, "SA test", "This is a test message.");
         string sMessageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (sMessageContents.Contains("X-Spam-Status"))
         {
            _settings.AntiSpam.SpamAssassinEnabled = false;
            throw new Exception("Spam assassin not run");
         }

         TestSetup.AssertReportedError("The SpamAssassin tests did not complete. Please confirm that the configuration (host name and port) is valid and that SpamAssassin is running.");
      }

      [Test]
      public void TestIpAddressAsHostName()
      {
         var smtpClientSimulator = new SMTPClientSimulator();

         _settings.AntiSpam.SpamAssassinEnabled = true;
         _settings.AntiSpam.SpamAssassinHost = "127.0.0.1"; 
         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         string messageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         if (!messageContents.Contains("X-Spam-Status"))
         {
            CustomAssert.Fail("SpamAssassin did not run");
         }

         
      }

      [Test]
      public void TestMessageScore()
      {
         // Send a messages to this account.
         var oSMTP = new SMTPClientSimulator();

         oSMTP.Send(account.Address, account.Address, "SA test",
                    "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         string sMessageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int scoreStart = sMessageContents.IndexOf("X-Spam-Status: Yes, score") + "X-Spam-Status: Yes, score".Length +
                          1;
         int scoreEnd = sMessageContents.IndexOf(".", scoreStart);
         int scoreLength = scoreEnd - scoreStart;

         string score = sMessageContents.Substring(scoreStart, scoreLength);
         double scoreValue = Convert.ToDouble(score);

         CustomAssert.Greater(scoreValue, 500);
      }

      [Test]
      public void TestMessageScoreNotMerged()
      {
         // Send a messages to this account.
         var smtpClientSimulator = new SMTPClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test",
                    "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         string sMessageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int scoreStart = sMessageContents.IndexOf("X-hMailServer-Reason-Score");
         CustomAssert.AreNotEqual(0, scoreStart);

         scoreStart = sMessageContents.IndexOf(":", scoreStart) + 2;
         int scoreEnd = sMessageContents.IndexOf("\r\n", scoreStart);
         int scoreLength = scoreEnd - scoreStart;
         string score = sMessageContents.Substring(scoreStart, scoreLength);

         double scoreValue = Convert.ToDouble(score);
         CustomAssert.Less(scoreValue, 10);
      }

      [Test]
      public void TestSANotRunning()
      {
         TestSetup.StopSpamAssassin();
         
         // Send a messages to this account.
         var oSMTP = new SMTPClientSimulator();

         CustomAssert.IsTrue(oSMTP.Send(account.Address, account.Address, "SA test", "This is a test message."));
         string sMessageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         CustomAssert.IsFalse(sMessageContents.Contains("X-Spam-Status"));

         TestSetup.AssertReportedError();
      }

      [Test]
      public void TestScoreMerge()
      {
         _settings.AntiSpam.SpamAssassinMergeScore = true;

         // Send a messages to this account.
         var oSMTP = new SMTPClientSimulator();

         oSMTP.Send(account.Address, account.Address, "SA test",
                    "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         string sMessageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int scoreStart = sMessageContents.IndexOf("X-hMailServer-Reason-Score");
         CustomAssert.AreNotEqual(-1, scoreStart, sMessageContents);

         try
         {
            scoreStart = sMessageContents.IndexOf(":", scoreStart) + 2;
         }
         catch (Exception)
         {
            CustomAssert.Fail(sMessageContents);
         }

         CustomAssert.AreNotEqual(-1, scoreStart, sMessageContents);

         int scoreEnd = sMessageContents.IndexOf("\r\n", scoreStart);
         CustomAssert.AreNotEqual(-1, scoreEnd, sMessageContents);

         int scoreLength = scoreEnd - scoreStart;
         string score = sMessageContents.Substring(scoreStart, scoreLength);

         double scoreValue = Convert.ToDouble(score);
         CustomAssert.Greater(scoreValue, 100);
      }

      [Test]
      public void TestSpamMessage()
      {
         // Send a messages to this account.
         var oSMTP = new SMTPClientSimulator();

         oSMTP.Send(account.Address, account.Address, "SA test",
                    "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         string sMessageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (!sMessageContents.Contains("X-Spam-Status: Yes"))
            CustomAssert.Fail("Spam message not treated as spam (no X-Spam-Status-header).");

         if (!sMessageContents.Contains("X-hMailServer-Spam"))
            CustomAssert.Fail("Spam message not treated as spam (no X-hMailServer-Spam header).");

         if (!sMessageContents.Contains("X-hMailServer-Reason"))
            CustomAssert.Fail("Spam message not treated as spam (no X-hMailServer-Reason header).");

         if (!sMessageContents.Contains("X-hMailServer-Reason-Score"))
            CustomAssert.Fail("Spam message not treated as spam (no X-hMailServer-Reason-Score header).");
      }

      [Test]
      [Description("Make sure that after SA has been run, the message header is still valid.")]
      public void MessageHeaderShouldBeValidAfterSAHasRun()
      {
         // Send a messages to this account.
         var smtpClient = new SMTPClientSimulator();
        smtpClient.Send(account.Address, account.Address, "SA test",
                    "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         string fullMessage = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         string messageHeader = fullMessage.Substring(0, fullMessage.IndexOf("\r\n\r\n"));
         CustomAssert.IsTrue(messageHeader.Contains("Received:"));
         CustomAssert.IsTrue(messageHeader.Contains("Return-Path:"));
         CustomAssert.IsTrue(messageHeader.Contains("From:"));
         CustomAssert.IsTrue(messageHeader.Contains("Subject: ThisIsSpam"));
         
      }

      [Test]
      public void TestWhiteList()
      {
         // First white-list the sender address
         WhiteListAddress address = _settings.AntiSpam.WhiteListAddresses.Add();
         address.Description = "TestWhiteList";
         address.EmailAddress = "test-sender@test.com";
         address.LowerIPAddress = "0.0.0.0";
         address.UpperIPAddress = "255.255.255.255";
         address.Save();


         // Send a messages to this account.
         var oSMTP = new SMTPClientSimulator();
         oSMTP.Send("test-sender@test.com", account.Address, "SA test",
                    "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         string sMessageContents = POP3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         CustomAssert.IsFalse(sMessageContents.Contains("X-Spam-Status: Yes"));
      }
   }
}