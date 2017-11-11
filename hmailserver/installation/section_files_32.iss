[Files]
; Main server
Source: "..\source\server\hMailServer\Release\hMailServer.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\Release\hMailServer.tlb"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\Release\hMailServer.Minidump.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
Source: "Microsoft.VC120.CRT\*"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;

Source: "SQLCE\SSCERuntime_x86-ENU.msi"; Flags: deleteafterinstall ; Excludes: ".svn"; DestDir: "{tmp}"; Components: server;
Source: "SQLCE\SSCERuntime_x64-ENU.msi"; Flags: deleteafterinstall ; Excludes: ".svn"; DestDir: "{tmp}"; Components: server;

; Common tools
Source: "..\source\tools\Administrator\bin\x86\Release\hMailAdmin.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: admintools;
Source: "..\source\tools\DBUpdater\Bin\x86\Release\DBUpdater.exe"; DestDir: "{app}\Bin";  Flags: ignoreversion; Components: server;
Source: "..\source\tools\DBSetup\Bin\x86\Release\DBSetup.exe"; DestDir: "{app}\Bin";Flags: ignoreversion;Components: server;
Source: "..\Source\tools\DBSetupQuick\bin\x86\release\DBSetupQuick.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
Source: "..\source\tools\Administrator\bin\x86\Release\Interop.hMailServer.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: admintools;
Source: "..\source\tools\shared\bin\x86\Release\Shared.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; Data directory synchronizer
Source: "..\source\Tools\DataDirectorySynchronizer\Bin\x86\Release\*.exe"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion recursesubdirs;Components: server;
Source: "..\source\tools\Administrator\bin\x86\Release\Interop.hMailServer.dll"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion; Components: admintools;
Source: "..\source\Tools\Shared\Bin\x86\Release\*.dll"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion recursesubdirs;Components: server;

; OpenSSL
Source: "{#HMAILSERVERLIBS}\openssl-1.0.2m\out32\libeay32.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "{#HMAILSERVERLIBS}\openssl-1.0.2m\out32\ssleay32.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
