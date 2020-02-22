// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Diagnostics;
using System.ServiceProcess;
using NUnit.Framework;
using RegressionTests.Infrastructure;
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
         CustomAsserts.AssertSpamAssassinIsRunning();

         // Enable spam assassin
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
      

      [Test]
      public void ItShouldBePossibleToTestSAConnectionUsingAPISuccess()
      {
         hMailServer.AntiSpam antiSpam = _settings.AntiSpam;

         string resultText;
         Assert.IsTrue(antiSpam.TestSpamAssassinConnection("localhost", 783, out resultText));
         Assert.IsTrue(resultText.Contains("Content analysis details:"));
      }

      [Test]
      public void ItShouldBePossibleToTestSAConnectionUsingAPIFailure()
      {
         hMailServer.AntiSpam antiSpam = _settings.AntiSpam;

         string resultText;

         Assert.IsFalse(antiSpam.TestSpamAssassinConnection("localhost", 0, out resultText));
      }


      [Test]
      public void TestBasic()
      {
         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (!sMessageContents.Contains("X-Spam-Status"))
         {
            Assert.Fail("SpamAssassin did not run");
         }
      }

      [Test]
      public void TestDisabled()
      {
         var smtpClientSimulator = new SmtpClientSimulator();

         _settings.AntiSpam.SpamAssassinEnabled = false;
         _settings.AntiSpam.SpamAssassinHost = "localhost";
         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
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
         _settings.AntiSpam.SpamAssassinHost = "localholst"; // <- mispelled
         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
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
         _settings.AntiSpam.SpamAssassinHost = "localhost"; // <- mispelled
         _settings.AntiSpam.SpamAssassinPort = 12345;

         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
         if (sMessageContents.Contains("X-Spam-Status"))
         {
            _settings.AntiSpam.SpamAssassinEnabled = false;
            throw new Exception("Spam assassin not run");
         }

         CustomAsserts.AssertReportedError("The SpamAssassin tests did not complete. Please confirm that the configuration (host name and port) is valid and that SpamAssassin is running.");
      }

      [Test]
      public void TestIpAddressAsHostName()
      {
         var smtpClientSimulator = new SmtpClientSimulator();

         _settings.AntiSpam.SpamAssassinEnabled = true;
         _settings.AntiSpam.SpamAssassinHost = "127.0.0.1"; 
         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         string messageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         if (!messageContents.Contains("X-Spam-Status"))
         {
            Assert.Fail("SpamAssassin did not run");
         }

         
      }

      [Test]
      public void TestMessageScore()
      {
         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test",
                    "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int scoreStart = sMessageContents.IndexOf("X-Spam-Status: Yes, score") + "X-Spam-Status: Yes, score".Length +
                          1;
         int scoreEnd = sMessageContents.IndexOf(".", scoreStart);
         int scoreLength = scoreEnd - scoreStart;

         string score = sMessageContents.Substring(scoreStart, scoreLength);
         double scoreValue = Convert.ToDouble(score);

         Assert.Greater(scoreValue, 500);
      }

      [Test]
      public void TestMessageScoreNotMerged()
      {
         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test",
                    "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int scoreStart = sMessageContents.IndexOf("X-hMailServer-Reason-Score");
         Assert.AreNotEqual(0, scoreStart);

         scoreStart = sMessageContents.IndexOf(":", scoreStart) + 2;
         int scoreEnd = sMessageContents.IndexOf("\r\n", scoreStart);
         int scoreLength = scoreEnd - scoreStart;
         string score = sMessageContents.Substring(scoreStart, scoreLength);

         double scoreValue = Convert.ToDouble(score);
         Assert.Less(scoreValue, 10);
      }

      [Test]
      public void TestSANotRunning()
      {
         StopSpamAssassin();
         
         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test", "This is a test message.");
         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

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

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int scoreStart = sMessageContents.IndexOf("X-hMailServer-Reason-Score");
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

         int scoreEnd = sMessageContents.IndexOf("\r\n", scoreStart);
         Assert.AreNotEqual(-1, scoreEnd, sMessageContents);

         int scoreLength = scoreEnd - scoreStart;
         string score = sMessageContents.Substring(scoreStart, scoreLength);

         double scoreValue = Convert.ToDouble(score);
         Assert.Greater(scoreValue, 100);
      }

      [Test]
      public void TestSpamMessage()
      {
         // Send a messages to this account.
         var smtpClientSimulator = new SmtpClientSimulator();

         smtpClientSimulator.Send(account.Address, account.Address, "SA test",
                    "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");
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

         string fullMessage = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         string messageHeader = fullMessage.Substring(0, fullMessage.IndexOf("\r\n\r\n"));
         Assert.IsTrue(messageHeader.Contains("Received:"));
         Assert.IsTrue(messageHeader.Contains("Return-Path:"));
         Assert.IsTrue(messageHeader.Contains("From:"));
         Assert.IsTrue(messageHeader.Contains("Subject: ThisIsSpam"));
         
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
         var smtpClientSimulator = new SmtpClientSimulator();
         smtpClientSimulator.Send("test-sender@test.com", account.Address, "SA test",
                    "This is a test message with spam.\r\n XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X.");

         string sMessageContents = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

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