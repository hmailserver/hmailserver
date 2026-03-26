// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

using System.IO;
using System.Text.RegularExpressions;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.MIME
{
   [TestFixture]
   public class RoundTrip : TestFixtureBase
   {
      private static void AddSetHeaderRule(Account account, string headerName, string headerValue)
      {
         var rule = account.Rules.Add();
         rule.Name = "Round-trip test rule";
         rule.Active = true;

         var criteria = rule.Criterias.Add();
         criteria.UsePredefined = true;
         criteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         criteria.MatchType = eRuleMatchType.eMTGreaterThan;
         criteria.MatchValue = "0";
         criteria.Save();

         var action = rule.Actions.Add();
         action.Type = eRuleActionType.eRASetHeaderValue;
         action.HeaderName = headerName;
         action.Value = headerValue;
         action.Save();

         rule.Save();
      }

      private static string ExtractHeaderValue(string message, string headerName)
      {
         // Normalize line endings, then match the header value including any
         // continuation (folded) lines.
         var normalized = message.Replace("\r\n", "\n");
         var match = Regex.Match(normalized,
            $@"(?m)^{Regex.Escape(headerName)}:[ \t]*((?:[^\n]|\n[ \t])+)",
            RegexOptions.IgnoreCase);
         return match.Success ? match.Groups[1].Value.TrimEnd() : null;
      }

      private static string ExtractBody(string message)
      {
         // Body starts after the blank line separating headers from body.
         var normalized = message.Replace("\r\n", "\n");
         var idx = normalized.IndexOf("\n\n");
         return idx >= 0 ? normalized.Substring(idx + 2) : string.Empty;
      }

      [Test]
      [Description("Adding a new header to a message must not modify existing header values.")]
      public void TestAddingHeaderPreservesExistingHeaders()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "roundtrip@example.test", "test");
         AddSetHeaderRule(account, "X-Test", "test-value");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, TestResources.MessageWithValidDkim);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var storedFilename = account.IMAPFolders.get_ItemByName("Inbox").Messages[0].Filename;
         var storedContent = File.ReadAllText(storedFilename);
         var originalContent = TestResources.MessageWithValidDkim;

         StringAssert.Contains("X-Test: test-value", storedContent,
            "The rule should have added the X-Test header");

         var originalDkim = ExtractHeaderValue(originalContent, "DKIM-Signature");
         var storedDkim = ExtractHeaderValue(storedContent, "DKIM-Signature");
         Assert.AreEqual(originalDkim, storedDkim,
            "DKIM-Signature header value was modified during re-serialization");

         var originalFrom = ExtractHeaderValue(originalContent, "From");
         var storedFrom = ExtractHeaderValue(storedContent, "From");
         Assert.AreEqual(originalFrom, storedFrom,
            "From header value was modified during re-serialization");
      }

      [Test]
      [Description("Adding a new header to a multipart message must not modify the body content.")]
      public void TestAddingHeaderPreservesMultipartBody()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "roundtrip2@example.test", "test");
         AddSetHeaderRule(account, "X-Test", "test-value");

         SmtpClientSimulator.StaticSendRaw(account.Address, account.Address, TestResources.MessageWithValidDkim);

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var storedFilename = account.IMAPFolders.get_ItemByName("Inbox").Messages[0].Filename;
         var storedContent = File.ReadAllText(storedFilename);
         var originalContent = TestResources.MessageWithValidDkim;

         var originalContentType = ExtractHeaderValue(originalContent, "Content-Type");
         var storedContentType = ExtractHeaderValue(storedContent, "Content-Type");
         Assert.AreEqual(originalContentType, storedContentType,
            "Content-Type header (including multipart boundary) was modified during re-serialization");

         var originalBody = ExtractBody(originalContent);
         var storedBody = ExtractBody(storedContent);
         Assert.AreEqual(originalBody, storedBody,
            "Multipart body content was modified during re-serialization");
      }
   }
}
