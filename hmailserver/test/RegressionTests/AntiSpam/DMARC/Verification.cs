// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using NUnit.Framework;
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
