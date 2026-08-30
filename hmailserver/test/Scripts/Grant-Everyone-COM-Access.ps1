$log = Join-Path $PSScriptRoot 'result.txt'
try {
  $ErrorActionPreference = 'Stop'
  $path = 'HKLM:\SOFTWARE\Classes\AppID\{5EDEC473-39E0-43F6-A234-1947071721C8}'

  # 0x1F = Local/Remote Launch + Local/Remote Activation, 0x7 = Local/Remote Access
  $launchSddl = 'O:BAG:BAD:(A;;0x1f;;;WD)(A;;0x1f;;;BA)(A;;0x1f;;;SY)'
  $accessSddl = 'O:BAG:BAD:(A;;0x7;;;WD)(A;;0x7;;;BA)(A;;0x7;;;SY)'

  $helper = [wmiclass]'Win32_SecurityDescriptorHelper'
  $launch = $helper.SDDLToBinarySD($launchSddl).BinarySD
  $access = $helper.SDDLToBinarySD($accessSddl).BinarySD

  New-ItemProperty -Path $path -Name 'LaunchPermission' -PropertyType Binary -Value $launch -Force | Out-Null
  New-ItemProperty -Path $path -Name 'AccessPermission' -PropertyType Binary -Value $access -Force | Out-Null

  $p = Get-ItemProperty $path
  Set-Content $log @(
    ('Launch: ' + $helper.BinarySDToSDDL($p.LaunchPermission).SDDL),
    ('Access: ' + $helper.BinarySDToSDDL($p.AccessPermission).SDDL)
  )
} catch {
  Set-Content $log ("ERROR: " + $_.Exception.Message)
}
