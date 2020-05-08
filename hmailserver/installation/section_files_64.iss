[Files]
; Main server
Source: "..\source\server\hMailServer\x64\Release\hMailServer.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\x64\Release\hMailServer.tlb"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\x64\Release\hMailServer.Minidump.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
Source: "Microsoft.VC142.CRT\*"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;

Source: "SQLCE\SSCERuntime_x64-ENU.msi"; Flags: deleteafterinstall ; Excludes: ".svn"; DestDir: "{tmp}"; Components: server;

; Required Universal C Runtime Updates for - Win7 x64 SP1 and Win 2008 R2 x64 SP1 / Win8 and Win2012 /  8.1 and 2012 R2
Source: "Microsoft.VC142.CRT\Windows6.1-KB2999226-x64.msu";  DestDir: "{app}\Temp"; Flags: ignoreversion; Components: server; AfterInstall: UCrt_Win7_SP1_or_Win2008_R2_SP1_Update
Source: "Microsoft.VC142.CRT\Windows8-RT-KB2999226-x64.msu"; DestDir: "{app}\Temp"; Flags: ignoreversion; Components: server; AfterInstall: UCrt_Win8_or_Win2012_Update
Source: "Microsoft.VC142.CRT\Windows8.1-KB2999226-x64.msu";  DestDir: "{app}\Temp"; Flags: ignoreversion; Components: server; AfterInstall: UCrt_Win81_or_Win2012_R2_Update

; Common tools
Source: "..\source\tools\Administrator\bin\x64\Release\hMailAdmin.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: admintools;
Source: "..\source\tools\DBUpdater\Bin\x64\Release\DBUpdater.exe"; DestDir: "{app}\Bin";  Flags: ignoreversion; Components: server;
Source: "..\source\tools\DBSetup\Bin\x64\Release\DBSetup.exe"; DestDir: "{app}\Bin";Flags: ignoreversion;Components: server;
Source: "..\Source\tools\DBSetupQuick\bin\x64\release\DBSetupQuick.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
Source: "..\source\tools\Administrator\bin\x64\Release\Interop.hMailServer.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: admintools;
Source: "..\source\tools\shared\bin\x64\Release\Shared.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; Data directory synchronizer
Source: "..\source\Tools\DataDirectorySynchronizer\Bin\x64\Release\*.exe"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion recursesubdirs;Components: server;
Source: "..\source\tools\Administrator\bin\x64\Release\Interop.hMailServer.dll"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion; Components: admintools;
Source: "..\source\Tools\Shared\Bin\x64\Release\*.dll"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion recursesubdirs;Components: server;

; OpenSSL
Source: "{#HMAILSERVERLIBS}\openssl-1.1.1g\out64\bin\libcrypto-1_1-x64.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "{#HMAILSERVERLIBS}\openssl-1.1.1g\out64\bin\libssl-1_1-x64.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; PQSQL (PostgreSQL client)
Source: "..\..\libraries\libpq-12.2\x64\*.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; Libmysql.dll x64 v5.7.30 - (self build with Visual Studio 2019 to avoid Visual Studio 2013 vc110 Redist DLL dependencies.
Source: "..\..\libraries\libmysql-5.7.x\x64\libmysql.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;