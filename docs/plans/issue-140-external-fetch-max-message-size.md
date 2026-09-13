# Plan: enforce max message size for external account downloads (#140)

Issue: https://github.com/hmailserver/hmailserver/issues/140

Messages fetched from external POP3 accounts ignore the maximum message size
configured globally (Settings > Protocols > SMTP) and per domain (Domain >
Limits). An oversized message is written to disk in full and delivered. The
account quota, by contrast, is enforced, because it is checked later by the
SMTP deliverer. This plan closes the gap and adds a notification so the local
recipient learns that a message was withheld, without the message being lost.

## 1. Current behavior

### 1.1 Where SMTP enforces the limit

`SMTPConnection` enforces the limit in three places:

- `GetMaxMessageSize_` (SMTPConnection.cpp) merges the global SMTP limit with
  the sender domain's limit, smaller wins, 0 means unlimited.
- On `MAIL FROM` the RFC 1870 `SIZE=` estimate is checked against it.
- On `DATA` the limit is passed to the transmission buffer with
  `SetMaxSizeKB`, so `TransparentTransmissionBuffer::SaveToFile_` stops
  writing to disk once the limit is passed, and the message is rejected with
  554 in `OnPreAcceptTransfer_` once the transfer has ended.

`IMAPCommandAppend::GetMaxMessageSize_` is a verbatim copy of the same rule.

### 1.2 Where the fetcher does not

`POP3ClientConnection` (source/Server/ExternalFetcher):

- `ParseData(ByteBuffer)` creates the transmission buffer for a `RETR`
  response without calling `SetMaxSizeKB`. Everything is written to disk.
- `HandlePOP3FinalizationTaskCompleted_` only rejects a 0-byte file, then
  parses headers, runs spam protection, saves the message and marks the UID
  as downloaded.
- `GetCancelTransmission()` (the "line too long" guard in the buffer) is never
  consulted either, so that protection is missing for fetched mail.
- `downloaded_messages_` is filled in `ParseRETRResponse_` as soon as the
  `+OK` arrives, before any message data. The cleanup pass (`MessageCleanup_`)
  issues `DELE` for every entry in that map, subject to Days To Keep.

Account quota works because `LocalDelivery::CheckAccountQuotas_` runs in the
SMTP deliverer, after the message has been saved. The message is then bounced
to the address parsed from the `From:` header and, since it is already marked
as downloaded, deleted from the external server on schedule.

### 1.3 Recent changes to build on

PR #603 (commit e4485b5, "Make POP3 downloading more robust") reworked this
class: `cur_message_` was split into `cur_download_` and `cur_cleanup_`, the
first `RETR` line is now assembled in `_firstRetrResponseBuffer`, the
finalization task closes the buffer before touching the file, and the account
lock is held by `FetchAccountLock` for the lifetime of the task. The changes
below fit that structure and do not touch the locking or cleanup logic.

## 2. Design decisions

1. **Same rule as SMTP.** Global SMTP limit combined with the domain limit of
   the fetch account's owner, smaller wins, 0 means unlimited. No new setting
   is introduced for the limit itself.
2. **Reject, do not consume.** An oversized message is not delivered and is
   left on the external server. It is never marked as downloaded and never
   deleted. This is the POP3 counterpart of an SMTP 554 reject: nothing is
   lost, and the admin can raise the limit or the user can fetch the message
   with another client. The alternative in the issue (consume, delete on
   schedule, notify) destroys the only copy and is not taken.
3. **Do not download what will be rejected.** Use the mandatory POP3 `LIST`
   command to learn message sizes before `RETR`, and skip oversized messages
   without transferring them. The buffer limit stays as defense in depth
   against servers that misreport sizes.
4. **Notify the local recipient once per message.** A skipped message
   generates one notification to the fetch account's owner, using the same
   server-message mechanism as bounces and virus notifications. "Once" needs
   persistence, which is the only schema change in this plan.
