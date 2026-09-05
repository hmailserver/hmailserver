<#
.SYNOPSIS
	Lets non-administrators read and change the hMailServer data folder.

.DESCRIPTION
	The installer restricts the data folder so that only the service, SYSTEM and
	the administrators may change it, and everyone else may only read it. Opening
	or changing the folder therefore needs an elevated program.

	This script grants the Users group modify access - read, write and delete - to
	the folder, so that it can be used without elevating.

	The folder holds the messages, hMailServer.ini and the event handler scripts.
	The server runs those scripts as its own service account, so anyone who can
	write to them can run code as that account. Only use this where every account
	on the computer is trusted, such as a test machine.

	Run elevated; the script elevates itself if needed.

.PARAMETER DataFolder
	The folder to change. Defaults to the data folder used by new installations.

.PARAMETER Revoke
	Removes the access granted by this script instead of adding it.

.EXAMPLE
	powershell -ExecutionPolicy Bypass -File GrantUsersDataFolderAccess.ps1

.EXAMPLE
	powershell -ExecutionPolicy Bypass -File GrantUsersDataFolderAccess.ps1 -Revoke
#>
[CmdletBinding()]
Param(
	[string]$DataFolder = "$env:ProgramData\hMailServer",
	[switch]$Revoke
)

$ErrorActionPreference = 'Stop'

# S-1-5-32-545 is the Users group. The SID is used rather than the name, since the
# name is translated on non-English versions of Windows.
$UsersSid = New-Object Security.Principal.SecurityIdentifier('S-1-5-32-545')

function Get-ExplicitUsersRules($folder)
{
	$acl = Get-Acl -Path $folder

	return @($acl.GetAccessRules($true, $false, [Security.Principal.SecurityIdentifier]) |
		Where-Object { $_.IdentityReference -eq $UsersSid })
}

# --- Elevate if needed. ------------------------------------------------------

$identity = [Security.Principal.WindowsIdentity]::GetCurrent()

if (-not ([Security.Principal.WindowsPrincipal]$identity).IsInRole(
	[Security.Principal.WindowsBuiltInRole]::Administrator))
{
	$scriptArguments = @(
		'-NoProfile'
		'-ExecutionPolicy', 'Bypass'
		'-File', "`"$PSCommandPath`""
		'-DataFolder', "`"$DataFolder`""
	)

	if ($Revoke) { $scriptArguments += '-Revoke' }

	Write-Host 'Elevation is required to change the permissions of a folder.'

	$process = Start-Process powershell -Verb RunAs -Wait -PassThru -ArgumentList $scriptArguments
	exit $process.ExitCode
}

# --- Apply the change. -------------------------------------------------------

try
{
	if (-not (Test-Path -Path $DataFolder -PathType Container))
		{ throw "The folder '$DataFolder' does not exist. Pass -DataFolder if hMailServer stores its data elsewhere." }

	$acl = Get-Acl -Path $DataFolder

	if ($Revoke)
	{
		if ((Get-ExplicitUsersRules $DataFolder).Count -eq 0)
		{
			Write-Host "The Users group has no access to '$DataFolder'. Nothing to do."
			exit 0
		}

		$acl.PurgeAccessRules($UsersSid)
	}
	else
	{
		# Applies to the folder, its subfolders and its files.
		$acl.AddAccessRule((New-Object Security.AccessControl.FileSystemAccessRule(
			$UsersSid,
			[Security.AccessControl.FileSystemRights]::Modify,
			'ContainerInherit,ObjectInherit',
			'None',
			[Security.AccessControl.AccessControlType]::Allow)))
	}

	Set-Acl -Path $DataFolder -AclObject $acl

	# Read the permissions back rather than trusting that they were written.
	$rules = Get-ExplicitUsersRules $DataFolder

	if ($Revoke)
	{
		if ($rules.Count -ne 0)
			{ throw "The Users group still has access to '$DataFolder'." }

		Write-Host "The Users group can no longer read '$DataFolder'."
	}
	else
	{
		$granted = @($rules | Where-Object {
			$_.AccessControlType -eq 'Allow' -and
			($_.FileSystemRights -band [Security.AccessControl.FileSystemRights]::Modify) -eq
				[Security.AccessControl.FileSystemRights]::Modify })

		if ($granted.Count -eq 0)
			{ throw "The permissions of '$DataFolder' were written but do not read back as expected." }

		Write-Host "The Users group can now read, write and delete files in '$DataFolder'."
	}

	exit 0
}
catch
{
	Write-Error $_
	exit 1
}
