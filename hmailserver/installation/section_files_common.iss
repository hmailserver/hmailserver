[Files]
Source: isxdl.dll; DestDir: {tmp}; Flags: dontcopy
Source: "License.rtf"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; 3'rd party dependencies
Source: "System files\dnsapi.dll"; DestDir: "{sys}"; Flags: uninsneveruninstall onlyifdoesntexist; Components: admintools; OnlyBelowVersion: 0,6;
Source: "System files\ATL\atl70.dll"; DestDir: "{sys}";  Components: server;
Source: "SQLCE\SSCERuntime-ENU.msi"; Flags: deleteafterinstall ; Excludes: ".svn"; DestDir: "{tmp}"; Components: server;
Source: ".\Extras\7za.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
Source: "{#HMAILSERVERLIBS}\openssl-1.0.1j\out64\libeay32.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
Source: "{#HMAILSERVERLIBS}\openssl-1.0.1j\out64\ssleay32.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
Source: "Microsoft.VC120.CRT\*"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;

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
