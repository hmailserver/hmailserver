[Files]
; Main server
Source: "..\source\server\hMailServer\x64\Release\hMailServer.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\x64\Release\hMailServer.tlb"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\x64\Release\hMailServer.Minidump.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
; The Visual C++ runtime, in the newest version each Windows version can load - see
; VCREDIST_PATH_MODERN and VCREDIST_PATH_LEGACY in hMailServer64.iss.
;
; Only the three that are actually loaded. Everything in Bin resolves to these: hMailServer.exe
; and hMailServer.Minidump.exe import all three, libcrypto, libssl and libpq import
; vcruntime140.dll, and the rest is managed code with no native imports at all. Neither exe
; delay-loads anything, and msvcp140.dll depends on the two vcruntime DLLs and nothing else -
; the msvcp140_* satellites depend on it, never the other way round.
;
; The rest of the redistributable is left out: concrt140.dll, vccorlib140.dll (C++/CX, which
; cannot even load below Windows 8), vcruntime140_threads.dll and the msvcp140_1, _2,
; _atomic_wait and _codecvt_ids satellites. That saves about 1.1 MB per tier.
;
; Which satellites are needed follows from the code, not from the toolset: std::atomic::wait
; and std::counting_semaphore pull in msvcp140_atomic_wait.dll, PPL pulls in concrt140.dll,
; and so on. Such a reference would build and test clean here and then fail to start on a
; machine without the redistributable installed, so re-check this list with
; "dumpbin /imports" when adding a dependency or moving to a newer toolset.
Source: "{#VCREDIST_PATH_MODERN}\msvcp140.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools; MinVersion: 10.0;
Source: "{#VCREDIST_PATH_MODERN}\vcruntime140.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools; MinVersion: 10.0;
Source: "{#VCREDIST_PATH_MODERN}\vcruntime140_1.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools; MinVersion: 10.0;
Source: "{#VCREDIST_PATH_LEGACY}\msvcp140.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools; OnlyBelowVersion: 10.0;
Source: "{#VCREDIST_PATH_LEGACY}\vcruntime140.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools; OnlyBelowVersion: 10.0;
Source: "{#VCREDIST_PATH_LEGACY}\vcruntime140_1.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools; OnlyBelowVersion: 10.0;
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
Source: "..\source\tools\Administrator\bin\x64\Release\Interop.hMailServer.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\tools\shared\bin\x64\Release\Shared.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; Data directory synchronizer
Source: "..\source\Tools\DataDirectorySynchronizer\Bin\x64\Release\*.exe"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion recursesubdirs; Components: server;
Source: "..\source\tools\Administrator\bin\x64\Release\Interop.hMailServer.dll"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion; Components: server;
Source: "..\source\Tools\Shared\Bin\x64\Release\Shared.dll"; DestDir: "{app}\Addons\DataDirectorySynchronizer"; Flags: ignoreversion; Components: server;

; OpenSSL
Source: "{#OPENSSL_LIBS_PATH}\libcrypto-3-x64.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "{#OPENSSL_LIBS_PATH}\libssl-3-x64.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; PQSQL (PostgreSQL client)
Source: "{#POSTGRESQL_LIBPQ_PATH}\*.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; MariaDB Connector/C (MySQL / MariaDB client)
Source: "{#MARIADB_LIBS_PATH}\libmariadb.dll"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;