Param(
    [string]$Configuration = 'Debug'
)

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition

# Load shared msbuild locator
. (Join-Path $scriptRoot "Find-MsBuild.ps1")
$msbuild = Find-MsBuild
if (-not $msbuild) {
    Write-Error "MSBuild not found. Install Visual Studio 2019 or 2022 (Build Tools) or ensure msbuild is on PATH."
    exit 2
}

$solutionRelative = "..\hmailserver\test\RegressionTests\RegressionTests.sln"
try {
    $solution = Resolve-Path (Join-Path $scriptRoot $solutionRelative) -ErrorAction Stop
} catch {
    Write-Error "Solution not found: $solutionRelative (resolved from $scriptRoot)"
    exit 1
}

$logsDir = Join-Path $scriptRoot "..\logs"
if (-not (Test-Path $logsDir)) { New-Item -Path $logsDir -ItemType Directory -Force | Out-Null }

Write-Host "Using MSBuild: $msbuild"
Write-Host "Building solution: $solution"
Write-Host "Configuration: $Configuration"

$msbuildArgs = @(
    $solution
    '/m'
    "/p:Configuration=$Configuration"
    '/p:Platform=x64'
)

# Run MSBuild and tee output to log
& "$msbuild" @msbuildArgs *>&1

$exitCode = $LASTEXITCODE
if ($exitCode -ne 0) {
    exit $exitCode
}

Write-Host "Build succeeded. Build log: $msbuildLog"
