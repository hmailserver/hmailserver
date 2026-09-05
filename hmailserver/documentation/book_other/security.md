---
title: "Security"
slug: security
parent: book_other
index: 0
is_book: false
---

## The service account

The hMailServer service runs as the virtual account `NT SERVICE\hMailServer`. The
account belongs to the service alone and has no password. If Windows cannot apply the
account, the service runs as `LocalSystem` instead.

## Where hMailServer stores its data

Everything the server writes is kept under `C:\ProgramData\hMailServer`:

| Item | Contains |
| --- | --- |
| `hMailServer.ini` | The database password and the hMailServer administrator password |
| `Data` | All messages |
| `Database` | The built-in database, if one is used |
| `Logs` | The log files |
| `Events` | The event handler scripts |
| `Temp` | Temporary files |

## Default permissions

The installer removes the inherited permissions on the folder and grants these:

| Account | Access |
| --- | --- |
| `NT SERVICE\hMailServer` | Full control |
| `SYSTEM` | Full control |
| `Administrators` | Full control |
| `Users` | Read and execute |

Users are given read access so that whoever administers the server can open the logs
and the event handler scripts without elevating.

## Restricting access

Read access to the folder is also read access to the messages and to
`hMailServer.ini`. The database password in `hMailServer.ini` is obfuscated rather
than encrypted, so anyone who can read the file can recover it.

If other people log on to the computer that runs hMailServer, remove the read access.
Run this in an elevated command prompt:

```
icacls "C:\ProgramData\hMailServer" /remove:g *S-1-5-32-545
```

`S-1-5-32-545` is the Users group. The SID is used instead of the name because the
name is translated on non-English versions of Windows.

Afterwards the folder can only be opened by a program that runs elevated. The buttons
in hMailServer Administrator that open the log and the event handler folders stop
working: Windows Explorer opens the Documents folder instead of the folder it was
asked for.
