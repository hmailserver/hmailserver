[Setup]
AppName=hMailServer
AppVerName=hMailServer 5.4-B1929
AppCopyright=Copyright (C) 2008
DefaultDirName={pf}\hMailServer
DefaultGroupName=hMailServer
PrivilegesRequired=admin
OutputBaseFilename=hMailServer-5.4-B1929
SolidCompression=yes
WizardImageFile=setup.bmp
LicenseFile=license.rtf
AllowNoIcons=yes
Uninstallable=true
DirExistsWarning=no
CreateAppDir=true
MinVersion=0,5.1sp3

[CustomMessages]
;configurations
AppName=hMailServer
DependenciesDir=MyProgramDependencies

MSI20Title=Windows Installer 2.0
MSI31Title=Windows Installer 3.1
IE6Title=Internet Explorer 6
MDACTitle=MDAC 2.8
JETTitle=JET 4
DOTNET20Title=Microsoft .NET Framework 2.0

;memos
en.DependenciesDownloadTitle=Download Dependencies
en.DependenciesInstallTitle=Install Dependencies

;messages
en.Win2000Sp3Msg=Windows 2000 Service Pack 3 must be installed before setup can continue. Please install Windows 2000 Service Pack 3 and run Setup again.
en.WinXPSp2Msg=Windows XP Service Pack 2 must be installed before setup can continue. Please install Windows XP Service Pack 2 and run Setup again.
en.DownloadMsg1=The following applications are required before setup can continue:
en.DownloadMsg2=Download and install now?

;install text
en.MSI20DownloadSize=~1.7 MB
en.MSI31DownloadSize=~2.5 MB
en.IE6DownloadSize=~46 MB
en.MDACDownloadSize=~5.4 MB
en.JETDownloadSize=~3.7 MB
en.DOTNET20DownloadSize=~23 MB

en.MSI20InstallMsg=Installing Windows Installer 2.0... (May take a few minutes)
en.MSI31InstallMsg=Installing Windows Installer 3.1... (This may take a few minutes)
en.IE6InstallMsg=Installing Internet Explorer 6... (May take a few minutes)
en.MDACInstallMsg=Installing MDAC 2.8... (May take a few minutes)
en.JETInstallMsg=Installing JET 4... (May take a few minutes)
en.DOTNET20InstallMsg=Installing Microsoft .NET Framework 2.0... (May take a few minutes)

[Languages]
Name: "en"; MessagesFile: "compiler:Default.isl"

[_ISTool]
EnableISX=true

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: "server"; Description: "Server"; Types: full custom
Name: "admintools"; Description: "Administrative tools"; Types: full custom

[Files]
Source: isxdl.dll; DestDir: {tmp}; Flags: dontcopy
Source: "License.rtf"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; 3'rd party dependencies
Source: "System files\dnsapi.dll"; DestDir: "{sys}"; Flags: uninsneveruninstall onlyifdoesntexist; Components: admintools; OnlyBelowVersion: 0,6;
Source: "System files\ATL\atl70.dll"; DestDir: "{sys}";  Components: server;
Source: "SQLCE\SSCERuntime-ENU.msi"; Flags: deleteafterinstall ; Excludes: ".svn"; DestDir: "{tmp}"; Components: server;
Source: ".\Extras\7za.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;

Source: "Microsoft.VC120.CRT\*"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; Main server
Source: "..\source\server\hMailServer\Release\hMailServer.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\Release\hMailServer.tlb"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\Release\hMailServer.Minidump.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;

; Database scripts
Source: "..\source\DBScripts\*.sql"; DestDir: "{app}\DBScripts";Flags: ignoreversion recursesubdirs; Components: server;

; Common tools
Source: "..\source\tools\Administrator\bin\Release\hMailAdmin.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: admintools;
Source: "..\source\tools\DBUpdater\Bin\Release\DBUpdater.exe"; DestDir: "{app}\Bin";  Flags: ignoreversion; Components: server;
Source: "..\source\tools\DBSetup\Bin\Release\DBSetup.exe"; DestDir: "{app}\Bin";Flags: ignoreversion;Components: server;
Source: "..\Source\tools\DBSetupQuick\bin\release\DBSetupQuick.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
Source: "..\source\tools\Administrator\bin\Release\Interop.hMailServer.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: admintools;
Source: "..\source\tools\shared\bin\Release\Shared.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; Data directory synchronizer
Source: "..\source\Tools\DataDirectorySynchronizer\Bin\Release\*.exe"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion recursesubdirs;Components: server;
Source: "..\source\tools\Administrator\bin\Release\Interop.hMailServer.dll"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion; Components: admintools;
Source: "..\source\Tools\Shared\Bin\Release\*.dll"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion recursesubdirs;Components: server;

Source: "..\source\Addons\*.*"; DestDir: "{app}\Addons"; Flags: ignoreversion recursesubdirs; Excludes: "Events";  Components: server;
Source: "..\source\Addons\Events\*.*"; DestDir: "{app}\Events"; Flags: onlyifdoesntexist;Components: server;

Source: "..\source\WebAdmin\*.*"; DestDir: "{app}\PHPWebAdmin"; Flags: recursesubdirs; Components: admintools;
Source: "..\source\Translations\*"; Excludes: "CVS,.cvsignore,.#*"; DestDir: "{app}\Languages"; Components: server admintools;

