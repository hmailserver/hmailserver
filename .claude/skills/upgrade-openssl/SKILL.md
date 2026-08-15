---
name: upgrade-openssl
description: >-
  Upgrade the OpenSSL version hMailServer builds against, for 3.5.x patch bumps
  (e.g. 3.5.5 -> 3.5.7). Builds the new OpenSSL into %hMailServerLibs% via
  libraries\build-openssl.ps1, rewrites every pinned OpenSSL version reference in
  the repo, keeps the README build instructions current, and verifies hMailServer
  still compiles and links. Use when asked to bump, update, or upgrade OpenSSL for
  hMailServer. Supports OpenSSL 3.5.x only.
---

# Upgrade OpenSSL (3.5.x) for hMailServer

This skill upgrades the OpenSSL version hMailServer links against. OpenSSL is not
vendored in the repo; it is built locally into `%hMailServerLibs%\openssl-<version>\out64`,
and its concrete version is pinned in a few build/config files. This skill builds the new
version, rewrites those references, and verifies the build.

## Scope

- **OpenSSL 3.5.x only.** If the requested target is 3.0.x or 4.x, stop and tell the user
  it is out of scope — the build recipe (Configure flags, DLL/lib sonames) differs and this
  skill is not validated for it.
- Updating the CI/CD-built copy of OpenSSL is out of scope, but the README build
  instructions must stay accurate (they reference `libraries\build-openssl.ps1`, so they
  rarely change).

## Steps

### 1. Determine current and target version

- Read the current version by searching `hmailserver/source/Server/hMailServer/post-build.bat`
  for `openssl-3\.5\.\d+` (e.g. `openssl-3.5.5`).
- Take the target version from the user (e.g. `3.5.7`).
- **Validate the target matches `^3\.5\.\d+$`.** Abort with the out-of-scope message above
  if it does not.
- If current == target, there is nothing to do — say so and stop.

### 2. Build the new OpenSSL

Invoke the build script (prerequisites — `%hMailServerLibs%`, Perl on PATH, VS2019 x64
tools — are validated inside it; surface its error message if it fails):

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-openssl.ps1 -Version <target>
```

**Run this through the native PowerShell tool, NOT the Bash tool.** The Bash tool prepends
MSYS/Git (`/usr/bin`) to `PATH`, which shadows Strawberry Perl with an MSYS Perl that is
missing modules OpenSSL's `Configure` needs (e.g. `Locale::Maketext::Simple`), and puts a GNU
`tar` ahead of the Windows `bsdtar` (GNU tar misreads the `C:` in `C:\...` as a remote rmt
host — "Cannot connect to C: resolve failed"). The native PowerShell tool has a clean Windows
`PATH` where `perl` resolves to `C:\Strawberry\perl\bin\perl.exe`. The script also pins tar to
`%SystemRoot%\System32\tar.exe` defensively, but Perl must still come from the Windows PATH.

The build takes several minutes and the PowerShell tool only returns its captured output
once it finishes, so the console can look stalled. The script mirrors every step to
`libraries\build-openssl.log` (git-ignored) as it runs. Tell the user they can watch progress
live from another shell with `Get-Content libraries\build-openssl.log -Wait`, and if the
script fails, read that log for the full transcript rather than relying on the (often
truncated) captured console output.

On success it produces `%hMailServerLibs%\openssl-<target>\out64` containing
`bin\libcrypto-3-x64.dll`, `bin\libssl-3-x64.dll`, `include`, and `lib`. Only proceed if the
script exits 0.

### 3. Rewrite the pinned version references

Replace the old version with the new one (`openssl-<old>` -> `openssl-<new>`) in these files:

| File | Occurrences |
|------|-------------|
| `hmailserver/source/Server/hMailServer/post-build.bat` | 2 (the two `xcopy` lines) |
| `hmailserver/source/Server/hMailServer/hMailServer.vcxproj` | 4 (include-dir + lib-dir, across Debug and Release) |
| `hmailserver/installation/hMailServer64.iss` | 1 (`OPENSSL_LIBS_PATH`) |

Then grep the whole repo for `openssl-3\.5\.\d+` and confirm the only remaining matches are
the intentional `openssl-3.5.x` placeholder in `README.md` and any license text — there must
be **zero** references to the old concrete version in the three files above.

**Do not** change the `libcrypto-3-x64.dll` / `libssl-3-x64.dll` DLL names or the
`libcrypto.lib` / `libssl.lib` import-lib names (in `hMailServer.vcxproj` linker deps and
`hmailserver/installation/section_files_64.iss`). The `-3-` soname is stable across all
OpenSSL 3.x, so it does not change on a 3.5.x bump.

### 4. Keep build instructions current

The `README.md` "Building OpenSSL" section references `libraries\build-openssl.ps1` and uses
a generic `3.5.x` placeholder, so a patch bump normally needs no README edit. Skim the new
release's notes; only if OpenSSL changed the build process (Configure flags, `nmake` targets,
DLL names) update both `libraries\build-openssl.ps1` and the README accordingly. State
whether a change was needed.

### 5. libpq note (usually no action)

libpq (in `%hMailServerLibs%\postgresql-15.18\Release\libpq`) is compiled against OpenSSL but
loads `libcrypto-3-x64.dll` / `libssl-3-x64.dll` at runtime. The OpenSSL 3.5.x line is
ABI-stable, so the prebuilt `libpq.dll` does **not** need rebuilding for a patch bump. If a
rebuild is ever required, follow README's "Building PostgreSQL" section, pointing its
`config.pl` `openssl => ...\openssl-<new>\out64` entry at the newly built OpenSSL.

### 6. Verify

Build hMailServer to confirm it compiles and links against the new OpenSSL:

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File build\build.ps1
```

The link step pulls `libssl.lib` / `libcrypto.lib` from the new `openssl-<new>\out64\lib`, so
a successful link confirms the reference updates are consistent. `post-build.ps1` (needs
elevation) copies the new DLLs into the output dir; for deeper DLL-linkage checks use the
[Dependencies](https://github.com/lucasg/Dependencies/releases) tool per the README TIP.

## Reference: all OpenSSL touch points

- **Version-pinned (edited by step 3):** `post-build.bat`, `hMailServer.vcxproj`,
  `hMailServer64.iss`.
- **Build recipe (edited only if the release changes it):** `libraries\build-openssl.ps1`,
  `README.md`.
- **Not version-bearing within 3.5.x (leave alone):** DLL/lib names in `section_files_64.iss`
  and the `hMailServer.vcxproj` linker dependencies; the OpenSSL version macros in
  `libraries/asio-overrides/boost/asio/ssl/impl/context.ipp` (only relevant on a major jump).
