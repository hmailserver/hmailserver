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

      // The SPF results the Authentication-Results header reports, over real DNS.
      //
      // The conformance suite under Server/Smtp/Spf/Conformance decides what an
      // evaluation comes to, and runs inside hMailServer.exe through
      // MainOperations.TestInternals, so it needs no help here. What these cover
      // is the wiring the suite cannot see: that a check is made at all on the
      // SMTP path, that the client address, the sender and the HELO argument reach
      // it, that DNS answers it, and that the result reaches the header.
      //
      // Each sender is chosen so that the answer does not depend on the address
      // the test connects from, which is the machine's own:
      //
      //   example.com                  v=spf1 -all     - reserved by IANA
      //   hmailserver.com              v=spf1 mx -all  - this project's domain
      //   nonexistent.hmailserver.com  no record at all
      //
      // Each enables SPF rather than leaning on the DMARC test to evaluate it.
      // DMARC does evaluate SPF, but only after finding a policy for the From
      // domain, so a sender which publishes no DMARC record would never be
      // checked - and the sender with no SPF record has no DMARC record either.

      private void EnableSpf()
      {
         _antiSpam.UseSPF = true;

         // Enough to be visible in a score and far below SpamMarkThreshold, so
         // that a failing message is still delivered and can be read back.
         _antiSpam.UseSPFScore = 1;
      }

      [Test]
      [Description("A domain whose record authorizes no client at all should be reported as an SPF failure.")]
      public void TestSpfFailureIsReported()
      {
         EnableSpf();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw("sender@example.com", account.Address,
            "From: sender@example.com\r\n" +
            "Subject: SPF test\r\n" +
            "\r\n" +
            "Test body\r\n");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("spf=fail smtp.mailfrom=example.com"), text);
      }

      [Test]
      [Description("A record whose mx mechanism does not cover the client should be reported as an SPF failure.")]
      public void TestSpfFailureThroughMxIsReported()
      {
         EnableSpf();

         // "v=spf1 mx -all", so reaching the fail means the MX records were looked
         // up and their addresses resolved. A lookup that could not be answered
         // would be a temperror instead, which is what tells this apart from the
         // case above: that one needs no DNS beyond the record itself.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw("sender@hmailserver.com", account.Address,
            "From: sender@hmailserver.com\r\n" +
            "Subject: SPF test\r\n" +
            "\r\n" +
            "Test body\r\n");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("spf=fail smtp.mailfrom=hmailserver.com"), text);
      }

      [Test]
      [Description("A domain which publishes no SPF record should be reported as none rather than as neutral.")]
      public void TestSpfNoneIsReported()
      {
         EnableSpf();

         // RFC 7208 section 4.3: a domain with no record has said nothing, which
         // is not the same as having said nothing about this client. The two are
         // different results, and reporting them apart is the point.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw("sender@nonexistent.hmailserver.com", account.Address,
            "From: sender@nonexistent.hmailserver.com\r\n" +
            "Subject: SPF test\r\n" +
            "\r\n" +
            "Test body\r\n");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         Assert.IsTrue(text.Contains("spf=none smtp.mailfrom=nonexistent.hmailserver.com"), text);
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

      [Test]
      [Description("Our header should be placed above headers added by others, since the topmost one is trusted.")]
      public void TestHeaderIsAddedAboveExistingHeaders()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address,
            "Authentication-Results: upstream.example; dmarc=pass header.from=bank.example\r\n" +
            "From: sender@example.test\r\n" +
            "Subject: Authentication-Results order test\r\n" +
            "\r\n" +
            "Test body\r\n");

         var text = Pop3ClientSimulator.AssertGetFirstMessageText(account.Address, "test");

         int ourIndex = text.IndexOf("Authentication-Results: " + HostName + ";");
         int upstreamIndex = text.IndexOf("Authentication-Results: upstream.example;");

         Assert.AreNotEqual(-1, ourIndex, text);
         Assert.AreNotEqual(-1, upstreamIndex, text);
         Assert.IsTrue(ourIndex < upstreamIndex, text);
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
