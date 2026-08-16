<#
.SYNOPSIS
    Shared helpers for the hMailServer library build scripts.

.DESCRIPTION
    Dot-sourced by build-boost.ps1, build-openssl.ps1 and build-pgsql.ps1. Provides:

      - A single build log (mirrored to a file next to the calling script and to the
        console) plus a helper to run a native build step with its stdout+stderr
        captured to that log: Start-BuildLog, Write-Log, Invoke-BuildStep.
      - The build-environment plumbing every script shares: Resolve-HMailServerLibs,
        Resolve-VcVars64, Import-VsEnvironment and Get-SourceArchive.

    Usage from a build script:

        . (Join-Path -Path $PSScriptRoot -ChildPath "build-common.ps1")
        Start-BuildLog -LogPath (Join-Path $PSScriptRoot "build-openssl.log") -Title "OpenSSL 3.5.7 build log"
        $libsPath = Resolve-HMailServerLibs
        $vcvars64 = Resolve-VcVars64
        Get-SourceArchive -Url $tarUrl -SrcDir $srcDir -LibsPath $libsPath
        Import-VsEnvironment -VcVars64 $vcvars64
        Invoke-BuildStep "Compiling" { nmake install_sw }
        if ($LastExitCode -ne 0) { Throw "..." }

    The log path/encoding are held in script scope. Because this file is dot-sourced,
    those variables and the functions live in the caller's script scope, so
    Start-BuildLog and the helpers all share the same state.
#>

# All log writes use this one encoding. Under Windows PowerShell 5.1 the various
# file cmdlets default to *different* encodings (Set-Content/Add-Content ->
# ANSI, Tee-Object -FilePath -> UTF-16LE), so mixing them produces a log where
# some lines render with a NUL between every character. Pin everything to UTF-8.
$script:BuildLogEncoding = "UTF8"
$script:BuildLogPath = $null

# Initialize the build log: record its path and write the header line. Call once
# before Write-Log / Invoke-BuildStep.
function Start-BuildLog
{
    param(
        [Parameter(Mandatory = $true)][string]$LogPath,
        [Parameter(Mandatory = $true)][string]$Title
    )
    $script:BuildLogPath = $LogPath
    Set-Content -Path $LogPath -Value "$Title - started $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" -Encoding $script:BuildLogEncoding
}

# Write a message to both the console and the log file.
function Write-Log
{
    param([string]$Message)
    Write-Host $Message
    Add-Content -Path $script:BuildLogPath -Value $Message -Encoding $script:BuildLogEncoding
}

# Run a build step, mirroring its stdout+stderr to the console and the log file.
# The step's native exit code is left in $LastExitCode for the caller to check.
function Invoke-BuildStep
{
    param(
        [string]$Description,
        [scriptblock]$Command
    )
    Write-Log $Description
    Add-Content -Path $script:BuildLogPath -Value "----- $Description -----" -Encoding $script:BuildLogEncoding
    # Merge the step's stderr into the output stream so it is logged too. Native
    # tools (nmake, the compiler invoked by build.pl) legitimately write
    # progress/warnings to stderr; under $ErrorActionPreference='Stop' a
    # 2>&1-redirected stderr line is otherwise turned into a terminating
    # NativeCommandError before we can inspect the exit code. Force Continue for
    # just this pipeline; the caller still gates on $LastExitCode.
    #
    # We deliberately do NOT use 'Tee-Object -FilePath' here: on Windows
    # PowerShell 5.1 it has no -Encoding switch and always writes UTF-16LE, which
    # corrupts a log the rest of the script writes as UTF-8. Instead echo each
    # line to the console and append it to the log with the shared encoding.
    #
    # The 2>&1 stream carries stdout lines as plain strings but stderr lines as
    # ErrorRecords (native tools such as cl.exe write the current source file
    # name to stderr). Casting such a record to [string] yields the useless text
    # "System.Management.Automation.RemoteException"; the real stderr text is in
    # its .Exception.Message, so pull that out explicitly.
    #
    # Write through a single StreamWriter held open for the whole step rather than
    # an Add-Content call per line: these builds emit many thousands of lines and a
    # per-line open/seek/close is needless disk churn. AutoFlush keeps the log
    # tailable live (Get-Content -Wait). UTF8Encoding($false) => no BOM, matching
    # the UTF-8 the rest of the script writes.
    $prevEAP = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    $writer = New-Object System.IO.StreamWriter($script:BuildLogPath, $true, (New-Object System.Text.UTF8Encoding($false)))
    $writer.AutoFlush = $true
    try
    {
        & $Command 2>&1 | ForEach-Object {
            if ($_ -is [System.Management.Automation.ErrorRecord])
            {
                $line = $_.Exception.Message
            }
            else
            {
                $line = [string]$_
            }
            Write-Host $line
            $writer.WriteLine($line)
        }
    }
    finally
    {
        $writer.Close()
        $ErrorActionPreference = $prevEAP
    }
}

# Resolve and validate the hMailServerLibs library folder (where sources are built).
# Returns the path; throws with an actionable message if the variable is unset or the
# folder is missing.
function Resolve-HMailServerLibs
{
    $libsPath = $env:hMailServerLibs

    if ([string]::IsNullOrEmpty($libsPath))
    {
        Throw "The environment variable hMailServerLibs was not found. Please create it."
    }

    if (!(Test-Path $libsPath))
    {
        Throw "The environment variable hMailServerLibs was found, but the folder it was pointing at ($libsPath) was not. Please create it."
    }

    return $libsPath
}

