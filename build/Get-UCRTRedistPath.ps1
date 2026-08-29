function Get-UCRTRedistPath {
    # Finds the Universal CRT to ship beside hMailServer.exe for the Windows versions that do
    # not have one: the installer supports Vista SP2 and later (MinVersion in
    # section_setup.iss), and the UCRT only became part of Windows in Windows 10. On Windows
    # 10 and later these files are never loaded - api-ms-win-crt-* are API sets the loader
    # resolves to the in-box ucrtbase.dll before it ever searches the application directory -
    # so section_files_64.iss installs them only below Windows 10.
    #
    # The version is an allow-list rather than "whatever the newest SDK is", because these
    # DLLs have to keep loading on Vista. What decides that is ucrtbase.dll's own dependency
    # surface, and it grows over time. Measured against the 10.0.10240 set hMailServer shipped
    # for a decade:
    #
    #   10.0.18362.0   25 API sets, identical to 10240
    #   10.0.19041.0   25 API sets, identical to 10240
    #   10.0.22000.0   26 API sets, adds core-fibers-l1-1-0
    #   10.0.26100.0   29 API sets, adds core-fibers, core-kernel32-legacy, core-sysinfo-l1-2-0
    #
    # Only the first two are known to ask no more of the OS than what already works, so only
    # those may ship. Before adding a version here, compare its ucrtbase.dll imports against
    # that baseline - and test the result on the oldest Windows the installer still allows.
    param(
        [string[]]$AllowedVersions = @('10.0.19041.0', '10.0.18362.0')
    )

    $kits = @("${env:ProgramFiles(x86)}\Windows Kits\10\Redist", "${env:ProgramFiles}\Windows Kits\10\Redist")

    # In allow-list order, which is newest first - not whatever the file system offers.
    foreach ($version in $AllowedVersions) {
        foreach ($kit in $kits) {
            $candidate = Join-Path $kit "$version\ucrt\DLLs\x64"
            if (Test-Path (Join-Path $candidate 'ucrtbase.dll')) { return $candidate }
        }
    }

    return $null
}