5. **Always log.** Every skip is written to the POP3 log and the application
   log, with size and limit, whether or not a notification is sent.

## 3. Work items

### 3.1 Shared size-limit helper

Files: `source/Server/SMTP/SMTPConfiguration.h/.cpp`,
`source/Server/SMTP/SMTPConnection.cpp`, `source/Server/IMAP/IMAPCommandAppend.cpp`.

- Add `int SMTPConfiguration::GetMaxMessageSizeKB(std::shared_ptr<const Domain> domain)`
  that implements the merge rule.
- Replace the private copies in `SMTPConnection::GetMaxMessageSize_` and
  `IMAPCommandAppend::GetMaxMessageSize_` with calls to it. No behavior change.

### 3.2 Enforce the limit in `POP3ClientConnection`

File: `source/Server/ExternalFetcher/POP3ClientConnection.cpp/.h`.

- New member `size_t max_message_size_kb_`, computed once in the constructor:
  look up the owner with `CacheContainer::Instance()->GetAccount(account_->GetAccountID())`,
  its domain with `GetDomain(StringParser::ExtractDomain(address))`, then call
  the helper from 3.1. The owner's domain is used even when MIME recipient
  processing routes the message elsewhere, because the download is done on
  behalf of that account.
- New member `std::map<int, __int64> message_sizes_` filled from `LIST`
  (see 3.3). Missing entries mean "unknown, download and check".
- In `ParseData(ByteBuffer)`, right after `transmission_buffer_->Initialize(fileName)`,
  call `transmission_buffer_->SetMaxSizeKB(max_message_size_kb_)`. The rest of
  the `RETR` response is still read from the socket to stay in sync with the
  protocol, but nothing past the limit reaches the disk.
- In `HandlePOP3FinalizationTaskCompleted_`, after `Close()`, add a check
  before the 0-byte test:

  ```
  bool tooLarge = max_message_size_kb_ > 0 &&
                  transmission_buffer_->GetSize() / 1024 > max_message_size_kb_;
  if (tooLarge || transmission_buffer_->GetCancelTransmission())
     RejectCurrentMessage_(reason);
  ```

  `RejectCurrentMessage_` (new):
  - deletes the message file,
  - logs to the POP3 log and via `LOG_APPLICATION` (message index, UID, size
    in KB, limit in KB, fetch account name, owner address),
  - erases the message index from `downloaded_messages_` so the cleanup pass
    never sends `DELE` for it,
  - does not call `SaveMessage_`, `FireOnExternalAccountDownload_` or
    `MarkCurrentMessageAsRead_` (so no `AddUID`),
  - hands over to the notification step (3.5),
  - then continues exactly like the success path: `SetReceiveBinary(false)`,
    `cur_download_++`, `RequestNextMessage_()`, `EnqueueRead("")`.
- `transmission_buffer_->GetSize()` counts bytes received including the
  prepended `X-hMailServer-ExternalAccount` header, which matches what SMTP
  measures (its buffer includes the Received header). Accept the small
  difference; it is well under 1 KB.

### 3.3 Skip oversized messages before `RETR` using `LIST`

File: `source/Server/ExternalFetcher/POP3ClientConnection.cpp/.h`.

- Add state `StateLISTRequestSent` between `StatePasswordSent` and
  `StateUIDLRequestSent`. After a successful `PASS`, send `LIST` first. Only
  do this when `max_message_size_kb_ > 0`; otherwise go straight to `UIDL`
  as today, so servers with unlimited accounts see no protocol change.
- Parse the multi-line response the same way `UIDL` is parsed today (buffer
  in `command_buffer_` until `\r\n.\r\n`), into `message_sizes_`. Each line is
  `<index> <octets>`. A `-ERR` reply to `LIST` is logged and ignored: fall back
  to download-and-check.
