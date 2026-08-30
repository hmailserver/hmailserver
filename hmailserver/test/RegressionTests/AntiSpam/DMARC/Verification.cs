// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.AntiSpam.DMARC
{
   [TestFixture]
   public class Verification : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _antiSpam = _application.Settings.AntiSpam;

         _antiSpam.DMARCEnabled = true;
         _antiSpam.DMARCFailureScore = 6;
         _antiSpam.SpamMarkThreshold = 5;
         _antiSpam.SpamDeleteThreshold = 100;
      }

      private hMailServer.AntiSpam _antiSpam;

      [Test]
      [Description("A message where a passing DKIM signature aligns with the From domain should pass DMARC.")]
      public void TestAlignedDkimPassIsNotScored()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, TestResources.MessageWithValidDkim);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsFalse(text.Contains("X-hMailServer-Spam"), text);
         Assert.IsFalse(text.Contains("Rejected by DMARC"), text);
      }

      [Test]
      [Description("A message where neither SPF nor DKIM authenticates the From domain should be scored.")]
      public void TestUnauthenticatedMessageIsScored()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // The From domain publishes a DMARC policy, but the DKIM signature is broken and
         // the envelope sender domain doesn't align with it.
         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, TestResources.MessageWithInvalidDkim);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("Rejected by DMARC. (outlook.com) - (Score: 6)"), text);
         Assert.IsTrue(text.Contains("X-hMailServer-Spam: YES"), text);
      }

      [Test]
      [Description("A From domain which doesn't publish a DMARC record should not produce a result.")]
      public void TestDomainWithoutPolicyIsNotScored()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
            "From: sender@example.test\r\n" +
            "To: " + account.Address + "\r\n" +
            "Subject: DMARC test\r\n" +
            "\r\n" +
            "Test body\r\n");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsFalse(text.Contains("X-hMailServer-Spam"), text);
         Assert.IsFalse(text.Contains("Rejected by DMARC"), text);
      }

      [Test]
      [Description("A message failing DMARC for a domain publishing p=reject should be rejected when the policy is honored.")]
      public void TestRejectPolicyIsHonored()
      {
         _antiSpam.DMARCHonorPolicy = true;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         CustomAsserts.Throws<DeliveryFailedException>(() =>
            SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, GetMessage_("sender@example.com")));
      }

      [Test]
      [Description("A message failing DMARC for a domain publishing p=reject should only be scored when the policy isn't honored.")]
      public void TestRejectPolicyIsIgnoredWhenNotHonored()
      {
         _antiSpam.DMARCHonorPolicy = false;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, GetMessage_("sender@example.com"));

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("Rejected by DMARC. (example.com) - (Score: 6)"), text);
      }

      [Test]
      [Description("A message failing DMARC for a domain publishing sp=quarantine should be marked as spam.")]
      public void TestQuarantinePolicyIsHonored()
      {
         _antiSpam.DMARCHonorPolicy = true;

         // The spam score alone must not be enough to mark the message as spam, so that
         // the mark can only come from the published policy.
         _antiSpam.DMARCFailureScore = 1;
         _antiSpam.SpamMarkThreshold = 50;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // The subdomain publishes no DMARC record of its own, so the record at the
         // organizational domain applies - and there sp=quarantine.
         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
            GetMessage_("sender@dmarctest.gmail.com"));

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("X-hMailServer-Spam: YES"), text);
      }

      [Test]
      [Description("DMARC should run even when the spam score threshold has already been reached.")]
      public void TestDmarcRunsAfterScoreThresholdReached()
      {
         // DKIM runs before DMARC and reaches the threshold on its own, which stops
         // the remaining tests from running.
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 10;
         _antiSpam.SpamDeleteThreshold = 0;
         _antiSpam.SpamMarkThreshold = 5;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, TestResources.MessageWithInvalidDkim);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("Rejected by DMARC. (outlook.com)"), text);
      }

      private static string GetMessage_(string fromAddress)
      {
         return "From: " + fromAddress + "\r\n" +
                "Subject: DMARC test\r\n" +
                "\r\n" +
                "Test body\r\n";
      }

      [Test]
      [Description("DMARC should not be evaluated when the test is disabled.")]
      public void TestDisabledDmarcDoesNotScore()
      {
         _antiSpam.DMARCEnabled = false;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, TestResources.MessageWithInvalidDkim);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsFalse(text.Contains("Rejected by DMARC"), text);
      }
   }
}
