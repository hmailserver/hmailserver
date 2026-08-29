<#
.SYNOPSIS
    Builds a specific Boost version for hMailServer.

.DESCRIPTION
    Downloads the Boost source for the requested version into
    %hMailServerLibs%\boost_<underscored-Version> (e.g. boost_1_90_0), bootstraps
    b2, and builds the static, multithreaded x64 libraries hMailServer links
    against into stage\lib, matching the layout the project expects
    (boost_<ver>\stage\lib for libs, boost_<ver> itself for headers).

    Only the subset of Boost libraries hMailServer uses is built: thread,
    filesystem, regex, chrono, system and atomic. Everything else is header-only
    and needs no compilation.

    Prerequisites (must be on PATH / installed):
      - The environment variable hMailServerLibs, pointing at your library folder.
      - Visual Studio 2019 with the x64 build tools, or Visual Studio 2022/2026 with the
        "MSVC v142 build tools" component (vcvars64.bat is located automatically via
        vswhere). b2 is driven with the msvc-14.2 toolset, which hMailServer's own
        projects are built with.

.PARAMETER Version
    The Boost version to build, e.g. 1.90.0. Must match 1.x.y.

.PARAMETER Toolset
    The b2 toolset to build with. Defaults to msvc-14.2, the v142 toolset
    hMailServer's project files expect. It is provided by Visual Studio 2019, and by
    Visual Studio 2022 and 2026 with the "MSVC v142 build tools" component.

.PARAMETER Jobs
    Number of parallel compilations (b2 -j). Defaults to the number of logical
    processors.

.EXAMPLE
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-boost.ps1 -Version 1.90.0
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^1\.\d+\.\d+$')]
    [string]$Version,

    [Parameter(Mandatory = $false)]
    [string]$Toolset = "msvc-14.2",

    [Parameter(Mandatory = $false)]
    [int]$Jobs = [int]$env:NUMBER_OF_PROCESSORS
)

$ErrorActionPreference = "Stop"

# Handle native-command exit codes explicitly (checked after each step) rather than
# letting a nonzero exit or stderr text abort the pipeline on its own.
$PSNativeCommandUseErrorActionPreference = $false

# --- Set up a build log ---------------------------------------------------------

# b2's compile output is verbose and the host console may buffer it, so mirror
# every step to build-boost.log next to this script. This gives a file you can
# watch live from another shell to confirm the build is progressing:
#
#     Get-Content libraries\build-boost.log -Wait
#
# and a full transcript to inspect if a step fails. The logging helpers
# (Start-BuildLog, Write-Log, Invoke-BuildStep) are shared with the other build
# scripts.
. (Join-Path -Path $PSScriptRoot -ChildPath "build-common.ps1")

$logPath = Join-Path -Path $PSScriptRoot -ChildPath "build-boost.log"
Start-BuildLog -LogPath $logPath -Title "Boost $Version build log"

# --- Resolve the library folder -------------------------------------------------

$libsPath = Resolve-HMailServerLibs

# Boost's source folder / tarball use underscores (boost_1_90_0), not dots.
$underscored = $Version -replace '\.', '_'
$srcDir = Join-Path -Path $libsPath -ChildPath "boost_$underscored"

if ($Jobs -lt 1)
{
    $Jobs = 4
}

# --- Locate the Visual Studio build environment via vswhere --------------------

# Boost must be compiled with the requested toolset, not simply the newest compiler
# installed (see Resolve-VcVars64 for why -latest is wrong): its static libraries end up
# inside hMailServer.exe, and the auto-linking pragma encodes the toolset in the library
# name (libboost_thread-vc142-mt-s-x64-1_92.lib).
#
# msvc-14.2 (v142) is available two ways: from VS2019, where it is the default compiler, and
# from VS2022 and VS2026, where it is the optional "MSVC v142 build tools" component selected
# with -vcvars_ver=14.29. Accept all three, preferring VS2019; the GitHub Actions
# windows-2025-vs2026 image has only VS2026. A custom/unknown toolset falls back to -latest
# ($null ranges).
$vsVersionRanges = switch -Regex ($Toolset)
{
    '^msvc-14\.2$' { @('[16.0,17.0)', '[17.0,18.0)', '[18.0,19.0)'); break }  # VS2019, or VS2022/VS2026 + v142
    '^msvc-14\.3$' { @('[17.0,18.0)', '[18.0,19.0)'); break }                 # VS2022, or VS2026 + v143
    default        { $null }                                                  # custom/unknown toolset
}

# The compiler vcvars must select when the toolset is not that Visual Studio's default.
# Import-VsEnvironment applies this only on VS2022 and newer.
$vcVarsToolsetVersion = switch -Regex ($Toolset)
{
    '^msvc-14\.2$' { $script:HMailServerVcToolsetVersion; break }  # 14.29
    default        { $null }
}

$vsInstall = Resolve-VcVars64 -VersionRanges $vsVersionRanges

# --- Download and extract the source (always a clean tree) ---------------------

$tarUrl = "https://archives.boost.io/release/$Version/source/boost_$underscored.tar.gz"
Get-SourceArchive -Url $tarUrl -SrcDir $srcDir -LibsPath $libsPath

# --- Import the VS x64 build environment ---------------------------------------