- In `RequestNextMessage_`, before sending `RETR`, if the message's listed
  size in KB exceeds the limit, call `RejectCurrentMessage_` without
  downloading. Because no `RETR` is sent, `ParseRETRResponse_` never runs and
  the message is not added to `downloaded_messages_`. The loop then continues
  with the next message.
- The `LIST` size is the size of the message as stored on the server. It can
  differ slightly from what hMailServer would store (dot-unstuffing, the
  prepended header). Compare with the same integer KB truncation SMTP uses,
  and let the buffer check in 3.2 catch anything the `LIST` check let through.

### 3.4 Persist "skipped" state per UID

The notification must be sent once per message, not on every fetch cycle
(every `MinutesBetweenFetch` minutes). The UID table already tracks per-message
state for the fetch account, so extend it rather than add a table.

Schema, `source/DBScripts/`:

- `Upgrade5711to5712{MySQL,MSSQL,MSSQLCE,PGSQL}.sql`:
  `alter table hm_fetchaccounts_uids add [column] uidskippedsize int not null default 0`
  (MySQL and PostgreSQL use `add column`, MSSQL and MSSQLCE use `add`, following
  `Upgrade5703to5704*.sql`), the new server message from 3.5, and
  `update hm_dbversion set value = 5712`.
- Add the column to the four `CreateTables*.sql` files and the server message
  to the `hm_servermessages` seed data in each.
- Bump `REQUIRED_DB_VERSION` in `source/Server/Common/Application/Constants.h`
  and add `new UpgradeScript(5711, 5712)` plus the version label in
  `source/Tools/DBUpdater/formMain.cs`.

Meaning of `uidskippedsize`: 0 means the row is a normal "downloaded" marker.
Non-zero means the message was skipped for being too large, and the value is
the size in KB observed at the time. It is not a boolean so that a later raise
of the limit can be detected cheaply (see below).

Code:

- `FetchAccountUID` (BO): add `skipped_size_kb_` with accessors; include it
  in `XMLStore`/`XMLLoad` so backup and restore preserve it (attribute
  `SkippedSizeKB`, default 0 when missing so old backups restore).
- `PersistentFetchAccountUID::SaveObject`: write the column. Add
  `AddSkippedUID(faid, uid, sizeKB)`.
- `FetchAccountUIDList::Refresh`: read the column. Add
  `AddSkippedUID(uid, sizeKB)`, `bool IsSkipped(uid)`, `GetSkippedSizeKB(uid)`.
- `POP3ClientConnection::RequestNextMessage_` currently treats any UID in the
  list as downloaded and adds it to `downloaded_messages_` for deletion. Change
  the branch to:
  - UID in list and not skipped: unchanged (mark for cleanup, fire the
    already-downloaded script event).
  - UID in list and skipped: if the listed size now fits within the limit, or
    the limit is now 0, remove the row (`DeleteUID`) and fall through to the
    normal download branch. Otherwise skip silently (no download, no
    notification, no `DELE`) and log at debug level.
  - UID not in list: unchanged.
- `RejectCurrentMessage_` records the skip with `AddSkippedUID(uid, sizeKB)`
  after the notification has been created, so a crash between the two at
  worst repeats the notification once.
- `DeleteUIDsNoLongerOnServer_` already removes rows for UIDs that vanished
  from the server, so a skipped message the user deleted elsewhere cleans
  itself up. `MessageCleanup_` must never `DELE` a skipped UID; this holds
  because skipped messages are never added to `downloaded_messages_`.

Backward compatibility: a server downgraded to a build without the column
simply ignores it, and the row then reads as "downloaded", which would let
the old build delete the message on schedule. Mention this in the release
notes; it is the same class of risk as any schema addition.

### 3.5 Notification to the local recipient

Model: `SMTPVirusNotifier::CreateMessageDeletedNotification` in
`source/Server/SMTP/SMTPVirusNotifier.cpp`.

