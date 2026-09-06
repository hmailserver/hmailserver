<#
.SYNOPSIS
Gives the network-transport test VMs a second adapter on a private Internal
switch with a static address, and rebuilds the "Ready" checkpoint.

.DESCRIPTION
VMTestRunner reaches guests older than Windows 10 over SMB and WMI. That traffic
used to go over the Hyper-V Default Switch, whose subnet Hyper-V re-randomizes
on every host reboot - which invalidates the port ACLs and leaves the guests
unreachable until Set-TestVMNetworkAcl.ps1 is run again.

This script moves that traffic onto a switch whose subnet never changes:

  * an Internal switch (host and VMs only, no internet),
  * a second adapter on each VM with a static address,
  * the Default Switch adapter kept only for DNS and DHCP, with port ACLs that
    no longer name a subnet, so a moving Default Switch cannot break them.

For every VM it restores the "Ready" checkpoint, applies the change, renames the
old checkpoint to "Ready <date>" and takes a new "Ready".

Must be run elevated.

.EXAMPLE
.\Add-TestVMTestNetAdapter.ps1 -WhatIf
Shows what would be done without changing anything.

.EXAMPLE
.\Add-TestVMTestNetAdapter.ps1
#>
[CmdletBinding(SupportsShouldProcess = $true, ConfirmImpact = 'High')]
param(
   # VM name -> static address on the Internal switch.
   [hashtable] $AddressMap = @{
      'Windows-8.1'       = '192.168.210.11'
      'Windows-Vista-SP2' = '192.168.210.12'
   },

   [string] $SwitchName   = 'TestNet',
   [string] $HostAddress  = '192.168.210.1',
   [int]    $PrefixLength = 24,
   [string] $SubnetMask   = '255.255.255.0',

   [string] $CheckpointName = 'Ready',

   # The account VMTestRunner uses inside the guests.
   [string] $GuestUser     = 'vmware',
   [string] $GuestPassword = 'Secret123',

   [int] $GuestTimeoutSeconds = 600,

   # Blocks DNS as well, leaving only DHCP and ICMP. The SURBL and DNS blacklist
   # tests are DNS lookups, so they fail when this is set.
   [switch] $BlockDns
)

$ErrorActionPreference = 'Stop'

$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
if (-not (New-Object Security.Principal.WindowsPrincipal($identity)).IsInRole(
          [Security.Principal.WindowsBuiltInRole]::Administrator)) {
   throw 'This script must be run elevated.'
}

# ---------------------------------------------------------------- helpers ----

function Test-TcpPort {
   param([string] $Address, [int] $Port, [int] $TimeoutMs = 2000)

   $client = New-Object Net.Sockets.TcpClient
   try {
      $async = $client.BeginConnect($Address, $Port, $null, $null)
      if (-not $async.AsyncWaitHandle.WaitOne($TimeoutMs)) { return $false }
      try { $client.EndConnect($async); return $true } catch { return $false }
   } finally { $client.Close() }
}

function Get-DefaultSwitchSubnet {
   # Same derivation as Set-TestVMNetworkAcl.ps1: the subnet moves, so it is
   # always read from the host rather than remembered.
   $hostIp = Get-NetIPAddress -AddressFamily IPv4 -InterfaceAlias 'vEthernet (Default Switch)' |
             Select-Object -First 1

   $bytes = ([Net.IPAddress]::Parse($hostIp.IPAddress)).GetAddressBytes()
   $bits  = $hostIp.PrefixLength

   for ($i = 0; $i -lt 4; $i++) {
      $keep = [Math]::Max(0, [Math]::Min(8, $bits - ($i * 8)))
      $bytes[$i] = $bytes[$i] -band (0xFF -shl (8 - $keep)) -band 0xFF
   }

   return "$($bytes -join '.')/$bits"
}

function Resolve-GuestAddress {
   # Finds the guest's Default Switch address. Guests with integration services
   # report it themselves; older ones are looked up by MAC in the host's
   # neighbour cache, which fills in once the guest talks on the network.
   param($Adapter)

   $reported = $Adapter.IPAddresses |
      Where-Object { $_ -match '^\d+\.\d+\.\d+\.\d+$' -and $_ -notlike '169.254.*' -and $_ -notlike '127.*' } |
      Select-Object -First 1

   if ($reported) { return $reported }

   $mac = ($Adapter.MacAddress -replace '(..)(?!$)', '$1-')

   return Get-NetNeighbor -AddressFamily IPv4 -LinkLayerAddress $mac -ErrorAction SilentlyContinue |
      Where-Object { $_.State -ne 'Unreachable' -and $_.IPAddress -notlike '169.254.*' } |
      Select-Object -ExpandProperty IPAddress -First 1
}

