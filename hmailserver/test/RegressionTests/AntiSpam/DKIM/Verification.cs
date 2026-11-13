// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.AntiSpam.DKIM
{
   [TestFixture]
   public class Verification : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _antiSpam = _application.Settings.AntiSpam;

         _antiSpam.SpamDeleteThreshold = 5;
      }

      private hMailServer.AntiSpam _antiSpam;

      [Test]
      [Description("Test that a message with an invalid body hash is blocked.")]
      public void TestInvalidBodyHash()
      {
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 100;


         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address,
               TestResources.MessageWithInvalidDkim));
      }

      [Test]
      [Description("Test that tagging of spam works.")]
      public void TestInvalidBodyHashMark()
      {
         _antiSpam.SpamDeleteThreshold = 1000;
         _antiSpam.SpamMarkThreshold = 5;
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 6;

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, TestResources.MessageWithInvalidDkim);
         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");

         Assert.IsTrue(text.Contains("Rejected by DKIM. - (Score: 6)"));
      }

      [Test]
      [Description("A failing signature should still be scored when a later signature is ignored as unsupported.")]
      public void TestInvalidSignatureFollowedByUnsupportedSignature()
      {
         _antiSpam.SpamDeleteThreshold = 1000;
         _antiSpam.SpamMarkThreshold = 5;
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 6;

         // Insert a v=2 signature right after the failing one. It's ignored and gives a neutral result.
         var message = TestResources.MessageWithInvalidDkim;
         var newLine = message.Contains("\r\n") ? "\r\n" : "\n";
         var insertAt = message.IndexOf(newLine) + newLine.Length;
         message = message.Insert(insertAt,
            "DKIM-Signature: v=2; a=rsa-sha256; d=example.com; s=selector; h=From; bh=AAAA; b=AAAA" + newLine);

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, message);
         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");

         Assert.IsTrue(text.Contains("Rejected by DKIM. - (Score: 6)"), text);
      }

      [Test]
      [Description("Test that a message with an invalid signature is not blocked.")]
      public void TestInvalidSignature()
      {
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 100;

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         CustomAsserts.Throws<DeliveryFailedException>(() =>
            SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address,
               TestResources.MessageWithInvalidDkim));
      }

      [Test]
      [Description("Test that a message with a valid SHA1 signature is not blocked.")]
      public void TestValidSignatureSHA256()
      {
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 100;

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, TestResources.MessageWithValidDkim);
         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
      }

      [Test]
      public void TestCanonicalizationWithTrailingSlash()
      {
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 100;

         var account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address,
            "DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/; s=selector; d=example.test;\r\n" +
            " h=from:subject; bh=AAAA; b=AAAA\r\n" +
            "From: test@example.test\r\n" +
            "Subject: DKIM canonicalization test\r\n" +
            "\r\n" +
            "Test body\r\n");

         Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
      }
   }
}