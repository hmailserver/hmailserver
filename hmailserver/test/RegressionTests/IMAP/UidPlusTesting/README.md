# UIDPLUS testing (temporary branch, not for merge)

Run the functional probes:

```
nunit3-console.exe RegressionTests.dll --where "cat == UidPlusTesting && cat != Stress"
```

Run the stress tests (Explicit, so they must be named):

```
nunit3-console.exe RegressionTests.dll --where "class == RegressionTests.Stress.IMAP.UidPlusStressTests"
```

`TrackedImapSession` checks every response a session receives: an `EXPUNGE` or `FETCH` for a
number above what the client was told via `EXISTS` is recorded as a violation.

## Suspected gaps (category `UidPlusGap`, expected to fail today)

| Test | Suspected problem |
|---|---|
| `CopyIntoSelectedFolderIsReportedToSameSession` | COPY into the selected folder is never reported to the copying session. Notifications skip their source, and COPY doesn't update the view. The COPYUID UIDs can't be fetched until reselect. |
| `ExpungeAfterCopyIntoSelectedFolderUsesKnownSequenceNumbers`, `UidExpungeOfCopyUidInSelectedFolderUsesKnownSequenceNumbers` | EXPUNGE calls `AppendNewMessages` silently, then reports `* n EXPUNGE` for numbers the client never saw. |
| `RecreatedFolderGetsNewUidValidityOrHigherUids` | UIDVALIDITY is the creation time in seconds. Delete and re-create in the same second: same UIDVALIDITY, UIDs restart at 1. |
| `RenamingOlderFolderOverDeletedOneRaisesUidValidity` | Renaming an older folder over a deleted name lowers that name's UIDVALIDITY. |
| `CopyWithDuplicatesInSetCopiesEachMessageOnce` | `1,1` or overlapping ranges copy a message twice. COPYUID lists the source UID twice. Also amplifies FETCH output (see `PathologicalSetsDoNotHangServer`). |
| `CopyToMissingFolderIsNoTryCreate` | Missing destination is BAD, not `NO [TRYCREATE]`. With a set matching nothing, it's OK. |
| `CopyWithoutInsertPermissionIsNo`, `UidExpungeWithoutPermissionIsNo` | ACL denials return BAD instead of NO. |
| `UidExpungeWithInvalidTokenIsBad` | `0`, `"1"` and `(1)` are accepted as sets. |

## Stress tests

- Concurrent COPY into one folder: destination UIDs distinct and holding the right message.
- Concurrent draft replacement (APPEND / UID STORE / UID EXPUNGE) with a polling observer.
- Overlapping concurrent UID EXPUNGE.
- Mixed workload (append, copy, self-copy, expunge, UID EXPUNGE) with observers whose view must converge.
- COPY rollback on quota while another session frees space.
- Message-set fuzz against a model, for FETCH, UID FETCH and UID COPY ordering.
- Timing of long sets on a 5000-message folder, and pathological sets (`1:*` × 20000 and so on).
