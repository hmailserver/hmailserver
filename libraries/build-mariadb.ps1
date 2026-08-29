<#
.SYNOPSIS
    Builds the MariaDB Connector/C client library (libmariadb.dll) for hMailServer.

.DESCRIPTION
    Downloads the MariaDB Connector/C source for the requested version into
    %hMailServerLibs%\libmariadb-<Version>, configures it with CMake against a
    previously built OpenSSL, and builds libmariadb.dll with the VS2019 x64 toolchain.
    The result is the file hMailServer ships in its Bin directory:
    libmariadb-<Version>\build64\libmariadb\RelWithDebInfo\libmariadb.dll.

    The authentication plugins hMailServer needs are compiled STATICALLY into the DLL
    (caching_sha2_password, sha256_password, client_ed25519, parsec, plus the
    mysql_native_password/mysql_old_password defaults) so no plugin directory and no
    extra plugin DLLs have to be shipped. The plugins that require user interaction or
    Kerberos (dialog, mysql_clear_password, auth_gssapi_client) are turned off.

    Only MariaDB Connector/C 3.4.x is supported; other release lines use different
    CMake options and are out of scope.

    Prerequisites (must be on PATH / installed):
      - The environment variable hMailServerLibs, pointing at your library folder.
      - A previously built OpenSSL under %hMailServerLibs%\openssl-<OpenSSLVersion>\out64
        (build it with build-openssl.ps1). It is the TLS backend of the client library.
      - CMake, either on PATH or from Visual Studio's "C++ CMake tools for Windows"
        component - MariaDB Connector/C has no other build system.
      - Visual Studio 2019, or Visual Studio 2022, with the x64 build tools (vcvars64.bat
        is located automatically via vswhere).

.PARAMETER Version
    The MariaDB Connector/C version to build, e.g. 3.4.9. Must match 3.4.x.

.PARAMETER OpenSSLVersion
    The OpenSSL version to link libmariadb against, e.g. 3.5.8. Must correspond to an
    existing %hMailServerLibs%\openssl-<OpenSSLVersion>\out64 build. If omitted, the
    script auto-detects it from hMailServer.vcxproj (the openssl-<ver> the project
    currently links against).

.EXAMPLE
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-mariadb.ps1 -Version 3.4.9 -OpenSSLVersion 3.5.8

.EXAMPLE
    # Auto-detect the OpenSSL version from hMailServer.vcxproj:
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-mariadb.ps1 -Version 3.4.9
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^3\.4\.\d+$')]
    [string]$Version,

    [Parameter(Mandatory = $false)]
    [ValidatePattern('^\d+\.\d+\.\d+$')]
    [string]$OpenSSLVersion
)

$ErrorActionPreference = "Stop"

# Handle native-command exit codes explicitly (checked after each step) rather than
# letting a nonzero exit or stderr text abort the pipeline on its own.
$PSNativeCommandUseErrorActionPreference = $false

# --- Set up a build log ---------------------------------------------------------

# CMake and MSBuild output is verbose and the host console may buffer it, so mirror
# every step to build-mariadb.log next to this script. This gives a file you can watch
# live from another shell to confirm the build is progressing:
#
#     Get-Content libraries\build-mariadb.log -Wait
#
# and a full transcript to inspect if a step fails. The logging helpers
# (Start-BuildLog, Write-Log, Invoke-BuildStep) are shared with the other build scripts.
. (Join-Path -Path $PSScriptRoot -ChildPath "build-common.ps1")

$logPath = Join-Path -Path $PSScriptRoot -ChildPath "build-mariadb.log"
Start-BuildLog -LogPath $logPath -Title "MariaDB Connector/C $Version build log"

# --- Resolve the library folder -------------------------------------------------

$libsPath = Resolve-HMailServerLibs

$srcDir   = Join-Path -Path $libsPath -ChildPath "libmariadb-$Version"
$buildDir = Join-Path -Path $srcDir -ChildPath "build64"

