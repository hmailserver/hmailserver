<#
.SYNOPSIS
    Builds libpq from a specific PostgreSQL version for hMailServer.

.DESCRIPTION
    Downloads the PostgreSQL source for the requested version into
    %hMailServerLibs%\postgresql-<Version>, configures it with Meson against a previously
    built OpenSSL, and builds the libpq shared library with the Visual Studio x64
    toolchain. The result is copied into the layout hMailServer links against:
    postgresql-<Version>\Release\libpq (libpq.dll / libpq.lib / libpq.pdb) plus the
    libpq-fe.h header under src\interfaces\libpq and postgres_ext.h under src\include.

    Only PostgreSQL 17.x and 18.x are supported. They are built with Meson, the only build
    system PostgreSQL ships for MSVC since 17 removed src\tools\msvc. libpq from
    PostgreSQL 16 and later requires Windows 10 / Windows Server 2016.

    Meson needs a few tools PostgreSQL's old build did not. The script fetches the ones that
    are not commonly installed, at pinned versions, into %hMailServerLibs%\tools, so a local
    build and the CI build use the same ones:
      - Meson and Ninja, installed from PyPI into a Python virtual environment.
      - flex and bison, from the winflexbison release on GitHub, checked against the
        SHA-256 recorded below. Meson's PostgreSQL build requires both even for libpq.

    Prerequisites (must be on PATH / installed):
      - The environment variable hMailServerLibs, pointing at your library folder.
      - A previously built OpenSSL under %hMailServerLibs%\openssl-<OpenSSLVersion>\out64
        (build it with build-openssl.ps1). libpq is linked against it for SSL support.
      - Perl (e.g. Strawberry Perl) - PostgreSQL's build generates sources with it.
      - Python 3 - runs Meson.
      - Visual Studio 2019, 2022 or 2026, with the x64 build tools (vcvars64.bat
        is located automatically via vswhere).

.PARAMETER Version
    The PostgreSQL version to build, e.g. 18.6. Must match 17.x or 18.x.

.PARAMETER OpenSSLVersion
    The OpenSSL version to link libpq against, e.g. 3.5.9. Must correspond to an
    existing %hMailServerLibs%\openssl-<OpenSSLVersion>\out64 build. If omitted, the
    script auto-detects it from hMailServer.vcxproj (the openssl-<ver> the project
    currently links against).

.EXAMPLE
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-pgsql.ps1 -Version 18.6 -OpenSSLVersion 3.5.9

.EXAMPLE
    # Auto-detect the OpenSSL version from hMailServer.vcxproj:
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-pgsql.ps1 -Version 18.6
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^1[78]\.\d+$')]
    [string]$Version,

    [Parameter(Mandatory = $false)]
    [ValidatePattern('^\d+\.\d+\.\d+$')]
    [string]$OpenSSLVersion
)

$ErrorActionPreference = "Stop"

# Handle native-command exit codes explicitly (checked after each step) rather than
# letting a nonzero exit or stderr text abort the pipeline on its own.
$PSNativeCommandUseErrorActionPreference = $false

# --- Pinned build tools ---------------------------------------------------------

# Meson and Ninja come from PyPI, which verifies what it serves against the checksum it
# recorded for that version.
$MesonVersion = "1.12.1"
$NinjaVersion = "1.13.2"

# winflexbison has no package index behind it, so its archive is hash-pinned instead.
# Recorded with:
#     (Get-FileHash win_flex_bison-2.5.25.zip -Algorithm SHA256).Hash
$WinFlexBisonVersion = "2.5.25"
$WinFlexBisonHash = "8D324B62BE33604B2C45AD1DD34AB93D722534448F55A16CA7292DE32B6AC135"

# --- Set up a build log ---------------------------------------------------------

# Meson and Ninja output is verbose and the host console may buffer it, so mirror
# every step to build-pgsql.log next to this script. This gives a file you can watch
# live from another shell to confirm the build is progressing:
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

$srcDir   = Join-Path -Path $libsPath -ChildPath "postgresql-$Version"
$buildDir = Join-Path -Path $srcDir -ChildPath "build64"
$outDir   = Join-Path -Path $srcDir -ChildPath "Release\libpq"
$toolsDir = Join-Path -Path $libsPath -ChildPath "tools"

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

