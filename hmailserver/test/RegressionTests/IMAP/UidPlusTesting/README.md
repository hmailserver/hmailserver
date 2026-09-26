# UIDPLUS testing (temporary branch, not for merge)

Run the functional tests and probes:

```
nunit3-console.exe RegressionTests.dll --where "cat == UidPlusTesting && cat != Stress"
```

Only the suspected gaps: `--where "cat == UidPlusGap"`. The stress tests are Explicit:

```
nunit3-console.exe RegressionTests.dll --where "class == RegressionTests.Stress.IMAP.UidPlusStressTests"
```

`TrackedImapSession` checks every response a session receives:
- `EXPUNGE`/`FETCH` numbers must be within what `EXISTS` told the client.
- `APPENDUID`/`COPYUID` must match the RFC 4315 grammar (nz-numbers, no `*`) and appear only in an OK.

## RFC requirement matrix

| Requirement | Source | Covered by | Expected |
|---|---|---|---|
| UIDPLUS in CAPABILITY | 4315 1 | `Basics` | pass |
| UID EXPUNGE removes only `\Deleted` messages in the set | 4315 2.1 | `UidPlus`, `UidExpungeLeavesOtherDeletedMessages` | pass |
| UID EXPUNGE sends EXPUNGE, renumbered in order | 4315 2.1, 3501 7.4.1 | `UidExpungeOfSeveralMessagesAdjustsSequenceNumbers` | pass |
| UID EXPUNGE refused on read-only / without `e` right | 3501 6.4.3, 4314 4 | `UidPlus`, `AclDenialIsNoPerm` | NO: pass; `[NOPERM]`: **gap** |
| APPENDUID = destination UIDVALIDITY + assigned UID | 4315 3 | `UidPlus`, `UidValidityIsConsistentAcrossCommands` | pass |
| COPYUID sets correspond, in copy order | 4315 3 | `CopyUidCorrespondsForUnorderedSet`, stress | pass |
| COPYUID sets have no extraneous UIDs or `*` | 4315 3 | tracker, `CopyWithDuplicatesInSetCopiesEachMessageOnce` | **gap** with duplicates |
| UIDVALIDITY and UIDs are nz-number | 4315 4, 3501 9 | tracker | pass |
| No UIDs revealed without read access | 4315 6 | `UidPlus` | pass |
| COPY is atomic; failed COPY leaves destination unchanged | 3501 6.4.7 | `UidPlus`, `FailedCopyIsConsistentForObserverOfDestination`, stress | pass |
| COPY to missing mailbox: `NO [TRYCREATE]` | 3501 6.4.7 | `CopyToMissingFolderIsNoTryCreate` | **gap** |
| UID COPY ignores UIDs that don't exist | 3501 6.4.8 | `UidCopySkipsMessageExpungedElsewhere` | pass |
| COPY/UID COPY errors are NO, not BAD | 3501 7.1 | `UidCopyRacingWithExpungeIsNeverBad` | likely **gap** (file-copy failure is BAD) |
| Server MUST send size updates seen while processing a command | 3501 5.2 | `CopyIntoSelectedFolderIsReportedToSameSession` | **gap** |
| EXPUNGE only for numbers the client knows | 3501 7.4.1 | `ExpungeAfterCopyIntoSelectedFolder...`, stress | **gap** |
| UIDVALIDITY greater when UIDs don't persist (recreate) | 3501 2.3.1.1 | `RecreatedFolderGetsNewUidValidityOrHigherUids` | **gap** |
| Same, rename over a deleted name | 3501 2.3.1.1 | `RenamingOlderFolderOverDeletedOneRaisesUidValidity` | gap, low (Dovecot same) |
| UIDVALIDITY survives restart | 3501 2.3.1.1 | `UidValiditySurvivesServerRestart` | pass |
| UIDNEXT above every assigned UID, incl. rolled-back | 3501 2.3.1.1 | `UidNextFollows*` | pass |
| `*` is the highest number; `N:*` includes it; ranges either order | 3501 9 | `UidPlus`, fuzz, `UidSearchUidAgreesWithUidFetch` | FETCH/COPY/STORE/EXPUNGE pass; **SEARCH gap** |
| `0` is not a valid number | 3501 9 | `UidExpungeWithInvalidTokenIsBad` | **gap** |
| APPEND/COPY set `\Deleted` only with the `t` right | 4314 4 | `*WithoutWriteDeletedRightDoesNotSetDeleted` | **gap** |
| Quota failures carry `[OVERQUOTA]` | 9208, 5530 | `QuotaFailuresCarryOverQuota` | **gap** |
| APPEND to missing mailbox: `NO [TRYCREATE]` | 3501 6.3.11 | `AppendToMissingFolderHasTryCreateAndNoAppendUid` | pass |

