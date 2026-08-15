<#
.SYNOPSIS
    Builds a specific OpenSSL 3.5.x version for hMailServer.

.DESCRIPTION
    Downloads the OpenSSL source for the requested version into
    %hMailServerLibs%\openssl-<Version>, then configures and builds it with the
    VS2019 x64 toolchain into an "out64" install prefix, matching the layout
    hMailServer and libpq link against (out64\include, out64\lib, out64\bin with
    libcrypto-3-x64.dll / libssl-3-x64.dll).

    Only OpenSSL 3.5.x is supported. The build recipe for 3.0.x and 4.x differs and
    is intentionally out of scope.

    Prerequisites (must be on PATH / installed):
      - The environment variable hMailServerLibs, pointing at your library folder.
      - Perl (e.g. Strawberry Perl) - required by OpenSSL's Configure.
      - Visual Studio 2019 with the x64 build tools (vcvars64.bat is located
        automatically via vswhere).

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

$libsPath = $env:hMailServerLibs

if ([string]::IsNullOrEmpty($libsPath))
{
    Throw "The environment variable hMailServerLibs was not found. Please create it."
}

if (!(Test-Path $libsPath))
{
    Throw "The environment variable hMailServerLibs was found, but the folder it was pointing at ($libsPath) was not. Please create it."
}

$srcDir = Join-Path -Path $libsPath -ChildPath "openssl-$Version"
$outDir = Join-Path -Path $srcDir -ChildPath "out64"

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
    Throw "Perl was not found on PATH. OpenSSL's Configure requires Perl (e.g. Strawberry Perl)."
}

# --- Download and extract the source (skipped if already present) --------------

if (!(Test-Path $srcDir))
{
    $tarUrl  = "https://www.openssl.org/source/openssl-$Version.tar.gz"
    $tarPath = Join-Path -Path $libsPath -ChildPath "openssl-$Version.tar.gz"

    Write-Log "Downloading OpenSSL $Version from $tarUrl"
    Invoke-WebRequest -Uri $tarUrl -OutFile $tarPath

    Write-Log "Extracting to $libsPath"
    # Use the Windows-bundled bsdtar (System32\tar.exe) explicitly rather than a
    # 'tar' resolved from PATH: a GNU tar (e.g. from Git/MSYS) treats the "C:" in
    # a "C:\..." path as a remote rmt host ("Cannot connect to C: resolve failed"),
    # whereas bsdtar handles drive letters. The tarball extracts to openssl-<Version>\.
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

# --- Import the VS x64 build environment ---------------------------------------

# vcvars64.bat sets environment variables (PATH, INCLUDE, LIB, ...) that perl
# Configure and nmake need. Rather than chaining every build step into one
# 'cmd /c' (which collapses all failures into a single opaque exit code), import
# those variables into this session once, then run each step separately with its
# own exit-code check. vcvars' own stdout is discarded so only 'set' output is
# parsed; the '&&' ensures 'set' runs only if vcvars succeeded.
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
