using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using hMailServer;
using NUnit.Framework;
using RegressionTests.Infrastructure;
using RegressionTests.Shared;

namespace RegressionTests.IMAP
{
   /// <summary>
   ///    Issue #458: message sequence numbers must be stable within an IMAP session. When another
   ///    session expunges a message, this session's numbering may only change at points where the
   ///    server is allowed to send an untagged EXPUNGE (RFC 3501 7.4.1, RFC 2180 chapter 4).
   /// </summary>
   [TestFixture]
   public class SessionMessageNumbering : TestFixtureBase
   {
      // Distinct in their first 12 bytes, so the base64 prefix of each is unique as well.
      private const string MarkerOne = "MARKER-ONE-1111";
      private const string MarkerTwo = "MARKER-TWO-2222";
      private const string MarkerThree = "MARKER-THREE-33";

      #region The reported defect

      [Test]
      [Description("Issue #458: FETCH of a body part returned another message's attachment after " +
                   "a concurrent expunge.")]
      public void FetchOfBodyPartMustNotReturnAnotherMessagesAttachment()
      {
         var account = CreateAccountWithThreeAttachmentMessages();

         var sim1 = SelectInbox(account);

         // Baseline: in this session, message 2 is the one carrying the second attachment.
         var before = sim1.Fetch("2 BODY[2]");
         AssertContainsAttachment(before, MarkerTwo);

         ExpungeMessageInSeparateSession(account, 1);

         // No command in between, so this session has not been told about the expunge.
         var after = sim1.Fetch("2 BODY[2]");

         AssertDoesNotContainAttachment(after, MarkerThree);
         AssertContainsAttachment(after, MarkerTwo);

         sim1.Disconnect();
      }

      [Test]
      [Description("Issue #458: the second half of the reported transcript - re-fetching the " +
                   "bodystructure showed a different message.")]
      public void FetchOfBodyStructureMustNotDescribeAnotherMessage()
      {
         var account = CreateAccountWithThreeAttachmentMessages();

         var sim1 = SelectInbox(account);

         var before = sim1.Fetch("2 BODYSTRUCTURE");
         Assert.IsTrue(before.Contains("two.txt"), before);

         ExpungeMessageInSeparateSession(account, 1);

         var after = sim1.Fetch("2 BODYSTRUCTURE");

         Assert.IsFalse(after.Contains("three.txt"), after);
         Assert.IsTrue(after.Contains("two.txt"), after);

         sim1.Disconnect();
      }

      [Test]
      [Description("Issue #458: fetching a message another session expunged must not return the " +
                   "message that took its place.")]
      public void FetchOfExpungedMessageMustNotReturnAnotherMessage()
      {
         var account = CreateAccountWithThreeAttachmentMessages();

         var sim1 = SelectInbox(account);
         AssertContainsAttachment(sim1.Fetch("2 BODY[2]"), MarkerTwo);

         ExpungeMessageInSeparateSession(account, 2);

         var after = sim1.Fetch("2 BODY[2]");

         // Message 2 is gone, so no body data may be returned at all - and above all not
         // the data of the message that slid into its place. RFC 2180 4.1.3 allows the
         // command to fail, and RFC 5530 chapter 3 gives it the EXPUNGEISSUED response code.
         AssertDoesNotContainAttachment(after, MarkerThree);
         AssertDoesNotContainAttachment(after, MarkerTwo);
         Assert.IsTrue(after.Contains("A17 NO [EXPUNGEISSUED]"), after);

         sim1.Disconnect();
      }

      #endregion

      #region Numbering must not shift during commands that may not send EXPUNGE

      [Test]
      [Description("RFC 3501 7.4.1: EXPUNGE must not be sent during FETCH, STORE, SEARCH or SORT, " +
                   "so the numbering those commands use must not change either.")]
      public void SequenceNumbersAreStableAcrossFetchStoreSearchSort()
      {
         var account = CreateAccountWithMessages(5);

         var sim1 = SelectInbox(account);
         var uids = GetUidBySequence(sim1, 5);

         ExpungeMessageInSeparateSession(account, 1);

         var fetch = sim1.Fetch("3 (UID)");
         Assert.IsTrue(fetch.Contains("UID " + uids[3]), "FETCH 3 returned the wrong message. " + fetch);
         AssertNoUntaggedExpunge(fetch, "FETCH");

         var store = sim1.SendSingleCommand("A40 STORE 3 +FLAGS (\\Flagged)");
         Assert.IsTrue(store.Contains("UID " + uids[3]), "STORE 3 hit the wrong message. " + store);
         AssertNoUntaggedExpunge(store, "STORE");

         var search = sim1.SendSingleCommand("A41 SEARCH FLAGGED");
         Assert.IsTrue(SearchResult(search, "A41") == "3", "SEARCH returned " + search);
         AssertNoUntaggedExpunge(search, "SEARCH");

         // Message 1 was expunged elsewhere so it cannot be returned (RFC 2180 4.3), but the
         // messages that remain must keep the numbers this session knows them by. The bug
         // would renumber them to 1 2 3 4.
         var sort = sim1.SendSingleCommand("A42 SORT (DATE) UTF-8 ALL");
         Assert.IsTrue(SearchResult(sort, "A42") == "2 3 4 5", "SORT returned " + sort);
         AssertNoUntaggedExpunge(sort, "SORT");

         sim1.Disconnect();
      }

