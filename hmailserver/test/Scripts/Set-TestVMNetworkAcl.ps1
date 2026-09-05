<#
.SYNOPSIS
Restricts test VMs to the Hyper-V Default Switch network.

.DESCRIPTION
Rebuilds the extended port ACLs on each VM network adapter connected to the
Default Switch: everything is denied except traffic to/from the host-only
subnet, DNS and DHCP. This keeps regression test VMs from reaching the
internet while still letting them talk to the test host.

Must be run elevated.

.EXAMPLE
.\Set-TestVMNetworkAcl.ps1 -VMName Windows-Vista-SP2, Windows-Server-2025

.EXAMPLE
.\Set-TestVMNetworkAcl.ps1
Applies the ACLs to every VM connected to the Default Switch.
#>
param(
   [string[]] $VMName
)

$ErrorActionPreference = 'Stop'

# Derive the Default Switch subnet from the host vEthernet address. Hyper-V
# re-randomizes this subnet, so it must never be hard-coded.
$hostIp = Get-NetIPAddress -AddressFamily IPv4 -InterfaceAlias 'vEthernet (Default Switch)'
$bytes  = ([System.Net.IPAddress]::Parse($hostIp.IPAddress)).GetAddressBytes()
$bits   = $hostIp.PrefixLength
for ($i = 0; $i -lt 4; $i++) {
   $keep = [Math]::Max(0, [Math]::Min(8, $bits - ($i * 8)))
   $bytes[$i] = $bytes[$i] -band (0xFF -shl (8 - $keep)) -band 0xFF
}
$subnet = "$($bytes -join '.')/$bits"

Write-Output "Default Switch host IP : $($hostIp.IPAddress)/$($hostIp.PrefixLength)"
Write-Output "Allowed subnet         : $subnet"
Write-Output ""

$vms = if ($VMName) { $VMName | ForEach-Object { Get-VM -Name $_ } } else { Get-VM }

foreach ($vm in $vms) {
   $applied = $false

   foreach ($na in Get-VMNetworkAdapter -VM $vm) {
      if ($na.SwitchName -ne 'Default Switch') { continue }

      # Start from a clean slate so stale subnets don't linger.
      Get-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $na | Remove-VMNetworkAdapterExtendedAcl

      foreach ($dir in 'Inbound','Outbound') {
         # Default: block everything.
         Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $na -Direction $dir -Action Deny -Weight 10

         # Host-only network (includes the DHCP server and the test host).
         Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $na -Direction $dir -Action Allow -Weight 100 -RemoteIPAddress $subnet

         # DNS, so name resolution keeps working even for external names.
         Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $na -Direction $dir -Action Allow -Weight 200 -Protocol UDP -RemotePort 53
         Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $na -Direction $dir -Action Allow -Weight 210 -Protocol TCP -RemotePort 53
      }

      # DHCP is broadcast-based, so it is not covered by the subnet rule above.
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $na -Direction Outbound -Action Allow -Weight 300 -Protocol UDP -RemotePort 67
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $na -Direction Outbound -Action Allow -Weight 305 -Protocol UDP -LocalPort 68
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $na -Direction Inbound  -Action Allow -Weight 300 -Protocol UDP -RemotePort 67
      Add-VMNetworkAdapterExtendedAcl -VMNetworkAdapter $na -Direction Inbound  -Action Allow -Weight 305 -Protocol UDP -LocalPort 68

      $applied = $true
      Write-Output "$($vm.Name): ACLs rebuilt on adapter '$($na.Name)'"
   }

   if (-not $applied) {
      Write-Warning "$($vm.Name): no adapter connected to the Default Switch - skipped"
   }
}

foreach ($vm in $vms) {
   Write-Output ""
   Write-Output "=== Resulting ACLs ($($vm.Name)) ==="
   Get-VMNetworkAdapterExtendedAcl -VMName $vm.Name |
      Sort-Object Direction,Weight |
      Format-Table Direction,Action,RemoteIPAddress,LocalPort,RemotePort,Protocol,Weight -AutoSize |
      Out-String | Write-Output
}
