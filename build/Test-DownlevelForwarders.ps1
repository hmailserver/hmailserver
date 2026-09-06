function Test-DownlevelForwarders {
    # Fails if any DLL in $Path exports a forwarder whose module part carries its own ".dll"
    # extension - "kernel32.dll.VirtualAlloc" rather than "kernel32.VirtualAlloc".
    #
    # The loader in Windows Vista cannot resolve that form. Windows 10 can, so a build made
    # with a newer Universal CRT redistributable passes every test on a modern machine and
    # then dies at startup on Vista with an access violation in ntdll, before any hMailServer
    # code runs. That is what happened when the build moved to GitHub Actions and the
    # Universal CRT went from the 10.0.10240 redistributable to the SDK's 10.0.19041: 27 of
    # its forwarders use the new form, three of them ones ucrtbase.dll itself imports.
    #
    # Imports are not enough to catch this - both sets import the same functions from the same
    # modules. Only the forwarder strings differ, which is why this checks them directly.
    #
    # Passing is necessary, not sufficient. Microsoft supports no Universal CRT newer than
    # 10.0.14393 on Windows Vista through 8.1 - see "Universal CRT deployment" on Microsoft
    # Learn - and a redistributable above that ceiling can fail there in ways this does not
    # look for. Keep to a supported version; this only catches the one that got through.
    param(
        [Parameter(Mandatory = $true)][string]$Path
    )

    $dumpbin = Get-ChildItem (Join-Path ${env:ProgramFiles} 'Microsoft Visual Studio\*\*\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe') -ErrorAction SilentlyContinue |
        Sort-Object FullName -Descending | Select-Object -First 1
    if (-not $dumpbin) { throw "dumpbin.exe was not found; it ships with the Visual C++ build tools." }

    $offenders = @()
    foreach ($file in Get-ChildItem (Join-Path $Path '*.dll')) {
        foreach ($line in (& $dumpbin.FullName /exports $file.FullName)) {
            if ($line -match '\s(\S+)\s+\(forwarded to\s+([A-Za-z0-9_\-]+\.dll)\.(\S+)\)') {
                $offenders += "$($file.Name): $($Matches[1]) -> $($Matches[2]).$($Matches[3])"
            }
        }
    }

    if ($offenders) {
        throw "These forwarders cannot be resolved by the Windows Vista loader, which the installer still supports (MinVersion in section_setup.iss):`n" +
              ($offenders -join "`n") +
              "`nShip a redistributable that writes them as <module>.<function>, or drop Vista support."
    }

    "No forwarders that the Windows Vista loader cannot resolve were found in $Path."
}
