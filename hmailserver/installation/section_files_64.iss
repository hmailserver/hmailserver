[Files]
; Main server
Source: "..\source\server\hMailServer\x64\Release\hMailServer.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\x64\Release\hMailServer.tlb"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\x64\Release\hMailServer.Minidump.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
; The Visual C++ runtime, in the newest version each Windows version can load - see
; VCREDIST_PATH_MODERN and VCREDIST_PATH_LEGACY in hMailServer64.iss. vccorlib140.dll is left
; out of both: it is the C++/CX runtime, nothing in Bin imports it, and it cannot even load on
; the Windows versions below 8 that the installer still allows.
Source: "{#VCREDIST_PATH_MODERN}\*"; DestDir: "{app}\Bin"; Flags: ignoreversion; Excludes: "vccorlib140.dll"; Components: server admintools; MinVersion: 10.0;
Source: "{#VCREDIST_PATH_LEGACY}\*"; DestDir: "{app}\Bin"; Flags: ignoreversion; Excludes: "vccorlib140.dll"; Components: server admintools; OnlyBelowVersion: 10.0;
; Windows 10 and later carry the Universal CRT themselves, and resolve api-ms-win-crt-* to it
; through the API set schema without ever looking in {app}\Bin - so it is shipped only to the
; older versions that need it.
Source: "{#UCRT_PATH}\*"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools; OnlyBelowVersion: 10.0;

Source: "SQLCE\SSCERuntime_x64-ENU.msi"; Flags: deleteafterinstall ; Excludes: ".svn"; DestDir: "{tmp}"; Components: server;

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
Source: "{#OPENSSL_LIBS_PATH}\libcrypto-3-x64.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "{#OPENSSL_LIBS_PATH}\libssl-3-x64.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; PQSQL (PostgreSQL client)
Source: "{#POSTGRESQL_LIBPQ_PATH}\*.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; MariaDB Connector/C (MySQL / MariaDB client)
Source: "{#MARIADB_LIBS_PATH}\libmariadb.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;