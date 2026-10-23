[Setup]
AppName=hMailServer
DefaultDirName={commonpf}\hMailServer
DefaultGroupName=hMailServer
PrivilegesRequired=admin
SolidCompression=yes
WizardImageFile=setup.bmp
LicenseFile=license.rtf
AllowNoIcons=yes
Uninstallable=true
DirExistsWarning=no
CreateAppDir=true
; Windows 10 / Windows Server 2016. hMailServer.exe imports libpq.dll, and libpq from
; PostgreSQL 16 and later requires Windows 10, so on anything older the service would not
; start at all - whichever database it is configured for.
MinVersion=10.0
