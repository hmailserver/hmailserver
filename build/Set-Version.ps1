<#
.SYNOPSIS
    Stamps a version and build number into the sources and the installer script.

.DESCRIPTION
    The version hMailServer reports is not stored in the repository: the files that carry it
    are committed with a 1.0.0 placeholder and rewritten at build time. This script performs
    that rewrite, and has to run before the server is compiled, since Version.h is compiled
    into hMailServer.exe.

    Three files are written:

        Server\Common\Application\Version.h   the version the server reports and the
                                              numeric version in its resources
        WebAdmin\include_versioncheck.php     the version WebAdmin refuses to run against
                                              anything else (it is generated, not committed)
        installation\section_setup_64.iss     the version Inno Setup stamps into the
                                              installer, and the installer's file name

    The CI build workflow calls this with the version and build number it derives for the
    run. A developer building locally does not need it: the placeholder compiles fine, and
    the resulting build simply reports itself as 1.0.0.

.PARAMETER Version
    Three-part product version, for example 5.7.0.

.PARAMETER Build
    Build number, appended as -B<build> where a full version is shown.

.PARAMETER RepoRoot
    Repository root to write into. Defaults to the repository this script lives in.

.EXAMPLE
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File build\Set-Version.ps1 5.7.0 1234
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true, Position = 0)]
    [ValidatePattern('^\d+\.\d+\.\d+$')]
    [string] $Version,

    [Parameter(Mandatory = $true, Position = 1)]
    [ValidatePattern('^\d+$')]
    [string] $Build,

    [Parameter(Position = 2)]
    [string] $RepoRoot
)

$ErrorActionPreference = "Stop"

# Not a param() default: $PSScriptRoot is not yet set while parameters are bound.
if (-not $RepoRoot) {
    $RepoRoot = (Get-Item $PSScriptRoot).Parent.FullName
}

if (-not (Test-Path -LiteralPath $RepoRoot -PathType Container)) {
    throw "Repository root $RepoRoot was not found."
}

# No BOM and CRLF, matching how the files are committed.
function Write-TextFile([string] $Path, [string] $Content) {
    $encoding = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($Path, ($Content -replace "`r?`n", "`r`n"), $encoding)
}

$versionFile = Join-Path $RepoRoot "hmailserver\source\Server\Common\Application\Version.h"
$phpFile = Join-Path $RepoRoot "hmailserver\source\WebAdmin\include_versioncheck.php"
$setupFile = Join-Path $RepoRoot "hmailserver\installation\section_setup_64.iss"

foreach ($file in @($versionFile, $setupFile)) {
    if (-not (Test-Path -LiteralPath $file -PathType Leaf)) {
        throw "$file was not found."
    }
}

# The resource compiler wants the version as a comma-separated list of four numbers.
$numericVersion = ($Version -replace '\.', ',') + ",$Build"

Write-Host "Writing the C++ version info to $versionFile"
Write-TextFile $versionFile @"
#pragma once
#define HMAILSERVER_VERSION "$Version"
#define HMAILSERVER_VERSION_NUMERIC $numericVersion
#define HMAILSERVER_BUILD $Build

"@

Write-Host "Writing the PHP version info to $phpFile"
Write-TextFile $phpFile @"
<?php
define('REQUIRED_VERSION', '$Version-B$Build');
?>
"@

Write-Host "Writing the install version and output name to $setupFile"
$settings = [ordered]@{
    "OutputBaseFilename" = "hMailServer-$Version-B$Build-x64"
    "AppVerName"         = "hMailServer $Version-B$Build-x64"
    "AppVersion"         = "$Version.$Build"
    "VersionInfoVersion" = "$Version.$Build"
}

$setup = Get-Content -LiteralPath $setupFile
foreach ($key in $settings.Keys) {
    $pattern = "^$([regex]::Escape($key))="
    if (-not ($setup -match $pattern)) {
        throw "$setupFile has no $key setting to write."
    }

    $setup = $setup -replace "$pattern.*", "$key=$($settings[$key])"
}

Write-TextFile $setupFile (($setup -join "`r`n") + "`r`n")

Write-Host "All done. Exiting."