function Wait-ForGuestSmb {
   param([scriptblock] $AddressLookup, [int] $TimeoutSeconds)

   $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
   $address  = $null

   while ((Get-Date) -lt $deadline) {
      $address = & $AddressLookup

      if ($address -and (Test-TcpPort -Address $address -Port 445)) { return $address }

      Start-Sleep -Seconds 5
   }

   $seen = if ($address) { $address } else { '(none)' }
   throw "Timed out waiting for SMB on the guest. Last address seen: $seen"
}

function Invoke-Net {
   # net.exe writes to stderr for harmless things, such as deleting a connection
   # that isn't there. Redirected stderr would otherwise end the script.
   param([string[]] $Arguments)

   $previous = $ErrorActionPreference
   $ErrorActionPreference = 'Continue'

   try {
      $output = & net.exe @Arguments 2>&1
      return [pscustomobject]@{ ExitCode = $LASTEXITCODE; Output = ($output | Out-String).Trim() }
   } finally {
      $ErrorActionPreference = $previous
   }
}

function Connect-GuestShare {
   # A guest that has just booted accepts the connection on port 445 before its
   # SMB server is ready to authenticate, which shows up as error 64 (the network
   # name is no longer available). So this is retried rather than trusted once.
   param([string] $Address, [int] $TimeoutSeconds = 180)

   $share    = '\\' + $Address + '\IPC$'
   $deadline = (Get-Date).AddSeconds($TimeoutSeconds)
   $last     = '(not attempted)'

   while ((Get-Date) -lt $deadline) {
      # A session may be left over from an earlier attempt - failing to drop it is fine.
      Invoke-Net -Arguments @('use', $share, '/delete', '/y') | Out-Null

      $result = Invoke-Net -Arguments @('use', $share, $GuestPassword, "/user:$Address\$GuestUser")

      if ($result.ExitCode -eq 0) { return }

      $last = $result.Output
      Start-Sleep -Seconds 5
   }

   throw "Unable to authenticate to $share : $last"
}

function Disconnect-GuestShare {
   param([string] $Address)

   Invoke-Net -Arguments @('use', ('\\' + $Address + '\IPC$'), '/delete', '/y') | Out-Null
}

function Invoke-GuestCommandOverService {
   <#
   Runs a command in the guest by creating and starting a service over the SMB
   named pipe, the way PsExec does. This needs nothing but port 445, which makes
   it the dependable option for old guests where DCOM tends to hang.

   The service is expected to fail to start - a plain command is not a service
   and never reports back - but Windows runs the executable before giving up, so
   the command does execute. Error 1053 means exactly that.
   #>
   param([string] $Address, [string] $CommandLine, [string] $ServiceName = 'hmTestNetSetup')

   $target = '\\' + $Address

   Invoke-Net -Arguments @('use', $target + '\IPC$', '/delete', '/y') | Out-Null

   $create = & sc.exe $target create $ServiceName binPath= $CommandLine start= demand 2>&1 | Out-String
   if ($LASTEXITCODE -ne 0) { throw "sc create failed on $Address : $($create.Trim())" }

   try {
      $start = & sc.exe $target start $ServiceName 2>&1 | Out-String

      # 1053 is the service failing to check in, which is expected here.
      if ($LASTEXITCODE -ne 0 -and $start -notmatch '1053') {
         throw "sc start failed on $Address : $($start.Trim())"
      }
   } finally {
      & sc.exe $target delete $ServiceName 2>&1 | Out-Null
   }
}

function Invoke-GuestCommandOverWmi {
   # WMI over DCOM. Bounded, since an unreachable RPC endpoint otherwise hangs
   # for minutes.
   param([string] $Address, [string] $CommandLine, [int] $TimeoutSeconds = 30)

   $securePassword = ConvertTo-SecureString $GuestPassword -AsPlainText -Force
   $credential     = New-Object PSCredential("$Address\$GuestUser", $securePassword)

   # Old guests have no WinRM, so WMI goes over DCOM.
   $option = New-CimSessionOption -Protocol Dcom

   $session = New-CimSession -ComputerName $Address -Credential $credential -SessionOption $option `
                 -OperationTimeoutSec $TimeoutSeconds

   try {
      $result = Invoke-CimMethod -CimSession $session -ClassName Win32_Process -MethodName Create `
                  -Arguments @{ CommandLine = $CommandLine } -OperationTimeoutSec $TimeoutSeconds

      if ($result.ReturnValue -ne 0) {
         throw "Win32_Process.Create failed with $($result.ReturnValue)"
      }
   } finally {
      Remove-CimSession $session -ErrorAction SilentlyContinue
   }
}

