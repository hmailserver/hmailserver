// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System.Text.RegularExpressions;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.AntiSpam
{
   [TestFixture]
   public class AuthenticationResults : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _antiSpam = _application.Settings.AntiSpam;

         _antiSpam.DMARCEnabled = true;
         _antiSpam.DMARCFailureScore = 1;
         _antiSpam.AddAuthenticationResultsHeader = true;
         _antiSpam.SpamMarkThreshold = 50;
         _antiSpam.SpamDeleteThreshold = 100;

         _originalHostName = _settings.HostName;
         _settings.HostName = HostName;
      }

      [TearDown]
      public new void TearDown()
      {
         _settings.HostName = _originalHostName;

         base.TearDown();
      }

      private const string HostName = "dmarctest.example";

      private hMailServer.AntiSpam _antiSpam;
      private string _originalHostName;

      [Test]
      [Description("The Authentication-Results header should report the DMARC and DKIM results.")]
      public void TestHeaderIsAdded()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, TestResources.MessageWithValidDkim);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("Authentication-Results: " + HostName + ";"), text);
         Assert.IsTrue(text.Contains("dmarc=pass header.from=outlook.com"), text);
         Assert.IsTrue(text.Contains("dkim=pass header.d=outlook.com"), text);
      }

      [Test]
      [Description("A DMARC failure should be reported in the Authentication-Results header.")]
      public void TestFailureIsReported()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, TestResources.MessageWithInvalidDkim);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("dmarc=fail header.from=outlook.com"), text);
      }

      [Test]
      [Description("A header claiming to have been added by us should be removed, while other ones are kept.")]
      public void TestSpoofedHeaderIsRemoved()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
            "Authentication-Results: " + HostName + "; dmarc=pass header.from=spoofed.example\r\n" +
            "Authentication-Results: upstream.example; dkim=pass header.d=upstream.example\r\n" +
            "From: sender@example.test\r\n" +
            "Subject: Authentication-Results test\r\n" +
            "\r\n" +
            "Test body\r\n");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsFalse(text.Contains("header.from=spoofed.example"), text);
         Assert.IsTrue(text.Contains("Authentication-Results: upstream.example; dkim=pass header.d=upstream.example"), text);
         Assert.IsTrue(text.Contains("Authentication-Results: " + HostName + ";"), text);
      }

      [Test]
      [Description("A DKIM d= value containing extra tags should not be able to inject results into the header.")]
      public void TestDkimDomainCannotInjectResults()
      {
         _antiSpam.DKIMVerificationEnabled = true;
         _antiSpam.DKIMVerificationFailureScore = 0;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
            "DKIM-Signature: v=1; a=rsa-sha256; c=relaxed/relaxed; s=selector;\r\n" +
            " d=evil.example dmarc=pass header.from=bank.example; h=from:subject;\r\n" +
            " bh=AAAA; b=AAAA\r\n" +
            "From: sender@example.test\r\n" +
            "Subject: Authentication-Results injection test\r\n" +
            "\r\n" +
            "Test body\r\n");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         var header = ExtractAuthenticationResults(text);

         // The domain must be quoted, so that it's parsed as a single value.
         Assert.IsTrue(header.Contains("header.d=\"evil.example dmarc=pass header.from=bank.example\""), text);
         Assert.IsFalse(header.Contains("header.d=evil.example"), text);
      }

      private static string ExtractAuthenticationResults(string message)
      {
         // Match the header value including any continuation (folded) lines.
         var normalized = message.Replace("\r\n", "\n");
         var match = Regex.Match(normalized, @"(?m)^Authentication-Results:[ \t]*((?:[^\n]|\n[ \t])+)");

         Assert.IsTrue(match.Success, message);

         return match.Groups[1].Value.TrimEnd().Replace("\n", "");
      }

      [Test]
      [Description("No header should be added when the setting is disabled.")]
      public void TestHeaderIsNotAddedWhenDisabled()
      {
         _antiSpam.AddAuthenticationResultsHeader = false;

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, TestResources.MessageWithValidDkim);

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsFalse(text.Contains("Authentication-Results:"), text);
      }
   }
}