      [Test]
      public void ExpungeIsDeliveredAtTheNextPermittedCommandNotDuringFetch()
      {
         var account = CreateAccountWithMessages(3);

         var sim1 = SelectInbox(account);
         ExpungeMessageInSeparateSession(account, 1);

         var fetch = sim1.Fetch("1 (UID)");
         AssertNoUntaggedExpunge(fetch, "FETCH");

         var noop = sim1.NOOP() + sim1.NOOP();
         Assert.IsTrue(noop.Contains("* 1 EXPUNGE"), "EXPUNGE not delivered by NOOP. " + noop);

         sim1.Disconnect();
      }

      [Test]
      [Description("Issue #458, write side: STORE addressed by sequence number must not set flags " +
                   "on a message the client never referred to.")]
      public void StoreMustNotSetFlagsOnTheWrongMessage()
      {
         var account = CreateAccountWithMessages(3);

         var sim1 = SelectInbox(account);
         var uids = GetUidBySequence(sim1, 3);

         ExpungeMessageInSeparateSession(account, 1);

         var store = sim1.SendSingleCommand("A43 STORE 2 +FLAGS (\\Flagged)");

         Assert.IsFalse(store.Contains("UID " + uids[3]), "STORE flagged the wrong message. " + store);
         Assert.IsTrue(store.Contains("UID " + uids[2]), store);

         sim1.Disconnect();
      }

