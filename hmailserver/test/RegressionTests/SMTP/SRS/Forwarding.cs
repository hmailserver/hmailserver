using System;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SMTP.SRS
{
   [TestFixture]
   public class Forwarding : SrsTestBase
   {
      [Test]
      [Description("The envelope sender of a forwarded message is rewritten into the forwarding account's domain.")]
      public void ForwardingToAnotherServerRewritesTheSender()
      {
         EnableSrs();

         AddForwardingAccount("forwarder@example.test", ForwardTarget);

         using (var server = StartExternalServer(1, ForwardTarget))
         {
            var sender = GetSenderOfForwardedMessage(server, ExternalSender, "forwarder@example.test");

            var address = SrsAddress.Parse(sender);

            Assert.AreEqual("SRS0", address.Tag);

            // The message now comes from a domain we host, which is what makes it pass
            // the SPF check at the receiving server.
            Assert.AreEqual(_domain.Name, address.ForwardingDomain);

            // And the address it came from is still in there, so that a bounce can be
            // passed on to whoever sent the message.
            Assert.AreEqual(ExternalSender, address.OriginalAddress);

            Assert.AreEqual(SrsAddress.CreateTimestamp(DateTime.UtcNow), address.Timestamp);
            Assert.AreEqual(SrsAddress.DefaultHashLength, address.Hash.Length);
            Assert.IsTrue(address.HasValidHash(Secret), "The address was not signed with the server's SRS secret.");

            // The whole address, built here rather than by the server, matches what the
            // server produced.
            Assert.AreEqual(SrsAddress.Create(Secret, ExternalSender, _domain.Name), sender);
         }
      }

      [Test]
      [Description("The sender is left alone when the message is not leaving the server.")]
      public void ForwardingToALocalAccountDoesNotRewriteTheSender()
      {
         EnableSrs();

         var recipient = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "recipient@example.test", "test");

         AddForwardingAccount("forwarder@example.test", recipient.Address);

         SmtpClientSimulator.StaticSend(ExternalSender, "forwarder@example.test", "Forwarded message", "This is the body");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         var message = Pop3ClientSimulator.AssertGetFirstMessageText(recipient.Address, "test");

         Assert.IsTrue(message.Contains("Return-Path: <" + ExternalSender + ">"), message);
      }

      [Test]
      [Description("The sender is left alone when we are the sender's own mail server.")]
      public void ForwardingAMessageFromOneOfOurOwnDomainsDoesNotRewriteTheSender()
      {
         EnableSrs();

         var sender = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "localsender@example.test", "test");

         AddForwardingAccount("forwarder@example.test", ForwardTarget);

         using (var server = StartExternalServer(1, ForwardTarget))
         {
            // Our own domain already lists this server as a permitted sender, so there is
            // nothing to fix, and leaving the address alone keeps the message aligned
            // with its From header for DMARC.
            Assert.AreEqual(sender.Address,
               GetSenderOfForwardedMessage(server, sender.Address, "forwarder@example.test"));
         }
      }

      [Test]
      [Description("A bounce is forwarded with the empty sender it arrived with.")]
      public void AnEmptySenderIsNotRewritten()
      {
         EnableSrs();

         _settings.DenyMailFromNull = false;

         AddForwardingAccount("forwarder@example.test", ForwardTarget);

         using (var server = StartExternalServer(1, ForwardTarget))
         {
            Assert.AreEqual("", GetSenderOfForwardedMessage(server, "", "forwarder@example.test"));
         }
      }

      [Test]
      [Description("Nothing is rewritten while SRS is switched off.")]
      public void TheSenderIsNotRewrittenWhenSrsIsDisabled()
      {
         Assert.IsFalse(_settings.SRSEnabled);

         AddForwardingAccount("forwarder@example.test", ForwardTarget);

         using (var server = StartExternalServer(1, ForwardTarget))
         {
            Assert.AreEqual(ExternalSender,
               GetSenderOfForwardedMessage(server, ExternalSender, "forwarder@example.test"));
         }
      }

      [Test]
      [Description("A message forwarded by an account rule is rewritten in the same way.")]
      public void RuleForwardingRewritesTheSender()
      {
         EnableSrs();

         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "ruleforwarder@example.test", "test");

         var rule = account.Rules.Add();
         rule.Name = "Forward";
         rule.Active = true;

         var criteria = rule.Criterias.Add();
         criteria.UsePredefined = true;
         criteria.PredefinedField = eRulePredefinedField.eFTMessageSize;
         criteria.MatchType = eRuleMatchType.eMTGreaterThan;
         criteria.MatchValue = "0";
         criteria.Save();

         var action = rule.Actions.Add();
         action.Type = eRuleActionType.eRAForwardEmail;
         action.To = ForwardTarget;
         action.Save();

         rule.Save();

         using (var server = StartExternalServer(1, ForwardTarget))
         {
            var sender = GetSenderOfForwardedMessage(server, ExternalSender, account.Address);

            Assert.AreEqual(SrsAddress.Create(Secret, ExternalSender, _domain.Name), sender);
         }
      }

      [Test]
      [Description("A sender another server has already rewritten is chained rather than wrapped.")]
      public void AnAlreadyRewrittenSenderIsChained()
      {
         EnableSrs();

         AddForwardingAccount("forwarder@example.test", ForwardTarget);

         // The message reaches us from a server which forwarded it before we did, and
         // rewrote the sender when it did so. Its secret is not ours, and does not have
         // to be: the first hop is the one which will have to reverse that part.
         var firstHop = SrsAddress.Create("the other server's secret", ExternalSender, "hop1.example");

         using (var server = StartExternalServer(1, ForwardTarget))
         {
            var sender = GetSenderOfForwardedMessage(server, firstHop, "forwarder@example.test");

            var address = SrsAddress.Parse(sender);

            Assert.AreEqual("SRS1", address.Tag);

            // The first hop is kept as it is, so that a bounce goes back the way the
            // message came rather than the address growing with every hop.
            Assert.AreEqual("hop1.example", address.Domain);
            Assert.AreEqual(_domain.Name, address.ForwardingDomain);
            Assert.IsTrue(address.HasValidHash(Secret), "The address was not signed with the server's SRS secret.");

            Assert.AreEqual(SrsAddress.CreateChained(Secret, firstHop, _domain.Name, SrsAddress.DefaultHashLength), sender);
         }
      }

      [Test]
      [Description("An address which would grow past what a message can carry is left alone.")]
      public void AnAddressWhichWouldBecomeTooLongIsNotRewritten()
      {
         EnableSrs();

         AddForwardingAccount("forwarder@example.test", ForwardTarget);

         var longSender = new string('a', 240) + "@" + ExternalDomain;

         using (var server = StartExternalServer(1, ForwardTarget))
         {
            // Rewriting it would produce an address longer than the 255 characters a
            // message can carry, so the message is forwarded as it is rather than with
            // an address which cannot be delivered or stored.
            Assert.AreEqual(longSender, GetSenderOfForwardedMessage(server, longSender, "forwarder@example.test"));
         }
      }

      [Test]
      [Description("SRS takes precedence over RewriteEnvelopeFromWhenForwarding where it rewrites a sender.")]
      public void SrsTakesPrecedenceOverTheLegacyRewriteSetting()
      {
         EnableSrs();

         _settings.RewriteEnvelopeFromWhenForwarding = true;

         var forwarder = AddForwardingAccount("forwarder@example.test", ForwardTarget);

         using (var server = StartExternalServer(1, ForwardTarget))
         {
            // The older setting would have put the forwarding account here, leaving a
            // bounce with the person who set the forwarding up. SRS wins, and the
            // address it creates leads back to whoever wrote the message.
            Assert.AreEqual(SrsAddress.Create(Secret, ExternalSender, _domain.Name),
               GetSenderOfForwardedMessage(server, ExternalSender, forwarder.Address));
         }
      }

      [Test]
      [Description("RewriteEnvelopeFromWhenForwarding still applies where SRS deliberately leaves the sender alone.")]
      public void TheLegacyRewriteSettingStillAppliesWhereSrsDoesNot()
      {
         EnableSrs();

         _settings.RewriteEnvelopeFromWhenForwarding = true;

         var forwarder = AddForwardingAccount("forwarder@example.test", ForwardTarget);
         var localSender = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "localsender@example.test", "test");

         using (var server = StartExternalServer(1, ForwardTarget))
         {
            // SRS does not rewrite a sender in one of our own domains, so the message is
            // forwarded the way it was before SRS existed.
            Assert.AreEqual(forwarder.Address,
               GetSenderOfForwardedMessage(server, localSender.Address, forwarder.Address));
         }
      }

      [Test]
      [Description("The number of hash characters in an address can be configured.")]
      public void TheHashLengthIsConfigurable()
      {
         EnableSrs();

         _settings.SRSHashLength = 4;

         AddForwardingAccount("forwarder@example.test", ForwardTarget);

         using (var server = StartExternalServer(1, ForwardTarget))
         {
            var sender = GetSenderOfForwardedMessage(server, ExternalSender, "forwarder@example.test");

            var address = SrsAddress.Parse(sender);

            Assert.AreEqual(4, address.Hash.Length);
            Assert.IsTrue(address.HasValidHash(Secret), "The address was not signed with the server's SRS secret.");
         }
      }
   }
}
