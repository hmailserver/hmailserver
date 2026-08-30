<#
.SYNOPSIS
	Lets a non-administrator account start and stop the hMailServer service.

.DESCRIPTION
	Rebuilding hMailServer means stopping the service, and building it means
	starting it again. By default that needs an elevated prompt every time.

	This script adds one allow-ACE to the service's security descriptor, so the
	given account can start, stop and query the service without elevating.
	It grants nothing else - the account still can't reconfigure or delete the
	service.

	Run once, elevated. The grant survives rebuilds, redeploys and
	"hMailServer.exe /Register", but is lost if the service itself is deleted
	and recreated - by an uninstall, a reinstall, or "sc delete". Re-run it then.

.PARAMETER ServiceName
	The service to grant access to. Defaults to hMailServer.

.PARAMETER Account
	The account to grant access to, for example DOMAIN\User or a SID.
	Defaults to the user running the script.

.PARAMETER Revoke
	Removes the account's ACE instead of adding it.

.EXAMPLE
	powershell -ExecutionPolicy Bypass -File build\grant-service-control.ps1

.EXAMPLE
	powershell -ExecutionPolicy Bypass -File build\grant-service-control.ps1 -Account DESKTOP\Someone
#>
[CmdletBinding()]
Param(
	[string]$ServiceName = 'hMailServer',
	[string]$Account,
	[switch]$Revoke
)

$ErrorActionPreference = 'Stop'

# Query config, query status, enumerate dependents, start, stop, pause/continue,
# interrogate, user-defined control, read the security descriptor.
$AccessMask = 'CCLCSWRPWPDTLOCRRC'

function Resolve-AccountSid([string]$account)
{
	if (-not $account)
		{ return ([Security.Principal.WindowsIdentity]::GetCurrent()).User.Value }

	# Already a SID?
	if ($account -match '^S-\d-(\d+-){1,14}\d+$')
		{ return $account }

	try
	{
		return (New-Object Security.Principal.NTAccount($account)).Translate(
			[Security.Principal.SecurityIdentifier]).Value
	}
	catch
	{
		throw "Could not resolve '$account' to a SID. Use DOMAIN\User, .\User or a SID."
	}
}

# Splits an SDDL string into its O: G: D: S: sections. Section letters also occur
# inside ACE strings, so only markers at parenthesis depth zero count.
function Split-Sddl([string]$sddl)
{
	$sections = [ordered]@{}
	$current = $null
	$start = 0
	$depth = 0

	for ($i = 0; $i -lt $sddl.Length; $i++)
	{
		switch ($sddl[$i])
		{
			'(' { $depth++ }
			')' { $depth-- }
		}

		if ($depth -ne 0 -or $i + 1 -ge $sddl.Length) { continue }

		if ($sddl[$i + 1] -eq ':' -and 'OGDS'.Contains($sddl[$i]))
		{
			if ($current) { $sections[$current] = $sddl.Substring($start, $i - $start) }

			$current = [string]$sddl[$i]
			$start = $i + 2
			$i++
		}
	}

	if ($current) { $sections[$current] = $sddl.Substring($start) }

	return $sections
}

function Join-Sddl($sections)
{
	$sddl = ''
	foreach ($key in $sections.Keys) { $sddl += "$key`:" + $sections[$key] }
	return $sddl
}

# Returns the individual "(...)" ACE strings of a section.
function Get-Aces([string]$section)
{
	return [regex]::Matches($section, '\([^)]*\)') | ForEach-Object { $_.Value }
}

function Get-AceTrustee([string]$ace)
{
	# (type;flags;rights;object_guid;inherit_object_guid;trustee)
	$fields = $ace.Trim('(', ')').Split(';')
	if ($fields.Count -lt 6) { return $null }
	return $fields[5]
}

function Invoke-Sc([string[]]$scArguments)
{
	$output = & sc.exe @scArguments 2>&1
	return [PSCustomObject]@{
		ExitCode = $LASTEXITCODE
		Output   = ($output | Out-String).Trim()
	}
}

