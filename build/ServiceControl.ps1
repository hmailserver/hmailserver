<#
.SYNOPSIS
	Shared helpers for stopping and starting the hMailServer service during a build.

.DESCRIPTION
	Dot-source this from a build script. None of these need elevation once the
	account has been granted service control; see build\grant-service-control.ps1.
#>

$HMailServerServiceName = 'hMailServer'

function Get-HMailServerService
{
	return Get-Service -Name $HMailServerServiceName -ErrorAction SilentlyContinue
}

# Stop-Service returns as soon as the service manager reports Stopped, which can be
# before the process has exited and released the executable and its DLLs.
function Wait-ForHMailServerExit([int]$timeoutSeconds = 30)
{
	$deadline = (Get-Date).AddSeconds($timeoutSeconds)

	while ((Get-Date) -lt $deadline)
	{
		if (-not (Get-Process -Name $HMailServerServiceName -ErrorAction SilentlyContinue)) { return }

		Start-Sleep -Milliseconds 250
	}

	throw "The $HMailServerServiceName process is still running after the service was stopped."
}

function Get-ServiceControlHint
{
	return "Run build\grant-service-control.ps1 once to control the $HMailServerServiceName " +
		'service without elevating, or run this script elevated.'
}

# Returns true if the service was running and has been stopped.
function Stop-HMailServer
{
	$service = Get-HMailServerService

	if (-not $service -or $service.Status -ne 'Running') { return $false }

	Write-Host "Stopping $HMailServerServiceName."

	try
	{
		Stop-Service -Name $HMailServerServiceName -Force -ErrorAction Stop
	}
	catch
	{
		throw "Could not stop the $HMailServerServiceName service: $($_.Exception.Message)`n" +
			(Get-ServiceControlHint)
	}

	Wait-ForHMailServerExit

	return $true
}

function Start-HMailServer
{
	if (-not (Get-HMailServerService)) { return }

	Write-Host "Starting $HMailServerServiceName."

	try
	{
		Start-Service -Name $HMailServerServiceName -ErrorAction Stop
	}
	catch
	{
		throw "Could not start the $HMailServerServiceName service: $($_.Exception.Message)`n" +
			(Get-ServiceControlHint)
	}
}
