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

         string messageText = @"DKIM-Signature: v=1; a=rsa-sha1; c=simple; d=messiah.edu; h=from:to" + "\r\n" +
                              "	:subject:date; s=test3; i==6Along@messiah.edu; bh=PW2otvzd7V2TO8" + "\r\n" +
                              "	w056SjbYRFCa0=; b=Vfr8HgUlyVf1ZaRVMV8VJNSDXn7f1j2N/rFM4PPmYIC2GD" + "\r\n" +
                              "	pSelCRrdA979Buuu/Mmx9FTWoZJBL+s5tafFM8bw==" + "\r\n" +
                              "Received: from x.y.test" + "\r\n" +
                              "   by example.net" + "\r\n" +
                              "   via TCP" + "\r\n" +
                              "   with ESMTP" + "\r\n" +
                              "   id ABC12345" + "\r\n" +
                              "   for <mary@example.net>;  21 Nov 1997 10:05:43 -0600" + "\r\n" +
                              "Received: from machine.example by x.y.test; 21 Nov 1997 10:01:22 -0600" + "\r\n" +
                              "From: Jason Long <jlong@messiah.edu>" + "\r\n" +
                              "To: Nobody <nobody@messiah.edu>" + "\r\n" +
                              "Subject: dkim test (i= uses quoted-printable)" + "\r\n" +
                              "Date: Wed, 9 Apr 2008 09:11:00 -0500" + "\r\n" +
                              "" + "\r\n" +
                              "Should pass." + "\r\n" +
                              "" + "\r\n" +
                              "This is a test" + "\r\n" +
                              "  More lines here" + "\r\n" +
                              "" + "\r\n" +
                              "Blah  blah  blah" + "\r\n" +
                              "" + "\r\n" +
                              "" + "\r\n" +
                              "" + "\r\n" +
                              "" + "\r\n";

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, messageText));
      }

      [Test]
      [Description("Test that tagging of spam works.")]
      public void TestInvalidBodyHashMark()
      {
         _antiSpam.SpamDeleteThreshold = 1000;
         _antiSpam.SpamMarkThreshold = 5;
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 6;

         string messageText = @"DKIM-Signature: v=1; a=rsa-sha1; c=simple; d=messiah.edu; h=from:to" + "\r\n" +
                              "	:subject:date; s=test3; i==6Along@messiah.edu; bh=OW2otvzd7V2TO8" + "\r\n" +
                              "	w056SjbYRFCa0=; b=Vfr8HgUlyVf1ZaRVMV8VJNSDXn7f1j2N/rFM4PPmYIC2GD" + "\r\n" +
                              "	pSelCRrdA979Buuu/Mmx9FTWoZJBL+s5tafFM8bw==" + "\r\n" +
                              "Received: from x.y.test" + "\r\n" +
                              "   by example.net" + "\r\n" +
                              "   via TCP" + "\r\n" +
                              "   with ESMTP" + "\r\n" +
                              "   id ABC12345" + "\r\n" +
                              "   for <mary@example.net>;  21 Nov 1997 10:05:43 -0600" + "\r\n" +
                              "Received: from machine.example by x.y.test; 21 Nov 1997 10:01:22 -0600" + "\r\n" +
                              "From: Jason Long <jlong@messiah.edu>" + "\r\n" +
                              "To: Nobody <nobody@messiah.edu>" + "\r\n" +
                              "Subject: dkim test (i= uses quoted-printable)" + "\r\n" +
                              "Date: Wed, 9 Apr 2008 09:11:00 -0500" + "\r\n" +
                              "" + "\r\n" +
                              "Should pass." + "\r\n" +
                              "" + "\r\n" +
                              "This is a test" + "\r\n" +
                              "  More lines here" + "\r\n" +
                              "" + "\r\n" +
                              "Blah  blah  blah" + "\r\n" +
                              "" + "\r\n" +
                              "" + "\r\n" +
                              "" + "\r\n" +
                              "" + "\r\n";

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, messageText);
         string text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");

         Assert.IsTrue(text.Contains("Rejected by DKIM. - (Score: 6)"));
      }

      [Test]
      [Description("Test that a message with an invalid signature is not blocked.")]
      public void TestInvalidSignature()
      {
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 100;

         string messageText = @"DKIM-Signature: v=1; a=rsa-sha1; c=simple; d=messiah.edu; h=from:to" + "\r\n" +
                              "	:subject:date; s=test3; i==6Along@messiah.edu; bh=OW2otvzd7V2TO8" + "\r\n" +
                              "	w056SjbYRFCa0=; b=Vfr9HgUlyVf1ZaRVMV8VJNSDXn7f1j2N/rFM4PPmYIC2GF" + "\r\n" +
                              "	pSelCRrdA979Buuu/Mmx9FTWoZJBL+s5tafFM8bw==" + "\r\n" +
                              "Received: from x.y.test" + "\r\n" +
                              "   by example.net" + "\r\n" +
                              "   via TCP" + "\r\n" +
                              "   with ESMTP" + "\r\n" +
                              "   id ABC12345" + "\r\n" +
                              "   for <mary@example.net>;  21 Nov 1997 10:05:43 -0600" + "\r\n" +
                              "Received: from machine.example by x.y.test; 21 Nov 1997 10:01:22 -0600" + "\r\n" +
                              "From: Jason Long <jlong@messiah.edu>" + "\r\n" +
                              "To: Nobody <nobody@messiah.edu>" + "\r\n" +
                              "Subject: dkim test (i= uses quoted-printable)" + "\r\n" +
                              "Date: Wed, 9 Apr 2008 09:11:00 -0500" + "\r\n" +
                              "" + "\r\n" +
                              "Should pass." + "\r\n" +
                              "" + "\r\n" +
                              "This is a test" + "\r\n" +
                              "  More lines here" + "\r\n" +
                              "" + "\r\n" +
                              "Blah  blah  blah" + "\r\n" +
                              "" + "\r\n" +
                              "" + "\r\n" +
                              "" + "\r\n" +
                              "" + "\r\n";

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         CustomAsserts.Throws<DeliveryFailedException>(() => SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, messageText));
      }

      [Test]
      [Description("Test that a message with a valid SHA1 signature is not blocked.")]
      public void TestValidSignatureSHA256()
      {
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 100;

         string messageText = @"Return-Path: <tony@att.com>" + "\r\n" +
                              "X-Original-To: test@dkimtest.jason.long.name" + "\r\n" +
                              "Delivered-To: dkimtest@mx2.messiah.edu" + "\r\n" +
                              "Received: from voicemail.cis.att.net (unknown [12.34.200.188])" + "\r\n" +
                              "	by mx2.messiah.edu (Postfix) with ESMTP id 02F12E15D8" + "\r\n" +
                              "	for <test@dkimtest.jason.long.name>; Wed,  3 May 2006 15:06:32 -0400 (EDT)" + "\r\n" +
                              "Received: from  (localhost[127.0.0.1]) by voicemail.cis.att.net (vm2) with SMTP" +
                              "\r\n" +
                              "          id <2006050319071918800spa0re>; Wed, 3 May 2006 19:07:19 +0000" + "\r\n" +
                              "DKIM-Signature: a=rsa-sha256;  c=relaxed; d=vmt2.cis.att.net; t=1146680862; " + "\r\n" +
                              " h=Date : From : MIME-Version : To : Subject : Content-Type : Content-Transfer-Encoding; bh=HryPFX2R6r7JPsX1Z7+yReZddQR2PjvCvdXgaxW5QYU=; s=shan; " +
                              "\r\n" +
                              " b=QXd8h2UbBO7fIPz/Iy3wNwbVU6dih6ozokPXqAvI6p9iG5SqFahyTXwqZeltC4az3Sjay7Vx+b5e" +
                              "\r\n" +
                              " 1s2rQuhT4SKD47gJYs4kw0JgV2WLanF3oR1hWD0tL0vuDeUgH6kr" + "\r\n" +
                              "Date: Wed, 15 Feb 2006 17:32:54 -0500" + "\r\n" +
                              "From: Tony Hansen <tony@att.com>" + "\r\n" +
                              "MIME-Version: 1.0" + "\r\n" +
                              "To: dkim-test@altn.org, sa-test@sendmail.net, autorespond+dkim@dk.elandsys.com" +
                              "\r\n" +
                              "Subject: this is a test message minimum.ietf-01.sha256-relaxed" + "\r\n" +
                              "Content-Type: text/plain; charset=ISO-8859-1" + "\r\n" +
                              "Content-Transfer-Encoding: 7bit" + "\r\n" +
                              "Message-Id: <20060503190632.02F12E15D8@mx2.messiah.edu>" + "\r\n" +
                              "" + "\r\n" +
                              "The quick brown fox jumped over the lazy dog." + "\r\n";

         Account account1 = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@test.com", "test");
         SmtpClientSimulator.StaticSendRaw(account1.Address, account1.Address, messageText);
         string text = Pop3ClientSimulator.AssertGetFirstMessageText(account1.Address, "test");
      }
   }
}