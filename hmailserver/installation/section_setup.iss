[Setup]
AppName=hMailServer
AppCopyright=Copyright (C) 2008
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
; Windows Vista SP2. This is below the 6.1 that Inno Setup wants: Vista's loader cannot
; parse the manifest block that pins mpr.dll, version.dll and friends to system32, so the
; compiler strips that block from setup.exe for every target - the compiler warns about it.
; Setup still hardens its DLL search path at run time. Inno Setup 6.3 dropped Vista
; altogether, which is why the installer is built with 6.2.2 rather than a later release.
MinVersion=6.0.6002