      [Test]
      [Description("Issue #458, write side: COPY addressed by sequence number must never copy a " +
                   "message the client was not told about.")]
      public void CopyMustNotCopyTheWrongMessage()
      {
         var account = CreateAccountWithThreeAttachmentMessages();
         account.IMAPFolders.Add("Target");

         var sim1 = SelectInbox(account);
         ExpungeMessageInSeparateSession(account, 1);

         // COPY is not one of the commands during which EXPUNGE is forbidden, so the server
         // reports the expunge first and the numbering may change - but only because the
         // client was told. Message 2 is the third message from that point on.
         var copy = sim1.SendSingleCommand("A44 COPY 2 \"Target\"");
         Assert.IsTrue(copy.Contains("A44 OK"), copy);


         var expunge = copy.IndexOf("* 1 EXPUNGE", StringComparison.Ordinal);
         Assert.IsTrue(expunge >= 0, "COPY renumbered the mailbox without reporting the expunge. " + copy);

         sim1.Disconnect();

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Target", 1);

         var reader = new ImapClientSimulator();
         Assert.IsTrue(reader.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(reader.SelectFolder("Target"));
         var copied = reader.Fetch("1 BODY[2]");
         reader.Disconnect();

         AssertContainsAttachment(copied, MarkerThree);
      }

      [Test]
      public void SearchResultsMustUseTheSessionsOwnNumbering()
      {
         var account = CreateAccountWithMessages(3);

         var sim1 = SelectInbox(account);
         sim1.SendSingleCommand("A45 STORE 3 +FLAGS (\\Flagged)");

         ExpungeMessageInSeparateSession(account, 1);

         var search = sim1.SendSingleCommand("A46 SEARCH FLAGGED");
         Assert.AreEqual("3", SearchResult(search, "A46"), search);

         sim1.Disconnect();
      }

      #endregion

      #region Resync points

      [Test]
      public void NoopDeliversExpungeThenExistsWithOwnNumbering()
      {
         var account = CreateAccountWithThreeAttachmentMessages();

         var sim1 = SelectInbox(account);
         ExpungeMessageInSeparateSession(account, 1);

         var noop = sim1.NOOP() + sim1.NOOP();

         var expunge = noop.IndexOf("* 1 EXPUNGE", StringComparison.Ordinal);
         var exists = noop.IndexOf("* 2 EXISTS", StringComparison.Ordinal);

         Assert.IsTrue(expunge >= 0, "No EXPUNGE delivered. " + noop);
         Assert.IsTrue(exists >= 0, "No EXISTS delivered. " + noop);
         Assert.IsTrue(expunge < exists, "EXISTS was sent before the EXPUNGE. " + noop);

         // Only now may the client's message 2 mean the third message.
         AssertContainsAttachment(sim1.Fetch("2 BODY[2]"), MarkerThree);

         sim1.Disconnect();
      }

      [Test]
      public void IdleReceivesExpungeWithOwnSequenceNumbers()
      {
         _settings.IMAPIdleEnabled = true;

         var account = CreateAccountWithMessages(3);

         var sim1 = SelectInbox(account);

         // Not StartIdle/EndIdle: those assume one response per read, but the unsolicited
         // responses arrive interleaved.
         sim1.SendRaw("A47 IDLE\r\n");
         ReceiveUntil(sim1, "+ idling");

         ExpungeMessageInSeparateSession(account, 1);

         var pushed = ReceiveUntil(sim1, "EXPUNGE");
         Assert.IsTrue(pushed.Contains("* 1 EXPUNGE"), pushed);

         sim1.SendRaw("DONE\r\n");
         ReceiveUntil(sim1, "A47 OK");

         sim1.Disconnect();
      }

      [Test]
      public void NewMessagesDoNotRenumberExistingOnes()
      {
         var account = CreateAccountWithThreeAttachmentMessages();

         var sim1 = SelectInbox(account);

         SmtpClientSimulator.StaticSend(account.Address, account.Address, "Fourth", "Fourth body");
         Pop3ClientSimulator.AssertMessageCount(account.Address, "test", 4);

         var noop = sim1.NOOP() + sim1.NOOP();
         Assert.IsTrue(noop.Contains("* 4 EXISTS"), noop);

         AssertContainsAttachment(sim1.Fetch("1 BODY[2]"), MarkerOne);
         AssertContainsAttachment(sim1.Fetch("2 BODY[2]"), MarkerTwo);
         AssertContainsAttachment(sim1.Fetch("3 BODY[2]"), MarkerThree);

         sim1.Disconnect();
      }

      [Test]
      [Description("A flag notification must be renumbered by the same view as the EXPUNGE that " +
                   "precedes it, and must arrive after it.")]
      public void ExpungeThenFlagNotificationArriveInOrder()
      {
         var account = CreateAccountWithMessages(3);

         var sim1 = SelectInbox(account);
         var uids = GetUidBySequence(sim1, 3);

         ExpungeMessageInSeparateSession(account, 1);

         // Flag the third message by UID, so this is independent of the flagging session's view.
         var flagger = SelectInbox(account);
         flagger.SendSingleCommand("A48 UID STORE " + uids[3] + " +FLAGS (\\Flagged)");
         flagger.Disconnect();

         var noop = sim1.NOOP() + sim1.NOOP();

         var expunge = noop.IndexOf("* 1 EXPUNGE", StringComparison.Ordinal);
         var flags = noop.IndexOf("* 2 FETCH (FLAGS", StringComparison.Ordinal);

         Assert.IsTrue(expunge >= 0, "No EXPUNGE delivered. " + noop);
         Assert.IsTrue(flags >= 0, "Flag change not reported with the post-expunge number. " + noop);
         Assert.IsTrue(expunge < flags, "Flag change was reported before the EXPUNGE. " + noop);

         sim1.Disconnect();
      }

      #endregion

      #region UID forms

      [Test]
      [Description("RFC 3501 6.4.8: a non-existent unique identifier is ignored without an error.")]
      public void UidFetchIgnoresMessagesExpungedElsewhere()
      {
         var account = CreateAccountWithMessages(3);

         var sim1 = SelectInbox(account);
         var uids = GetUidBySequence(sim1, 3);

         ExpungeMessageInSeparateSession(account, 1);

         var result = sim1.SendSingleCommand("A49 UID FETCH " + uids[1] + " (UID)");

         Assert.IsTrue(result.Contains("A49 OK"), result);
         Assert.IsFalse(result.Contains("UID " + uids[1]), "Data returned for an expunged UID. " + result);

         sim1.Disconnect();
      }

      [Test]
      [Description("Issue #458: the reporter's client fetches by UID but then addresses body parts " +
                   "by the sequence number in the response, so that number must never change " +
                   "without the client being told.")]
      public void UidFetchUsesTheSessionsOwnSequenceNumbersInUntaggedResponses()
      {
         var account = CreateAccountWithMessages(3);

         var sim1 = SelectInbox(account);
         var uids = GetUidBySequence(sim1, 3);

         ExpungeMessageInSeparateSession(account, 1);

         // EXPUNGE is permitted during the UID commands (RFC 5256 chapter 3 spells this out for
         // UID SORT), so the number in the response may be the post-expunge one - but the
         // EXPUNGE that justifies it has to come first, in the same response.
         var result = sim1.SendSingleCommand("A50 UID FETCH " + uids[3] + " (UID)");

         var expunge = result.IndexOf("* 1 EXPUNGE", StringComparison.Ordinal);
         var fetch = result.IndexOf("* 2 FETCH", StringComparison.Ordinal);

         if (expunge < 0)
         {
            // The client was not told, so the numbering must be unchanged.
            Assert.IsTrue(result.Contains("* 3 FETCH"), "Silent renumbering in UID FETCH. " + result);
         }
         else
         {
            Assert.IsTrue(fetch > expunge, "The message was renumbered before the EXPUNGE. " + result);
         }

         sim1.Disconnect();
      }

      [Test]
      public void UidFetchOfBodyPartMustNotReturnAnotherMessagesAttachment()
      {
         var account = CreateAccountWithThreeAttachmentMessages();

         var sim1 = SelectInbox(account);
         var uids = GetUidBySequence(sim1, 3);

         ExpungeMessageInSeparateSession(account, 1);

         var result = sim1.SendSingleCommand("A51 UID FETCH " + uids[2] + " (BODY[2])");

         AssertDoesNotContainAttachment(result, MarkerThree);
         AssertContainsAttachment(result, MarkerTwo);

         sim1.Disconnect();
      }

      #endregion

      #region Deletion paths other than IMAP EXPUNGE

      [Test]
      [Description("Issue #458: POP3 DELE goes through FolderManager::DeleteInboxMessages, a second " +
                   "producer of index-based delete notifications.")]
      public void Pop3DeletionMustNotRenumberAnActiveImapSession()
      {
         var account = CreateAccountWithThreeAttachmentMessages();

         var sim1 = SelectInbox(account);
         AssertContainsAttachment(sim1.Fetch("2 BODY[2]"), MarkerTwo);

         var pop3 = new Pop3ClientSimulator();
         Assert.IsTrue(pop3.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(pop3.DELE(1));
         pop3.QUIT();
         pop3.Disconnect();

         ImapClientSimulator.AssertMessageCount(account.Address, "test", "Inbox", 2);

         var after = sim1.Fetch("2 BODY[2]");

         AssertDoesNotContainAttachment(after, MarkerThree);
         AssertContainsAttachment(after, MarkerTwo);

         sim1.Disconnect();
      }

      [Test]
      [Description("Issue #458: CLOSE deletes flagged messages without sending any change " +
                   "notification, so other sessions are never told.")]
      public void CloseInOneSessionMustNotSilentlyRenumberAnother()
      {
         var account = CreateAccountWithThreeAttachmentMessages();

         var sim1 = SelectInbox(account);
         AssertContainsAttachment(sim1.Fetch("2 BODY[2]"), MarkerTwo);

         var sim2 = SelectInbox(account);
         Assert.IsTrue(sim2.SetDeletedFlag(1));
         Assert.IsTrue(sim2.Close());
         sim2.Disconnect();

         var after = sim1.Fetch("2 BODY[2]");
         AssertDoesNotContainAttachment(after, MarkerThree);
         AssertContainsAttachment(after, MarkerTwo);

         // ...and the removal must eventually be reported.
         var noop = sim1.NOOP() + sim1.NOOP();
         Assert.IsTrue(noop.Contains("* 1 EXPUNGE"), "CLOSE was never reported to the other session. " + noop);

         sim1.Disconnect();
      }

      #endregion

      #region Helpers

      private Account CreateAccountWithThreeAttachmentMessages()
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         AddMessageWithAttachment(account, "one.txt", MarkerOne);
         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 1);

         AddMessageWithAttachment(account, "two.txt", MarkerTwo);
         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 2);

