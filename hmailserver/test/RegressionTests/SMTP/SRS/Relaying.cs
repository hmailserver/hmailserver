using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.SMTP.SRS
{
   /// <summary>
   /// A rewritten address is an address in one of our own domains which mail sent to it
   /// leaves again for whoever wrote the message that was forwarded. That is what makes it
   /// worth attacking: these tests are about what stops it from being a way through the
   /// server for anyone who has come by one.
   /// </summary>
   [TestFixture]
   public class Relaying : SrsTestBase
   {
      [Test]
      [Description("A message with an envelope sender of its own is not relayed through a rewritten address.")]
      public void AMessageWhichIsNotABounceIsNotRelayed()
      {
         EnableSrs();

         // Everything about this address is real: it is signed with the server's own
         // secret, it is well inside its validity period, and a bounce sent to it would be
         // passed on. What makes this a relay attempt rather than a bounce is that the
         // message carries an envelope sender of its own.
         var address = SrsAddress.Create(Secret, ExternalSender, _domain.Name);

         var client = new SmtpClientSimulator();

         Assert.Throws<DeliveryFailedException>(
            () => client.Send("attacker@" + ExternalDomain, address, "Not a bounce", "Relayed through you"),
            "An external client was allowed to relay a message to the sender an SRS address was created for.");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
      }

      [Test]
      [Description("The same address does accept the bounce it exists for.")]
      public void ABounceToTheSameAddressIsAccepted()
      {
         EnableSrs();

         // The other half of the test above: what is refused is the message which is not a
         // bounce, not the address itself.
         var address = SrsAddress.Create(Secret, ExternalSender, _domain.Name);

         using (var server = StartExternalServer(1, ExternalSender))
         {
            SmtpClientSimulator.StaticSend("", address, "Undelivered mail", "The message could not be delivered.");

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.AreEqual(ExternalSender, server.RcptTos[0]);
         }
      }

      [Test]
      [Description("A hash the server issued for one form of address is not accepted as a hash over the other.")]
      public void AnSrs0SignatureIsNotAcceptedOnAnSrs1Address()
      {
         EnableSrs();

         // What the two forms of address hash is easy to make into the same string unless
         // the tag is part of it: one covers "<timestamp>=<domain>=<local part>", the other
         // "<first hop>" followed by an embedded SRS0 payload which opens with a separator
         // of its own. This sender's domain reads like a hash and its local part like the
         // tail of an SRS0 payload, so the address the server hands out for it carries a
         // signature which would otherwise carry over to an address it never created.
         var sender = "7g=example.org=victim@" + ExternalDomain;

         var rewritten = SrsAddress.Create(Secret, sender, _domain.Name);
         var parsed = SrsAddress.Parse(rewritten);

         Assert.AreEqual("SRS0", parsed.Tag);
         Assert.IsTrue(parsed.HasValidHash(Secret), "The address was not signed with the server's SRS secret.");

         var crafted = "SRS1=" + parsed.Hash + "=" + parsed.Timestamp + "==" + ExternalDomain +
                       "=7g=example.org=victim@" + _domain.Name;

         var client = new SmtpClientSimulator();

         Assert.Throws<DeliveryFailedException>(
            () => client.Send("", crafted, "Undelivered mail", "The message could not be delivered."),
            "An SRS1 address carrying the hash of an SRS0 address was accepted.");

         CustomAsserts.AssertRecipientsInDeliveryQueue(0);
      }

      [Test]
      [Description("A client which may relay through the server can still send to a rewritten address.")]
      public void AnAuthenticatedClientCanSendToARewrittenAddress()
      {
         EnableSrs();

         // Refusing the message above is the relaying rules doing their ordinary work, not
         // a rule of its own: a client which is allowed to relay is allowed to send here
         // as well.
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "user@example.test", "test");

         var address = SrsAddress.Create(Secret, ExternalSender, _domain.Name);

         using (var server = StartExternalServer(1, ExternalSender))
         {
            var client = new SmtpClientSimulator();

            string errorMessage;
            client.Send(false, account.Address, "test", account.Address, address, "A reply", "The body",
               out errorMessage);

            CustomAsserts.AssertRecipientsInDeliveryQueue(0);

            server.WaitForCompletion();

            Assert.AreEqual(ExternalSender, server.RcptTos[0]);
         }
      }
   }
}