# Locate vcvars64.bat via vswhere and return its path.
#
# We import vcvars/cl from a SPECIFIC Visual Studio version, not simply the newest one
# installed. If a newer VS (e.g. VS2022) is also present, 'vswhere -latest' would import
# its environment and the libraries would be compiled with the newer STL/CRT. For C++
# code (Boost) that newer STL emits vectorized-algorithm symbols (e.g.
# __std_find_last_trivial_2) that the VS2019 runtime hMailServer links against does not
# provide, producing LNK2019 unresolved externals at link time; even for the C libraries
# it is wrong to build against a different toolset than the rest of hMailServer. The
# default range targets VS2019 (16.x), matching hMailServer's own toolset; callers that
# support other toolsets (build-boost.ps1) pass the matching range, or $null to fall back
# to -latest.
function Resolve-VcVars64
{
    param(
        [Parameter(Mandatory = $false)]
        [string]$VersionRange = '[16.0,17.0)'
    )

    $vsWhere = Join-Path -Path ${env:ProgramFiles(x86)} -ChildPath "Microsoft Visual Studio\Installer\vswhere.exe"

    if (!(Test-Path $vsWhere))
    {
        Throw "vswhere.exe was not found at $vsWhere. Please install Visual Studio 2019 (or the Visual Studio Installer)."
    }

    $vsWhereArgs = @('-products', '*', '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', '-property', 'installationPath')
    if ($VersionRange)
    {
        $vsWhereArgs += @('-version', $VersionRange)
    }
    else
    {
        $vsWhereArgs = @('-latest') + $vsWhereArgs
    }

    $vsInstallPath = & $vsWhere @vsWhereArgs
    $vsInstallPath = ($vsInstallPath | Select-Object -First 1)

    if ([string]::IsNullOrEmpty($vsInstallPath))
    {
        Throw "No Visual Studio installation with the x64 C++ toolchain (VC.Tools.x86.x64$(if ($VersionRange) { ", version $VersionRange" })) was found."
    }

    $vcvars64 = Join-Path -Path $vsInstallPath -ChildPath "VC\Auxiliary\Build\vcvars64.bat"

    if (!(Test-Path $vcvars64))
    {
        Throw "vcvars64.bat was not found at $vcvars64."
    }

    return $vcvars64
}

# Import the VS x64 build environment (PATH, INCLUDE, LIB, ...) from vcvars64.bat into
# this session so cl.exe, nmake, perl's Configure/build.pl and b2 find the toolchain and
# Windows SDK. Rather than chaining every build step into one 'cmd /c' (which collapses
# all failures into a single opaque exit code), the variables are imported once here and
# each build step is then run separately with its own exit-code check. vcvars' own stdout
# is discarded so only 'set' output is parsed; the '&&' ensures 'set' runs only if vcvars
# succeeded.
function Import-VsEnvironment
{
    param(
        [Parameter(Mandatory = $true)]
        [string]$VcVars64
    )

    Write-Log "Importing the VS x64 build environment"

    $vcVarsOutput = cmd /c "call `"$VcVars64`" >nul 2>&1 && set"
    if ($LastExitCode -ne 0)
    {
        Throw "Failed to initialize the VS x64 build environment via $VcVars64 (exit code $LastExitCode)."
    }

    foreach ($line in $vcVarsOutput)
    {
        if ($line -match '^([^=]+)=(.*)$')
        {
            Set-Item -Path "Env:\$($matches[1])" -Value $matches[2]
        }
    }
}

# Fetch a source tarball and extract it under $LibsPath, leaving the tree in $SrcDir.
#
# Every run starts from a clean tree: any existing $SrcDir is deleted first, then the
# archive is downloaded, extracted and removed. This makes each build a full
# delete/download/unzip/build with no reuse of stale, possibly cross-toolset output.
function Get-SourceArchive
{
    param(
        [Parameter(Mandatory = $true)]
        [string]$Url,

        [Parameter(Mandatory = $true)]
        [string]$SrcDir,

        [Parameter(Mandatory = $true)]
        [string]$LibsPath
    )

    if (Test-Path $SrcDir)
    {
        Write-Log "Removing existing source folder $SrcDir for a clean build"
        Remove-Item -LiteralPath $SrcDir -Recurse -Force
    }

    # Name the local tarball after the URL's file (e.g. boost_1_92_0.tar.gz).
    $tarPath = Join-Path -Path $LibsPath -ChildPath (Split-Path -Leaf $Url)

    Write-Log "Downloading $Url"
    Invoke-WebRequest -Uri $Url -OutFile $tarPath

    Write-Log "Extracting to $LibsPath"
    # Use the Windows-bundled bsdtar (System32\tar.exe) explicitly rather than a
    # 'tar' resolved from PATH: a GNU tar (e.g. from Git/MSYS) treats the "C:" in
    # a "C:\..." path as a remote rmt host ("Cannot connect to C: resolve failed"),
    # whereas bsdtar handles drive letters. The tarball extracts to $SrcDir.
    $tarExe = Join-Path -Path $env:SystemRoot -ChildPath "System32\tar.exe"
    if (!(Test-Path $tarExe))
    {
        Throw "The Windows-bundled tar.exe was not found at $tarExe. Windows 10/11 ships it; please install it or extract $tarPath manually."
    }
    & $tarExe -xzf $tarPath -C $LibsPath
    if ($LastExitCode -ne 0)
    {
        Throw "Extraction of $tarPath failed with error code $LastExitCode."
    }

    Remove-Item $tarPath -Force

    if (!(Test-Path $SrcDir))
    {
        Throw "Expected source folder $SrcDir was not found after extraction."
    }
}
