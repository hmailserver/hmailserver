# hMailServer — Codebase Guide for Agents

hMailServer is a free, open-source email server for Microsoft Windows, licensed under AGPLv3. It implements SMTP, IMAP, and POP3, and runs as a Windows service. The codebase is written primarily in C++ (server core) and C# (admin tools), targeting the Visual Studio v142 toolset (VS2026 with the v142 build tools installed) with 64-bit Windows builds.

## Repository Layout

The repository root contains two top-level directories plus a handful of config files:

```
hmailserver/          # All source code, tools, tests, installation scripts
libraries/            # Vendored third-party libraries checked into the repo
build/                # Scripts for building and running tests.
.editorconfig
.gitattributes
.gitignore
README.md
```

Everything of substance lives under `hmailserver/`.

---

## `hmailserver/` — Top-Level Structure

```
hmailserver/
  installation/       # InnoSetup scripts that produce the installer .exe
  source/             # All source code
```

### `installation/`

Contains InnoSetup 6 scripts (`.iss`) and third-party components bundled with the installer. The installer is built as the final step of the release pipeline. Do not add business logic here; this directory is purely packaging.

---

## `hmailserver/source/`

```
source/
  Addons/             # Various scripts, for example, default event scripts
  DBScripts/          # SQL scripts: create DB schema, upgrade old DBs
  Migration/          # Unpublished tools for migrating from other mail servers
  Server/             # The hMailServer service — main C++ codebase
  Tools/              # C# GUI tools (Administrator, DBSetup, etc.)
```

---

## `source/Server/` — The Core Mail Server

This is where almost all feature work happens. The entry point is the Visual Studio solution `source/Server/hMailServer/hMailServer.sln`.

```
Server/
  COM/                # COM/API layer (hMailServer public API)
  common/             # Shared C++ infrastructure used by all server components
  ExternalFetcher/    # POP3 download from remote servers (fetch-from-external)
  hMailServer/        # Service entry point (WinMain, Windows service wiring)
  IMAP/               # IMAP protocol implementation
  POP3/               # POP3 protocol implementation
  SMTP/               # SMTP protocol implementation
```

### `Server/COM/`

Defines and implements the COM interfaces that make up the hMailServer public API (the `.idl` file lives here). The hMailServer Administrator GUI, DBSetup, and any external scripts all talk to the server through this COM API. When adding a new configurable feature, the COM interface typically needs a new property or method.

### `Server/Common/`

Shared infrastructure used by SMTP, IMAP, POP3, and the COM layer. Broken into focused sub-folders:

| Sub-folder | Purpose |
|---|---|
| `AntiSpam/` | Anti-spam algorithms: SPF validation, SURBL lookups, DNS blacklists, greylisting, and score-based filtering |
| `AntiVirus/` | Anti-virus software integrations (Custom scanner, ClamAV scanner)|
| `Application/` | Application startup, configuration loading, scheduling, and logging |
| `BO/` | Business objects — domain-model classes for domains, accounts, aliases, distribution lists, rules, etc. This is the core domain layer |
| `Cache/` | In-memory caches that wrap the BO layer to reduce database round-trips |
| `Diagnostics/` | Diagnostic checks exposed through the Administrator GUI ("Run diagnostics") |
| `Mime/` | MIME encoding and decoding for message parsing and construction |
| `Persistence/` | ORM-like persistence layer — one class per business object, maps to DB columns |
| `Scripting/` | Server-side event hooks that fire VBScript/JScript event handlers |
| `SQL/` | Low-level database abstraction: connection management, parameterised queries; supports MySQL, MS SQL, and SQLite |
| `TCPIP/` | Async networking using Boost.Asio: DNS resolution, connection management, TLS |
| `Threading/` | Thread pool, task queuing, and thread lifecycle management |
| `Tracking/` | Publish/subscribe notification bus between server components |
| `Util/` | Miscellaneous utilities; a mixed bag of helpers that don't fit elsewhere |

When adding a feature that persists new data, touch `BO/` for the domain object, `Persistence/` for database mapping, and `Cache/` if hot-path reads are expected.

### `Server/SMTP/`

Full SMTP server implementation: incoming message reception, relay decisions, delivery queue, bounce handling, DKIM signing and verification, and outbound delivery. This is the most complex protocol module. Key concepts: the delivery queue is disk-backed; outbound delivery is asynchronous via Boost.Asio.

### `Server/IMAP/`

IMAP4rev1 server implementation including IDLE, SORT, Quota, and ACL extensions. Each IMAP command has a corresponding command-handler class. Folder hierarchy and message flags are stored in the database via the Persistence layer.

### `Server/POP3/`

POP3 server implementation. Simpler than IMAP; messages are read from the same storage backend.

### `Server/ExternalFetcher/`

Implements the "fetch mail from external POP3 accounts" feature, where hMailServer connects as a POP3 *client* to external servers and downloads messages on behalf of local accounts.

### `Server/hMailServer/`

The Windows service shell: `WinMain`, service control handler, and the code that registers and starts the service. Does not contain protocol or business logic.

---

## `source/Tools/` — C# Administration Tools

Built by the separate solution `source/Tools/hMailServer Tools.sln`.