- New class `ExternalFetchNotifier` in `source/Server/ExternalFetcher/` with
  `static void CreateMessageTooLargeNotification(std::shared_ptr<const FetchAccount> fetchAccount, const String &recipientAddress, const AnsiString &originalHeader, __int64 sizeKB, int maxSizeKB)`.
- Content comes from a new server message `EXTERNAL_ACCOUNT_MESSAGE_TOO_LARGE`
  in `hm_servermessages`, editable in Administrator under Settings > Advanced
  > Server messages. The GUI lists whatever rows exist, so no GUI change is
  needed. Suggested default text:

  ```
  A message in the external account "%MACRO_ACCOUNT%" was not downloaded
  because it exceeds the maximum message size.

     From: %MACRO_FROM%
     Sent: %MACRO_SENT%
     Subject: %MACRO_SUBJECT%
     Size: %MACRO_SIZE% KB
     Maximum size: %MACRO_MAXSIZE% KB

  The message has been left on the external server. Contact your
  administrator to raise the limit, or download it with another client.

  hMailServer
  ```

  Subject: the existing `MESSAGE_UNDELIVERABLE` text followed by the original
  subject, or a fixed "Message too large" when no subject is known.
- Headers of the original message are needed for the macros. When the
  message was rejected after download (3.2), read them from the truncated
  file before deleting it (`PersistentMessage::LoadHeader`). When it was
  skipped from `LIST` (3.3), issue `TOP <index> 0` first to get the headers
  only; `TOP` is optional in RFC 1939 but advertised by `CAPA` and supported by
  every mainstream server. If `TOP` fails, send the notification with the
  From, Sent and Subject macros replaced by "(unknown)". This adds a state
  `StateTOPSent` and reuses the `command_buffer_` multi-line collection.
- The notification is a normal message: From is the mailer-daemon address
  (`MailerDaemonAddressDeterminer`), To is the fetch account owner's address,
  `Auto-Submitted` is set via `SetAutoGenerated`, the rule loop count is
  increased, recipients are built with `RecipientParser`, saved with
  `PersistentMessage::SaveObject`, and `SubmitPendingEmail` is called. It is
  delivered by the SMTP deliverer like any other message, so rules and
  forwarding apply to it.
- Recipient: always the fetch account owner, even when MIME recipient
  processing is enabled, because the actual recipients are unknown until the
  headers are parsed and may be external addresses that must not receive
  auto-generated mail from this server.
- No new on/off setting in this iteration. Administrators who do not want the
  notification can blank the server message text; `ExternalFetchNotifier`
  skips sending when the text is empty. A COM property on the fetch account
  can be added later if requested.

### 3.6 Test support

File: `test/RegressionTests/Shared/Pop3ServerSimulator.cs`.

- Handle `LIST`: `+OK`, then `<index> <length>` per message, then `.`. Add
  `SupportsLIST` (default true) and `ListedSizes` override
  (`Dictionary<int,int>`) so a test can make the server lie about sizes.
- Handle `TOP <index> <n>`: send `+OK`, the header block of the message (up
  to the first blank line) and `.`. Add `SupportsTOP` (default true).
- Expose `RetrievedMessages` (already exists) and initialize it in the
  constructor; today it is null until assigned.

### 3.7 Regression tests

New fixture `test/RegressionTests/POP3/Fetching/MaxMessageSize.cs`, using the
`CreateFetchAccount` and `CreateServer` helpers from `ServerBehaviors.cs`.
Write the tests first and confirm they fail against the current build.

Setup: a domain with `MaxMessageSize = 1` (KB), owner account, a 2 KB message
and a 300-byte message on the simulated server.

1. `DomainLimitRejectsOversizedMessage`: after `DownloadNow` the owner has
   exactly one message (the small one), `RetrievedMessages` does not contain
   the large one, `DeletedMessages` is empty, no orphan `.eml` remains in the
   data directory, and `LogHandler.ReadCurrentDefaultLog()` names the size
   and limit.
