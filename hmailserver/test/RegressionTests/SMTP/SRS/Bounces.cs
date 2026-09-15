using System;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SMTP.SRS
{
   /// <summary>
   /// The other half of SRS: a bounce sent back to an address hMailServer handed out when
   /// it forwarded a message has to reach whoever wrote that message - and an address
   /// somebody made up has to reach nobody at all.
   /// </summary>
   [TestFixture]
   public class Bounces : SrsTestBase
   {
      [Test]
      [Description("A bounce sent to a rewritten address is passed on to the original sender.")]
      public void ABounceIsPassedOnToTheOriginalSender()
      {
         EnableSrs();

         _settings.DenyMailFromNull = false;

         var bounceAddress = SrsAddress.Create(Secret, ExternalSender, _domain.Name);

         using (var server = StartExternalServer(1, ExternalSender))
         {
            SmtpClientSimulator.StaticSend("", bounceAddress, "Undelivered mail", "The message could not be delivered.");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            // The bounce leaves here addressed to the person who wrote the message that
            // was forwarded, rather than to the account which forwarded it.
            Assert.AreEqual(1, server.RcptTos.Count);
            Assert.AreEqual(ExternalSender, server.RcptTos[0]);
            Assert.AreEqual("", server.MailFrom);
            Assert.IsTrue(server.MessageData.Contains("The message could not be delivered."));
         }
      }

      [Test]
      [Description("A message forwarded by the server can be bounced back to it, and reaches the original sender.")]
      public void AnAddressTheServerHandedOutCanBeBouncedTo()
      {
         EnableSrs();

         _settings.DenyMailFromNull = false;

         AddForwardingAccount("forwarder@example.test", ForwardTarget);

         using (var server = StartExternalServer(2, ForwardTarget, ExternalSender))
         {
            SmtpClientSimulator.StaticSend(ExternalSender, "forwarder@example.test", "Forwarded message", "This is the body");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            // Whatever the receiving server was told to bounce to, we send a bounce to.
            Assert.IsTrue(Poll.Until(TimeSpan.FromSeconds(20), () => server.MailFrom.StartsWith("SRS0=")),
               "The forwarded message did not have a rewritten sender: " + server.MailFrom);

            var bounceAddress = server.MailFrom;

            SmtpClientSimulator.StaticSend("", bounceAddress, "Undelivered mail", "The message could not be delivered.");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.AreEqual(2, server.RcptTos.Count);
            Assert.AreEqual(ForwardTarget, server.RcptTos[0]);
            Assert.AreEqual(ExternalSender, server.RcptTos[1]);
         }
      }

      [Test]
      [Description("An address which is not signed with the server's secret is rejected.")]
      public void AForgedAddressIsRejected()
      {
         EnableSrs();

         // Anyone can work out what an address which relays to a recipient of their
         // choosing would look like. Without the secret they cannot sign it, and this is
         // what keeps SRS from turning the server into an open relay.
         var forged = SrsAddress.Create("not the server's secret", "victim@" + ExternalDomain, _domain.Name);

         AssertRejected(forged, "The SRS address has an invalid hash.");
      }

      [Test]
      [Description("An address whose hash has been altered is rejected.")]
      public void AnAlteredAddressIsRejected()
      {
         EnableSrs();

         var address = SrsAddress.Create(Secret, ExternalSender, _domain.Name);

         // The recipient the address decodes to is changed, and the hash no longer
         // matches what the rest of the address says.
         AssertRejected(address.Replace("=sender@", "=victim@"), "The SRS address has an invalid hash.");

         // As is the reverse: a valid hash for another address.
         var otherAddress = SrsAddress.Parse(SrsAddress.Create(Secret, "someone@" + ExternalDomain, _domain.Name));
         var parsed = SrsAddress.Parse(address);

         AssertRejected("SRS0=" + otherAddress.Hash + "=" + parsed.Timestamp + "=" + parsed.Domain + "=" +
                        parsed.LocalPart + "@" + _domain.Name, "The SRS address has an invalid hash.");
      }

      [Test]
      [Description("An address which has passed its validity period is rejected.")]
      public void AnExpiredAddressIsRejected()
      {
         EnableSrs();

         var expired = SrsAddress.Create(Secret, ExternalSender, _domain.Name,
            DateTime.UtcNow.AddDays(-(SrsAddress.DefaultMaxAgeDays + 1)), SrsAddress.DefaultHashLength);

         AssertRejected(expired, "The SRS address has expired.");
      }

      [Test]
      [Description("An address is valid for as many days as the setting says, and no longer.")]
      public void TheValidityPeriodIsConfigurable()
      {
         EnableSrs();

         _settings.SRSMaxAgeDays = 3;

         // The timestamp counts whole days, and the server allows for a clock which runs a
         // little behind the one of the server that created the address, so an age is only
         // ever accurate to a day. These two are a day clear of the limit on either side,
         // so that the test says the same thing whatever time of day it runs at.
         var insideTheLimit = SrsAddress.Create(Secret, ExternalSender, _domain.Name,
            DateTime.UtcNow.AddDays(-2), SrsAddress.DefaultHashLength);

         var outsideTheLimit = SrsAddress.Create(Secret, ExternalSender, _domain.Name,
            DateTime.UtcNow.AddDays(-5), SrsAddress.DefaultHashLength);

         AssertAccepted(insideTheLimit);
         AssertRejected(outsideTheLimit, "The SRS address has expired.");
      }

      [Test]
      [Description("Changing the hash length keeps the addresses already handed out reversible.")]
      public void AddressesSurviveAChangeOfHashLength()
      {
         EnableSrs();

         // Bounces keep arriving for addresses handed out under the previous setting for
         // as long as those addresses are valid, so a hash of either length is accepted.
         var shortHashAddress = SrsAddress.Create(Secret, ExternalSender, _domain.Name, DateTime.UtcNow,
            SrsAddress.MinHashLength);

         var longHashAddress = SrsAddress.Create(Secret, ExternalSender, _domain.Name, DateTime.UtcNow,
            SrsAddress.MaxHashLength);

         _settings.SRSHashLength = SrsAddress.MaxHashLength;
         AssertAccepted(shortHashAddress);

         _settings.SRSHashLength = SrsAddress.MinHashLength;
         AssertAccepted(longHashAddress);
      }

      [Test]
      [Description("An address carrying fewer hash characters than the minimum is rejected.")]
      public void AnAddressWithATooShortHashIsRejected()
      {
         EnableSrs();

         // How many characters of hash an address carries is the sender's choice, so
         // comparing a short hash as far as it goes would leave one cheap to guess at.
         var parsed = SrsAddress.Parse(SrsAddress.Create(Secret, ExternalSender, _domain.Name));

         var truncated = "SRS0=" + parsed.Hash.Substring(0, SrsAddress.MinHashLength - 1) + "=" +
                         parsed.Timestamp + "=" + parsed.Domain + "=" + parsed.LocalPart + "@" + _domain.Name;

         AssertRejected(truncated, "The SRS address has an invalid hash.");
      }

      [Test]
      [Description("An address which looks like ours but does not hold the fields we write is rejected.")]
      public void AMalformedAddressIsRejected()
      {
         EnableSrs();

         AssertRejected("SRS0=hash=7G@" + _domain.Name, "The SRS address is malformed.");
         AssertRejected("SRS1=hash=hop.example@" + _domain.Name, "The SRS address is malformed.");
      }

      [Test]
      [Description("Mail servers which change the case of an address do not break it.")]
      public void TheAddressSurvivesHavingItsCaseChanged()
      {
         EnableSrs();

         var address = SrsAddress.Create(Secret, "Sender.Name@" + ExternalDomain, _domain.Name);

         using (var server = StartExternalServer(1, "sender.name@" + ExternalDomain))
         {
            SmtpClientSimulator.StaticSend("", address.ToLowerInvariant(),
               "Undelivered mail", "The message could not be delivered.");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.AreEqual("sender.name@" + ExternalDomain, server.RcptTos[0]);
         }
      }

      [Test]
      [Description("An address a second forwarding server created is reversed back to the first hop.")]
      public void AChainedAddressIsReversedToTheFirstHop()
      {
         EnableSrs();

         // We are the second hop: the address we handed out embeds the one the first hop
         // did, and a bounce goes back there for that server to make sense of.
         var firstHopAddress = SrsAddress.Create("the other server's secret", "writer@example.org", ExternalDomain);
         var ourAddress = SrsAddress.CreateChained(Secret, firstHopAddress, _domain.Name, SrsAddress.DefaultHashLength);

         using (var server = StartExternalServer(1, firstHopAddress))
         {
            SmtpClientSimulator.StaticSend("", ourAddress, "Undelivered mail",
               "The message could not be delivered.");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.AreEqual(firstHopAddress, server.RcptTos[0]);
         }
      }

      [Test]
      [Description("A chained address which is not signed with the server's secret is rejected.")]
      public void AForgedChainedAddressIsRejected()
      {
         EnableSrs();

         var firstHopAddress = SrsAddress.Create("the other server's secret", "writer@example.org", ExternalDomain);
         var forged = SrsAddress.CreateChained("not the server's secret", firstHopAddress, _domain.Name,
            SrsAddress.DefaultHashLength);

         AssertRejected(forged, "The SRS address has an invalid hash.");
      }

      [Test]
      [Description("With SRS switched off, an address which looks like one of ours is just an unknown address.")]
      public void AddressesAreNotReversedWhenSrsIsDisabled()
      {
         Assert.IsFalse(_settings.SRSEnabled);

         var address = SrsAddress.Create(Secret, ExternalSender, _domain.Name);

         AssertRejected(address, "Unknown user");
      }

      [Test]
      [Description("An address which cannot be reversed is treated as any other unknown address.")]
      public void AnAddressWhichCannotBeReversedReachesTheCatchAllAccount()
      {
         EnableSrs();

         // An address which does not reverse is not ours to deliver to the sender it
         // claims, but it is still an address in one of our domains, and the catch-all
         // account is where every other unknown address in the domain ends up.
         var postmaster = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "postmaster@example.test", "test");

         _domain.Postmaster = postmaster.Address;
         _domain.Save();

         SmtpClientSimulator.StaticSend("", SrsAddress.Create("not the server's secret", ExternalSender, _domain.Name),
            "Undelivered mail", "The message could not be delivered.");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);

         Pop3ClientSimulator.AssertMessageCount(postmaster.Address, "test", 1);
      }

      [Test]
      [Description("An account whose address looks like a rewritten one still gets its own mail.")]
      public void AnAccountNamedLikeARewrittenAddressKeepsItsMail()
      {
         EnableSrs();

         // SRS0 and SRS1 are not reserved words. The address is only reversed once nothing
         // in the domain answers to it, so switching SRS on does not make an account, an
         // alias or a distribution list which happens to be named this way unreachable.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "srs1-announce@example.test", "test");

         SmtpClientSimulator.StaticSend(ExternalSender, account.Address, "Hello", "This is the body");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);
      }

      [Test]
      [Description("A bounce which cannot be delivered because its address no longer reverses is reported.")]
      public void ABounceWhichCannotBeReversedAtDeliveryTimeIsReported()
      {
         EnableSrs();

         // The server writes an address like this into the envelope of every message it
         // forwards, and a bounce comes back to it. Once the secret behind it is gone - it
         // has been rotated, or the address has expired - there is nothing left to deliver
         // that bounce to, and it must not disappear without a word.
         var unreversibleSender = SrsAddress.Create("not the server's secret", ExternalSender, _domain.Name);

         using (var server = StartRejectingExternalServer(ForwardTarget))
         {
            SmtpClientSimulator.StaticSend(unreversibleSender, ForwardTarget, "Forwarded message", "This is the body");

            server.WaitForCompletion();

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            CustomAsserts.AssertReportedError("The SRS address", "could not be reversed");
         }
      }

      private void AssertRejected(string address, string expectedError)
      {
         var client = new SmtpClientSimulator();

         var exception = Assert.Throws<DeliveryFailedException>(
            () => client.Send("", address, "Undelivered mail", "The body"));

         Assert.IsTrue(exception.Message.Contains(expectedError),
            "Expected the delivery to fail with '" + expectedError + "', but it failed with: " + exception.Message);
      }

      private void AssertAccepted(string address)
      {
         using (var server = StartExternalServer(1, ExternalSender))
         {
            SmtpClientSimulator.StaticSend("", address, "Undelivered mail", "The body");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.AreEqual(ExternalSender, server.RcptTos[0]);
         }
      }
   }
}
