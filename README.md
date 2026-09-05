hMailServer
===========

hMailServer is an open source email server for Microsoft Windows.

This page describes how to compile and run hMailServer in debug. 

For other information about hMailServer, please go to http://www.hmailserver.com

Building hMailServer
====================

Branches
--------

   * The master branch contains the latest development version of hMailServer. This version is typically not yet released for production usage. If you want to add new features to hMailServer, use this branch.
   
   * The x.y.z (for example 5.6.2) contains the code for the version with the same name as the branch. For example, branch 5.6.1 contains hMailServer version 5.6.1. These branches are typically only used for bugfixes or minor features.

Environment set up
---------------------

**Required software**

   * An installed version of hMailServer 5.7 (configured with a database)
   * Visual Studio 2026 Community edition with the v142 (VS2019) build tools
   * InnoSetup 6.2.2 (https://github.com/jrsoftware/issrc/releases/tag/is-6_2_2) - the last
     release whose installers run on Windows Vista, which hMailServer still supports
   * Perl 5 (https://strawberryperl.com/) - required by the OpenSSL and PostgreSQL library builds
   * CMake (https://cmake.org/download/) - unless Visual Studio's "C++ CMake tools for Windows" component is installed
   
**NOTE**

You should not be compiling hMailServer on a computer which already runs a production version of hMailServer. When compiling hMailServer, the compilation will stop any already running version of hMailServer, and will register the compiled version as the hMailServer version on the machine (configuring the Windows service). This means that if you are running a production version of hMailServer on the machine, this version will stop running if you compile hMailServer. If this happens, the easiest path is to reinstall the production version.

Installing Visual Studio
------------------------

hMailServer's C++ projects are built with the v142 platform toolset. Visual Studio 2026
provides it as an optional component, so it has to be selected during installation.

**Visual Studio 2026**

1. Download [Visual Studio 2026](https://visualstudio.microsoft.com/vs/) and launch the installation.
2. Select the following _Workloads_
  * .NET desktop development
  * Desktop development with C++
3. Select the following _Individual components_
  * MSVC v142 - VS 2019 C++ x64/x86 build tools
  * C++ v14.29 (16.11) ATL for v142 build tools (x86 & x64)

The build scripts locate the installed Visual Studio automatically and select the v142
compiler out of it.

3rd party libraries
-------------------

Some 3rd party libraries which hMailServer relies on are large and updated frequently. Rather than including these large libraries into the hMailServer git repository, they have to be downloaded and built, currently manually. When you build hMailServer, Visual Studio will use a system environment variable, named hMailServerLibs, to locate these libraries.

Create an environment variable named hMailServerLibs pointing at a folder where you will store hMailServer libraries, such as C:\Dev\hMailLibs.

Building OpenSSL
----------------
OpenSSL 3.5.x is built by the `libraries\build-openssl.ps1` script, which downloads the
requested version into %hMailServerLibs%\openssl-&lt;Version&gt; and builds it into an `out64`
install prefix (headers, import libs and `libcrypto-3-x64.dll` / `libssl-3-x64.dll`).

Prerequisites:
- The environment variable hMailServerLibs (see above).
- Perl (e.g. [Strawberry Perl](https://strawberryperl.com/)) on PATH - required by OpenSSL's Configure.
- Visual Studio 2026 with the x64 C++ build tools (the script locates vcvars64.bat automatically).

Run, from the repository root:

   <pre>
   powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-openssl.ps1 -Version 3.5.x
   </pre>

Only OpenSSL 3.5.x is supported; the build recipe for 3.0.x and 4.x differs.

Building PostgreSQL
-------------------
hMailServer talks to PostgreSQL through libpq. libpq is built by the `libraries\build-pgsql.ps1`
script, which downloads the requested version into %hMailServerLibs%\postgresql-&lt;Version&gt;,
generates the `src\tools\msvc\config.pl` that links libpq against a previously built OpenSSL, and
builds `libpq.dll` / `libpq.lib` into `postgresql-&lt;Version&gt;\Release\libpq`.

Prerequisites:
- The environment variable hMailServerLibs (see above).
- A matching OpenSSL build (`openssl-&lt;Version&gt;\out64`) already present - build it first with the OpenSSL script above.
- Perl (e.g. [Strawberry Perl](https://strawberryperl.com/)) on PATH - required by PostgreSQL's build.pl.
- Visual Studio 2026 with the x64 C++ build tools.

Run, from the repository root:

   <pre>
   powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-pgsql.ps1 -Version 15.19
   </pre>

The script auto-detects the OpenSSL version to link against from the hMailServer project; pass
`-OpenSSLVersion 3.5.x` to override it. Only PostgreSQL 15.x and 16.x are supported (17 removed
the `src\tools\msvc\build.pl` build system this relies on).

Building MariaDB Connector/C
----------------------------
hMailServer talks to MySQL and MariaDB through MariaDB Connector/C, and ships its
`libmariadb.dll` in the Bin directory. It is built by the `libraries\build-mariadb.ps1` script,
which downloads the requested version into %hMailServerLibs%\libmariadb-&lt;Version&gt;, configures it
with CMake against a previously built OpenSSL, and builds
`libmariadb.dll` into `libmariadb-&lt;Version&gt;\build64\libmariadb\RelWithDebInfo`. The authentication
plugins are compiled into the DLL, so no plugin files have to be shipped alongside it.

Prerequisites:
- The environment variable hMailServerLibs (see above).
- A matching OpenSSL build (`openssl-&lt;Version&gt;\out64`) already present - build it first with the OpenSSL script above.
- CMake - either on PATH, or Visual Studio's "C++ CMake tools for Windows" component. MariaDB Connector/C has no other build system.
- Visual Studio 2026 with the x64 C++ build tools.

Run, from the repository root:

   <pre>
   powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-mariadb.ps1 -Version 3.4.9
   </pre>

The script auto-detects the OpenSSL version to link against from the hMailServer project; pass
`-OpenSSLVersion 3.5.x` to override it. Only MariaDB Connector/C 3.4.x is supported.

Building Boost
--------------
hMailServer links statically against a handful of compiled Boost libraries (thread, filesystem,
regex, chrono, atomic; Boost.System is header-only as of Boost 1.92). Boost is built by the
`libraries\build-boost.ps1` script, which
downloads the requested version into %hMailServerLibs%\boost_&lt;Version&gt; (e.g. `boost_1_92_0`),
bootstraps `b2`, and builds the static, multithreaded x64 libraries into `stage\lib`.

Prerequisites:
- The environment variable hMailServerLibs (see above).
- Visual Studio 2026 with the "MSVC v142 build tools" component (the
  script locates vcvars64.bat automatically and drives `b2` with the msvc-14.2 toolset, which is
  the toolset hMailServer itself is built with).

Run, from the repository root:

   <pre>
   powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-boost.ps1 -Version 1.92.0
   </pre>

Pass `-Toolset &lt;name&gt;` to override the default `msvc-14.2`, or `-Jobs &lt;n&gt;` to change the
number of parallel compilations (defaults to the number of logical processors). Only Boost 1.x is
supported.

Fetching 7-Zip
--------------
The backup and restore feature launches the 7-Zip command line tool to compress and read
backup archives. It is a prebuilt binary rather than something hMailServer compiles, so the
`libraries\build-7zip.ps1` script downloads the "7-Zip Extra" package into
%hMailServerLibs%\7zip-&lt;Version&gt; (e.g. `7zip-26.03`) and keeps the x64 `7za.exe` and its
LGPL license text.

Prerequisites:
- The environment variable hMailServerLibs (see above).
- Windows 10 or 11 - the bundled tar.exe is used to unpack the download.

Run, from the repository root:

   <pre>
   powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-7zip.ps1 -Version 26.03
   </pre>

The download is checked against a SHA-256 pinned in the script, so only versions listed
there are accepted. To move to a newer 7-Zip, record its hash in `$ArchiveHashes` and update
the `7zip-&lt;Version&gt;` folder name in `post-build.bat` and `hMailServer64.iss`.

Building hMailServer
--------------------

Visual Studio must be started with _Run as Administrator_.

1. Download the source code from this Git repository.
2. Compile the solution hmailserver\source\Server\hMailServer\hMailServer.sln.
   This will build the hMailServer server-part (hMailServer.exe)
3. Compile the solution hmailserver\source\Tools\hMailServer Tools.sln.
   This will build hMailServer related tools, such as hMailServer Administrator and hMailServer DB Setup.
4. Compile hmailserver\installation\hMailServer64.iss (using InnoSetup)
   This will build the hMailServer installation program.

   The installer ships two runtimes beside hMailServer.exe. The Visual C++ runtime is taken
   from the toolset that compiled it, so point the environment at it first:

   ```
   . build\Get-VCRedistPath.ps1
   $env:VCRedistPath = Get-VCRedistPath
   ```

   The Universal CRT, for the Windows versions older than 10 that have none of their own, is
   checked in under hmailserver\installation\Microsoft.UCRT.WindowsSDK14393 and needs no
   setup. Do not replace it with a newer Windows SDK redistributable. 10.0.14393 is the
   newest Universal CRT Microsoft supports on Windows Vista through 8.1, and above that the
   Vista loader cannot resolve the forwarders they use - hMailServer.exe then fails to start
   with an access violation. build\Test-DownlevelForwarders.ps1 checks for that.

   The files come from the Windows 10 SDK version 1607 (10.0.14393), which installs them to
   Windows Kits\10\Redist\ucrt\DLLs\x64. Download it from
   https://learn.microsoft.com/en-us/windows/apps/windows-sdk/downloads-archive

Running in Debug
----------------

If you want to run hMailServer in debug mode in Visual Studio, add the command argument /debug. You find this setting in the Project properties, under Configuration Properties -> Debugging.

Running tests
-------------

hMailServer source code contains a number of automated tests which excercises the basic functionality. When adding new features or fixing bugs, corresponding tests should be added. hMailServer tests are implemented using NUnit. To run them in Visual Studio, follow these steps:

NOTE: When running tests, your local hMailServer installation will be updated with test accounts. Existing domains and accounts are deleted. Each tests prepares the server configuration in different ways. In other words, do not run the automated tests in an environment where you need to preserve hMailServer data.

1. Make sure hMailServer.exe is built and can be run. The tests will launch the service.
2. Open the test solution, `\hmailserver\test\hMailServer Tests.sln`
3. In Visual Studio, select Test Explorer from the View-menu. 
4. Locate a test to run under "RegressionTests"
5. Right-click on a test or test category and select "Run".

You can also navigate to the source code for a test, right-click anywhere and select "Run Test(s)" to run it.

Releasing hMailServer
=====================

Without finding any serious issues:

1. Run all integration tests on supported versions of Windows and the different supported databases. 
2. Run all server stress tests
3. Enable Gflags (gflags /p /enable hmailserver.exe) and run all integration tests to check for memory issues
4. Run for at least 1 week in production for hMailServer.com
5. Wait for at least 500 downloads of the beta version
