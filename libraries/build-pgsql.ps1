<#
.SYNOPSIS
    Builds libpq from a specific PostgreSQL version for hMailServer.

.DESCRIPTION
    Downloads the PostgreSQL source for the requested version into
    %hMailServerLibs%\postgresql-<Version>, generates the src\tools\msvc\config.pl
    that links libpq against a previously built OpenSSL, and builds libpq with the
    Visual Studio x64 toolchain (perl build.pl Release libpq). The result is the layout
    hMailServer links against: postgresql-<Version>\Release\libpq (libpq.dll /
    libpq.lib) plus the libpq-fe.h header under src\interfaces\libpq.

    Only PostgreSQL versions that ship the src\tools\msvc\build.pl MSVC build system
    are supported, i.e. 15.x and 16.x. PostgreSQL 17 removed that system in favour of
    Meson and is intentionally out of scope.

    Prerequisites (must be on PATH / installed):
      - The environment variable hMailServerLibs, pointing at your library folder.
      - A previously built OpenSSL under %hMailServerLibs%\openssl-<OpenSSLVersion>\out64
        (build it with build-openssl.ps1). Without it libpq is built without SSL and
        cannot make encrypted connections to PostgreSQL.
      - Perl (e.g. Strawberry Perl) - required by PostgreSQL's build.pl.
      - Visual Studio 2019, 2022 or 2026, with the x64 build tools (vcvars64.bat
        is located automatically via vswhere).

.PARAMETER Version
    The PostgreSQL version to build, e.g. 15.18. Must match 15.x or 16.x.

.PARAMETER OpenSSLVersion
    The OpenSSL version to link libpq against, e.g. 3.5.7. Must correspond to an
    existing %hMailServerLibs%\openssl-<OpenSSLVersion>\out64 build. If omitted, the
    script auto-detects it from hMailServer.vcxproj (the openssl-<ver> the project
    currently links against).

.EXAMPLE
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-pgsql.ps1 -Version 15.18 -OpenSSLVersion 3.5.7

.EXAMPLE
    # Auto-detect the OpenSSL version from hMailServer.vcxproj:
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-pgsql.ps1 -Version 15.18
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^1[56]\.\d+$')]
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

# PostgreSQL's build.pl output is verbose and the host console may buffer it, so
# mirror every step to build-pgsql.log next to this script. This gives a file you
# can watch live from another shell to confirm the build is progressing:
#
#     Get-Content libraries\build-pgsql.log -Wait
#
# and a full transcript to inspect if a step fails. The logging helpers
# (Start-BuildLog, Write-Log, Invoke-BuildStep) are shared with build-openssl.ps1.
. (Join-Path -Path $PSScriptRoot -ChildPath "build-common.ps1")

$logPath = Join-Path -Path $PSScriptRoot -ChildPath "build-pgsql.log"
Start-BuildLog -LogPath $logPath -Title "PostgreSQL $Version (libpq) build log"

# --- Resolve the library folder -------------------------------------------------

$libsPath = Resolve-HMailServerLibs

$srcDir  = Join-Path -Path $libsPath -ChildPath "postgresql-$Version"
$msvcDir = Join-Path -Path $srcDir -ChildPath "src\tools\msvc"

# --- Resolve the OpenSSL build to link against ----------------------------------

# If the caller did not pin an OpenSSL version, auto-detect the one the project
# currently links against from hMailServer.vcxproj (openssl-<ver>\out64). This keeps
# libpq's SSL backend in lockstep with the rest of hMailServer by default.
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
    Throw "The OpenSSL build to link libpq against was not found at $openSslOut. Build it first with build-openssl.ps1 -Version $OpenSSLVersion. Without it libpq would be built without SSL support."
}

# --- Locate the Visual Studio build environment via vswhere --------------------

$vsInstall = Resolve-VcVars64

# --- Verify Perl is available ---------------------------------------------------

if ($null -eq (Get-Command perl -ErrorAction SilentlyContinue))
{
    Throw "Perl was not found on PATH. PostgreSQL's build.pl requires Perl (e.g. Strawberry Perl)."
}

# --- Download and extract the source (always a clean tree) ---------------------

$tarUrl = "https://ftp.postgresql.org/pub/source/v$Version/postgresql-$Version.tar.gz"
Get-SourceArchive -Url $tarUrl -SrcDir $srcDir -LibsPath $libsPath

if (!(Test-Path $msvcDir))
{
    Throw "The MSVC build folder $msvcDir was not found. PostgreSQL $Version may not ship the src\tools\msvc build system (it was removed in 17.x). Only 15.x and 16.x are supported."
}

# --- Generate config.pl ---------------------------------------------------------

# build.pl reads config.pl for build options. The openssl entry points at the root
# of the out64 install so libpq is linked with SSL support; cflags targets Windows
# Vista so libpq does not statically import GetSystemTimePreciseAsFileTime (which is
# unavailable before Windows 8). Backslashes are escaped for the single-quoted Perl
# string.
$openSslPerl = $openSslOut -replace '\\', '\\'
$configPl = @"
use strict;
use warnings;

our `$config = {
    # Target Windows Vista so libpq does not statically import
    # GetSystemTimePreciseAsFileTime (unavailable before Windows 8).
    cflags  => '/D_WIN32_WINNT=0x0600',
    # Link libpq against the OpenSSL built by build-openssl.ps1 (root of out64).
    openssl => '$openSslPerl',
};

1;
"@

$configPlPath = Join-Path -Path $msvcDir -ChildPath "config.pl"
Write-Log "Writing $configPlPath (linking libpq against $openSslOut)"
Set-Content -Path $configPlPath -Value $configPl -Encoding UTF8

# --- Import the VS x64 build environment ---------------------------------------

# libpq is a C library consumed through an import library and a DLL, so its ABI does not
# depend on the toolset; no -ToolsetVersion is needed (see Import-VsEnvironment). PostgreSQL's
# build.pl generates its own project files from the Visual Studio version it finds here.
Import-VsEnvironment -VsInstall $vsInstall

# --- Build libpq (run from src\tools\msvc) -------------------------------------

Write-Log "Building libpq from PostgreSQL $Version (this can take several minutes)"
Write-Log "Progress is being logged to $logPath (tail it with: Get-Content `"$logPath`" -Wait)"

Push-Location $msvcDir
try
{
    Invoke-BuildStep "Compiling libpq (perl build.pl Release libpq)" {
        perl build.pl Release libpq
    }
    if ($LastExitCode -ne 0)
    {
        Throw "PostgreSQL 'perl build.pl Release libpq' failed with exit code $LastExitCode. See $logPath for details."
    }
}
finally
{
    Pop-Location
}

# --- Verify the expected output -------------------------------------------------

$expected = @(
    (Join-Path -Path $srcDir -ChildPath "Release\libpq\libpq.dll"),
    (Join-Path -Path $srcDir -ChildPath "Release\libpq\libpq.lib"),
    (Join-Path -Path $srcDir -ChildPath "src\interfaces\libpq\libpq-fe.h")
)

foreach ($item in $expected)
{
    if (!(Test-Path $item))
    {
        Throw "Build completed but expected output was missing: $item"
    }
}

Write-Log "libpq from PostgreSQL $Version built successfully into $(Join-Path -Path $srcDir -ChildPath 'Release\libpq')"
