---
name: upgrade-7zip
description: >-
  Fetch the 7-Zip command line tool hMailServer ships for backup/restore and/or bump
  the pinned version (e.g. re-fetch 26.03, or upgrade 26.03 -> 26.04). Records the new
  release's SHA-256, downloads and unpacks it into %hMailServerLibs% via
  libraries\build-7zip.ps1, rewrites every pinned 7-Zip version reference in the repo,
  keeps the README build instructions current, and verifies backup and restore still
  work. Use when asked to fetch, re-fetch, bump, update, or upgrade 7-Zip / 7za.exe for
  hMailServer.
---

# Fetch / upgrade 7-Zip for hMailServer

hMailServer's backup and restore feature shells out to the 7-Zip command line tool: see
`Compression::AddFile`, `AddDirectory` and `Uncompress` in
`hmailserver/source/Server/Common/Util/Compression.cpp`, which resolve `7za.exe` from the
configured `BinDirectory`. The binary is **not** vendored in the repo. It is downloaded into
`%hMailServerLibs%\7zip-<version>` by `libraries\build-7zip.ps1`, copied into the build output
by `post-build.bat`, and shipped in `Bin` by the installer along with its LGPL license text.

There are two reasons to run this skill:

- **Re-fetch** at the *current* pinned version — e.g. on a fresh machine, or after clearing
  `%hMailServerLibs%`. No version rewrites needed, and the hash is already recorded.
- **Bump the version** (e.g. `26.03 -> 26.04`) — record the new release's hash, fetch it, *and*
  rewrite the pinned references.

## What makes this different from the other library skills

Nothing is compiled. 7-Zip ships as a prebuilt binary from Igor Pavlov, so there is no
toolchain, no OpenSSL dependency, and no build to wait on — the whole fetch takes seconds.

In exchange, the download is **hash-pinned**: the bytes downloaded are the bytes installed on
every hMailServer machine, so `build-7zip.ps1` refuses any version whose SHA-256 is not
recorded in its `$ArchiveHashes` table. On a version bump you must add the entry *before* the
script will run (step 2).

## Scope