# --- Verify Perl and Python are available ---------------------------------------

if ($null -eq (Get-Command perl -ErrorAction SilentlyContinue))
{
    Throw "Perl was not found on PATH. PostgreSQL's build requires Perl (e.g. Strawberry Perl)."
}

$python = Get-Command python -ErrorAction SilentlyContinue
if ($null -eq $python)
{
    Throw "Python was not found on PATH. Meson, which builds PostgreSQL, runs on Python 3."
}

# --- Fetch Meson and Ninja ------------------------------------------------------

# A virtual environment per Meson version keeps the pinned tools away from whatever the
# machine's own Python has installed, and lets a version bump sit next to the old one.
$mesonVenv = Join-Path -Path $toolsDir -ChildPath "meson-$MesonVersion-ninja-$NinjaVersion"
$mesonExe  = Join-Path -Path $mesonVenv -ChildPath "Scripts\meson.exe"
$ninjaExe  = Join-Path -Path $mesonVenv -ChildPath "Scripts\ninja.exe"

if (!(Test-Path $mesonExe) -or !(Test-Path $ninjaExe))
{
    if (Test-Path $mesonVenv)
    {
        Remove-Item -LiteralPath $mesonVenv -Recurse -Force
    }

    Invoke-BuildStep "Creating a Python virtual environment for Meson in $mesonVenv" {
        & $python.Source -m venv $mesonVenv
    }
    if ($LastExitCode -ne 0)
    {
        Throw "Creating the Python virtual environment at $mesonVenv failed with exit code $LastExitCode."
    }

    $venvPython = Join-Path -Path $mesonVenv -ChildPath "Scripts\python.exe"
    Invoke-BuildStep "Installing meson $MesonVersion and ninja $NinjaVersion" {
        & $venvPython -m pip install --disable-pip-version-check --no-input "meson==$MesonVersion" "ninja==$NinjaVersion"
    }
    if ($LastExitCode -ne 0)
    {
        Throw "Installing meson $MesonVersion and ninja $NinjaVersion failed with exit code $LastExitCode."
    }
}

# --- Fetch flex and bison -------------------------------------------------------

$winFlexBisonDir = Join-Path -Path $toolsDir -ChildPath "winflexbison-$WinFlexBisonVersion"

if (!(Test-Path (Join-Path $winFlexBisonDir "win_bison.exe")) -or !(Test-Path (Join-Path $winFlexBisonDir "win_flex.exe")))
{
    New-Item -ItemType Directory -Force -Path $toolsDir | Out-Null

    $zipUrl  = "https://github.com/lexxmark/winflexbison/releases/download/v$WinFlexBisonVersion/win_flex_bison-$WinFlexBisonVersion.zip"
    $zipPath = Join-Path -Path $toolsDir -ChildPath "win_flex_bison-$WinFlexBisonVersion.zip"

    Write-Log "Downloading $zipUrl"
    Invoke-WebRequest -Uri $zipUrl -OutFile $zipPath

    $actualHash = (Get-FileHash -Path $zipPath -Algorithm SHA256).Hash
    if ($actualHash -ne $WinFlexBisonHash)
    {
        Remove-Item $zipPath -Force
        Throw "The SHA-256 of win_flex_bison-$WinFlexBisonVersion.zip is $actualHash, but $WinFlexBisonHash was recorded for it. Refusing to use it."
    }

    if (Test-Path $winFlexBisonDir)
    {
        Remove-Item -LiteralPath $winFlexBisonDir -Recurse -Force
    }

    Write-Log "Extracting to $winFlexBisonDir"
    Expand-Archive -Path $zipPath -DestinationPath $winFlexBisonDir
    Remove-Item $zipPath -Force
}

# --- Download and extract the source (always a clean tree) ---------------------

$tarUrl = "https://ftp.postgresql.org/pub/source/v$Version/postgresql-$Version.tar.gz"
Get-SourceArchive -Url $tarUrl -SrcDir $srcDir -LibsPath $libsPath

if (!(Test-Path (Join-Path $srcDir "meson.build")))
{
    Throw "PostgreSQL $Version has no meson.build at the root of its source tree. Only 17.x and 18.x are supported."
}

