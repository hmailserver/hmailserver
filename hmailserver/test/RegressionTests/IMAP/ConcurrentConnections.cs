using NUnit.Framework;
using RegressionTests.Shared;

namespace RegressionTests.IMAP
{
   [TestFixture]
   public class ConcurrentConnections : TestFixtureBase
   {
      [Test]
      [Description("Bug: SendCachedNotifications passed lastExists to SendRECENT_ instead of lastRecent")]
      public void NoopRecentCountReflectsRecentMessagesNotExistsCount()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // SELECT an empty inbox; the connection's recent-messages set starts empty (size=0).
         var sim = new ImapClientSimulator();
         sim.ConnectAndLogon(account.Address, "test");
         sim.SelectFolder("INBOX");

         // Deliver two messages after SELECT. They are not added to this connection's
         // recent-messages set, so lastRecent remains 0 while lastExists becomes 2.
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test 1", "Body 1");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test 2", "Body 2");

         // Wait for delivery without opening a second IMAP SELECT (which would claim \Recent).
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 2);

         // NOOP flushes cached notifications. EXISTS must be 2, RECENT must be 0.
         var response = sim.NOOP();
         Assert.IsTrue(response.Contains("* 2 EXISTS"), response);
         Assert.IsTrue(response.Contains("* 0 RECENT"), response);

         sim.Disconnect();
      }


      [Test]
      public void ChangingFlagShouldAffectAllConnections()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");
         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Test", "Test");

         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 1);

         var sim1 = new ImapClientSimulator();
         Assert.IsTrue(sim1.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(sim1.SelectFolder("Inbox"));

         var sim2 = new ImapClientSimulator();
         Assert.IsTrue(sim2.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(sim2.SelectFolder("Inbox"));

         sim1.SetFlagOnMessage(1, true, "\\Deleted");

         var flags1 = sim1.GetFlags(1);
         var flags2 = sim2.GetFlags(1);

         Assert.IsTrue(flags2.Contains(@"* 1 FETCH (FLAGS (\Deleted))"), flags2);
         Assert.IsTrue(flags2.Contains(@"* 1 FETCH (FLAGS (\Deleted) UID 1)"), flags2);
      }

      /// <summary>
      /// Delivers the given messages to a new account and returns two IMAP connections
      /// which both have the inbox selected.
      /// </summary>
      private void SetupSharedMailbox(string[] subjects, out ImapClientSimulator sim1,
                                      out ImapClientSimulator sim2)
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         // Deliver the messages one at a time, to make sure they end up in the expected order.
         for (var i = 0; i < subjects.Length; i++)
         {
            SmtpClientSimulator.StaticSend(account.Address, account.Address, subjects[i], "Body of " + subjects[i]);
            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", i + 1);
         }

         sim1 = new ImapClientSimulator();
         Assert.IsTrue(sim1.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(sim1.SelectFolder("INBOX"));

         sim2 = new ImapClientSimulator();
         Assert.IsTrue(sim2.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(sim2.SelectFolder("INBOX"));
      }

      [Test]
      [Description("Issue 458: Fetching by message sequence number returned another message than the " +
                   "client asked for, if another client had expunged a message.")]
      public void FetchBySequenceNumberShouldFailIfOtherClientHasExpunged()
      {
         ImapClientSimulator sim1;
         ImapClientSimulator sim2;

         SetupSharedMailbox(new[] {"Message A", "Message B"}, out sim1, out sim2);

         // The second client expunges the first message. The first client can't be told
         // about this yet, since it hasn't issued any command since the expunge.
         Assert.IsTrue(sim2.SetDeletedFlag(1));
         Assert.IsTrue(sim2.Expunge());

         // As far as the first client knows, message 1 is still Message A. Rather than
         // giving it the content of Message B, we should tell it to resynchronize.
         var result = sim1.Fetch("1 BODY[]");
         Assert.IsTrue(result.Contains("A17 NO [EXPUNGEISSUED]"), result);
         Assert.IsFalse(result.Contains("Message B"), result);

         // After the client has been told about the expunge, the sequence numbers are
         // in sync again and message 1 is Message B.
         // The notification may come after the first NOOP response stream, so do noop twice.
         var noopResult = sim1.NOOP() + sim1.NOOP();
         Assert.IsTrue(noopResult.Contains("* 1 EXPUNGE"), noopResult);
         Assert.IsTrue(noopResult.Contains("* 1 EXISTS"), noopResult);

         result = sim1.Fetch("1 BODY[]");
         Assert.IsTrue(result.Contains("A17 OK"), result);
         Assert.IsTrue(result.Contains("Message B"), result);
      }

      [Test]
      [Description("Issue 458: Only messages after the expunged one have unreliable sequence numbers.")]
      public void FetchBySequenceNumberShouldSucceedForMessagesBeforeExpungedMessage()
      {
         ImapClientSimulator sim1;
         ImapClientSimulator sim2;

         SetupSharedMailbox(new[] {"Message A", "Message B", "Message C"}, out sim1, out sim2);

         // The second client expunges the last message.
         Assert.IsTrue(sim2.SetDeletedFlag(3));
         Assert.IsTrue(sim2.Expunge());

         // Message 1 and 2 are unaffected by the expunge, so fetching them is safe.
         var result = sim1.Fetch("1:2 BODY[]");
         Assert.IsTrue(result.Contains("A17 OK"), result);
         Assert.IsTrue(result.Contains("Message A"), result);
         Assert.IsTrue(result.Contains("Message B"), result);

         // Message 3 no longer refers to the message the client believes.
         result = sim1.Fetch("3 BODY[]");
         Assert.IsTrue(result.Contains("A17 NO [EXPUNGEISSUED]"), result);
      }

      [Test]
      [Description("Issue 458: Storing flags by message sequence number could affect another message " +
                   "than the client asked for.")]
      public void StoreBySequenceNumberShouldFailIfOtherClientHasExpunged()
      {
         ImapClientSimulator sim1;
         ImapClientSimulator sim2;

         SetupSharedMailbox(new[] {"Message A", "Message B"}, out sim1, out sim2);

         Assert.IsTrue(sim2.SetDeletedFlag(1));
         Assert.IsTrue(sim2.Expunge());

         var result = sim1.SendSingleCommand("A01 STORE 1 +FLAGS (\\Seen)");
         Assert.IsTrue(result.Contains("A01 NO [EXPUNGEISSUED]"), result);

         // Message B should not have been flagged.
         Assert.IsTrue((sim1.NOOP() + sim1.NOOP()).Contains("* 1 EXPUNGE"));

         var flags = sim1.GetFlags(1);
         Assert.IsFalse(flags.Contains("\\Seen"), flags);
      }

      [Test]
      [Description("Issue 458: SEARCH returns message sequence numbers, which the client would " +
                   "translate into the wrong messages.")]
      public void SearchShouldFailIfOtherClientHasExpunged()
      {
         ImapClientSimulator sim1;
         ImapClientSimulator sim2;

         SetupSharedMailbox(new[] {"Message A", "Message B"}, out sim1, out sim2);

         Assert.IsTrue(sim2.SetDeletedFlag(1));
         Assert.IsTrue(sim2.Expunge());

         var result = sim1.SendSingleCommand("A02 SEARCH ALL");
         Assert.IsTrue(result.Contains("A02 NO [EXPUNGEISSUED]"), result);

         // UID SEARCH is not affected, since UID's remain stable. Issuing it also
         // tells the client about the expunge.
         result = sim1.SendSingleCommand("A03 UID SEARCH ALL");
         Assert.IsTrue(result.Contains("* 1 EXPUNGE"), result);
         Assert.IsTrue(result.Contains("A03 OK"), result);

         result = sim1.SendSingleCommand("A04 SEARCH ALL");
         Assert.IsTrue(result.Contains("A04 OK"), result);
      }

      [Test]
      [Description("Issue 458: UID FETCH operates on unique identifiers and should not be affected.")]
      public void UidFetchShouldSucceedIfOtherClientHasExpunged()
      {
         ImapClientSimulator sim1;
         ImapClientSimulator sim2;

         SetupSharedMailbox(new[] {"Message A", "Message B"}, out sim1, out sim2);

         Assert.IsTrue(sim2.SetDeletedFlag(1));
         Assert.IsTrue(sim2.Expunge());

         // Message B has UID 2. Fetching it by UID gives the client the correct message.
         var result = sim1.SendSingleCommand("A05 UID FETCH 2 BODY[]");
         Assert.IsTrue(result.Contains("A05 OK"), result);
         Assert.IsTrue(result.Contains("Message B"), result);
      }
   }
}
