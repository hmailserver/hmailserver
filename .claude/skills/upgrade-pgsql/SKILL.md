---
name: upgrade-pgsql
description: >-
  Build libpq for hMailServer and/or bump the pinned PostgreSQL version, for 15.x
  and 16.x (e.g. rebuild 15.18 against a new OpenSSL, or upgrade 15.18 -> 15.19).
  Builds libpq into %hMailServerLibs% via libraries\build-pgsql.ps1 (linking it
  against the OpenSSL hMailServer uses), rewrites every pinned PostgreSQL version
  reference in the repo, keeps the README build instructions current, and verifies
  hMailServer still compiles and links. Use when asked to build, rebuild, bump,
  update, or upgrade PostgreSQL/libpq for hMailServer. Supports PostgreSQL 15.x/16.x
  only.
---

# Build / upgrade libpq (PostgreSQL 15.x/16.x) for hMailServer

hMailServer talks to PostgreSQL through libpq. PostgreSQL is not vendored in the repo;
libpq is built locally into `%hMailServerLibs%\postgresql-<version>\Release\libpq`, and its
concrete version is pinned in a few build/config files. This skill builds libpq (linking it
against the OpenSSL hMailServer uses), rewrites those references when the version changes, and
verifies the build.

There are two reasons to run it:

- **Rebuild libpq** at the *current* pinned version — most often to relink it against a
  freshly built OpenSSL (see the OpenSSL skill's libpq note). No version rewrites needed.
- **Bump the PostgreSQL version** (e.g. `15.18 -> 15.19`) — build the new version *and* rewrite
  the pinned references.

## Scope

- **PostgreSQL 15.x and 16.x only.** These ship the `src\tools\msvc\build.pl` MSVC build
  system this skill drives. **PostgreSQL 17 removed it** in favour of Meson; if the requested
  target is 17.x or newer, stop and tell the user it is out of scope — the build recipe differs
  and this skill is not validated for it. Reject anything not matching `^1[56]\.\d+$`.
- libpq links against OpenSSL at build time. The build script points `config.pl`'s `openssl`
  entry at `%hMailServerLibs%\openssl-<ver>\out64`, so **a matching OpenSSL build must already
  exist** (build it first with the upgrade-openssl skill / `build-openssl.ps1`).
- Updating any CI/CD-built copy of libpq is out of scope, but the README build instructions
  must stay accurate.

## Steps

### 1. Determine current and target version

- Read the current PostgreSQL version by searching
  `hmailserver/source/Server/hMailServer/post-build.bat` for `postgresql-1[56]\.\d+`
  (e.g. `postgresql-15.18`).
- Take the target version from the user (e.g. `15.19`). For a pure rebuild, the target equals
  the current version.
- **Validate the target matches `^1[56]\.\d+$`.** Abort with the out-of-scope message above if
  it does not.
- Read the OpenSSL version libpq should link against from
  `hmailserver/source/Server/hMailServer/hMailServer.vcxproj` (`openssl-<ver>`). The build
  script auto-detects this same value, so you normally do not need to pass it.

### 2. Build libpq

Invoke the build script (prerequisites — `%hMailServerLibs%`, an existing
`openssl-<ver>\out64`, Perl on PATH, VS2019 x64 tools — are validated inside it; surface its
error message if it fails):

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-pgsql.ps1 -Version <target>
```

Pass `-OpenSSLVersion <ver>` only to override the auto-detected value.

**Run this through the native PowerShell tool, NOT the Bash tool.** The Bash tool prepends
MSYS/Git (`/usr/bin`) to `PATH`, which shadows Strawberry Perl with an MSYS Perl that is
missing modules PostgreSQL's `build.pl` needs, and puts a GNU `tar` ahead of the Windows
`bsdtar` (GNU tar misreads the `C:` in `C:\...` as a remote rmt host — "Cannot connect to C:
resolve failed"). The native PowerShell tool has a clean Windows `PATH` where `perl` resolves
to Strawberry Perl. The script also pins tar to `%SystemRoot%\System32\tar.exe` defensively,
but Perl must still come from the Windows PATH.

The build takes several minutes and the PowerShell tool only returns its captured output once
it finishes, so the console can look stalled. The script mirrors every step to
`libraries\build-pgsql.log` (git-ignored via `*.log`) as it runs. Tell the user they can watch
progress live from another shell with `Get-Content libraries\build-pgsql.log -Wait`, and if the
script fails, read that log for the full transcript rather than relying on the (often
truncated) captured console output.

On success it produces `%hMailServerLibs%\postgresql-<target>\Release\libpq` containing
`libpq.dll` and `libpq.lib`, and the `libpq-fe.h` header under `src\interfaces\libpq`. Only
proceed if the script exits 0.

### 3. Rewrite the pinned version references

**Skip this step for a pure rebuild** (target == current). Otherwise replace the old version
with the new one (`postgresql-<old>` -> `postgresql-<new>`) in these files:

| File | Occurrences |
|------|-------------|
| `hmailserver/source/Server/hMailServer/post-build.bat` | 1 (the `xcopy` of libpq DLLs) |
| `hmailserver/source/Server/hMailServer/hMailServer.vcxproj` | 6 (include-dir + `src\include` + lib-dir, across Debug and Release) |
| `hmailserver/installation/hMailServer64.iss` | 1 (`POSTGRESQL_LIBPQ_PATH`) |
| `README.md` | 3 (the download / cd instructions in "Building PostgreSQL") |
| `.claude/skills/upgrade-openssl/SKILL.md` | 1 (the libpq-note path, to keep cross-references consistent) |

Then grep the whole repo for `postgresql-\d` and confirm the only remaining matches are the
intentional ones you just rewrote — there must be **zero** references to the old concrete
version.

**Do not** change the `libpq.dll` / `libpq.lib` names (in `section_files_64.iss` and the
`hMailServer.vcxproj` linker deps) or the `<libpq-fe.h>` include in
`Common/SQL/PGRecordset.h` / `PGConnection.h`. Those are not version-bearing.

### 4. Keep build instructions current

The `README.md` "Building PostgreSQL" section documents the manual recipe the script
automates. On a version bump, update its version numbers (step 3 already covers the
`postgresql-<ver>` strings). Skim the new release's notes; only if PostgreSQL changed the build
process (e.g. dropped/changed `src\tools\msvc\build.pl`, new flags) update both
`libraries\build-pgsql.ps1` and the README accordingly. State whether a change was needed.

### 5. Verify

Build hMailServer to confirm it compiles and links against the new libpq:

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File build\build.ps1
```

The link step pulls `libpq.lib` from the new `postgresql-<new>\Release\libpq`, so a successful
link confirms the reference updates are consistent. `post-build.bat` copies the new `libpq.dll`
into the output dir.

Confirm `libpq.dll` links against the expected OpenSSL DLLs (`libssl-3-x64.dll` /
`libcrypto-3-x64.dll`) by dumping its import table with `dumpbin /dependents` (the CLI
equivalent of the README's [Dependencies](https://github.com/lucasg/Dependencies/releases)
tool TIP — no extra tooling needed since `dumpbin` ships with VS):

```
powershell.exe -NoProfile -Command "$d = Get-ChildItem 'C:\Program Files*\Microsoft Visual Studio\*\*\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe' | Select-Object -First 1; & $d.FullName /dependents \"$env:hMailServerLibs\postgresql-<new>\Release\libpq\libpq.dll\""
```

Under `Image has the following dependencies:` both `libssl-3-x64.dll` and `libcrypto-3-x64.dll`
must be listed (dynamic-linked OpenSSL, in lockstep with the rest of hMailServer). If instead
you see no OpenSSL DLLs, libpq was built without SSL (missing/mismatched `openssl-<ver>\out64`
at build time) — rebuild it (step 2) before shipping. The other entries (`Secur32`, `WS2_32`,
`KERNEL32`, `VCRUNTIME140`, the `api-ms-win-crt-*` set) are the expected Windows/CRT
dependencies and need no action.

## Reference: all PostgreSQL/libpq touch points

- **Version-pinned (edited by step 3):** `post-build.bat`, `hMailServer.vcxproj`,
  `hMailServer64.iss`, `README.md`, and the libpq note in the upgrade-openssl skill.
- **Build recipe (edited only if the release changes it):** `libraries\build-pgsql.ps1`,
  `README.md`.
- **Not version-bearing (leave alone):** `libpq.dll` / `libpq.lib` names in
  `section_files_64.iss` and the `hMailServer.vcxproj` linker dependencies; the `<libpq-fe.h>`
  includes in `Common/SQL/PGRecordset.h` and `Common/SQL/PGConnection.h`.

## Note

The script's own end-to-end run needs the PostgreSQL source download, the VS2019 x64 toolchain,
Perl, and a matching `openssl-<ver>\out64` build present; it validates those prerequisites and
fails early with a clear message if any is missing.
