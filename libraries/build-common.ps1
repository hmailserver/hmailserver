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
        $vsInstall = Resolve-VcVars64
        Get-SourceArchive -Url $tarUrl -SrcDir $srcDir -LibsPath $libsPath
        Import-VsEnvironment -VsInstall $vsInstall
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

# The MSVC toolset hMailServer's own projects are compiled with: PlatformToolset v142,
# which is the 14.2x compiler. Visual Studio 2019 provides it by default; Visual Studio
# 2022 and 2026 provide it as the optional "MSVC v142 build tools" component, selected
# with 'vcvars64.bat -vcvars_ver=14.29'.
$script:HMailServerVcToolsetVersion = '14.29'

# Locate a Visual Studio installation with the x64 C++ toolchain and return what the callers
# need from it: the vcvars64.bat path, the installation path and its version.
#
# We do NOT simply take 'vswhere -latest'. The Visual Studio in use decides which STL/CRT the
# libraries are compiled against, and Boost in particular must match hMailServer's own v142
# toolset: a newer STL emits vectorized-algorithm symbols (e.g. __std_find_last_trivial_2)
# that the v142 runtime hMailServer links against does not provide, producing LNK2019
# unresolved externals at link time. So the caller states which Visual Studio versions are
# acceptable, in preference order, and the first one installed wins.
#
# The default order prefers VS2019 (what the README asks a developer to install) and falls
# back to VS2022 and then VS2026 - the latter being what the GitHub Actions
# windows-2025-vs2026 image ships, as its only Visual Studio. On those, callers that need the
# v142 compiler ask Import-VsEnvironment for it; see the comment there for which libraries
# actually care.
#
# Pass $null or an empty array to fall back to 'vswhere -latest'.
function Resolve-VcVars64
{
    param(
        [Parameter(Mandatory = $false)]
        [string[]]$VersionRanges = @('[16.0,17.0)', '[17.0,18.0)', '[18.0,19.0)')
    )

    $vsWhere = Join-Path -Path ${env:ProgramFiles(x86)} -ChildPath "Microsoft Visual Studio\Installer\vswhere.exe"

    if (!(Test-Path $vsWhere))
    {
        Throw "vswhere.exe was not found at $vsWhere. Please install Visual Studio 2019, 2022 or 2026 (or the Visual Studio Installer)."
    }

    $rangesToTry = if ($VersionRanges) { $VersionRanges } else { @($null) }

    $instance = $null
    foreach ($range in $rangesToTry)
    {
        $vsWhereArgs = @('-products', '*', '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', '-format', 'json')
        if ($range)
        {
            $vsWhereArgs += @('-version', $range)
        }
        else
        {
            $vsWhereArgs = @('-latest') + $vsWhereArgs
        }

        # -format json returns installationPath and installationVersion from a single query, so
        # the reported version can never describe a different install than the path. vswhere
        # prints '[]' when nothing matches, but guard against empty output too: under Windows
        # PowerShell 5.1, ConvertFrom-Json on an empty string is a terminating error.
        $json = (& $vsWhere @vsWhereArgs | Out-String)
        if ([string]::IsNullOrWhiteSpace($json))
        {
            continue
        }

        $found = ($json | ConvertFrom-Json) | Select-Object -First 1
        if ($found)
        {
            $instance = $found
            break
        }
    }

    if (-not $instance)
    {
        $wanted = if ($VersionRanges) { " in version " + ($VersionRanges -join ' or ') } else { "" }
        Throw "No Visual Studio installation with the x64 C++ toolchain (VC.Tools.x86.x64$wanted) was found."
    }

    $vcvars64 = Join-Path -Path $instance.installationPath -ChildPath "VC\Auxiliary\Build\vcvars64.bat"

    if (!(Test-Path $vcvars64))
    {
        Throw "vcvars64.bat was not found at $vcvars64."
    }

    return [PSCustomObject]@{
        VcVars64     = $vcvars64
        InstallPath  = $instance.installationPath
        Version      = $instance.installationVersion
        MajorVersion = [int]($instance.installationVersion -split '\.')[0]
    }
}

# Import the VS x64 build environment (PATH, INCLUDE, LIB, ...) from vcvars64.bat into
# this session so cl.exe, nmake, perl's Configure/build.pl and b2 find the toolchain and
# Windows SDK. Rather than chaining every build step into one 'cmd /c' (which collapses
# all failures into a single opaque exit code), the variables are imported once here and
# each build step is then run separately with its own exit-code check. vcvars' own stdout
# is discarded so only 'set' output is parsed; the '&&' ensures 'set' runs only if vcvars
# succeeded.
#
# -ToolsetVersion pins the MSVC compiler vcvars selects (vcvars64.bat -vcvars_ver=14.29). It
# is applied only on VS2022 and newer, whose default compiler is 14.4x or 14.5x rather than the 14.2x
# (v142) toolset hMailServer is built with; on VS2019 the default is already the right one and
# older 16.x installs may not even carry 14.29.
#
# Only Boost needs this. It is C++, linked statically into hMailServer, and its auto-linking
# pragma encodes the toolset in the library name (libboost_thread-vc142-...). OpenSSL, libpq
# and libmariadb are C libraries consumed through an import library and a DLL, so their C ABI
# is toolset-independent and they build fine with whatever compiler the resolved Visual Studio
# defaults to.
function Import-VsEnvironment
{
    param(
        [Parameter(Mandatory = $true)]
        [object]$VsInstall,

        [Parameter(Mandatory = $false)]
        [string]$ToolsetVersion
    )

    # Accept either the object Resolve-VcVars64 returns or a bare vcvars64.bat path.
    $vcVars64 = if ($VsInstall -is [string]) { $VsInstall } else { $VsInstall.VcVars64 }
    $majorVersion = if ($VsInstall -is [string]) { 0 } else { $VsInstall.MajorVersion }

    $vcVarsArguments = ""
    if ($ToolsetVersion -and $majorVersion -ge 17)
    {
        $vcVarsArguments = " -vcvars_ver=$ToolsetVersion"
        Write-Log "Selecting the MSVC $ToolsetVersion toolset from Visual Studio $majorVersion"
    }

    Write-Log "Importing the VS x64 build environment"

    $vcVarsOutput = cmd /c "call `"$vcVars64`"$vcVarsArguments >nul 2>&1 && set"
    if ($LastExitCode -ne 0)
    {
        Throw "Failed to initialize the VS x64 build environment via $vcVars64$vcVarsArguments (exit code $LastExitCode)."
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