         AddMessageWithAttachment(account, "three.txt", MarkerThree);
         CustomAsserts.AssertFolderMessageCount(account.IMAPFolders[0], 3);

         return account;
      }

      private Account CreateAccountWithMessages(int count)
      {
         var account = SingletonProvider<TestSetup>.Instance.AddAccount(_domain, "test@example.test", "test");

         for (var i = 1; i <= count; i++)
         {
            SmtpClientSimulator.StaticSend(account.Address, account.Address, "Message " + i, "Body " + i);
            Pop3ClientSimulator.AssertMessageCount(account.Address, "test", i);
         }

         return account;
      }

      /// <summary>
      ///    Creates a message whose second body part is an attachment holding the given marker.
      ///    Messages are stored in creation order, so message n carries the n:th marker.
      /// </summary>
      private static void AddMessageWithAttachment(Account account, string attachmentName, string marker)
      {
         var filename = Path.Combine(TestSetup.GetSharedTempDirectory(), attachmentName);
         File.WriteAllText(filename, marker);

         var message = new Message();
         message.Charset = "utf-8";
         message.AddRecipient("test", account.Address);
         message.From = "Test";
         message.FromAddress = account.Address;
         message.Body = "Body of " + attachmentName;
         message.Attachments.Add(filename);
         message.Save();
      }

