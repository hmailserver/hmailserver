[UninstallRun]
Filename: "{app}\Bin\hMailServer.exe"; Parameters: "/Unregister"; Flags: runhidden;
Filename: "{sys}\net.exe"; Parameters: "STOP hMailServer"; Flags: runhidden;
Filename: "{sys}\net.exe"; Parameters: "STOP hMailServerMySQL"; Flags: runhidden;
Filename: "{app}\MySQL\Bin\mysqld-nt.exe"; Parameters: "--remove hMailServerMySQL"; Flags: runhidden;
Filename: "{app}\Bin\hSMTPServer.exe"; Parameters: "unregister"; Flags: runhidden;
Filename: "{app}\Bin\hPOP3Server.exe"; Parameters: "unregister"; Flags: runhidden;