function Set-GuestStaticAddress {
   # Configures the new adapter from inside the guest. It is identified by its
   # MAC, since the connection name differs per Windows version. No gateway is
   # set - the Internal switch is host-only and the default route has to stay on
   # the Default Switch adapter.
   param([string] $GuestAddress, [string] $Mac, [string] $StaticIp)

   $wmiMac = ($Mac -replace '(..)(?!$)', '$1:')

   # The script logs what it found, so a run that matched no adapter can be told
   # apart from one where EnableStatic itself failed.
   # 'log' cannot be used as a variable name - it is a VBScript built-in function.
   $vbs = @"
Set fso = CreateObject("Scripting.FileSystemObject")
Set logFile = fso.CreateTextFile("C:\Temp\set-testnet-ip.log", True)
Set wmi = GetObject("winmgmts:\\.\root\cimv2")
Set adapters = wmi.ExecQuery("SELECT * FROM Win32_NetworkAdapterConfiguration WHERE MACAddress = '$wmiMac'")
logFile.WriteLine "adapters matched: " & adapters.Count
For Each adapter In adapters
   rc = adapter.EnableStatic(Array("$StaticIp"), Array("$SubnetMask"))
   logFile.WriteLine "EnableStatic returned " & rc
Next
logFile.Close
"@

   Write-Output '    Copying the configuration script...'

   $temp = '\\' + $GuestAddress + '\C$\Temp'
   New-Item -ItemType Directory -Path $temp -Force | Out-Null
   Set-Content -Path (Join-Path $temp 'set-testnet-ip.vbs') -Value $vbs -Encoding ASCII

   $commandLine = 'cscript.exe //nologo C:\Temp\set-testnet-ip.vbs'
   $logPath     = Join-Path $temp 'set-testnet-ip.log'

   # The guest may still be installing the driver for the new adapter, in which
   # case the query matches nothing and the script quietly does nothing. So it is
   # run until the address actually shows up.
   for ($attempt = 1; $attempt -le 10; $attempt++) {
      Remove-Item $logPath -ErrorAction SilentlyContinue

      try {
         Write-Output "    Running it over WMI (attempt $attempt)..."
         Invoke-GuestCommandOverWmi -Address $GuestAddress -CommandLine $commandLine
      }
      catch {
         Write-Warning "    WMI did not work ($($_.Exception.Message.Trim())). Falling back to a service."
         Write-Output '    Running it over the SMB named pipe...'
         Invoke-GuestCommandOverService -Address $GuestAddress -CommandLine $commandLine
      }

      Start-Sleep -Seconds 5

      $applied = Get-GuestAdapterAddresses -Address $GuestAddress -WmiMac $wmiMac

      if ($applied -contains $StaticIp) {
         Write-Output "    The guest reports $StaticIp on the adapter."
         return
      }

      $reported = if ($applied) { $applied -join ', ' } else { '(the adapter is not there yet)' }
      Write-Output "    Not applied yet - the adapter reports: $reported"

      Start-Sleep -Seconds 10
   }

   # An empty log means the script died before writing anything, so the emptiness
   # is worth reporting rather than tripping over.
   $guestLog = if (Test-Path $logPath) { "$(Get-Content $logPath -Raw)".Trim() } else { '(no log was written)' }
   if (-not $guestLog) { $guestLog = '(the log is empty - the script failed before writing to it)' }

   throw "$GuestAddress : $StaticIp was never applied. Guest log: $guestLog"
}

function Get-GuestAdapterAddresses {
   # Asks the guest what addresses the adapter actually has, which is the only
   # trustworthy confirmation that the change took.
   param([string] $Address, [string] $WmiMac, [int] $TimeoutSeconds = 30)

   $securePassword = ConvertTo-SecureString $GuestPassword -AsPlainText -Force
   $credential     = New-Object PSCredential("$Address\$GuestUser", $securePassword)
   $option         = New-CimSessionOption -Protocol Dcom

   try {
      $session = New-CimSession -ComputerName $Address -Credential $credential -SessionOption $option `
                    -OperationTimeoutSec $TimeoutSeconds -ErrorAction Stop
   } catch {
      return @()
   }

   try {
      $configuration = Get-CimInstance -CimSession $session -ClassName Win32_NetworkAdapterConfiguration `
                          -OperationTimeoutSec $TimeoutSeconds |
                       Where-Object { $_.MACAddress -eq $WmiMac }

      if (-not $configuration) { return @() }

      return @($configuration.IPAddress)
   } finally {
      Remove-CimSession $session -ErrorAction SilentlyContinue
   }
}