# --- Resolve the OpenSSL build to link against ----------------------------------

# If the caller did not pin an OpenSSL version, auto-detect the one the project
# currently links against from hMailServer.vcxproj (openssl-<ver>\out64). This keeps
# the client library's TLS backend in lockstep with the rest of hMailServer by default.
if ([string]::IsNullOrEmpty($OpenSSLVersion))
{
    $vcxproj = Join-Path -Path $PSScriptRoot -ChildPath "..\hmailserver\source\Server\hMailServer\hMailServer.vcxproj"
    if (!(Test-Path $vcxproj))
    {
        Throw "OpenSSLVersion was not supplied and hMailServer.vcxproj was not found at $vcxproj to auto-detect it. Pass -OpenSSLVersion explicitly."
    }
    $match = Select-String -Path $vcxproj -Pattern 'openssl-(\d+\.\d+\.\d+)' | Select-Object -First 1
    if ($null -eq $match)
    {
        Throw "Could not auto-detect the OpenSSL version from $vcxproj. Pass -OpenSSLVersion explicitly."
    }
    $OpenSSLVersion = $match.Matches[0].Groups[1].Value
    Write-Log "Auto-detected OpenSSL version $OpenSSLVersion from hMailServer.vcxproj"
}

$openSslOut = Join-Path -Path $libsPath -ChildPath "openssl-$OpenSSLVersion\out64"

if (!(Test-Path $openSslOut))
{
    Throw "The OpenSSL build to link libmariadb against was not found at $openSslOut. Build it first with build-openssl.ps1 -Version $OpenSSLVersion. Without it libmariadb would be built without OpenSSL support."
}

# --- Locate the Visual Studio build environment via vswhere --------------------

$vsInstall = Resolve-VcVars64

# --- Import the VS x64 build environment ---------------------------------------

# Imported before the CMake check below: Visual Studio's own cmake.exe (the
# "C++ CMake tools for Windows" component) is not on the ambient PATH, only on the
# one vcvars64.bat sets up. Importing first means either a standalone CMake or the
# one shipped with Visual Studio satisfies the prerequisite.
# libmariadb is a C library consumed through an import library and a DLL, so its ABI does not
# depend on the toolset; no -ToolsetVersion is needed (see Import-VsEnvironment).
Import-VsEnvironment -VsInstall $vsInstall

# --- Verify CMake is available --------------------------------------------------

if ($null -eq (Get-Command cmake -ErrorAction SilentlyContinue))
{
    Throw "CMake was not found on PATH, and Visual Studio's own copy was not either. MariaDB Connector/C is built with CMake only; either install it from https://cmake.org/download/ and make sure cmake.exe is on PATH, or add the 'C++ CMake tools for Windows' component to your Visual Studio installation."
}

# --- Download and extract the source (always a clean tree) ---------------------

# The GitHub tarball extracts to mariadb-connector-c-<Version>; normalise that to
# libmariadb-<Version> so the folder name matches the DLL the rest of the repo pins.
$tarUrl       = "https://github.com/mariadb-corporation/mariadb-connector-c/archive/refs/tags/v$Version.tar.gz"
$extractedDir = Join-Path -Path $libsPath -ChildPath "mariadb-connector-c-$Version"

if (Test-Path $srcDir)
{
    Write-Log "Removing existing source folder $srcDir for a clean build"
    Remove-Item -LiteralPath $srcDir -Recurse -Force
}

Get-SourceArchive -Url $tarUrl -SrcDir $extractedDir -LibsPath $libsPath

Write-Log "Renaming $extractedDir to $srcDir"
Rename-Item -LiteralPath $extractedDir -NewName "libmariadb-$Version"

# --- Configure and build libmariadb --------------------------------------------

