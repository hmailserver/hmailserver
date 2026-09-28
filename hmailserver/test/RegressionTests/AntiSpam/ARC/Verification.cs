// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.AntiSpam.ARC
{
   // The chain validation itself, with signed chains, is covered by ARCTester, which runs
   // inside hMailServer.exe through MainOperations with a key it generates. These cover
   // the wiring: that the chain is checked on the SMTP path and the result is reported.
   [TestFixture]
   public class Verification : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _antiSpam = _application.Settings.AntiSpam;

         _antiSpam.ARCEnabled = true;
         _antiSpam.AddAuthenticationResultsHeader = true;
         _antiSpam.SpamMarkThreshold = 5;
         _antiSpam.SpamDeleteThreshold = 100;
      }

      private hMailServer.AntiSpam _antiSpam;

      private const string PlainMessage =
         "From: sender@example.test\r\n" +
         "Subject: ARC test\r\n" +
         "\r\n" +
         "Test body\r\n";

      [Test]
      [Description("A message without ARC header fields should be reported as arc=none.")]
      public void TestMessageWithoutChainIsReportedAsNone()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, PlainMessage);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("arc=none"), text);
      }

      [Test]
      [Description("An incomplete chain should be reported as arc=fail, and add no spam score.")]
      public void TestIncompleteChainIsReportedAsFail()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
            "ARC-Seal: i=1; a=rsa-sha256; cv=none; d=example.org; s=arc; b=abc\r\n" +
            PlainMessage);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("arc=fail (incomplete ARC set)"), text);
         Assert.IsFalse(text.Contains("X-hMailServer-Spam"), text);
      }

      [Test]
      [Description("No ARC result should be reported when ARC is disabled.")]
      public void TestNothingIsReportedWhenDisabled()
      {
         _antiSpam.ARCEnabled = false;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, PlainMessage);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("Authentication-Results:"), text);
         Assert.IsFalse(text.Contains("arc="), text);
      }

      [Test]
      [Description("A trusted sealer should not rescue a DMARC failure when the message has no chain.")]
      public void TestDMARCFailureWithoutChainIsNotOverridden()
      {
         _antiSpam.DMARCEnabled = true;
         _antiSpam.DMARCFailureScore = 6;
         foreach (var domain in new[] { "outlook.com", "example.org" })
         {
            var sealer = _antiSpam.ARCTrustedSealers.Add();
            sealer.Domain = domain;
            sealer.Save();
         }

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, TestResources.MessageWithInvalidDkim);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("Rejected by DMARC. (outlook.com) - (Score: 6)"), text);
         Assert.IsFalse(text.Contains("overridden"), text);
      }
   }
}
