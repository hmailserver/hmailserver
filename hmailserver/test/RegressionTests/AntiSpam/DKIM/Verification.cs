// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;
using hMailServer;

namespace RegressionTests.AntiSpam.DKIM
{
   [TestFixture]
   public class Verification : TestFixtureBase
   {
      #region Setup/Teardown
      
      [SetUp]
      public new void SetUp()
      {
         _antiSpam = _application.Settings.AntiSpam;

         _antiSpam.SpamDeleteThreshold = 5;
      }

      #endregion

      private hMailServer.AntiSpam _antiSpam;

      [Test]
      [Description("Test that a message with an invalid body hash is blocked.")]
      public void TestInvalidBodyHash()
      {
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 100;

         

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, TestResources.MessageWithInvalidDkim));
      }

      [Test]
      [Description("Test that tagging of spam works.")]
      public void TestInvalidBodyHashMark()
      {
         _antiSpam.SpamDeleteThreshold = 1000;
         _antiSpam.SpamMarkThreshold = 5;
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 6;

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, TestResources.MessageWithInvalidDkim);
         string text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");

         Assert.IsTrue(text.Contains("Rejected by DKIM. - (Score: 6)"));
      }

      [Test]
      [Description("Test that a message with an invalid signature is not blocked.")]
      public void TestInvalidSignature()
      {
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 100;

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, TestResources.MessageWithInvalidDkim));
      }

      [Test]
      [Description("Test that a message with a valid SHA1 signature is not blocked.")]
      public void TestValidSignatureSHA256()
      {
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 100;
         
         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, TestResources.MessageWithValidDkim);
         string text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
      }
   }
}