function Set-DefaultSwitchAcl {
   # Rebuilds the Default Switch ACLs without a subnet rule, so they stay valid
   # when Hyper-V moves the Default Switch. Only DNS and DHCP get through - the
   # runner uses the Internal switch instead.
   param($Adapter)

   Get-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter | Remove-VMNetworkAdapterExtendedAcl

   foreach ($direction in 'Inbound', 'Outbound') {
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Deny  -Weight 10
      if (-not $BlockDns) {
         Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Allow -Weight 200 -Protocol UDP -RemotePort 53
         Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Allow -Weight 210 -Protocol TCP -RemotePort 53
      }
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Allow -Weight 300 -Protocol UDP -RemotePort 67
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Allow -Weight 305 -Protocol UDP -LocalPort 68
   }
}

function Set-ConfigurationAcl {
   # While the guest is being configured it is still reached over the Default
   # Switch, so that path has to work for the duration.
   param($Adapter, [string] $Subnet)

   Get-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter | Remove-VMNetworkAdapterExtendedAcl

   foreach ($direction in 'Inbound', 'Outbound') {
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Deny  -Weight 10
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Allow -Weight 100 -RemoteIPAddress $Subnet
      if (-not $BlockDns) {
         Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Allow -Weight 200 -Protocol UDP -RemotePort 53
         Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Allow -Weight 210 -Protocol TCP -RemotePort 53
      }
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Allow -Weight 300 -Protocol UDP -RemotePort 67
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $Adapter -Direction $direction -Action Allow -Weight 305 -Protocol UDP -LocalPort 68
   }
}

# ------------------------------------------------------------- the switch ----

if (-not (Get-VMSwitch -Name $SwitchName -ErrorAction SilentlyContinue)) {
   if ($PSCmdlet.ShouldProcess($SwitchName, 'Create Internal switch')) {
      New-VMSwitch -Name $SwitchName -SwitchType Internal | Out-Null
      Write-Output "Created Internal switch '$SwitchName'."
   }
}

$hostAlias = "vEthernet ($SwitchName)"

$hasAddress = Get-NetIPAddress -AddressFamily IPv4 -InterfaceAlias $hostAlias -ErrorAction SilentlyContinue |
              Where-Object IPAddress -eq $HostAddress

if (-not $hasAddress) {
   if ($PSCmdlet.ShouldProcess($hostAlias, "Assign $HostAddress/$PrefixLength")) {
      New-NetIPAddress -InterfaceAlias $hostAlias -IPAddress $HostAddress -PrefixLength $PrefixLength | Out-Null
      Write-Output "Assigned $HostAddress/$PrefixLength to '$hostAlias'."
   }
}

$defaultSwitchSubnet = Get-DefaultSwitchSubnet
Write-Output "Default Switch subnet (used only while configuring): $defaultSwitchSubnet"
Write-Output ''

# ---------------------------------------------------------------- the VMs ----