# --- Import the VS x64 build environment ---------------------------------------

# libpq is a C library consumed through an import library and a DLL, so its ABI does not
# depend on the toolset; no -ToolsetVersion is needed (see Import-VsEnvironment).
Import-VsEnvironment -VsInstall $vsInstall

# Meson finds flex and bison by searching PATH for win_flex / win_bison.
$env:PATH = "$winFlexBisonDir;$env:PATH"

# --- Configure ------------------------------------------------------------------

# --auto-features=disabled turns every optional dependency off (ICU, zlib, readline, the
# PL languages, GSSAPI, ...), so the build depends on nothing beyond OpenSSL, which is then
# required explicitly. debugoptimized is an optimized build that also writes libpq.pdb,
# which the symbols package ships; b_vscrt=md pins it to the release C runtime regardless.
# OpenSSL's headers and import libraries are handed to Meson as extra search directories,
# where PostgreSQL looks for libssl / libcrypto when pkg-config does not know about them.
Write-Log "Building libpq from PostgreSQL $Version (this can take several minutes)"
Write-Log "Progress is being logged to $logPath (tail it with: Get-Content `"$logPath`" -Wait)"

Push-Location $srcDir
try
{
    Invoke-BuildStep "Configuring PostgreSQL (meson setup)" {
        & $mesonExe setup $buildDir `
            --backend=ninja `
            --buildtype=debugoptimized `
            -Db_vscrt=md `
            --auto-features=disabled `
            -Dssl=openssl `
            "-Dextra_include_dirs=$(Join-Path $openSslOut 'include')" `
            "-Dextra_lib_dirs=$(Join-Path $openSslOut 'lib')"
    }
    if ($LastExitCode -ne 0)
    {
        Throw "PostgreSQL 'meson setup' failed with exit code $LastExitCode. See $logPath and $buildDir\meson-logs\meson-log.txt for details."
    }

    # --- Build libpq --------------------------------------------------------------

    # libpq is defined twice, as a static and a shared library of the same name; only the
    # shared one (libpq.dll and its import library libpq.lib) is wanted.
    Invoke-BuildStep "Compiling libpq (meson compile libpq:shared_library)" {
        & $mesonExe compile -C $buildDir "src/interfaces/libpq/libpq:shared_library"
    }
    if ($LastExitCode -ne 0)
    {
        Throw "PostgreSQL 'meson compile' of libpq failed with exit code $LastExitCode. See $logPath for details."
    }
}
finally
{
    Pop-Location
}

# --- Verify libpq was built with SSL --------------------------------------------

# --auto-features=disabled would have turned SSL off too had -Dssl=openssl not been passed,
# and a libpq without it cannot connect to a PostgreSQL server that requires encryption.
$pgConfig = Join-Path -Path $buildDir -ChildPath "src\include\pg_config.h"
if (!(Select-String -Path $pgConfig -Pattern '^#define USE_OPENSSL 1' -Quiet))
{
    Throw "libpq was configured without OpenSSL support ($pgConfig does not define USE_OPENSSL)."
}

# --- Copy the output into the layout hMailServer links against -----------------

# hMailServer.vcxproj, post-build.bat, the installer and the CI symbols step all expect
# Release\libpq, the layout the old build.pl produced.
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$builtDir = Join-Path -Path $buildDir -ChildPath "src\interfaces\libpq"
foreach ($file in @("libpq.dll", "libpq.lib", "libpq.pdb"))
{
    Copy-Item -Path (Join-Path $builtDir $file) -Destination $outDir -Force
}

# --- Verify the expected output -------------------------------------------------

$expected = @(
    (Join-Path -Path $outDir -ChildPath "libpq.dll"),
    (Join-Path -Path $outDir -ChildPath "libpq.lib"),
    (Join-Path -Path $outDir -ChildPath "libpq.pdb"),
    (Join-Path -Path $srcDir -ChildPath "src\interfaces\libpq\libpq-fe.h"),
    (Join-Path -Path $srcDir -ChildPath "src\include\postgres_ext.h")
)

foreach ($item in $expected)
{
    if (!(Test-Path $item))
    {
        Throw "Build completed but expected output was missing: $item"
    }
}

Write-Log "libpq from PostgreSQL $Version built successfully into $outDir"
