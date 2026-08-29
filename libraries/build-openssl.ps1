<#
.SYNOPSIS
    Builds a specific OpenSSL 3.5.x version for hMailServer.

.DESCRIPTION
    Downloads the OpenSSL source for the requested version into
    %hMailServerLibs%\openssl-<Version>, then configures and builds it with the
    Visual Studio x64 toolchain into an "out64" install prefix, matching the layout
    hMailServer and libpq link against (out64\include, out64\lib, out64\bin with
    libcrypto-3-x64.dll / libssl-3-x64.dll).

    Only OpenSSL 3.5.x is supported. The build recipe for 3.0.x and 4.x differs and
    is intentionally out of scope.

    Prerequisites (must be on PATH / installed):
      - The environment variable hMailServerLibs, pointing at your library folder.
      - Perl (e.g. Strawberry Perl) - required by OpenSSL's Configure.
      - Visual Studio 2019, or Visual Studio 2022, with the x64 build tools (vcvars64.bat
        is located automatically via vswhere).

.PARAMETER Version
    The OpenSSL version to build, e.g. 3.5.7. Must match 3.5.x.

.EXAMPLE
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-openssl.ps1 -Version 3.5.7
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^3\.5\.\d+$')]
    [string]$Version
)

$ErrorActionPreference = "Stop"

# Handle native-command exit codes explicitly (checked after each step) rather than
# letting a nonzero exit or stderr text abort the pipeline on its own.
$PSNativeCommandUseErrorActionPreference = $false

# --- Set up a build log ---------------------------------------------------------

# OpenSSL's Configure/nmake output is verbose and the host console may buffer it, so
# mirror every step to build-openssl.log next to this script. This gives a file you
# can watch live from another shell to confirm the build is progressing:
#
#     Get-Content libraries\build-openssl.log -Wait
#
# and a full transcript to inspect if a step fails. The logging helpers
# (Start-BuildLog, Write-Log, Invoke-BuildStep) are shared with build-pgsql.ps1.
. (Join-Path -Path $PSScriptRoot -ChildPath "build-common.ps1")

$logPath = Join-Path -Path $PSScriptRoot -ChildPath "build-openssl.log"
Start-BuildLog -LogPath $logPath -Title "OpenSSL $Version build log"

# --- Resolve the library folder -------------------------------------------------

$libsPath = Resolve-HMailServerLibs

$srcDir = Join-Path -Path $libsPath -ChildPath "openssl-$Version"
$outDir = Join-Path -Path $srcDir -ChildPath "out64"

# --- Locate the Visual Studio build environment via vswhere --------------------

$vsInstall = Resolve-VcVars64

# --- Verify Perl is available ---------------------------------------------------

if ($null -eq (Get-Command perl -ErrorAction SilentlyContinue))
{
    Throw "Perl was not found on PATH. OpenSSL's Configure requires Perl (e.g. Strawberry Perl)."
}

# --- Download and extract the source (always a clean tree) ---------------------

$tarUrl = "https://www.openssl.org/source/openssl-$Version.tar.gz"
Get-SourceArchive -Url $tarUrl -SrcDir $srcDir -LibsPath $libsPath

# --- Import the VS x64 build environment ---------------------------------------

# OpenSSL is a C library consumed through an import library and a DLL, so its ABI does not
# depend on the toolset; no -ToolsetVersion is needed (see Import-VsEnvironment).
Import-VsEnvironment -VsInstall $vsInstall

# --- Configure and build (each step checked individually) ----------------------

Write-Log "Building OpenSSL $Version (this can take several minutes)"
Write-Log "Progress is being logged to $logPath (tail it with: Get-Content `"$logPath`" -Wait)"

$env:CFLAGS = "-DOPENSSL_TLS_SECURITY_LEVEL=0"

Push-Location $srcDir
try
{
    Invoke-BuildStep "Configuring OpenSSL $Version for target VC-WIN64A" {
        perl Configure no-asm VC-WIN64A "--prefix=$outDir" "--openssldir=$outDir" -D_WIN32_WINNT=0x600 --api=1.1.1 no-deprecated
    }
    if ($LastExitCode -ne 0)
    {
        Throw "OpenSSL 'perl Configure' failed with exit code $LastExitCode. See $logPath for details."
    }

    Invoke-BuildStep "Cleaning any previous build output (nmake clean)" {
        nmake clean
    }
    if ($LastExitCode -ne 0)
    {
        Throw "OpenSSL 'nmake clean' failed with exit code $LastExitCode. See $logPath for details."
    }

    Invoke-BuildStep "Compiling and installing OpenSSL (nmake install_sw)" {
        nmake install_sw
    }
    if ($LastExitCode -ne 0)
    {
        Throw "OpenSSL 'nmake install_sw' failed with exit code $LastExitCode. See $logPath for details."
    }
}
finally
{
    Pop-Location
}

# --- Verify the expected output -------------------------------------------------

$expected = @(
    (Join-Path -Path $outDir -ChildPath "bin\libcrypto-3-x64.dll"),
    (Join-Path -Path $outDir -ChildPath "bin\libssl-3-x64.dll"),
    (Join-Path -Path $outDir -ChildPath "include"),
    (Join-Path -Path $outDir -ChildPath "lib")
)

foreach ($item in $expected)
{
    if (!(Test-Path $item))
    {
        Throw "Build completed but expected output was missing: $item"
    }
}

Write-Log "OpenSSL $Version built successfully into $outDir"