2. `GlobalLimitAppliesWhenDomainHasNone`: domain limit 0, global
   `Settings.MaxMessageSize = 1`; same assertions.
3. `SmallerOfGlobalAndDomainWins`: global 1, domain 5 and the reverse.
4. `NoLimitDeliversLargeMessage`: both 0, the 2 KB message is delivered.
5. `MessageAtLimitIsDelivered`: message a few bytes under the limit.
6. `ServerLyingAboutSizeIsStillRejected`: `ListedSizes` reports 100 bytes for
   the 2 KB message; it is retrieved but not delivered, not deleted, and the
   file is removed.
7. `ServerWithoutListFallsBackToDownloadCheck`: `SupportsLIST = false`; same
   outcome as test 1 except `RetrievedMessages` contains the large message.
8. `SkippedMessageIsNotDeletedOnSchedule`: `DaysToKeepMessages = -1` (delete
   immediately); the large message is still not in `DeletedMessages`.
9. `NotificationIsSentOnce`: owner receives one auto-generated message whose
   body contains the subject, size and limit, and whose `Auto-Submitted`
   header is set. A second `DownloadNow` against a fresh simulator with the
   same messages delivers nothing new and sends no second notification.
10. `NotificationWithoutTopSupport`: `SupportsTOP = false`; the notification
    is still sent with "(unknown)" for the header macros.
11. `RaisedLimitDownloadsPreviouslySkippedMessage`: after test 9's first
    cycle, set the domain limit to 0 and fetch again; the large message is
    delivered and the skipped row is gone (verify via a third cycle that the
    message is now deleted on schedule).
12. `EmptyServerMessageSuppressesNotification`: blank the
    `EXTERNAL_ACCOUNT_MESSAGE_TOO_LARGE` text; the message is skipped and
    logged but the owner receives no notification.
13. `TooLongLineIsRejected`: a message with a 200 000-character line is not
    delivered, not deleted, and logged.
14. Backup/restore: extend the existing backup tests so a fetch account with
    a skipped UID round-trips with `uidskippedsize` intact.
15. Regression: run the whole `POP3.Fetching` namespace and the SMTP and
    IMAP max-size tests, since 3.1 touches their code path.

Persistence layer: add a test in
`test/RegressionTests/Infrastructure/Persistence/` that saves and reloads a
`FetchAccountUID` with a non-zero skipped size through the COM API if the UID
list is reachable there; otherwise cover it through test 9 and 11 only.

### 3.8 Documentation

- `hmailserver/documentation`: in the external accounts page, state that the
  global and domain maximum message size now apply, that oversized messages
  stay on the external server, and describe the notification and how to edit
  or blank it.
- Release notes entry referencing #140 and the schema version bump.

## 4. Sequencing

1. 3.1 shared helper, with the existing SMTP and IMAP tests as the safety net.
2. 3.6 simulator changes and the failing tests from 3.7 (1 to 8, 13).
3. 3.2 buffer limit and post-download rejection.
4. 3.3 `LIST` pre-check.
5. 3.4 schema and UID persistence, plus tests 9, 11, 14.
6. 3.5 notification and `TOP`, plus tests 10, 12.
7. 3.8 documentation.

Steps 1 to 4 are a complete fix for the reported bug and can ship on their
own if the schema change needs to wait for a release that already bumps the
database version. Without steps 5 and 6, an oversized message is re-checked
(and, without `LIST`, re-downloaded and discarded) on every fetch cycle, and
the only notification is the application log entry.

## 5. Out of scope

- A per-fetch-account or global on/off switch for the notification (blanking
  the server message covers it for now).
- Notifying the original sender. hMailServer never spoke to the sender's
  server, so it has no standing to bounce; a bounce would also expose the
  fetch relationship to third parties.
- Changing the quota behavior for fetched messages described in the issue's
  second comment. That path already bounces to the sender and should be
  reviewed separately.