foreach ($vmName in $AddressMap.Keys) {
   $staticIp = $AddressMap[$vmName]
   Write-Output "=== $vmName -> $staticIp ==="

   Get-VM -Name $vmName | Out-Null

   if (-not $PSCmdlet.ShouldProcess($vmName, "Add '$SwitchName' adapter at $staticIp and rebuild '$CheckpointName'")) {
      continue
   }

   $checkpoint = Get-VMSnapshot -VMName $vmName -Name $CheckpointName

   # Start from the known-good state.
   Restore-VMSnapshot -VMSnapshot $checkpoint -Confirm:$false
   Stop-VM -Name $vmName -TurnOff -Force -ErrorAction SilentlyContinue

   # A dynamic MAC is only assigned at first power-on, and the guest-side
   # configuration needs it up front, so it is set explicitly.
   $staticMac = '00155DD200{0:X2}' -f [int]($staticIp.Split('.')[-1])

   # A generation 1 VM cannot hot-add an adapter, so this always happens powered off.
   $testNetAdapter = Get-VMNetworkAdapter -VMName $vmName |
                     Where-Object SwitchName -eq $SwitchName | Select-Object -First 1

   # An adapter left behind by an earlier run may have the wrong MAC - a dynamic
   # one reads as all zeroes until the VM has been started once. It is replaced
   # rather than reused, since the guest is configured by MAC.
   if ($testNetAdapter -and $testNetAdapter.MacAddress -ne $staticMac) {
      Write-Output "  Replacing the '$SwitchName' adapter (MAC $($testNetAdapter.MacAddress))."
      Remove-VMNetworkAdapter -VMNetworkAdapter $testNetAdapter
      $testNetAdapter = $null
   }

   if ($testNetAdapter) {
      Write-Output "  Adapter on '$SwitchName' already present with MAC $staticMac - reusing it."
   }
   else {
      # Guests without integration services have no synthetic NIC driver, so the
      # new adapter has to be the same kind the VM already uses.
      $isLegacy = [bool](Get-VMNetworkAdapter -VMName $vmName | Where-Object IsLegacy)

      Add-VMNetworkAdapter -VMName $vmName -SwitchName $SwitchName -IsLegacy:$isLegacy -StaticMacAddress $staticMac

      $testNetAdapter = Get-VMNetworkAdapter -VMName $vmName |
                        Where-Object SwitchName -eq $SwitchName | Select-Object -First 1

      $kind = if ($isLegacy) { 'legacy adapter' } else { 'adapter' }
      Write-Output "  Added $kind on '$SwitchName' (MAC $($testNetAdapter.MacAddress))."
   }

   if ($testNetAdapter.MacAddress -ne $staticMac) {
      throw "$vmName : the '$SwitchName' adapter has MAC $($testNetAdapter.MacAddress), expected $staticMac."
   }

   $defaultAdapter = Get-VMNetworkAdapter -VMName $vmName |
                     Where-Object SwitchName -eq 'Default Switch' | Select-Object -First 1

   if (-not $defaultAdapter) { throw "$vmName has no adapter on the Default Switch." }

   Set-ConfigurationAcl -Adapter $defaultAdapter -Subnet $defaultSwitchSubnet

   # The new adapter is unrestricted - the Internal switch reaches the host only.
   Get-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $testNetAdapter | Remove-VMNetworkAdapterExtendedAcl

   $mac = $staticMac

   Start-VM -Name $vmName
   Write-Output '  Waiting for the guest...'

   $guestAddress = Wait-ForGuestSmb -TimeoutSeconds $GuestTimeoutSeconds -AddressLookup {
      $adapter = Get-VMNetworkAdapter -VMName $vmName |
                 Where-Object SwitchName -eq 'Default Switch' | Select-Object -First 1
      Resolve-GuestAddress -Adapter $adapter
   }

   Write-Output "  Reached the guest at $guestAddress."

   Write-Output '    Authenticating to the guest...'
   Connect-GuestShare -Address $guestAddress

   try {
      Set-GuestStaticAddress -GuestAddress $guestAddress -Mac $mac -StaticIp $staticIp
   } finally {
      Disconnect-GuestShare -Address $guestAddress
   }

   # Prove the runner's path works before it is frozen into a checkpoint.
   Write-Output "  Waiting for SMB on $staticIp..."
   $deadline = (Get-Date).AddSeconds(120)
   while ((Get-Date) -lt $deadline -and -not (Test-TcpPort -Address $staticIp -Port 445)) {
      Start-Sleep -Seconds 5
   }

   if (-not (Test-TcpPort -Address $staticIp -Port 445)) {
      throw "$vmName did not become reachable on $staticIp. The checkpoint was left untouched."
   }

   Write-Output "  SMB reachable on $staticIp."

   # The runner no longer needs the Default Switch, so the subnet rule that used
   # to go stale can go away.
   Set-DefaultSwitchAcl -Adapter $defaultAdapter

   # Rename before creating, so the new checkpoint can take the name.
   $newName = "$CheckpointName $(Get-Date -Format 'yyyy-MM-dd')"
   if (Get-VMSnapshot -VMName $vmName -Name $newName -ErrorAction SilentlyContinue) {
      $newName = "$CheckpointName $(Get-Date -Format 'yyyy-MM-dd HHmm')"
   }

   Rename-VMSnapshot -VMSnapshot $checkpoint -NewName $newName
   Write-Output "  Renamed the old checkpoint to '$newName'."

   Checkpoint-VM -Name $vmName -SnapshotName $CheckpointName
   Write-Output "  Created a new '$CheckpointName' checkpoint."
   Write-Output ''
}

Write-Output 'Done. Set "guestAddress" in TestEnvironments.json to the static addresses above.'