Import-VsEnvironment -VsInstall $vsInstall -ToolsetVersion $vcVarsToolsetVersion

# When the toolset does not belong to the Visual Studio that is installed - v142 built out of
# VS2022 or VS2026 - b2 cannot auto-configure it: Boost.Build looks for a VS2019 installation to satisfy
# --toolset=msvc-14.2 and finds none. Point it at the compiler vcvars just put on PATH with a
# generated user-config.jam, which also keeps the library name tag at vc142 (the tag comes
# from the declared version, and hMailServer's auto-linking pragma expects
# libboost_thread-vc142-mt-s-x64-...). On VS2019 nothing is generated and b2 auto-configures
# exactly as before.
$userConfigArgument = $null
if ($vcVarsToolsetVersion -and $vsInstall.MajorVersion -ge 17)
{
    $clPath = Join-Path -Path $env:VCToolsInstallDir -ChildPath "bin\Hostx64\x64\cl.exe"
    if (!(Test-Path $clPath))
    {
        Throw "The $vcVarsToolsetVersion compiler was not found at $clPath. Visual Studio $($vsInstall.MajorVersion) needs the 'MSVC v142 build tools' component to build Boost with $Toolset."
    }

    # Jam reads backslashes as escapes, so write the path with forward slashes.
    $jamClPath = $clPath -replace '\\', '/'
    $jamVersion = $Toolset -replace '^msvc-', ''
    $userConfigPath = Join-Path -Path $libsPath -ChildPath "boost-user-config.jam"
    Set-Content -Path $userConfigPath -Value "using msvc : $jamVersion : `"$jamClPath`" ;" -Encoding ASCII
    Write-Log "Pinning b2 to $clPath via $userConfigPath"

    $userConfigArgument = "--user-config=$userConfigPath"
}

# --- Bootstrap and build (each step checked individually) -----------------------

Write-Log "Building Boost $Version with toolset $Toolset (this can take several minutes)"
Write-Log "Progress is being logged to $logPath (tail it with: Get-Content `"$logPath`" -Wait)"

# Boost's bootstrap.bat / b2 are invoked from the source directory and rely on cmd
# resolving batch files (bootstrap.bat, its internal guess_toolset.bat, .\b2) from the
# current directory. If NoDefaultCurrentDirectoryInExePath is set in the environment,
# cmd refuses to search the cwd and every such call fails with "is not recognized as an
# internal or external command". Clear it for this process (and the child cmd/b2
# processes that inherit it) so the build works regardless of the host's setting.
Remove-Item Env:\NoDefaultCurrentDirectoryInExePath -ErrorAction SilentlyContinue

Push-Location $srcDir
try
{
    Invoke-BuildStep "Bootstrapping b2" {
        cmd /c "bootstrap.bat"
    }
    if ($LastExitCode -ne 0)
    {
        Throw "Boost 'bootstrap.bat' failed with exit code $LastExitCode. See $logPath for details."
    }

    # Build only the compiled libraries hMailServer links against, as static,
    # multithreaded, x64. Intermediate build output goes to out64; the finished
    # import/static libs are staged into stage\lib (what the project references).
    Invoke-BuildStep "Compiling Boost libraries (b2 stage)" {
        # $userConfigArgument is $null unless the toolset had to be pinned above; passing a
        # $null through the argument list would send an empty argument to b2, so splat an
        # array that simply omits it.
        $b2Arguments = @(
            'debug', 'release', 'threading=multi', 'link=static',
            '--with-thread', '--with-filesystem', '--with-regex', '--with-chrono', '--with-system', '--with-atomic',
            "--toolset=$Toolset", 'address-model=64', 'stage', '--build-dir=out64', '-j', $Jobs
        )
        if ($userConfigArgument) { $b2Arguments += $userConfigArgument }

        .\b2 @b2Arguments
    }
    if ($LastExitCode -ne 0)
    {
        Throw "Boost 'b2 stage' failed with exit code $LastExitCode. See $logPath for details."
    }
}
finally
{
    Pop-Location
}

# --- Verify the expected output -------------------------------------------------

$stageLib = Join-Path -Path $srcDir -ChildPath "stage\lib"
$boostHeaders = Join-Path -Path $srcDir -ChildPath "boost"

if (!(Test-Path $boostHeaders))
{
    Throw "Build completed but the Boost headers folder was missing: $boostHeaders"
}

if (!(Test-Path $stageLib))
{
    Throw "Build completed but the staged library folder was missing: $stageLib"
}

# The staged libs are named like libboost_thread-vc142-mt[-s]-x64-1_90.lib. Confirm
# each requested compiled library produced at least one .lib. Boost.System is fully
# header-only as of Boost 1.92 (its compiled library was removed), so --with-system
# stages no .lib and it is not listed among the expected compiled libraries.
$expectedLibs = @("thread", "filesystem", "regex", "chrono", "atomic")
foreach ($lib in $expectedLibs)
{
    $found = Get-ChildItem -Path $stageLib -Filter "*boost_$lib-*.lib" -ErrorAction SilentlyContinue
    if ($null -eq $found -or $found.Count -eq 0)
    {
        Throw "Build completed but no staged library was found for boost_$lib in $stageLib."
    }
}

Write-Log "Boost $Version built successfully. Headers: $boostHeaders  Libs: $stageLib"