Not applicable (not advertised): MULTIAPPEND (uid-set in APPENDUID), MOVE (COPYUID in untagged OK),
QRESYNC (VANISHED from UID EXPUNGE), UIDNOTSTICKY (UIDs always persist).

## Comparison with Dovecot 2.3.21

Probed locally with the same commands.

| Case | Dovecot | hMailServer |
|---|---|---|
| `UID COPY 5,1,3` | `COPYUID v 1,3,5 1:3` (sorted) | `COPYUID v 5,1,3 1:3` (set order). Both valid. |
| `UID COPY 1,1` / `1:2,2:3` | copied once, `1 4` / `1:3 5:7` | copied twice, source UID listed twice |
| COPY to missing folder | `NO [TRYCREATE]` | BAD; OK if the set matches nothing |
| `UID COPY 999 Target` (no match) | `OK No messages found`, no COPYUID | same |
| `UID FETCH 0`, `UID EXPUNGE 0` | BAD Invalid uidset | OK (empty) |
| `UID EXPUNGE "1"` / `1, 2` | accepted | `"1"` accepted, `1, 2` BAD |
| `UID EXPUNGE (1)`, `4294967296` | BAD | BAD |
| `UID SEARCH UID *` / `100:*` / `4:2` | highest / highest / 2 3 4 | nothing / nothing / nothing |
| COPY into selected mailbox | `* n EXISTS` in the COPY response | never reported to the copying session |
| UID COPY with a UID expunged elsewhere | `NO [EXPUNGEISSUED]` | skips it, OK with COPYUID (closer to 3501 6.4.8) |
| COPY by sequence, message expunged elsewhere | `NO [EXPUNGEISSUED]` | same |
| UID EXPUNGE of an unreported `\Deleted` UID | EXISTS, message not expunged | EXISTS, then expunged |
| Delete + re-create mailbox | UIDVALIDITY always increases (counter) | same second: same UIDVALIDITY, UIDs restart at 1 |
| Rename older mailbox over deleted name | lower UIDVALIDITY | same |
| COPY over quota | `NO [OVERQUOTA]` | plain NO |

## Root causes worth fixing

1. `IMAPCopy` into the selected folder: the delayed notification skips its sender and the view isn't
   updated, unlike `IMAPCommandAppend::Finish_`. Fix: update the view and send EXISTS, as APPEND does.
2. `IMAPCommandEXPUNGE` calls `view->AppendNewMessages` without sending EXISTS first.
3. `IMAPFolder::GetUIDValidity` is the creation second. Dovecot and Cyrus keep a per-user counter
   and use max(now, last + 1).
4. `IMAPCommandRangeAction::ResolveTargets` keeps duplicates. Dedupe by message id (keep first).
5. `IMAPCommandSEARCH::MatchesUIDCriteria_` still uses `IMAPListLookup`. Resolve through `ResolveTargets`.
6. `ParseNumber_` accepts 0.
7. COPY: folder lookup is inside `DoAction`, after the set is resolved. Look up the destination first
   (TRYCREATE even for empty sets), and return NO for ACL, quota and file failures.
8. `IMAPCommandAppend::Finish_` / `IMAPCopy::DoAction` check only the `s` right. Check `t` and `w` too.

## Stress tests

- Concurrent COPY into one folder: destination UIDs distinct and holding the right message.
- Concurrent draft replacement (APPEND / UID STORE / UID EXPUNGE) with a polling observer.
- Overlapping concurrent UID EXPUNGE.
- Mixed workload (append, copy, self-copy, expunge, UID EXPUNGE) with observers whose view must converge.
- COPY rollback on quota while another session frees space.
- Message-set fuzz against a model, for FETCH, UID FETCH and UID COPY ordering.
- Timing of long sets on a 5000-message folder, and pathological sets (`1:*` × 20000 and so on).
