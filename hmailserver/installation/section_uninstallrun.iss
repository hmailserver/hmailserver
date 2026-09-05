[UninstallRun]
Filename: "{sys}\net.exe"; Parameters: "STOP hMailServer"; Flags: runhidden; RunOnceId: "StopService";
Filename: "{app}\Bin\hMailServer.exe"; Parameters: "/Unregister"; Flags: runhidden; RunOnceId: "UnregisterCOM";
Filename: "{sys}\net.exe"; Parameters: "STOP hMailServerMySQL"; Flags: runhidden; RunOnceId: "StopMySQL";
Filename: "{app}\MySQL\Bin\mysqld-nt.exe"; Parameters: "--remove hMailServerMySQL"; Flags: runhidden; RunOnceId: "RemoveMySQL";
Filename: "{app}\Bin\hSMTPServer.exe"; Parameters: "unregister"; Flags: runhidden; RunOnceId: "UnregisterSMTP";
Filename: "{app}\Bin\hPOP3Server.exe"; Parameters: "unregister"; Flags: runhidden; RunOnceId: "UnregisterPOP3";
