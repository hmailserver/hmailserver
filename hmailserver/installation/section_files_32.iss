[Files]
; Main server
Source: "..\source\server\hMailServer\Release\hMailServer.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\Release\hMailServer.tlb"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;
Source: "..\source\server\hMailServer\Release\hMailServer.Minidump.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;

Source: "SQLCE\SSCERuntime_4.0.SP1_x86-ENU.exe"; Flags: deleteafterinstall ; Excludes: ".svn"; DestDir: "{tmp}"; Components: server;
