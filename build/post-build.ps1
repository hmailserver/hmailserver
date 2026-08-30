<#
.SYNOPSIS
	Copies the runtime dependencies next to hMailServer.exe and registers the COM server.

.DESCRIPTION
	Only the registration writes to HKLM and reconfigures the Windows service, so
	only that part elevates - and it is skipped when the build output is already
	registered and the type library hasn't changed. The dependency copies and the
	service stop/start run as the current user.

	Starting and stopping the service needs no elevation once the account has been
	granted service control; see build\grant-service-control.ps1.

.PARAMETER Configuration
	Build configuration to post-process. Defaults to Debug.

.PARAMETER ForceRegister
	Register even when the build output already appears to be registered.

.PARAMETER NoServiceStart
	Leave the service stopped. Used by CI, which registers the server but has no
	reason to run it.
#>
Param(
	[string]$Configuration = 'Debug',
	[switch]$ForceRegister,
	[switch]$NoServiceStart
)

$ErrorActionPreference = 'Stop'

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Definition

. (Join-Path $scriptRoot 'ServiceControl.ps1')
$repoRoot = Resolve-Path (Join-Path $scriptRoot '..')

$outDir = Join-Path $repoRoot "hmailserver\source\Server\hMailServer\x64\$Configuration\"
$targetPath = Join-Path $outDir 'hMailServer.exe'
$typeLibPath = Join-Path $outDir 'hMailServer.tlb'
$stampPath = Join-Path $outDir 'post-build-registration.stamp'

$batchScript = Join-Path $repoRoot 'hmailserver\source\Server\hMailServer\post-build.bat'

# What the last successful registration in this output directory covered. The
# registered path is per-configuration, so switching Debug/Release re-registers.
function Get-RegistrationStamp
{
	# Hash the type library rather than looking at its timestamp: the build
	# rewrites it even when the interface hasn't changed.
	$typeLibHash = if (Test-Path $typeLibPath)
		{ (Get-FileHash -LiteralPath $typeLibPath -Algorithm SHA256).Hash } else { '' }

	return "$targetPath|$typeLibHash"
}

# The registered COM server, so that a registration done from another clone or
# configuration is noticed.
function Get-RegisteredServerPath
{
	try
	{
		$clsid = (Get-ItemProperty 'Registry::HKEY_CLASSES_ROOT\hMailServer.Application\CLSID' `
			-ErrorAction Stop).'(default)'

		return (Get-ItemProperty "Registry::HKEY_CLASSES_ROOT\CLSID\$clsid\LocalServer32" `
			-ErrorAction Stop).'(default)'
	}
	catch
	{
		return $null
	}
}

function Test-RegistrationNeeded
{
	if ($ForceRegister) { return $true }

	if (-not (Test-Path $stampPath)) { return $true }

	if ((Get-Content $stampPath -Raw).Trim() -ne (Get-RegistrationStamp)) { return $true }

	$registered = Get-RegisteredServerPath

	# The registered path is quoted and may carry arguments.
	return -not ($registered -and $registered.Contains($targetPath))
}

function Invoke-PostBuildBatch([string]$mode)
{
	& $batchScript $env:hMailServerLibs $outDir $targetPath $mode

	if ($LASTEXITCODE -ne 0)
		{ throw "post-build.bat $mode failed with exit code $LASTEXITCODE." }
}

if (-not $env:hMailServerLibs)
{
	Write-Error 'hMailServerLibs environment variable is not set.'
	exit 1
}

if (-not (Test-Path $targetPath))
{
	Write-Error "$targetPath not found. Build the $Configuration configuration first."
	exit 1
}

try
{
	Write-Host "Running post-build for configuration: $Configuration"
	Write-Host "Output directory: $outDir"

	Stop-HMailServer | Out-Null

	Invoke-PostBuildBatch 'COPY'

	if (Test-RegistrationNeeded)
	{
		Write-Host 'Registering the COM server. This needs elevation.'

		$isAdministrator = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).
			IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

		if ($isAdministrator)
		{
			Invoke-PostBuildBatch 'REGISTER'
		}
		else
		{
			# Elevate PowerShell rather than the batch file itself: the output
			# directory ends in a backslash, which the Windows command line parser
			# would read as escaping the closing quote of the argument.
			$logPath = Join-Path $env:TEMP 'hmailserver-post-build-register.log'

			$command = "& '$batchScript' '$env:hMailServerLibs' '$outDir' '$targetPath' REGISTER " +
				"*>&1 | Set-Content -LiteralPath '$logPath'; exit " + '$LASTEXITCODE'

			$process = Start-Process powershell -Verb RunAs -Wait -PassThru -ArgumentList @(
				'-NoProfile'
				'-ExecutionPolicy', 'Bypass'
				'-Command', $command
			)

			if (Test-Path $logPath) { Get-Content $logPath | Write-Host }

			if ($process.ExitCode -ne 0)
				{ throw "post-build.bat REGISTER failed with exit code $($process.ExitCode)." }
		}

		Set-Content -Path $stampPath -Value (Get-RegistrationStamp) -NoNewline
	}
	else
	{
		Write-Host 'The build output is already registered. Skipping registration.'
		Write-Host 'Pass -ForceRegister to register anyway.'
	}

	if ($NoServiceStart)
	{
		Write-Host 'Leaving the service stopped.'
	}
	else
	{
		Start-HMailServer
	}

	Write-Host 'Post-build completed successfully.'
	exit 0
}
catch
{
	Write-Error $_
	exit 1
}
