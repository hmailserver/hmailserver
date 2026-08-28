---
name: upgrade-mariadb
description: >-
  Build MariaDB Connector/C for hMailServer and/or bump the pinned version, for
  3.4.x (e.g. rebuild 3.4.9 against a new OpenSSL, or upgrade 3.4.9 -> 3.4.10).
  Builds libmariadb.dll into %hMailServerLibs% via libraries\build-mariadb.ps1
  (linking it against the OpenSSL hMailServer uses), rewrites every pinned MariaDB
  Connector/C version reference in the repo, keeps the README build instructions
  current, and verifies hMailServer still builds and ships the DLL. Use when asked to
  build, rebuild, bump, update, or upgrade the MySQL/MariaDB client library for
  hMailServer. Supports MariaDB Connector/C 3.4.x only.
---

# Build / upgrade MariaDB Connector/C (3.4.x) for hMailServer

hMailServer connects to MySQL and MariaDB through MariaDB Connector/C. The connector is not
vendored in the repo; `libmariadb.dll` is built locally into
`%hMailServerLibs%\libmariadb-<version>\build64\libmariadb\RelWithDebInfo`, copied into the
build output by `post-build.bat`, and shipped in `Bin` by the installer. Its concrete version
is pinned in a few build/config files. This skill builds the connector (linking it against the
OpenSSL hMailServer uses), rewrites those references when the version changes, and verifies the
result.

There are two reasons to run it:

- **Rebuild the connector** at the *current* pinned version — most often to relink it against a
  freshly built OpenSSL (see the OpenSSL skill's client-library note). No version rewrites needed.
- **Bump the version** (e.g. `3.4.9 -> 3.4.10`) — build the new version *and* rewrite the pinned
  references.

## Scope

- **MariaDB Connector/C 3.4.x only.** Reject anything not matching `^3\.4\.\d+$`. Other release
  lines use different CMake options and plugin names and are not validated here; if the requested
  target is outside 3.4.x, stop and tell the user it is out of scope.
- The connector links against OpenSSL at build time (`-DWITH_SSL=OPENSSL`), so **a matching
  OpenSSL build must already exist** under `%hMailServerLibs%\openssl-<ver>\out64` (build it
  first with the upgrade-openssl skill / `build-openssl.ps1`).
- Updating any CI/CD-built copy of the connector is out of scope, but the README build
  instructions must stay accurate.

## Steps

### 1. Determine current and target version

- Read the current version by searching
  `hmailserver/source/Server/hMailServer/post-build.bat` for `libmariadb-3\.4\.\d+`
  (e.g. `libmariadb-3.4.9`).
- Take the target version from the user (e.g. `3.4.10`). For a pure rebuild, the target equals
  the current version.
- **Validate the target matches `^3\.4\.\d+$`.** Abort with the out-of-scope message above if
  it does not.
- Read the OpenSSL version the connector should link against from
  `hmailserver/source/Server/hMailServer/hMailServer.vcxproj` (`openssl-<ver>`). The build
  script auto-detects this same value, so you normally do not need to pass it.

### 2. Build the connector

Invoke the build script (prerequisites — `%hMailServerLibs%`, an existing
`openssl-<ver>\out64`, CMake on PATH, VS2019 x64 tools — are validated inside it; surface its
error message if it fails):

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-mariadb.ps1 -Version <target>
```

Pass `-OpenSSLVersion <ver>` only to override the auto-detected value.

**Run this through the native PowerShell tool, NOT the Bash tool.** The Bash tool prepends
MSYS/Git (`/usr/bin`) to `PATH`, which puts a GNU `tar` ahead of the Windows `bsdtar` (GNU tar
misreads the `C:` in `C:\...` as a remote rmt host — "Cannot connect to C: resolve failed") and
can shadow other Windows tools. The native PowerShell tool has a clean Windows `PATH`.

The build takes several minutes and the PowerShell tool only returns its captured output once
it finishes, so the console can look stalled. The script mirrors every step to
`libraries\build-mariadb.log` (git-ignored via `*.log`) as it runs. Tell the user they can watch
progress live from another shell with `Get-Content libraries\build-mariadb.log -Wait`, and if the
script fails, read that log for the full transcript rather than relying on the (often
truncated) captured console output.

On success it produces
`%hMailServerLibs%\libmariadb-<target>\build64\libmariadb\RelWithDebInfo\libmariadb.dll`. The
script itself asserts that the DLL imports `libssl-3-x64.dll` and `libcrypto-3-x64.dll`, so a
zero exit code already proves it was built against OpenSSL. Only proceed if it exits 0.

The CMake configure output lists the plugin layout; confirm it reads

```
-- Static PLUGINS mysql_native_password;client_ed25519;caching_sha2_password;sha256_password;parsec;...
```

Those static plugins are the point of the build — they let the connector authenticate against
MySQL 8.x (`caching_sha2_password`) with no plugin directory and no extra DLLs.

### 3. Rewrite the pinned version references

**Skip this step for a pure rebuild** (target == current). Otherwise replace the old version
with the new one (`libmariadb-<old>` -> `libmariadb-<new>`) in these files:

| File | Occurrences |
|------|-------------|
| `hmailserver/source/Server/hMailServer/post-build.bat` | 1 (the `xcopy` of `libmariadb.dll`) |
| `hmailserver/installation/hMailServer64.iss` | 1 (`MARIADB_LIBS_PATH`) |
| `README.md` | 1 (the `-Version` argument in "Building MariaDB Connector/C") |

Then grep the whole repo for `libmariadb-\d` and confirm the only remaining matches are the
intentional ones you just rewrote — there must be **zero** references to the old concrete
version.

**Do not** change the `libmariadb.dll` file name — it appears in `section_files_64.iss`, in
`MySQLInterface::GetLibraryFileName_` and its 5094 error message
(`Common/SQL/MySQLInterface.cpp`), and in the VM test runner's `TestEnvironments.json`. None of
those are version-bearing. hMailServer resolves every entry point with `GetProcAddress`, so
there is no header, import lib or project setting to update either.

### 4. Keep build instructions current

The `README.md` "Building MariaDB Connector/C" section documents the recipe the script
automates. On a version bump, update its version number (step 3 already covers the
`-Version` argument). Skim the new release's notes; only if the connector changed its build
process (CMake options, plugin names, output paths) update both `libraries\build-mariadb.ps1`
and the README accordingly. State whether a change was needed.

### 5. Verify

Build hMailServer and run its post-build step, which copies the new DLL into the output dir:

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File build\build.ps1
powershell.exe -NoProfile -ExecutionPolicy Bypass -File build\post-build.ps1
```

`post-build.ps1` needs elevation and prompts via UAC. A successful copy confirms the
`post-build.bat` path is consistent with what the build script produced.

Then confirm the DLL still exports every symbol `MySQLInterface::Load` resolves — the connector
is loaded with `LoadLibrary` + `GetProcAddress`, so a missing export is a runtime failure, not a
link error:

```
powershell.exe -NoProfile -Command "$d = Get-ChildItem 'C:\Program Files*\Microsoft Visual Studio\*\*\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe' | Select-Object -First 1; $names = & $d.FullName /nologo /exports \"$env:hMailServerLibs\libmariadb-<new>\build64\libmariadb\RelWithDebInfo\libmariadb.dll\" | ForEach-Object { if ($_ -match '^\s+\d+\s+[0-9A-F]+\s+[0-9A-F]{8}\s+(\S+)') { $matches[1] } }; 'mysql_real_connect','mysql_close','mysql_init','mysql_error','mysql_query','mysql_store_result','mysql_free_result','mysql_insert_id','mysql_errno','mysql_num_rows','mysql_fetch_row','mysql_num_fields','mysql_fetch_field_direct','mysql_get_server_version','mysql_options' | Where-Object { $names -notcontains $_ }"
```

It must print nothing. Finally, run the regression tests against a MySQL or MariaDB database
(`build\run-tests.ps1`) to prove the connector actually authenticates and queries.

## Reference: all MariaDB Connector/C touch points

- **Version-pinned (edited by step 3):** `post-build.bat`, `hMailServer64.iss`, `README.md`.
- **Build recipe (edited only if the release changes it):** `libraries\build-mariadb.ps1`,
  `README.md`.
- **Not version-bearing (leave alone):** the `libmariadb.dll` name in `section_files_64.iss`,
  `Common/SQL/MySQLInterface.cpp` (the probe path and the 5094 message) and
  `hmailserver/test/VMTestRunner.Console/TestEnvironments.json`; the hardcoded option values
  `MYSQL_OPT_SSL_VERIFY_SERVER_CERT` = 21 and `MYSQL_OPT_SSL_ENFORCE` = 38 in
  `Common/SQL/MySQLConnection.cpp` (re-verify them against `include/mysql.h` only if the
  connector's major/minor version changes); `libraries\License - MariaDB Connector C.md`.

## Note

The script's own end-to-end run needs the source download, the VS2019 x64 toolchain, CMake, and
a matching `openssl-<ver>\out64` build present; it validates those prerequisites and fails early
with a clear message if any is missing.
