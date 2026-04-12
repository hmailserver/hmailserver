// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.AntiSpam.DKIM
{
   [TestFixture]
   public class Signing : TestFixtureBase
   {
      [SetUp]
      public new void SetUp()
      {
         _antiSpam = _application.Settings.AntiSpam;

         _antiSpam.SpamDeleteThreshold = 5;
      }

      private hMailServer.AntiSpam _antiSpam;

      private string GetPrivateKeyFile()
      {
         var sslPath = Path.Combine(TestContext.CurrentContext.TestDirectory, "..\\..\\..\\..\\SSL examples");

         var exampleKeyFile = Path.Combine(sslPath, "example.key");
         if (!File.Exists(exampleKeyFile))
            throw new Exception("Example key file could not be found.");

         return exampleKeyFile;
      }

      private string SendMessage()
      {
         return SendMessage("Test message");
      }

      private string SendMessage(string body)
      {
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         var deliveryResults = new Dictionary<string, int>();
         deliveryResults["test@example.com"] = 250;

         var port = TestSetup.GetNextFreePort();
         using (var server = new SmtpServerSimulator(1, port))
         {
            server.SecondsToWaitBeforeTerminate = 60;
            server.AddRecipientResult(deliveryResults);
            server.StartListen();

            // Add a route so we can connect to localhost.
            AddRoutePointingAtLocalhost(5, port);

            // Send message to this route.
            var smtp = new SmtpClientSimulator();
            var recipients = new List<string>();
            recipients.Add("test@example.com");
            smtp.Send("test@example.test", recipients, "Test", body);

            // Wait for the client to disconnect.
            server.WaitForCompletion();
            var messageData = server.MessageData;

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            return messageData;
         }
      }

      internal static Route AddRoutePointingAtLocalhost(int numberOfTries, int port)
      {
         // Add a route pointing at localhost
         var settings = SingletonProvider<TestSetup>.Instance.GetApp().Settings;

         var route = settings.Routes.Add();
         route.DomainName = "example.com";
         route.TargetSMTPHost = "localhost";
         route.TargetSMTPPort = port;
         route.NumberOfTries = numberOfTries;
         route.MinutesBetweenTry = 5;
         route.Save();

         return route;
      }

      [Test]
      [Description("Test usage of algorithm RSA-SHA1.")]
      public void TestAlgorithmSHA1()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSigningAlgorithm = eDKIMAlgorithm.eSHA1;
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage();
         Assert.IsTrue(result.ToLower().Contains("a=rsa-sha1"), result);
      }

      [Test]
      [Description("Test usage of algorithm RSA-SHA256.")]
      public void TestAlgorithmSHA256()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSigningAlgorithm = eDKIMAlgorithm.eSHA256;
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage();

         if (result.ToLower().Contains("a=rsa-sha256") == false) Assert.Fail(result);
      }

      [Test]
      [Description("Test that the default canonicalization method is relaxed/relaxed.")]
      public void TestCanonicalizationMethodSimpleSimple()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMBodyCanonicalizationMethod = eDKIMCanonicalizationMethod.eCanonicalizationSimple;
         _domain.DKIMHeaderCanonicalizationMethod = eDKIMCanonicalizationMethod.eCanonicalizationSimple;
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage();
         Assert.IsTrue(result.ToLower().Contains("simple/simple"), result);
      }

      [Test]
      [Description("Test that the default canonicalization method is relaxed/relaxed.")]
      public void TestDefaultAlgorithm()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage();
         Assert.IsTrue(result.ToLower().Contains("a=rsa-sha256"), result);
      }

      [Test]
      [Description("Test that the default canonicalization method is relaxed/relaxed.")]
      public void TestDefaultCanonicalizationMethod()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage();
         Assert.IsTrue(result.ToLower().Contains("relaxed/relaxed"), result);
      }

      [Test]
      [Description("Test that senders domain is specified in the d=tag.")]
      public void TestDomainInHeader()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage();
         Assert.IsTrue(result.ToLower().Contains("dkim-signature"), result);
         Assert.IsTrue(result.ToLower().Contains("d=" + _domain.Name.ToLower()), result);
      }

      [Test]
      [Description("Test that the selector specified in the domain is specified in the header.")]
      public void TestSelectorInHeader()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "MySelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage();
         Assert.IsTrue(result.ToLower().Contains("dkim-signature"), result);
         Assert.IsTrue(result.Contains("s=MySelector"), result);
      }


      [Test]
      [Description("Test to DKIM sign an empty body.")]
      public void TestSignSingleEmptyBody()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage("");
         Assert.IsTrue(result.Contains("bh=47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU=;"), result);
      }

      [Test]
      [Description("Test to DKIM sign an empty body.")]
      public void TestSignSingleFewWordBody()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage("Whati\r\nwhati\r\n\r\nwhati\r\n\r\n");
         Assert.IsTrue(result.Contains("bh=HfVBKUbHPvnfdY6y9RCu4IDyM+v+0HkekU0RPi3bgCk=;"), result);
      }

      [Test]
      [Description("Test to DKIM sign an empty body.")]
      public void TestSignSingleWordBody()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage("Test");
         Assert.IsTrue(result.Contains("bh=fdkeB/A0FkbVP2k4J4pNPoeWH6vqBm9+b0C3OY87Cw8=;"), result);
      }

      [Test]
      [Description("Test that a message sent through a new domain is not tagged.")]
      public void TestSigningDisabled()
      {
         var result = SendMessage();
         Assert.IsFalse(result.ToLower().Contains("dkim-signature"), result);
      }

      [Test]
      [Description("Test that a dkim-sigature is added if both the selector and private key is specified.")]
      public void TestSigningEnabled()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage();
         Assert.IsTrue(result.ToLower().Contains("dkim-signature"), result);
      }

      [Test]
      [Description("Test that no dkim-sigature is added if the private key is not specified.")]
      public void TestSigningEnabledNoPrivateKey()
      {
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage();
         Assert.IsFalse(result.ToLower().Contains("dkim-signature"), result);

         CustomAsserts.AssertReportedError("Either the selector or private key file was not specified.");
      }

      [Test]
      [Description("Test that no dkim-sigature is added if the selector is not specified.")]
      public void TestSigningEnabledNoSelector()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var result = SendMessage();
         Assert.IsFalse(result.ToLower().Contains("dkim-signature"), result);

         CustomAsserts.AssertReportedError("Either the selector or private key file was not specified.");
      }

      [Test]
      [Description("When RewriteEnvelopeFromWhenForwarding is enabled and a message is forwarded " +
                   "between two different local domains, the forwarded message should carry a DKIM " +
                   "signature from the original sender's domain before the envelope-From is rewritten " +
                   "(GitHub #511).")]
      public void WhenForwardingBetweenLocalDomains_WithRewriteEnabled_ForwardedMessageShouldBeDKIMSigned()
      {
         // _domain is "example.test" — configure it with DKIM signing.
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         // Add a second domain for the forwarding account. No DKIM is configured on this domain.
         var forwarderDomain = SingletonProvider<TestSetup>.Instance.AddDomain("other.test");

         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");

         var forwarder = SingletonProvider<TestSetup>.Instance.AddAccount(forwarderDomain, "forwarder@other.test", "test");

         var port = TestSetup.GetNextFreePort();
         using (var smtpServer = new SmtpServerSimulator(1, port))
         {
            smtpServer.SecondsToWaitBeforeTerminate = 60;
            smtpServer.AddRecipientResult(new Dictionary<string, int> { { "external@example.com", 250 } });
            smtpServer.StartListen();

            AddRoutePointingAtLocalhost(5, port);

            forwarder.ForwardEnabled = true;
            forwarder.ForwardAddress = "external@example.com";
            forwarder.ForwardKeepOriginal = false;
            forwarder.Save();

            // Enable envelope-From rewriting so the forwarded copy's sender becomes forwarder@other.test.
            // Without the fix this rewrite causes ExternalDelivery to attempt signing with other.test,
            // which has no DKIM key, so no signature is added. The test will therefore FAIL until
            // SMTPForwarding.cpp is updated to sign with the original sender's domain key first.
            _settings.RewriteEnvelopeFromWhenForwarding = true;
            try
            {
               var smtp = new SmtpClientSimulator();
               smtp.Send("sender@example.test", "forwarder@other.test", "Test subject", "Test body");

               CustomAsserts.AssertRecipientsInDeliveryQueue(0);
               smtpServer.WaitForCompletion();

               var messageData = smtpServer.MessageData;

               Assert.IsTrue(messageData.ToLower().Contains("dkim-signature"),
                  "Expected a DKIM-Signature header in the forwarded message but none was found.\r\n" + messageData);
               Assert.IsTrue(messageData.ToLower().Contains("d=example.test"),
                  "Expected DKIM-Signature with d=example.test (original sender domain) but it was not found.\r\n" + messageData);
            }
            finally
            {
               _settings.RewriteEnvelopeFromWhenForwarding = false;
            }
         }
      }

      [Test]
      [Description("In a two-hop forwarding chain where the first and last forwarder share the same " +
                   "DKIM-enabled domain (sender@a → forwarder_b@b → forwarder_a@a → external), " +
                   "the message must carry exactly one DKIM-Signature for the From: header domain. " +
                   "The intermediate domain b must not sign because the From: header belongs to domain a, " +
                   "and domain a must not sign a second time when the message revisits it (GitHub #511).")]
      public void WhenForwardingChainRevisitsDomain_DeliveredMessageShouldNotHaveDuplicateDKIMSignatureForThatDomain()
      {
         // _domain is "example.test" — configure it with DKIM signing.
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         // Intermediate domain that also has DKIM signing enabled.
         var intermediateDomain = SingletonProvider<TestSetup>.Instance.AddDomain("other.test");
         intermediateDomain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         intermediateDomain.DKIMSelector = "TestSelector";
         intermediateDomain.DKIMSignEnabled = true;
         intermediateDomain.Save();

         // Three accounts: sender and final-forwarder both on example.test;
         // intermediate forwarder on other.test.
         SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "sender@example.test", "test");
         var forwarderA = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "forwarder.a@example.test", "test");
         var forwarderB = SingletonProvider<TestSetup>.Instance.AddAccount(intermediateDomain, "forwarder.b@other.test", "test");

         var port = TestSetup.GetNextFreePort();
         using (var smtpServer = new SmtpServerSimulator(1, port))
         {
            smtpServer.SecondsToWaitBeforeTerminate = 60;
            smtpServer.AddRecipientResult(new Dictionary<string, int> { { "external@example.com", 250 } });
            smtpServer.StartListen();

            AddRoutePointingAtLocalhost(5, port);

            // Chain: forwarder.b@other.test → forwarder.a@example.test → external@example.com
            forwarderB.ForwardEnabled = true;
            forwarderB.ForwardAddress = "forwarder.a@example.test";
            forwarderB.ForwardKeepOriginal = false;
            forwarderB.Save();

            forwarderA.ForwardEnabled = true;
            forwarderA.ForwardAddress = "external@example.com";
            forwarderA.ForwardKeepOriginal = false;
            forwarderA.Save();

            _settings.RewriteEnvelopeFromWhenForwarding = true;
            try
            {
               // Send: sender@example.test → forwarder.b@other.test → forwarder.a@example.test → external
               var smtp = new SmtpClientSimulator();
               smtp.Send("sender@example.test", "forwarder.b@other.test", "Test subject", "Test body");

               CustomAsserts.AssertRecipientsInDeliveryQueue(0);
               smtpServer.WaitForCompletion();

               var messageData = smtpServer.MessageData;

               // Signing is driven by the RFC 5322 From: header domain, which is example.test
               // throughout the entire chain. other.test is only an intermediate relay and does
               // not own the From: domain, so it must not add a DKIM signature. example.test
               // signs on the first hop and the duplicate check prevents it from signing again
               // when the message revisits example.test on the second hop.
               int signatureCount = Regex.Matches(messageData, "DKIM-Signature", RegexOptions.IgnoreCase).Count;
               Assert.AreEqual(1, signatureCount,
                  $"Expected exactly 1 DKIM-Signature header (for the From: domain) but found {signatureCount}.\r\n" + messageData);

               int exampleTestCount = Regex.Matches(messageData, @"d=example\.test", RegexOptions.IgnoreCase).Count;
               Assert.AreEqual(1, exampleTestCount,
                  $"Expected d=example.test to appear exactly once but found {exampleTestCount}.\r\n" + messageData);

               int otherTestCount = Regex.Matches(messageData, @"d=other\.test", RegexOptions.IgnoreCase).Count;
               Assert.AreEqual(0, otherTestCount,
                  $"Expected d=other.test to not appear (other.test does not own the From: domain) but found {otherTestCount}.\r\n" + messageData);
            }
            finally
            {
               _settings.RewriteEnvelopeFromWhenForwarding = false;
            }
         }
      }

      [Test]
      [Description("When DKIM signing is enabled on the sender's domain, a message delivered to a " +
                   "local recipient should carry a DKIM-Signature header. " +
                   "This fails until signing is moved into SMTPDeliverer::PreprocessMessage_.")]
      public void WhenSigningEnabled_LocalRecipientShouldReceiveDKIMSignedMessage()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@example.test", "test");

         SmtpClientSimulator.StaticSend("sender@example.test", account.Address, "Test subject", "Test body");

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 1);

         var imap = new ImapClientSimulator();
         imap.ConnectAndLogon(account.Address, "test");
         imap.SelectFolder("Inbox");
         var messageData = imap.Fetch("1 RFC822");

         Assert.IsTrue(messageData.ToLower().Contains("dkim-signature"),
            "Expected a DKIM-Signature header in the locally-delivered message but none was found.\r\n" + messageData);
         Assert.IsTrue(messageData.ToLower().Contains("d=example.test"),
            "Expected DKIM-Signature with d=example.test but it was not found.\r\n" + messageData);
      }

      [Test]
      [Description("When DKIM signing is enabled and a message is delivered to both a local and an " +
                   "external recipient, the locally-stored copy should also carry a DKIM-Signature header. " +
                   "This fails until signing is moved into SMTPDeliverer::PreprocessMessage_.")]
      public void WhenSendingToBothLocalAndExternalRecipients_LocalCopyShouldBeDKIMSigned()
      {
         _domain.DKIMPrivateKeyFile = GetPrivateKeyFile();
         _domain.DKIMSelector = "TestSelector";
         _domain.DKIMSignEnabled = true;
         _domain.Save();

         var localAccount = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "local@example.test", "test");

         var port = TestSetup.GetNextFreePort();
         using (var smtpServer = new SmtpServerSimulator(1, port))
         {
            smtpServer.SecondsToWaitBeforeTerminate = 60;
            smtpServer.AddRecipientResult(new Dictionary<string, int> { { "external@example.com", 250 } });
            smtpServer.StartListen();

            AddRoutePointingAtLocalhost(5, port);

            var smtp = new SmtpClientSimulator();
            smtp.Send("sender@example.test", new List<string> { localAccount.Address, "external@example.com" },
               "Test subject", "Test body");

            smtpServer.WaitForCompletion();

            // External copy should be signed (already passes before the refactor).
            var externalData = smtpServer.MessageData;
            Assert.IsTrue(externalData.ToLower().Contains("dkim-signature"),
               "Expected a DKIM-Signature in the externally-delivered message.\r\n" + externalData);

            // Local copy must also be signed — currently fails because LocalDelivery does not sign.
            ImapClientSimulator.AssertMessageCount(localAccount.Address, "test", "Inbox", 1);

            var imap = new ImapClientSimulator();
            imap.ConnectAndLogon(localAccount.Address, "test");
            imap.SelectFolder("Inbox");
            var localData = imap.Fetch("1 RFC822");

            Assert.IsTrue(localData.ToLower().Contains("dkim-signature"),
               "Expected a DKIM-Signature header in the locally-delivered message but none was found.\r\n" + localData);
         }
      }
   }
}