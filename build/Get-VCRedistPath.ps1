function Get-VCRedistPath {
    # Finds the Visual C++ redistributable DLLs (msvcp140.dll, vcruntime140.dll and friends)
    # that the installer ships beside hMailServer.exe.
    #
    # These must never be older than the toolset that compiled the binaries: the compiler
    # emits code and exception tables that assume the runtime it shipped with. They used to
    # be checked into hmailserver\installation, which silently drifted when the build moved
    # from VS2019 (14.25) to VS2022's v142 toolset (14.29).
    #
    # A newer runtime is fine - the 14.x series is backwards compatible, so the v145 redist
    # runs v142 binaries - and only going backwards is unsupported, which is what
    # MinimumVersion guards against.
    #
    # The installer ships two of them, because the redistributable version is also an
    # operating system floor. Microsoft's advice is to use the latest available runtime, and
    # the latest one supports Windows 10 and later only: its msvcp140.dll and concrt140.dll
    # statically import CopyFile2, CreateFile2 and GetLogicalProcessorInformationEx, which do
    # not exist on Windows Vista, and neither DLL delay-loads them. So:
    #
    #   default   the redistributable matching the pinned toolset, which is the newest one
    #             that still runs on every Windows version the installer allows. Also the
    #             build that a developer machine carrying several Visual Studios reproduces.
    #   -Newest   the newest installed, shipped to Windows 10 and later.
    #
    # section_files_64.iss picks between them by OS version at install time.
    param(
        [string]$MinimumVersion = '14.29',
        [switch]$Newest
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

            # Named after the toolset that produced it - Microsoft.VC142.CRT, VC145, and so
            # on - but the DLLs inside carry the same names whichever it is.
            $crt = Get-ChildItem (Join-Path $versionDir.FullName 'x64') -Directory -Filter 'Microsoft.VC*.CRT' -ErrorAction SilentlyContinue |
                Select-Object -First 1
            if (-not $crt) { continue }
            if (-not (Test-Path (Join-Path $crt.FullName 'vcruntime140_1.dll'))) { continue }

            [pscustomobject]@{ Version = $version; Path = $crt.FullName }
        }
    }

    if ($Newest) {
        $best = $candidates | Sort-Object Version -Descending | Select-Object -First 1
    }
    else {
        $preferred = "$($minimum.Major).$($minimum.Minor)"
        $best = $candidates |
            Sort-Object @{ Expression = { "$($_.Version.Major).$($_.Version.Minor)" -eq $preferred }; Descending = $true },
                        @{ Expression = { $_.Version }; Descending = $true } |
            Select-Object -First 1
    }
    if ($best) { return $best.Path }
    return $null
}
