<#
.SYNOPSIS
    Reports the third-party library versions hMailServer is currently pinned to.

.DESCRIPTION
    The versions of OpenSSL, Boost, PostgreSQL (libpq), MariaDB Connector/C and 7-Zip that
    hMailServer builds against or ships are not recorded in one place: they are embedded in
    the include/library paths of hMailServer.vcxproj and in the files post-build.bat copies.
    This script reads them back out, so callers do not have to hard-code a version that then
    has to be remembered on every upgrade.

    It is used by the CI build workflow for two things: the cache keys of the prebuilt
    libraries (so bumping a version invalidates exactly the caches that depend on it) and the
    paths of the runtime DLLs that go into the build artifact. The upgrade-* skills rewrite
    the pinned versions in the files parsed here, so they keep working with no CI edit.

    Output is one 'name=value' line per value, and, when running inside GitHub Actions
    ($env:GITHUB_OUTPUT set), the same lines are appended there as step outputs:

        openssl=3.5.8               openssl_dir=openssl-3.5.8
        boost=1.92.0                boost_dir=boost_1_92_0
        postgresql=15.19            postgresql_dir=postgresql-15.19
        mariadb=3.4.9               mariadb_dir=libmariadb-3.4.9
        sevenzip=26.03              sevenzip_dir=7zip-26.03

    The *_dir values are the folder names under %hMailServerLibs% that the build scripts
    create, which is what the workflow caches.

.EXAMPLE
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File build\Get-LibraryVersions.ps1
#>
[CmdletBinding()]
param()

$ErrorActionPreference = "Stop"

$repoRoot = (Get-Item $PSScriptRoot).Parent.FullName
$vcxproj = Join-Path -Path $repoRoot -ChildPath "hmailserver\source\Server\hMailServer\hMailServer.vcxproj"
$postBuild = Join-Path -Path $repoRoot -ChildPath "hmailserver\source\Server\hMailServer\post-build.bat"

foreach ($file in @($vcxproj, $postBuild))
{
    if (!(Test-Path $file))
    {
        Throw "Expected file not found: $file"
    }
}

# Read each file once; the same content is searched for several versions.
$vcxprojText = Get-Content -Path $vcxproj -Raw
$postBuildText = Get-Content -Path $postBuild -Raw

# Pull the first match of $Pattern out of $Text, failing loudly rather than returning an
# empty version that would silently produce a wrong cache key or DLL path.
function Get-PinnedVersion
{
    param(
        [Parameter(Mandatory = $true)][string]$Text,
        [Parameter(Mandatory = $true)][string]$Pattern,
        [Parameter(Mandatory = $true)][string]$Description,
        [Parameter(Mandatory = $true)][string]$Source
    )

    $match = [regex]::Match($Text, $Pattern)
    if (-not $match.Success)
    {
        Throw "Could not determine the $Description version from $Source (pattern: $Pattern)."
    }

    return $match.Groups[1].Value
}

# The same version strings the library build scripts and the project file use:
#   openssl-3.5.8, boost_1_92_0, postgresql-15.19 in the project's include/library paths,
#   libmariadb-3.4.9 in the DLL post-build.bat copies into the output directory.
$openssl = Get-PinnedVersion -Text $vcxprojText -Pattern 'openssl-(\d+\.\d+\.\d+)' -Description 'OpenSSL' -Source $vcxproj
$boostUnderscored = Get-PinnedVersion -Text $vcxprojText -Pattern 'boost_(\d+_\d+_\d+)' -Description 'Boost' -Source $vcxproj
$postgresql = Get-PinnedVersion -Text $vcxprojText -Pattern 'postgresql-(\d+\.\d+)' -Description 'PostgreSQL' -Source $vcxproj
$mariadb = Get-PinnedVersion -Text $postBuildText -Pattern 'libmariadb-(\d+\.\d+\.\d+)' -Description 'MariaDB Connector/C' -Source $postBuild

# 7-Zip is a prebuilt tool rather than something hMailServer links against, so it appears in
# no compiler path; post-build.bat and hMailServer64.iss both name the 7zip-<version> folder.
$sevenzip = Get-PinnedVersion -Text $postBuildText -Pattern '7zip-(\d+\.\d+)' -Description '7-Zip' -Source $postBuild

# Boost is pinned as a folder name with underscores (boost_1_92_0); build-boost.ps1 takes the
# dotted form.
$boost = $boostUnderscored -replace '_', '.'

$values = [ordered]@{
    openssl            = $openssl
    openssl_dir        = "openssl-$openssl"
    boost              = $boost
    boost_dir          = "boost_$boostUnderscored"
    postgresql         = $postgresql
    postgresql_dir     = "postgresql-$postgresql"
    mariadb            = $mariadb
    mariadb_dir        = "libmariadb-$mariadb"
    sevenzip           = $sevenzip
    sevenzip_dir       = "7zip-$sevenzip"
}

foreach ($name in $values.Keys)
{
    "$name=$($values[$name])"
}

if ($env:GITHUB_OUTPUT)
{
    foreach ($name in $values.Keys)
    {
        Add-Content -Path $env:GITHUB_OUTPUT -Value "$name=$($values[$name])"
    }
}