      private static ImapClientSimulator SelectInbox(Account account)
      {
         var simulator = new ImapClientSimulator();
         Assert.IsTrue(simulator.ConnectAndLogon(account.Address, "test"));
         Assert.IsTrue(simulator.SelectFolder("INBOX"));
         return simulator;
      }

      private static void ExpungeMessageInSeparateSession(Account account, int messageIndex)
      {
         var simulator = SelectInbox(account);

         Assert.IsTrue(simulator.SetDeletedFlag(messageIndex));

         string result;
         Assert.IsTrue(simulator.Expunge(out result), result);
         Assert.IsTrue(result.Contains("EXPUNGE"), result);

         simulator.Disconnect();
      }

      /// <summary>
      ///    Returns the UID of every message, keyed on its sequence number in this session.
      /// </summary>
      private static Dictionary<int, int> GetUidBySequence(ImapClientSimulator simulator, int expectedCount)
      {
         var response = simulator.Fetch("1:* (UID)");
         var uids = new Dictionary<int, int>();

         foreach (Match match in Regex.Matches(response, @"\* (\d+) FETCH \(UID (\d+)\)"))
            uids[int.Parse(match.Groups[1].Value)] = int.Parse(match.Groups[2].Value);

         Assert.AreEqual(expectedCount, uids.Count, response);

         return uids;
      }

      /// <summary>
      ///    The message numbers (or UIDs) from an untagged SEARCH/SORT response.
      /// </summary>
      private static string SearchResult(string response, string tag)
      {
         foreach (var line in response.Split(new[] {"\r\n"}, StringSplitOptions.RemoveEmptyEntries))
         {
            if (!line.StartsWith("* SEARCH") && !line.StartsWith("* SORT"))
               continue;

            var separator = line.IndexOf(' ', 2);
            return line.Substring(separator + 1).Trim();
         }

         Assert.Fail("No untagged response found for " + tag + ". " + response);
         return null;
      }

      private static string ReceiveUntil(ImapClientSimulator simulator, string text)
      {
         var deadline = DateTime.UtcNow + TimeSpan.FromSeconds(30);
         var result = string.Empty;

         while (DateTime.UtcNow < deadline)
         {
            result += simulator.Receive();

            if (result.Contains(text))
               return result;

            Thread.Sleep(25);
         }

         Assert.Fail("Timeout while waiting for: " + text);
         return null;
      }

      /// <summary>
      ///    RFC 3501 7.4.1 forbids untagged EXPUNGE responses during these commands. The
      ///    EXPUNGEISSUED response code is a different thing and is allowed.
      /// </summary>
      private static void AssertNoUntaggedExpunge(string response, string command)
      {
         Assert.IsFalse(Regex.IsMatch(response, @"^\* \d+ EXPUNGE", RegexOptions.Multiline),
            "EXPUNGE sent during " + command + ". " + response);
      }

      private static void AssertContainsAttachment(string response, string marker)
      {
         Assert.IsTrue(ContainsAttachment(response, marker),
            "Expected the attachment holding " + marker + ". " + response);
      }

      private static void AssertDoesNotContainAttachment(string response, string marker)
      {
         Assert.IsFalse(ContainsAttachment(response, marker),
            "Returned the attachment holding " + marker + ". " + response);
      }

      /// <summary>
      ///    Attachments are base64 encoded, so match either the plain marker or the base64 of its
      ///    first whole 3-byte groups - that prefix is independent of any trailing bytes the
      ///    server may add.
      /// </summary>
      private static bool ContainsAttachment(string response, string marker)
      {
         if (response.Contains(marker))
            return true;

         var bytes = Encoding.ASCII.GetBytes(marker);
         var wholeGroups = bytes.Length / 3 * 3;

         return response.Contains(Convert.ToBase64String(bytes, 0, wholeGroups));
      }

      #endregion
   }
}