Source: ISC.dll; Flags: dontcopy
Source: ".\Extras\tlds.txt"; DestDir: "{app}\Bin";  Flags: ignoreversion; Components: server;
Source: ".\Extras\dh2048.pem"; DestDir: "{app}\Bin";  Flags: ignoreversion; Components: server;

[Messages]
BeveledLabel=hMailServer
WinVersionTooLowError=This program requires Microsoft Windows XP Service Pack 3 or later.

[INI]
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "ProgramFolder"; String: "{app}";
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "DatabaseFolder"; String: "{app}\Database";  Flags: createkeyifdoesntexist; Components: server;
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "DataFolder"; String: "{app}\Data";  Flags: createkeyifdoesntexist; Components: server;
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "LogFolder"; String: "{app}\Logs"; Flags: createkeyifdoesntexist; Components: server;
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "TempFolder"; String: "{app}\Temp"; Flags: createkeyifdoesntexist; Components: server;
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "EventFolder"; String: "{app}\Events"; Flags: createkeyifdoesntexist; Components: server;

; Languages
Filename: "{app}\Bin\hMailServer.INI"; Section: "GUILanguages"; Key: "ValidLanguages"; String: "english,swedish"; Flags: createkeyifdoesntexist; 
Filename: "{app}\Bin\hMailServer.INI"; Section: "Security"; Key: "AdministratorPassword"; String: "{code:GetHashedPassword}"; Flags: createkeyifdoesntexist; Components: server;

[Dirs]
Name: "{app}\Data"; Components: server;
Name: "{app}\Database"; Components: server;
Name: "{app}\Logs"; Components: server;
Name: "{app}\Languages"
Name: "{app}\DBScripts"; Components: server;
Name: "{app}\Events"; Components: server;
Name: "{app}\Temp"; Components: server;

[Messages]
BeveledLabel=hMailServer

[Run]
Filename: {ini:{tmp}\dep.ini,install,msi20}; Description: {cm:MSI20Title}; StatusMsg: {cm:MSI20InstallMsg}; Parameters: "/q"; Flags: skipifdoesntexist
Filename: {ini:{tmp}\dep.ini,install,msi31}; Description: {cm:MSI31Title}; StatusMsg: {cm:MSI31InstallMsg}; Parameters: "/quiet"; Flags: skipifdoesntexist
Filename: {ini:{tmp}\dep.ini,install,ie}; Description: {cm:IE6Title}; StatusMsg: {cm:IE6InstallMsg}; Parameters: "/Q /C:""ie6wzd /QU /R:N /S:#e"""; Flags: skipifdoesntexist
Filename: {ini:{tmp}\dep.ini,install,mdac}; Description: {cm:MDACTitle}; StatusMsg: {cm:MDACInstallMsg}; Parameters: "/Q /C:""setup /QNT"""; Flags: skipifdoesntexist
Filename: {ini:{tmp}\dep.ini,install,jet}; Description: {cm:JETTitle}; StatusMsg: {cm:JETInstallMsg}; Parameters: /Q; Flags: skipifdoesntexist
Filename: {ini:{tmp}\dep.ini,install,dotnet20}; Description: {cm:DOTNET20Title}; StatusMsg: {cm:DOTNET20InstallMsg}; Parameters: "/Q /T:{tmp}\dotnetfx /C:""install /q"""; Flags: skipifdoesntexist
Filename: "{app}\Bin\hMailAdmin.exe";  Flags: skipifsilent postinstall nowait; Description: Run hMailServer Administrator; Components: admintools;

[UninstallRun]
Filename: "{app}\Bin\hMailServer.exe"; Parameters: "/Unregister"; Flags: runhidden;
Filename: "{sys}\net.exe"; Parameters: "STOP hMailServer"; Flags: runhidden;
Filename: "{sys}\net.exe"; Parameters: "STOP hMailServerMySQL"; Flags: runhidden;
Filename: "{app}\MySQL\Bin\mysqld-nt.exe"; Parameters: "--remove hMailServerMySQL"; Flags: runhidden;
Filename: "{app}\Bin\hSMTPServer.exe"; Parameters: "unregister"; Flags: runhidden;
Filename: "{app}\Bin\hPOP3Server.exe"; Parameters: "unregister"; Flags: runhidden;

[Icons]
Name: "{group}\hMailServer Database Setup"; Filename: "{app}\Bin\DBSetup.exe"; Components: server;
Name: "{group}\hMailServer Administrator"; Filename: "{app}\Bin\hMailAdmin.exe"; Components: admintools;
Name: "{group}\Addons\Data Directory Synchronizer"; Filename: "{app}\Addons\DataDirectorySynchronizer\DataDirectorySynchronizer.exe"; Components: server;
Name: "{group}\Installation\Uninstall hMailServer"; Filename: "{app}\unins000.exe"; Components: admintools server;
Name: "{group}\Service\Start service"; Filename: "{sys}\net.exe"; Parameters: "START hMailServer"; Components: server;
Name: "{group}\Service\Stop service"; Filename: "{sys}\net.exe"; Parameters: "STOP hMailServer"; Components: server;

[code]

#include "hMailServerInnoExtension.iss"

