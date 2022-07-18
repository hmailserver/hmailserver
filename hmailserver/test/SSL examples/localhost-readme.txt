Certificate with CN=localhost

Created in certificate store using PowerShell:
 New-SelfSignedCertificate -certstorelocation cert:\localmachine\my -dnsname localhost

Exported to PFX with password Secret1