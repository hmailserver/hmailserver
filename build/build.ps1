Param(
	[string]$Configuration = 'Debug',
	[switch]$Clean
)

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition
$solutionRelative = "..\hmailserver\source\Server\hMailServer\hMailServer.sln"
try {
	$solution = Resolve-Path (Join-Path $scriptRoot $solutionRelative) -ErrorAction Stop
} catch {
	Write-Error "Solution not found: $solutionRelative (resolved from $scriptRoot)"
	exit 1
}

$logsDir = Join-Path $scriptRoot "..\logs"
if (-not (Test-Path $logsDir)) { New-Item -Path $logsDir -ItemType Directory -Force | Out-Null }

# Use shared msbuild locator
. (Join-Path $scriptRoot "Find-MsBuild.ps1")
$msbuild = Find-MsBuild
if (-not $msbuild) {
	Write-Error "MSBuild not found. Install Visual Studio 2019 (Build Tools) or ensure msbuild is on PATH."
	exit 2
}

Write-Host "Using MSBuild: $msbuild"
Write-Host "Building solution: $solution"
Write-Host "Configuration: $Configuration"

if ($Clean) {
	Write-Host "Cleaning..."
	& "$msbuild" $solution '/t:Clean' "/p:Configuration=$Configuration" '/p:Platform=x64' *>&1
	if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

$msbuildArgs = @(
	$solution
	'/m'
	"/p:Configuration=$Configuration"
	'/p:Platform=x64'
	'/p:PreBuildEventUseInBuild=false'
	'/p:PostBuildEventUseInBuild=false'
)

$sw = [System.Diagnostics.Stopwatch]::StartNew()

& "$msbuild" @msbuildArgs *>&1

$exitCode = $LASTEXITCODE
$sw.Stop()
Write-Host ("Build completed in {0:F1} seconds." -f $sw.Elapsed.TotalSeconds)
if ($exitCode -ne 0) {
	exit $exitCode
}

Write-Host "Build succeeded."