function Get-ServiceSddl([string]$serviceName)
{
	$result = Invoke-Sc @('sdshow', $serviceName)

	if ($result.ExitCode -ne 0)
		{ throw "sc.exe sdshow failed for '$serviceName': $($result.Output)" }

	# sdshow prints blank lines around the descriptor, and wraps long ones.
	$sddl = ($result.Output -split '\r?\n' | ForEach-Object { $_.Trim() }) -join ''

	if ($sddl -notmatch '^[OGDS]:')
		{ throw "Unexpected output from sc.exe sdshow: '$($result.Output)'" }

	return $sddl
}

# --- Elevate if needed. ------------------------------------------------------

$identity = [Security.Principal.WindowsIdentity]::GetCurrent()

if (-not ([Security.Principal.WindowsPrincipal]$identity).IsInRole(
	[Security.Principal.WindowsBuiltInRole]::Administrator))
{
	# Resolve the account before elevating, so that the default is the user who
	# started the script rather than the administrator it elevates to.
	$sid = Resolve-AccountSid $Account

	$scriptArguments = @(
		'-NoProfile'
		'-ExecutionPolicy', 'Bypass'
		'-File', "`"$PSCommandPath`""
		'-ServiceName', "`"$ServiceName`""
		'-Account', $sid
	)

	if ($Revoke) { $scriptArguments += '-Revoke' }

	Write-Host 'Elevation is required to change a service security descriptor.'

	$process = Start-Process powershell -Verb RunAs -Wait -PassThru -ArgumentList $scriptArguments
	exit $process.ExitCode
}

# --- Apply the change. -------------------------------------------------------

try
{
	if (-not (Get-Service -Name $ServiceName -ErrorAction SilentlyContinue))
		{ throw "The service '$ServiceName' does not exist. Install hMailServer, or run hMailServer.exe /Register." }

	$sid = Resolve-AccountSid $Account
	$account = try { (New-Object Security.Principal.SecurityIdentifier($sid)).Translate(
		[Security.Principal.NTAccount]).Value } catch { $sid }

	$sddl = Get-ServiceSddl $ServiceName
	Write-Host "Current descriptor: $sddl"

	$sections = Split-Sddl $sddl

	if (-not $sections.Contains('D'))
		{ throw "The descriptor of '$ServiceName' has no DACL, refusing to guess one." }

	$dacl = $sections['D']

	# Inherited-ACE flags and the like precede the first ACE; keep them.
	$aces = @(Get-Aces $dacl)
	$prefix = if ($aces.Count -gt 0) { $dacl.Substring(0, $dacl.IndexOf($aces[0])) } else { $dacl }

	$existing = @($aces | Where-Object { (Get-AceTrustee $_) -eq $sid })
	$wanted = "(A;;$AccessMask;;;$sid)"

	if ($Revoke)
	{
		if ($existing.Count -eq 0)
		{
			Write-Host "$account has no explicit ACE on '$ServiceName'. Nothing to do."
			exit 0
		}

		$kept = @($aces | Where-Object { (Get-AceTrustee $_) -ne $sid })
	}
	else
	{
		if ($existing -contains $wanted -and $existing.Count -eq 1)
		{
			Write-Host "$account can already control '$ServiceName'. Nothing to do."
			exit 0
		}

		# Drop any stale ACE for this account, then append the one we want.
		$kept = @($aces | Where-Object { (Get-AceTrustee $_) -ne $sid }) + $wanted
	}

	$sections['D'] = $prefix + ($kept -join '')
	$updated = Join-Sddl $sections

	Write-Host "New descriptor:     $updated"

	$result = Invoke-Sc @('sdset', $ServiceName, $updated)

	if ($result.ExitCode -ne 0)
		{ throw "sc.exe sdset failed: $($result.Output)" }

	# Read it back rather than trusting the exit code.
	$applied = Get-ServiceSddl $ServiceName
	$appliedHasAce = @(Get-Aces (Split-Sddl $applied)['D']) -contains $wanted

	if ($Revoke -eq $appliedHasAce)
		{ throw "The descriptor was written but does not read back as expected: $applied" }

	if ($Revoke)
		{ Write-Host "Revoked service control of '$ServiceName' from $account." }
	else
		{ Write-Host "$account can now start and stop '$ServiceName' without elevating." }

	exit 0
}
catch
{
	Write-Error $_
	exit 1
}