- **Versions matching `^\d+\.\d+$`** (7-Zip's `MM.NN` scheme, e.g. `26.03`). The script derives
  the download name by removing the dot — `26.03` -> `7z2603-extra.7z`. If a future release
  changes that scheme, stop and tell the user it is out of scope rather than guessing.
- Only the **x64 standalone** `7za.exe` is shipped. The package also carries x86/arm64 builds,
  the `7za.dll`/`7zxa.dll` variants and a Far Manager plugin; none are installed.
- Changing the archive **format** (7z -> zip), the compression flags, or replacing the
  subprocess with an in-process library are all out of scope here.

## Steps

### 1. Determine current and target version

- Read the current version by searching
  `hmailserver/source/Server/hMailServer/post-build.bat` for `7zip-(\d+\.\d+)` (e.g. `7zip-26.03`).
- Take the target from the user. For a pure re-fetch, target == current — **skip to step 3**.
- Validate the target matches `^\d+\.\d+$`; abort with the out-of-scope message above if not.
- Confirm the release exists and note what changed:

```
gh api repos/ip7z/7zip/releases/tags/<target> --jq '.name, (.assets[].name)'
```

The asset list must contain `7z<target-with-dot-removed>-extra.7z`. If it does not, the release
layout changed — stop and report rather than working around it.

### 2. Record the new release's SHA-256

Only for a version bump. Download the archive and hash it:

Run this through the native PowerShell tool, substituting the target version. It deliberately
uses only single quotes and string concatenation — nesting double quotes inside a
`powershell.exe -Command` string is what breaks these one-liners:

```powershell
$v='<target>'; $t=$v.Replace('.',''); $u='https://github.com/ip7z/7zip/releases/download/'+$v+'/7z'+$t+'-extra.7z'; $f=Join-Path $env:TEMP ('7z'+$t+'-extra.7z'); Invoke-WebRequest -Uri $u -OutFile $f; (Get-FileHash $f -Algorithm SHA256).Hash
```

Add the result to `$ArchiveHashes` in `libraries\build-7zip.ps1`, keeping the existing entries:

```powershell
$ArchiveHashes = @{
    "26.03" = "191894E6ACB3647FFB69CE630479FF318523B2E2B9890AA7F05C1127C2E59B8F"
    "26.04" = "<new hash>"
}
```

Keep old entries. They cost nothing, document what was shipped before, and let an older version
be re-fetched for comparison.

**Be honest with the user about what this hash does.** 7-Zip publishes no checksum files and no
signatures for these assets, so hashing the file you just downloaded is trust-on-first-use: it
pins the bytes for every later fetch and for CI, but it does **not** prove the download was
authentic. If the user wants more assurance, suggest fetching it a second time from a different
network and confirming the hash matches. Never present the recorded hash as proof of
authenticity.

### 3. Fetch it

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-7zip.ps1 -Version <target>
```

**Run this through the native PowerShell tool, NOT the Bash tool.** The Bash tool prepends
MSYS/Git (`/usr/bin`) to `PATH`, which puts a GNU `tar` ahead of the Windows `bsdtar` the script
unpacks the `.7z` with (GNU tar misreads the `C:` in `C:\...` as a remote rmt host — "Cannot
connect to C: resolve failed"). The native PowerShell tool has a clean Windows `PATH`.

The script downloads, verifies the SHA-256, unpacks with the Windows-bundled `tar.exe`, keeps
only `x64\7za.exe` and `License.txt` in `%hMailServerLibs%\7zip-<target>`, and deletes the
archive and the extract folder. It mirrors progress to `libraries\build-7zip.log` (git-ignored
via `*.log`). Only proceed if it exits 0.

A hash mismatch is a hard stop — the script discards the download. Do not "fix" it by pasting in
the hash that was actually received; work out why it differs first.

### 4. Rewrite the pinned version references

**Skip for a pure re-fetch.** Otherwise replace `7zip-<old>` with `7zip-<new>` and the old
`-Version` argument with the new one:

| File | Occurrences |
|------|-------------|
| `hmailserver/source/Server/hMailServer/post-build.bat` | 1 (the `xcopy` of `7za.exe`) |
| `hmailserver/installation/hMailServer64.iss` | 1 (`SEVENZIP_PATH`) |
| `README.md` | 2 (the `e.g. 7zip-<ver>` example and the `-Version` argument in "Fetching 7-Zip") |

Then grep the repo for `7zip-\d` and `<old version>` and confirm the only remaining matches are
the ones you just rewrote plus the illustrative examples in `libraries\build-7zip.ps1`'s own
comment-based help. There must be **zero** references to the old concrete version in the three
files above.

**No CI or workflow edit is needed.** `build\Get-LibraryVersions.ps1` parses the version back out
of `post-build.bat` and emits `sevenzip` / `sevenzip_dir`, which
`.github/workflows/build.yml` uses for the cache key and the fetch step. Bumping `post-build.bat`
therefore invalidates the CI cache and re-fetches automatically. Confirm with:

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File build\Get-LibraryVersions.ps1
```

It must report the new `sevenzip=<target>` / `sevenzip_dir=7zip-<target>`.

### 5. Keep build instructions and licensing current

- The `README.md` "Fetching 7-Zip" section documents what the script automates; step 4 covers its
  version numbers. Only if the release changed the package layout (the `x64\7za.exe` or
  `License.txt` paths inside the archive) update both `libraries\build-7zip.ps1` and the README.
  State whether a change was needed.
- **Diff the new `License.txt`** against the previously shipped one. It is installed as
  `{app}\Bin\7za.exe.license.txt` by `section_files_common.iss`, so a licensing change matters:

```powershell
Get-Content (Join-Path $env:hMailServerLibs '7zip-<target>\License.txt') -TotalCount 20
```

  7-Zip Extra has been LGPL for a long time; if that ever changes, stop and raise it with the
  user rather than shipping it.
- `hmailserver/installation/License.rtf`, shown during install, also carries a 7-Zip section.
  It is stale (it still reads "Copyright (C) 1999-2010" and describes the "7-Zip Command line
  version"). Leave it alone unless the user asks — editing legal text in an RTF is a deliberate
  decision, not upgrade housekeeping — but mention it if it has not been raised before.

### 6. Verify

Confirm the fetched binary is the version you asked for and still runs standalone (it must not
need `7za.dll`, which is deliberately not shipped):

```powershell
$e = Join-Path $env:hMailServerLibs '7zip-<target>\7za.exe'; (Get-Item $e).VersionInfo.FileVersion; & $e i | Select-Object -First 2
```

Then build hMailServer and run its post-build step, which copies `7za.exe` into the output dir:

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File build\build.ps1
powershell.exe -NoProfile -ExecutionPolicy Bypass -File build\post-build.ps1
```

`post-build.ps1` needs elevation and prompts via UAC. A successful copy confirms the
`post-build.bat` path matches what the fetch produced.

Finally — and this is the check that actually matters — run the backup/restore regression tests,
which drive a real backup and restore through the COM API and so exercise `7za.exe` end to end:

```
hmailserver\test\RegressionTests\packages\NUnit.ConsoleRunner.<version>\tools\nunit3-console.exe hmailserver\test\RegressionTests\bin\x64\Debug\RegressionTests.dll --where "class =~ BackupRestore"
```

They need an installed and running hMailServer. A version bump that breaks the command-line
syntax shows up here and nowhere else.

If the installer is being built too, compiling `hmailserver\installation\hMailServer64.iss` with
ISCC validates that both `{#SEVENZIP_PATH}\7za.exe` and `{#SEVENZIP_PATH}\License.txt` resolve.

## Reference: all 7-Zip touch points

- **Version-pinned (edited by steps 2 and 4):** `libraries\build-7zip.ps1` (`$ArchiveHashes`),
  `hmailserver/source/Server/hMailServer/post-build.bat`,
  `hmailserver/installation/hMailServer64.iss`, `README.md`.
- **Derived automatically (never edit for a version bump):** `build\Get-LibraryVersions.ps1`,
  `.github/workflows/build.yml`.
- **Not version-bearing (leave alone):** the `7za.exe` file name in
  `Compression::GetExecutableFullPath_` (`Common/Util/Compression.cpp`); the `7za.exe` and
  `7za.exe.license.txt` entries in `hmailserver/installation/section_files_common.iss`; the
  compression flags in `Compression::AddFile` / `AddDirectory`; the backup archive extension
  `.7z` in `Common/Application/BackupExecuter.cpp`, `BackupManager.cpp` and the Administrator's
  `ucBackup.Designer.cs` file filter; `hmailserver/documentation/book_maintenance/backup_restore.md`.

## Note

The script needs only `%hMailServerLibs%`, network access to github.com, and the Windows-bundled
`tar.exe` (Windows 10/11). It validates these and fails early with a clear message if any is
missing. No Visual Studio, CMake or Perl is involved.
