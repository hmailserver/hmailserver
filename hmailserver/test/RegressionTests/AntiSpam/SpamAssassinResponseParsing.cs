// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System.Text;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.AntiSpam
{
   /// <summary>
   ///    Tests how hMailServer handles invalid responses from spamd. Uses a simulated spamd,
   ///    so SpamAssassin does not need to be installed.
   /// </summary>
   [TestFixture]
   public class SpamAssassinResponseParsing : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sa@example.test", "test");
         _port = TestSetup.GetNextFreePort();

         var antiSpam = _settings.AntiSpam;
         antiSpam.SpamMarkThreshold = 1;
         antiSpam.SpamDeleteThreshold = 10000;
         antiSpam.SpamAssassinEnabled = true;
         antiSpam.SpamAssassinHost = "127.0.0.1";
         antiSpam.SpamAssassinPort = _port;
         antiSpam.SpamAssassinMergeScore = false;
         antiSpam.SpamAssassinScore = 5;
      }

      private Account _account;
      private int _port;

      private const string SimulatedMessage =
         "X-Simulated-Spamd: Yes\r\n" +
         "Subject: SA test\r\n" +
         "\r\n" +
         "Body\r\n";

      [TestCase("\r\n\r\n" + SimulatedMessage, TestName = "EmptyHeader")]
      [TestCase("SPAMD/1.1 0 EX_OK\r\n\r\n" + SimulatedMessage, TestName = "MissingContentLength")]
      [TestCase("SPAMD/1.1 0 EX_OK\r\nContent-length: -1\r\n\r\n" + SimulatedMessage, TestName = "NegativeContentLength")]
      public void InvalidSpamdResponseShouldBeIgnored(string response)
      {
         using (var spamd = new SpamdSimulator(_port, response))
         {
            spamd.StartListen();

            SmtpClientSimulator.StaticSend(_account.Address, _account.Address, "SA test", "This is a test message.");

            spamd.WaitForCompletion();

            StringAssert.StartsWith("PROCESS SPAMC/1.2", spamd.Request, "SpamAssassin was not called");
            Assert.IsTrue(spamd.ClientClosedConnection, "hMailServer did not close the connection to spamd");
         }

         ServiceRestartDetector.ValidateProcessId();

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
         Assert.IsFalse(messageText.Contains("X-Simulated-Spamd"), messageText);
         Assert.IsTrue(messageText.Contains("This is a test message."), messageText);

         LogHandler.DeleteErrorLog();
      }

      [Test]
      public void ResponseHeaderSplitAcrossReadsShouldBeAccepted()
      {
         var message =
            "X-Simulated-Spamd: Yes\r\n" +
            "X-Spam-Status: No, score=0.1 required=5.0\r\n" +
            "Subject: SA test\r\n" +
            "\r\n" +
            "This is a test message.\r\n";

         var firstPart = "SPAMD/1.1 0 EX_OK\r\n";
         var secondPart = "Content-length: " + Encoding.ASCII.GetByteCount(message) + "\r\n\r\n" + message;

         using (var spamd = new SpamdSimulator(_port, firstPart, secondPart))
         {
            spamd.StartListen();

            SmtpClientSimulator.StaticSend(_account.Address, _account.Address, "SA test", "This is a test message.");

            spamd.WaitForCompletion();

            StringAssert.StartsWith("PROCESS SPAMC/1.2", spamd.Request, "SpamAssassin was not called");
         }

         var messageText = Pop3ClientSimulator.AssertGetFirstMessageText(_account.Address, "test");
         Assert.IsTrue(messageText.Contains("X-Simulated-Spamd: Yes"), messageText);

         CustomAsserts.AssertNoReportedError();
      }
   }
}
