// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.AntiSpam
{
   [TestFixture]
   public class Combinations : TestFixtureBase
   {
      #region Setup/Teardown

      [SetUp]
      public new void SetUp()
      {
         CustomAsserts.AssertSpamAssassinIsRunning();
      }

      #endregion

      [Test]
      [Description(
         "Confirm that if you have a delete threshold lower than the mark threshhold, spam tests are run until" +
         "the mark threshold is reached.")]
      public void TestDeleteThresholdLowerThanMarkThreshold()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multihit@test.com", "test");

         hMailServer.AntiSpam antiSpam = _settings.AntiSpam;

         antiSpam.SpamMarkThreshold = 15;
         antiSpam.SpamDeleteThreshold = 0;

         antiSpam.AddHeaderReason = true;
         antiSpam.AddHeaderSpam = true;
         antiSpam.PrependSubject = true;
         antiSpam.PrependSubjectText = "ThisIsSpam";

         antiSpam.CheckHostInHelo = true;
         antiSpam.CheckHostInHeloScore = 10;

         // Enable SURBL.
         SURBLServer oSURBLServer = antiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 10;
         oSURBLServer.Save();

         // Send a messages to this account, containing both incorrect MX records an SURBL-hits.
         // We should only detect one of these two:
         var smtpClientSimulator = new SmtpClientSimulator();

         // Should not be possible to send this email since it's results in a spam
         // score over the delete threshold.
         smtpClientSimulator.Send("test@example.com", oAccount1.Address, "INBOX",
                                  "Test http://surbl-org-permanent-test-point.com/ Test 2");

         string message = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         Assert.IsTrue(message.Contains("X-hMailServer-Reason-1:"));
         Assert.IsTrue(message.Contains("X-hMailServer-Reason-2:"));
      }

      [Test]
      [Description("Test that only one result header is added if one test passes and one fails.")]
      public void TestOneFailOnePass()
      {
         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "multihit@test.com", "test");

         _settings.AntiSpam.SpamMarkThreshold = 1;
         _settings.AntiSpam.SpamDeleteThreshold = 100;

         _settings.AntiSpam.AddHeaderReason = true;
         _settings.AntiSpam.AddHeaderSpam = true;
         _settings.AntiSpam.PrependSubject = true;
         _settings.AntiSpam.PrependSubjectText = "ThisIsSpam";

         _settings.AntiSpam.CheckHostInHelo = true;
         _settings.AntiSpam.CheckHostInHeloScore = 5;

         // Enable SURBL.
         SURBLServer oSURBLServer = _settings.AntiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account, containing both incorrect MX records an SURBL-hits.
         // We should only detect one of these two:
         var smtpClientSimulator = new SmtpClientSimulator();

         // Should not be possible to send this email since it's results in a spam
         // score over the delete threshold.
         smtpClientSimulator.Send("test@domain.without.mxrecords.example.com", oAccount1.Address, "INBOX",
                                  "This is a test message.");

         string message = Pop3ClientSimulator.AssertGetFirstMessageText(oAccount1.Address, "test");

         Assert.IsTrue(message.Contains("X-hMailServer-Reason-1:"));
         Assert.IsFalse(message.Contains("X-hMailServer-Reason-2:"));
      }

      [Test]
      public void TestSpamMultipleHits()
      {
         CustomAsserts.AssertSpamAssassinIsRunning();  

         Account oAccount1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "mult'ihit@test.com", "test");

         _settings.AntiSpam.SpamMarkThreshold = 1;
         _settings.AntiSpam.SpamDeleteThreshold = 2;

         _settings.AntiSpam.AddHeaderReason = true;
         _settings.AntiSpam.AddHeaderSpam = true;
         _settings.AntiSpam.PrependSubject = true;
         _settings.AntiSpam.PrependSubjectText = "ThisIsSpam";

         // Enable SpamAssassin
         _settings.AntiSpam.SpamAssassinEnabled = true;
         _settings.AntiSpam.SpamAssassinHost = "localhost";
         _settings.AntiSpam.SpamAssassinPort = 783;
         _settings.AntiSpam.SpamAssassinMergeScore = false;
         _settings.AntiSpam.SpamAssassinScore = 5;


         // Enable SURBL.
         SURBLServer oSURBLServer = _settings.AntiSpam.SURBLServers[0];
         oSURBLServer.Active = true;
         oSURBLServer.Score = 5;
         oSURBLServer.Save();

         // Send a messages to this account, containing both incorrect MX records an SURBL-hits.
         // We should only detect one of these two:
         var smtpClientSimulator = new SmtpClientSimulator();

         _settings.Logging.LogSMTP = true;
         _settings.Logging.LogDebug = true;
         _settings.Logging.Enabled = true;
         _settings.Logging.EnableLiveLogging(true);

         // Access the log once to make sure it's cleared.
         string liveLog = _settings.Logging.LiveLog;

         // Should not be possible to send this email since it's results in a spam
         // score over the delete threshold.
         CustomAsserts.Throws<DeliveryFailedException>(() => smtpClientSimulator.Send("test@domain.without.mxrecords.example.com", oAccount1.Address, "INBOX",
                                   "This is a test message. It contains incorrect MX records and a SURBL string: http://surbl-org-permanent-test-point.com/ SpamAssassinString: XJS*C4JDBQADN1.NSBN3*2IDNEN*GTUBE-STANDARD-ANTI-UBE-TEST-EMAIL*C.34X"));

         liveLog = _settings.Logging.LiveLog;

         _settings.Logging.EnableLiveLogging(false);

         int iFirst = liveLog.IndexOf("Spam test:");
         int iLast = liveLog.LastIndexOf("Spam test:");
         Assert.AreNotEqual(-1, iFirst);

         // there should only be one spam test, since any spam match
         // should result in a spam score over the delete threshold.
         Assert.AreEqual(iFirst, iLast);
      }
   }
}