<#
.SYNOPSIS
    Fetches the 7-Zip command line tool hMailServer ships for backup and restore.

.DESCRIPTION
    Downloads the "7-Zip Extra" package for the requested version, verifies it against a
    pinned SHA-256, and extracts the x64 standalone console tool into
    %hMailServerLibs%\7zip-<Version>:

        7za.exe       the tool Compression.cpp launches
        License.txt   the LGPL text, installed beside it as 7za.exe.license.txt

    Unlike the other scripts in this folder nothing is compiled here: 7-Zip is shipped as
    the prebuilt binary Igor Pavlov publishes. That is also why the download is hash-pinned
    rather than merely fetched - the bytes downloaded are the bytes installed on every
    hMailServer machine, so a substituted archive would go straight to users. The pinned
    hashes live in $ArchiveHashes below.

    x64\7za.exe is the standalone build: it carries its own codecs and needs no 7za.dll, so
    it is the only file the server requires.

    Prerequisites:
      - The environment variable hMailServerLibs, pointing at your library folder.
      - Windows 10/11 (the bundled tar.exe extracts the .7z; see below).

.PARAMETER Version
    The 7-Zip version to fetch, e.g. 26.03. Must be a version listed in $ArchiveHashes.

.EXAMPLE
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File libraries\build-7zip.ps1 -Version 26.03
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^\d+\.\d+$')]
    [string]$Version
)

$ErrorActionPreference = "Stop"

# SHA-256 of each supported 7z<version>-extra.7z, recorded when the version was pinned.
# 7-Zip publishes no checksum files, so these are captured from the downloaded archive:
#
#     (Get-FileHash 7z2603-extra.7z -Algorithm SHA256).Hash
#
# Add an entry here when pinning a new version; an unlisted version is refused rather than
# downloaded unverified.
$ArchiveHashes = @{
    "26.03" = "191894E6ACB3647FFB69CE630479FF318523B2E2B9890AA7F05C1127C2E59B8F"
}

# --- Set up a build log ---------------------------------------------------------

. (Join-Path -Path $PSScriptRoot -ChildPath "build-common.ps1")

$logPath = Join-Path -Path $PSScriptRoot -ChildPath "build-7zip.log"
Start-BuildLog -LogPath $logPath -Title "7-Zip $Version fetch log"

# --- Resolve the library folder -------------------------------------------------

$libsPath = Resolve-HMailServerLibs

$destDir = Join-Path -Path $libsPath -ChildPath "7zip-$Version"

if (-not $ArchiveHashes.ContainsKey($Version))
{
    $known = ($ArchiveHashes.Keys | Sort-Object) -join ", "
    Throw "No pinned SHA-256 is recorded for 7-Zip $Version. Known versions: $known. To add one, download https://github.com/ip7z/7zip/releases and record the archive's hash in `$ArchiveHashes in $PSCommandPath."
}

$expectedHash = $ArchiveHashes[$Version]

# 7-Zip names its files after the version with the dot removed: 26.03 -> 7z2603-extra.7z.
$versionToken = $Version -replace '\.', ''
$archiveName = "7z$versionToken-extra.7z"
$archiveUrl = "https://github.com/ip7z/7zip/releases/download/$Version/$archiveName"

# --- Download and verify --------------------------------------------------------

# Every run starts from a clean folder, so a half-extracted tree from a failed run is never
# mistaken for a good one.
if (Test-Path $destDir)
{
    Write-Log "Removing existing folder $destDir for a clean fetch"
    Remove-Item -LiteralPath $destDir -Recurse -Force
}

$archivePath = Join-Path -Path $libsPath -ChildPath $archiveName

Write-Log "Downloading $archiveUrl"
# GitHub requires TLS 1.2; Windows PowerShell 5.1 does not always negotiate it by default.
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
Invoke-WebRequest -Uri $archiveUrl -OutFile $archivePath

$actualHash = (Get-FileHash -Path $archivePath -Algorithm SHA256).Hash
if ($actualHash -ne $expectedHash)
{
    Remove-Item $archivePath -Force
    Throw "The SHA-256 of $archiveName did not match the pinned value. Expected $expectedHash, got $actualHash. The download was discarded."
}

Write-Log "SHA-256 verified: $actualHash"

# --- Extract --------------------------------------------------------------------

$extractDir = Join-Path -Path $libsPath -ChildPath "7zip-$Version-extract"
if (Test-Path $extractDir)
{
    Remove-Item -LiteralPath $extractDir -Recurse -Force
}
New-Item -ItemType Directory -Force -Path $extractDir | Out-Null

Write-Log "Extracting $archiveName"
# The Windows-bundled bsdtar reads .7z (its libarchive has liblzma), which avoids needing a
# copy of 7-Zip to unpack 7-Zip. It is invoked by full path rather than resolved from PATH
# for the same reason as Get-SourceArchive: a GNU tar treats the "C:" in a path as a remote
# host and fails.
$tarExe = Join-Path -Path $env:SystemRoot -ChildPath "System32\tar.exe"
if (!(Test-Path $tarExe))
{
    Throw "The Windows-bundled tar.exe was not found at $tarExe. Windows 10/11 ships it; please install it or extract $archivePath manually."
}

Invoke-BuildStep "Extracting $archiveName to $extractDir" {
    & $tarExe -xf $archivePath -C $extractDir
}
if ($LastExitCode -ne 0)
{
    Throw "Extraction of $archivePath failed with exit code $LastExitCode. See $logPath for details."
}

# --- Keep only what hMailServer ships -------------------------------------------

# The package also carries x86 and arm64 builds, the 7za.dll/7zxa.dll variants, the Far
# Manager plugin and the help file. None of that is installed, so none of it is kept.
$sourceExe = Join-Path -Path $extractDir -ChildPath "x64\7za.exe"
$sourceLicense = Join-Path -Path $extractDir -ChildPath "License.txt"

foreach ($item in @($sourceExe, $sourceLicense))
{
    if (!(Test-Path $item))
    {
        Throw "The extracted package did not contain the expected file: $item"
    }
}

New-Item -ItemType Directory -Force -Path $destDir | Out-Null
Copy-Item -Path $sourceExe -Destination $destDir
Copy-Item -Path $sourceLicense -Destination $destDir

Remove-Item -LiteralPath $extractDir -Recurse -Force
Remove-Item $archivePath -Force

# --- Verify the expected output -------------------------------------------------

$expected = @(
    (Join-Path -Path $destDir -ChildPath "7za.exe"),
    (Join-Path -Path $destDir -ChildPath "License.txt")
)

foreach ($item in $expected)
{
    if (!(Test-Path $item))
    {
        Throw "Fetch completed but expected output was missing: $item"
    }
}

Write-Log "7-Zip $Version fetched successfully into $destDir"
