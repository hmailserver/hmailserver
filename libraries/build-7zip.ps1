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
    hashes live in $ArchiveHashes and $BootstrapHashes below.

    x64\7za.exe is the standalone build: it carries its own codecs and needs no 7za.dll, so
    it is the only file the server requires.

    The extra package is a .7z, so unpacking it needs a 7-Zip. The bootstrap is 7zr.exe,
    the small standalone extractor published as a plain .exe in the same release: it is
    downloaded and hash-pinned the same way, then used to unpack the archive.

    Prerequisites:
      - The environment variable hMailServerLibs, pointing at your library folder.

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

# SHA-256 of each release's 7zr.exe, the standalone extractor used to unpack the .7z above.
# Recorded the same way, from the downloaded file:
#
#     (Get-FileHash 7zr.exe -Algorithm SHA256).Hash
#
# Every version listed in $ArchiveHashes needs an entry here too.
$BootstrapHashes = @{
    "26.03" = "AD4C82FADCBDF93C03B4FC440F300509C7D60C5C2F4D183E35D9D70D6957037D"
}

# --- Set up a build log ---------------------------------------------------------

. (Join-Path -Path $PSScriptRoot -ChildPath "build-common.ps1")

$logPath = Join-Path -Path $PSScriptRoot -ChildPath "build-7zip.log"
Start-BuildLog -LogPath $logPath -Title "7-Zip $Version fetch log"

# --- Resolve the library folder -------------------------------------------------

$libsPath = Resolve-HMailServerLibs

$destDir = Join-Path -Path $libsPath -ChildPath "7zip-$Version"

if (-not $ArchiveHashes.ContainsKey($Version) -or -not $BootstrapHashes.ContainsKey($Version))
{
    $known = ($ArchiveHashes.Keys | Where-Object { $BootstrapHashes.ContainsKey($_) } | Sort-Object) -join ", "
    Throw "No pinned SHA-256 is recorded for 7-Zip $Version. Known versions: $known. To add one, download https://github.com/ip7z/7zip/releases and record the hashes of both the extra archive and 7zr.exe in `$ArchiveHashes and `$BootstrapHashes in $PSCommandPath."
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

# Unpacking the .7z needs a 7-Zip, so 7zr.exe is fetched first. The Windows-bundled tar.exe
# was used here before, but only recent builds of its libarchive carry the LZMA codec: on
# Windows Server 2019 it fails with "LZMA codec is unsupported". 7zr.exe is published as a
# plain .exe in the same release, so it can be downloaded, hash-pinned and run directly.
$bootstrapPath = Join-Path -Path $libsPath -ChildPath "7zr-$Version.exe"
$bootstrapUrl = "https://github.com/ip7z/7zip/releases/download/$Version/7zr.exe"

Write-Log "Downloading $bootstrapUrl"
Invoke-WebRequest -Uri $bootstrapUrl -OutFile $bootstrapPath

$expectedBootstrapHash = $BootstrapHashes[$Version]
$actualBootstrapHash = (Get-FileHash -Path $bootstrapPath -Algorithm SHA256).Hash
if ($actualBootstrapHash -ne $expectedBootstrapHash)
{
    Remove-Item $bootstrapPath -Force
    Throw "The SHA-256 of 7zr.exe did not match the pinned value. Expected $expectedBootstrapHash, got $actualBootstrapHash. The download was discarded."
}

Write-Log "SHA-256 verified: $actualBootstrapHash"

Invoke-BuildStep "Extracting $archiveName to $extractDir" {
    & $bootstrapPath x $archivePath "-o$extractDir" -y
}
$extractExitCode = $LastExitCode
Remove-Item $bootstrapPath -Force
if ($extractExitCode -ne 0)
{
    Throw "Extraction of $archivePath failed with exit code $extractExitCode. See $logPath for details."
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