Write-Log "Building MariaDB Connector/C $Version (this can take several minutes)"
Write-Log "Progress is being logged to $logPath (tail it with: Get-Content `"$logPath`" -Wait)"

# Static auth plugins are the point of this build: with them compiled into
# libmariadb.dll the connector resolves caching_sha2_password (MySQL 8.x),
# sha256_password, client_ed25519 and parsec without a plugin directory, so
# hMailServer ships a single DLL. dialog / mysql_clear_password / auth_gssapi_client
# are interactive or Kerberos-based and hMailServer never uses them.
# The generator has to name the Visual Studio that was actually resolved above: asking for
# "Visual Studio 16 2019" on a machine that only has VS2022 (as the GitHub Actions
# windows-2022 image does) fails at configure time.
$generator = if ($vsInstall.MajorVersion -ge 17) { "Visual Studio 17 2022" } else { "Visual Studio 16 2019" }
Write-Log "Using the CMake generator '$generator' for Visual Studio $($vsInstall.Version)"

$cmakeArgs = @(
    "-S", $srcDir,
    "-B", $buildDir,
    "-G", $generator,
    "-A", "x64",
    "-DCMAKE_BUILD_TYPE=RelWithDebInfo",
    "-DWITH_SSL=OPENSSL",
    "-DOPENSSL_ROOT_DIR=$openSslOut",
    "-DWITH_UNIT_TESTS=OFF",
    "-DWITH_CURL=OFF",
    "-DWITH_EXTERNAL_ZLIB=OFF",
    "-DCLIENT_PLUGIN_CACHING_SHA2_PASSWORD=STATIC",
    "-DCLIENT_PLUGIN_SHA256_PASSWORD=STATIC",
    "-DCLIENT_PLUGIN_CLIENT_ED25519=STATIC",
    "-DCLIENT_PLUGIN_PARSEC=STATIC",
    "-DCLIENT_PLUGIN_DIALOG=OFF",
    "-DCLIENT_PLUGIN_MYSQL_CLEAR_PASSWORD=OFF",
    "-DCLIENT_PLUGIN_AUTH_GSSAPI_CLIENT=OFF"
)

Invoke-BuildStep "Configuring the build (cmake)" {
    cmake @cmakeArgs
}
if ($LastExitCode -ne 0)
{
    Throw "CMake configuration failed with exit code $LastExitCode. See $logPath for details."
}

Invoke-BuildStep "Compiling libmariadb (cmake --build)" {
    cmake --build $buildDir --config RelWithDebInfo --target libmariadb
}
if ($LastExitCode -ne 0)
{
    Throw "Building libmariadb failed with exit code $LastExitCode. See $logPath for details."
}

# --- Verify the expected output -------------------------------------------------

$dllPath = Join-Path -Path $buildDir -ChildPath "libmariadb\RelWithDebInfo\libmariadb.dll"

if (!(Test-Path $dllPath))
{
    Throw "Build completed but expected output was missing: $dllPath"
}

# --- Verify the DLL really uses OpenSSL -----------------------------------------

# A misconfigured OPENSSL_ROOT_DIR makes CMake fall back to another TLS backend
# instead of failing, so confirm the import table before shipping the DLL.
Write-Log "Verifying that $dllPath imports the OpenSSL DLLs"

$dependents = & dumpbin /nologo /dependents $dllPath
if ($LastExitCode -ne 0)
{
    Throw "dumpbin /dependents failed with exit code $LastExitCode for $dllPath."
}

foreach ($opensslDll in @("libssl-3-x64.dll", "libcrypto-3-x64.dll"))
{
    if (($dependents | Select-String -SimpleMatch $opensslDll -Quiet) -ne $true)
    {
        Throw "$dllPath does not import $opensslDll, so it was not built against $openSslOut. Rebuild with -OpenSSLVersion pointing at a valid OpenSSL build."
    }
}

Write-Log "MariaDB Connector/C $Version built successfully into $(Split-Path -Parent $dllPath)"