```
Tools/
  Administrator/                # The hMailServer Administrator desktop GUI (C#/WinForms)
                                # See hmailserver\source\Tools\Administrator\README.md for more information about Administrator.
  DataDirectorySynchronizer/    # Tool for synchronizing data directory with database.
  DBSetup/                      # Interactive DB creation tool (uses the COM API)
  DBSetupQuick/                 # Headless/scripted DB setup variant
  DBUpdater/                    # Schema migration tool for upgrading existing databases
  Shared/                       # Logic shared between different tools
```

`Administrator/` talks exclusively to the server through the COM API (`Server/COM/`). `DBSetup` and `DBUpdater` also go through the COM API. No tool accesses the database directly.

---

## `source/Addons/`

Various scripts, for example, default event scripts

---

## `source/DBScripts/`

SQL scripts for creating the hMailServer database schema from scratch, and incremental upgrade scripts for migrating from one version to the next. These cover MySQL, MS SQL Server, and SQLite. When adding a new persistent field, add the corresponding `ALTER TABLE` statement here.

---

## `test/`

The automated test suite.

```
test/
  RegressionTests/    # Integration test suite (C#, talks to a live running server via COM)
  VolumeTests/        # Long-running volume tests (200 000 messages, 100 MB messages, ...)
  VMTestRunner.Console/ # Runs the regression tests in Hyper-V virtual machines
```

`RegressionTests/` is the primary test suite — it executes against a real hMailServer instance and exercises it end-to-end over SMTP, IMAP, and POP3. The full suite covers 500+ scenarios.

`RegressionTests/Stress/` holds stress tests: concurrency and malformed input. They are part of
`RegressionTests.dll` and run when the test runner is asked to include stress tests.

`VolumeTests/` is a separate assembly, built by `hMailServer Tests.sln`. Its tests push large
volumes through the server and take a long time to run, so they are never run automatically. Run
them by hand when needed:

```
nunit3-console.exe hmailserver\test\VolumeTests\bin\x64\Debug\VolumeTests.dll
```

**Every bug fix and every new feature needs a regression test.** Add it to `RegressionTests/`,
next to the existing tests for the same protocol or area. For a bug fix, write the test first and
confirm it fails before the fix, so it is known to actually reproduce the issue.

---

## `libraries/`

Vendored third-party C++ libraries checked directly into the repository. Large external dependencies (OpenSSL, Boost, libpq, MariaDB Connector/C) are **not** included here and must be built separately using the `hMailServerLibs` environment variable; this folder holds the `build-*.ps1` scripts that build them.

---

## Key Architectural Patterns

**COM API as the seam.** All configuration and management goes through the COM API in `Server/COM/`. The GUI, test suite, and external scripts all use this interface. New features should expose configuration through COM.

**Layered persistence.** Data flows: `BO/` (domain object) → `Persistence/` (SQL mapping) → `SQL/` (connection/query execution). The `Cache/` layer sits in front of `Persistence/` for frequently-read objects.

**Boost.Asio for all networking.** The `TCPIP/` module wraps Boost.Asio. Protocol implementations (SMTP, IMAP, POP3) use async I/O through this layer.

**Event scripting.** `EventHandlers/` allows administrators to run VBScript/JScript at server-defined hook points (e.g., on message receive). This is exposed as an optional, sandboxed scripting layer.

**Database portability.** The SQL layer abstracts over MySQL, MS SQL Server, and SQLite. Use parameterised queries exclusively; never construct SQL strings manually.

---

## Build Overview

| Artifact | Solution file |
|---|---|
| `hMailServer.exe` (the service) | `source/Server/hMailServer/hMailServer.sln` |
| Admin tools | `source/Tools/hMailServer Tools.sln` |
| Installer | `installation/hMailServer64.iss` (InnoSetup) |

**External dependencies** (not in repo): OpenSSL, Boost, PostgreSQL (libpq), MariaDB Connector/C (libmariadb). Set `%hMailServerLibs%` to the parent directory where these are built. See `README.md` for step-by-step build instructions.

**Branches:** `master` is the development branch for new features. Version branches (e.g., `5.6.2`) are used for bug fixes only.


# Running builds and tests

The below scripts will automatically locate prerequisites. They must be run using `powershell.exe` (not bash/sh). Use the `powershell.exe -NoProfile -ExecutionPolicy Bypass -File <script>` invocation from bash.

* Use `build/build.ps1` to build hMailServer server.
* Use `build/post-build.ps1` to copy DLLs and register the COM server after a successful build. It stops the
  service, copies the dependencies, registers the COM server if that is needed, and starts the service again.
  Only the registration needs elevation, and it is skipped when the build output is already registered and the
  type library is unchanged, so an ordinary rebuild does not prompt. Pass `-ForceRegister` to register anyway.
* Run `build/grant-service-control.ps1` once, elevated, to let your account start and stop the hMailServer
  service without a UAC prompt. Re-run it if the service is ever deleted and recreated.
* Use `build/build-tests.ps1` to build the regression test solution.
* Run the regression tests with the NUnit console runner from the restored packages, for example `hmailserver\test\RegressionTests\packages\NUnit.ConsoleRunner.<version>\tools\nunit3-console.exe hmailserver\test\RegressionTests\bin\x64\Debug\RegressionTests.dll`. They need an installed and running hMailServer, since they drive it through its COM API.
