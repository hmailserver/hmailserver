<#
Grant modify permissions for the hMailServer installation folder.

This script will relaunch itself elevated if not already running as Administrator.
It grants 'Everyone' Modify permissions recursively to:
  C:\Program Files (x86)\hMailServer
  C:\Program Files\hMailServer
  C:\ProgramData\hMailServer

Use this before running the test suite. The tests delete and read the server log
and the message files, which live under ProgramData. Run with care - granting
'Everyone' write access is insecure on multi-user machines. Prefer running tests
in a dedicated test VM.
#>

$targets = @(
    'C:\Program Files (x86)\hMailServer',
    'C:\Program Files\hMailServer',
    (Join-Path $env:ProgramData 'hMailServer')
)

function Test-IsElevated {
    $current = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($current)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

if (-not (Test-IsElevated)) {
    Write-Host "Not running elevated - relaunching as Administrator..."
    try {
        Start-Process -FilePath 'powershell.exe' -ArgumentList '-NoProfile','-ExecutionPolicy','Bypass','-File',$PSCommandPath -Verb RunAs -WindowStyle Normal
    } catch {
        Write-Error "Failed to elevate. Aborting."
    }
    exit
}

$exitCode = 0

foreach ($target in $targets) {
    if (-not (Test-Path -Path $target)) {
        Write-Host "Target path not found, skipping: $target"
        continue
    }

    Write-Host "Applying permissions to: $target"

    $cmd = "icacls `"$target`" /grant Everyone:(OI)(CI)M /T"
    Write-Host $cmd

    $proc = Start-Process -FilePath 'icacls' -ArgumentList @("`"$target`"", "/grant", "Everyone:(OI)(CI)M", "/T") -NoNewWindow -Wait -PassThru

    if ($proc.ExitCode -eq 0) {
        Write-Host "Permissions applied successfully."
    } else {
        Write-Error "icacls exited with code $($proc.ExitCode) for: $target"
        $exitCode = $proc.ExitCode
    }
}

exit $exitCode
