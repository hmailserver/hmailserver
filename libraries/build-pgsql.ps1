<#
.SYNOPSIS
    Builds libpq from a specific PostgreSQL version for hMailServer.

.DESCRIPTION
    Downloads the PostgreSQL source for the requested version into
    %hMailServerLibs%\postgresql-<Version>, generates the src\tools\msvc\config.pl
    that links libpq against a previously built OpenSSL, and builds libpq with the
    VS2019 x64 toolchain (perl build.pl Release libpq). The result is the layout
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
      - Visual Studio 2019 with the x64 build tools (vcvars64.bat is located
        automatically via vswhere).

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

$libsPath = $env:hMailServerLibs

if ([string]::IsNullOrEmpty($libsPath))
{
    Throw "The environment variable hMailServerLibs was not found. Please create it."
}

if (!(Test-Path $libsPath))
{
    Throw "The environment variable hMailServerLibs was found, but the folder it was pointing at ($libsPath) was not. Please create it."
}

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

# --- Locate vcvars64.bat via vswhere -------------------------------------------

$vsWhere = Join-Path -Path ${env:ProgramFiles(x86)} -ChildPath "Microsoft Visual Studio\Installer\vswhere.exe"

if (!(Test-Path $vsWhere))
{
    Throw "vswhere.exe was not found at $vsWhere. Please install Visual Studio 2019 (or the Visual Studio Installer)."
}

$vsInstallPath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$vsInstallPath = ($vsInstallPath | Select-Object -First 1)

if ([string]::IsNullOrEmpty($vsInstallPath))
{
    Throw "No Visual Studio installation with the x64 C++ toolchain (VC.Tools.x86.x64) was found."
}

$vcvars64 = Join-Path -Path $vsInstallPath -ChildPath "VC\Auxiliary\Build\vcvars64.bat"

if (!(Test-Path $vcvars64))
{
    Throw "vcvars64.bat was not found at $vcvars64."
}

# --- Verify Perl is available ---------------------------------------------------

if ($null -eq (Get-Command perl -ErrorAction SilentlyContinue))
{
    Throw "Perl was not found on PATH. PostgreSQL's build.pl requires Perl (e.g. Strawberry Perl)."
}

# --- Download and extract the source (skipped if already present) --------------

if (!(Test-Path $srcDir))
{
    $tarUrl  = "https://ftp.postgresql.org/pub/source/v$Version/postgresql-$Version.tar.gz"
    $tarPath = Join-Path -Path $libsPath -ChildPath "postgresql-$Version.tar.gz"

    Write-Log "Downloading PostgreSQL $Version from $tarUrl"
    Invoke-WebRequest -Uri $tarUrl -OutFile $tarPath

    Write-Log "Extracting to $libsPath"
    # Use the Windows-bundled bsdtar (System32\tar.exe) explicitly rather than a
    # 'tar' resolved from PATH: a GNU tar (e.g. from Git/MSYS) treats the "C:" in
    # a "C:\..." path as a remote rmt host ("Cannot connect to C: resolve failed"),
    # whereas bsdtar handles drive letters. The tarball extracts to
    # postgresql-<Version>\.
    $tarExe = Join-Path -Path $env:SystemRoot -ChildPath "System32\tar.exe"
    if (!(Test-Path $tarExe))
    {
        Throw "The Windows-bundled tar.exe was not found at $tarExe. Windows 10/11 ships it; please install it or extract $tarPath manually."
    }
    & $tarExe -xzf $tarPath -C $libsPath
    if ($LastExitCode -ne 0)
    {
        Throw "Extraction of $tarPath failed with error code $LastExitCode."
    }

    Remove-Item $tarPath -Force

    if (!(Test-Path $srcDir))
    {
        Throw "Expected source folder $srcDir was not found after extraction."
    }
}
else
{
    Write-Log "Source folder $srcDir already exists - skipping download."
}

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

# vcvars64.bat sets environment variables (PATH, INCLUDE, LIB, ...) that build.pl
# and the compiler need. Rather than chaining every build step into one 'cmd /c'
# (which collapses all failures into a single opaque exit code), import those
# variables into this session once, then run each step separately with its own
# exit-code check. vcvars' own stdout is discarded so only 'set' output is parsed;
# the '&&' ensures 'set' runs only if vcvars succeeded.
Write-Log "Importing the VS x64 build environment"

$vcVarsOutput = cmd /c "call `"$vcvars64`" >nul 2>&1 && set"
if ($LastExitCode -ne 0)
{
    Throw "Failed to initialize the VS x64 build environment via $vcvars64 (exit code $LastExitCode)."
}

foreach ($line in $vcVarsOutput)
{
    if ($line -match '^([^=]+)=(.*)$')
    {
        Set-Item -Path "Env:\$($matches[1])" -Value $matches[2]
    }
}

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
