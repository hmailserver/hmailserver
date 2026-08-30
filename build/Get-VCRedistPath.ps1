function Get-VCRedistPath {
    # Finds the Visual C++ redistributable DLLs (msvcp140.dll, vcruntime140.dll and friends)
    # that the installer ships beside hMailServer.exe.
    #
    # These must never be older than the toolset that compiled the binaries: the compiler
    # emits code and exception tables that assume the runtime it shipped with. They used to
    # be checked into hmailserver\installation, which silently drifted when the build moved
    # from VS2019 (14.25) to VS2022's v142 toolset (14.29).
    #
    # A newer runtime is fine - the 14.x series is backwards compatible, so the v143 redist
    # runs v142 binaries - and only going backwards is unsupported, which is what
    # MinimumVersion guards against. The redist matching the toolset is still preferred when
    # it is installed, so that a developer machine carrying several Visual Studios produces
    # the same installer the build server does.
    param(
        [string]$MinimumVersion = '14.29'
    )

    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) {
        Write-Verbose "vswhere not found at $vswhere"
        return $null
    }

    $installations = @(& $vswhere -products '*' -all -prerelease -property installationPath)
    $minimum = [version]$MinimumVersion

    $candidates = foreach ($installation in $installations) {
        $root = Join-Path $installation 'VC\Redist\MSVC'
        if (-not (Test-Path $root)) { continue }

        foreach ($versionDir in Get-ChildItem $root -Directory -ErrorAction SilentlyContinue) {
            # The folder is named for the exact redist version, except for the 'v142'/'v143'
            # junctions that sit beside them - those fail to parse and are skipped here.
            $version = $null
            if (-not [version]::TryParse($versionDir.Name, [ref]$version)) { continue }
            if ($version -lt $minimum) { continue }

            # Named after the toolset that produced it - Microsoft.VC142.CRT, VC143, and so
            # on - but the DLLs inside carry the same names whichever it is.
            $crt = Get-ChildItem (Join-Path $versionDir.FullName 'x64') -Directory -Filter 'Microsoft.VC*.CRT' -ErrorAction SilentlyContinue |
                Select-Object -First 1
            if (-not $crt) { continue }
            if (-not (Test-Path (Join-Path $crt.FullName 'vcruntime140_1.dll'))) { continue }

            [pscustomobject]@{ Version = $version; Path = $crt.FullName }
        }
    }

    $preferred = "$($minimum.Major).$($minimum.Minor)"
    $best = $candidates |
        Sort-Object @{ Expression = { "$($_.Version.Major).$($_.Version.Minor)" -eq $preferred }; Descending = $true },
                    @{ Expression = { $_.Version }; Descending = $true } |
        Select-Object -First 1
    if ($best) { return $best.Path }
    return $null
}
