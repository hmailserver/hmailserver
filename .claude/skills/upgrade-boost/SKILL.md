---
name: upgrade-boost
description: >-
  Build Boost for hMailServer and/or bump the pinned Boost version, for 1.x
  (e.g. rebuild 1.90.0, or upgrade 1.89.0 -> 1.90.0). Builds the static x64
  Boost libraries hMailServer links against into %hMailServerLibs% via
  libraries\build-boost.ps1, rewrites every pinned Boost version reference in the
  repo, keeps the README build instructions current, and verifies hMailServer
  still compiles and links. Use when asked to build, rebuild, bump, update, or
  upgrade Boost for hMailServer. Supports Boost 1.x only.
---

# Build / upgrade Boost (1.x) for hMailServer

hMailServer links against a handful of compiled Boost libraries (thread, filesystem, regex,
chrono, system, atomic) plus many header-only ones. Boost is not vendored in the repo; it is
built locally into `%hMailServerLibs%\boost_<underscored-version>` (e.g. `boost_1_90_0`), and
its concrete version is pinned in the project files and README. This skill builds Boost,
rewrites those references when the version changes, and verifies the build.

There are two reasons to run it:

- **Rebuild Boost** at the *current* pinned version — e.g. after switching VS toolset or a
  clean checkout of the libs folder. No version rewrites needed.
- **Bump the Boost version** (e.g. `1.89.0 -> 1.90.0`) — build the new version *and* rewrite
  the pinned references.

## Scope

- **Boost 1.x only** (built with the `bootstrap` + `b2` build system). Reject anything not
  matching `^1\.\d+\.\d+$`.
- Boost is linked **statically** (`link=static`), so the `.lib` files are pulled in at link
  time and there is **no Boost DLL** to copy in `post-build.bat` or ship in the installer —
  that is why Boost, unlike OpenSSL/libpq, has no touch points in `post-build.bat`,
  `section_files_64.iss`, or `hMailServer64.iss`.
- The compiled libs are built with the **msvc-14.2** toolset (v142 - VS2019, or VS2022 with the
  "MSVC v142 build tools" component), matching the
  rest of hMailServer. Updating any CI/CD-built copy of Boost is out of scope, but the README
  build instructions must stay accurate.

## Steps

### 1. Determine current and target version

- Read the current Boost version by searching
  `hmailserver/source/Server/hMailServer/hMailServer.vcxproj` for `boost_(\d+_\d+_\d+)`
  (e.g. `boost_1_90_0` -> `1.90.0`).
- Take the target version from the user (e.g. `1.90.0`). For a pure rebuild, the target equals
  the current version.
- **Validate the target matches `^1\.\d+\.\d+$`.** Abort with the out-of-scope message above if
  it does not.
- If current == target and this is not an explicit rebuild request, there is nothing to do —
  say so and stop.

### 2. Build Boost

Invoke the build script (prerequisites — `%hMailServerLibs%`, VS2019/VS2022 x64 tools — are validated
inside it; surface its error message if it fails):

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-boost.ps1 -Version <target>
```

Pass `-Toolset <name>` to override the default `msvc-14.2`, or `-Jobs <n>` to change the
parallelism (defaults to the number of logical processors).

**Run this through the native PowerShell tool, NOT the Bash tool.** The Bash tool prepends
MSYS/Git (`/usr/bin`) to `PATH`, which puts a GNU `tar` ahead of the Windows `bsdtar` (GNU tar
misreads the `C:` in `C:\...` as a remote rmt host — "Cannot connect to C: resolve failed"). The
native PowerShell tool has a clean Windows `PATH`. The script also pins tar to
`%SystemRoot%\System32\tar.exe` defensively.

The build takes several minutes and the PowerShell tool only returns its captured output once it
finishes, so the console can look stalled. The script mirrors every step to
`libraries\build-boost.log` (git-ignored via `*.log`) as it runs. Tell the user they can watch
progress live from another shell with `Get-Content libraries\build-boost.log -Wait`, and if the
script fails, read that log for the full transcript rather than relying on the (often truncated)
captured console output.

On success it produces `%hMailServerLibs%\boost_<underscored-target>` with the Boost headers
under `boost\` and the staged static libs under `stage\lib` (e.g.
`libboost_thread-vc142-mt-x64-1_90.lib`). Only proceed if the script exits 0.

### 3. Rewrite the pinned version references

**Skip this step for a pure rebuild** (target == current). Otherwise replace the old underscored
version with the new one (`boost_<old>` -> `boost_<new>`, e.g. `boost_1_89_0` -> `boost_1_90_0`)
in these files:

| File | Occurrences |
|------|-------------|
| `hmailserver/source/Server/hMailServer/hMailServer.vcxproj` | 4 (include-dir + lib-dir, across Debug and Release) |
| `hmailserver/source/Server/hMailServer.Minidump/hMailServer.Minidump.vcxproj` | 4 (include-dir + lib-dir, across Debug and Release) |
| `README.md` | 2 (the dotted "Boost 1.90.0" version in the download step and the `boost_1_90_0` example folder name) |

Then grep the whole repo for `boost_\d+_\d+_\d+` and the dotted `Boost \d+\.\d+\.\d+`, and
confirm the only remaining matches are the intentional ones you just rewrote — there must be
**zero** references to the old concrete version.

**Do not** change the `<Boost-Version>` placeholder in the README (it is generic) or the
`#include <boost/...>` directives throughout the source (not version-bearing).

### 4. Keep build instructions current

The `README.md` "Building Boost" section documents the manual `bootstrap` / `b2` recipe the
script automates (step 3 already covers the version strings). Skim the new release's notes; only
if Boost changed the build process (bootstrap/b2 flags, staged lib naming) update both
`libraries\build-boost.ps1` and the README accordingly. State whether a change was needed.

### 5. Verify

Build hMailServer to confirm it compiles and links against the new Boost:

```
powershell.exe -NoProfile -ExecutionPolicy Bypass -File build\build.ps1
```

The compile step pulls the Boost headers from `boost_<new>` and the link step pulls the static
libs from `boost_<new>\stage\lib`, so a successful build confirms the reference updates are
consistent. Because Boost is linked statically there is no DLL to copy — no `post-build` or
installer changes are involved.

## Reference: all Boost touch points

- **Version-pinned (edited by step 3):** `hMailServer.vcxproj`, `hMailServer.Minidump.vcxproj`,
  `README.md`.
- **Build recipe (edited only if the release changes it):** `libraries\build-boost.ps1`,
  `README.md`.
- **Not version-bearing (leave alone):** the `#include <boost/...>` directives across the source
  tree; the `$(BOOST_INCLUDE_PATH)` / `$(BOOST_LIBS_PATH)` MSBuild fallbacks in the project files;
  the `<Boost-Version>` placeholder in the README.

## Note

The script's own end-to-end run needs the Boost source download and the VS2019/VS2022 x64 toolchain
present; it validates those prerequisites and fails early with a clear message if any is missing.
