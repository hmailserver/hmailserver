# Determine repository root relative to this script (script is in the `build` folder)
$repoRoot = (Get-Item $PSScriptRoot).Parent.FullName

# Path to the NUnit console runner installed under the RegressionTests packages folder.
# The version is not hard coded here - it follows whatever packages.config restores.
$packagesRoot = Join-Path $repoRoot 'hmailserver\test\RegressionTests\packages'
$nunitExe = Get-ChildItem -Path $packagesRoot -Filter 'nunit3-console.exe' -Recurse -ErrorAction SilentlyContinue |
    Sort-Object FullName -Descending |
    Select-Object -First 1 -ExpandProperty FullName

if (-not $nunitExe) {
    Write-Error "NUnit console runner not found under: $packagesRoot. Restore NuGet packages first."
    exit 1
}

# Path to the test assembly to run (Debug x64 as requested)
$testAssembly = Join-Path $repoRoot 'hmailserver\test\RegressionTests\bin\x64\Debug\RegressionTests.dll'

if (-not (Test-Path $nunitExe)) {
    Write-Error "NUnit console runner not found: $nunitExe"
    exit 1
}

if (-not (Test-Path $testAssembly)) {
    Write-Error "Test assembly not found: $testAssembly"
    exit 1
}

Write-Host "Running tests:" -ForegroundColor Cyan
Write-Host "  Runner: $nunitExe"
Write-Host "  Assembly: $testAssembly"

# Execute the console runner and stream output in real time
# Add helpful NUnit arguments to show test names as they run
$nunitArgs = @(
    $testAssembly,
    '--labels=All',
    '/stoponerror'
)
$nunitArgString = [string]::Join(' ', $nunitArgs)

Write-Host "Starting NUnit runner (streaming output)..." -ForegroundColor Cyan

$psi = New-Object System.Diagnostics.ProcessStartInfo
$psi.FileName = $nunitExe
$psi.Arguments = $nunitArgString
$psi.RedirectStandardOutput = $true
$psi.RedirectStandardError = $true
$psi.UseShellExecute = $false
$psi.CreateNoWindow = $true

$proc = New-Object System.Diagnostics.Process
$proc.StartInfo = $psi
$proc.EnableRaisingEvents = $true


$proc.Start() | Out-Null

# Read redirected streams on the main thread to avoid PowerShell runspace errors
$stdout = $proc.StandardOutput
$stderr = $proc.StandardError

while (-not $proc.HasExited -or -not $stdout.EndOfStream -or -not $stderr.EndOfStream) {
    while ($stdout.Peek() -ne -1) {
        $line = $stdout.ReadLine()
        if ($line) { [System.Console]::WriteLine($line) }
    }
    while ($stderr.Peek() -ne -1) {
        $line = $stderr.ReadLine()
        if ($line) {
            $old = [System.Console]::ForegroundColor
            [System.Console]::ForegroundColor = 'Red'
            [System.Console]::WriteLine($line)
            [System.Console]::ForegroundColor = $old
        }
    }
    Start-Sleep -Milliseconds 50
}

$proc.WaitForExit()

$lastExit = $proc.ExitCode
if ($lastExit -ne 0) {
    Write-Error "Tests failed with exit code $lastExit"
}
exit $lastExit
