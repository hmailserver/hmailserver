[INI]
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "ProgramFolder"; String: "{app}";
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "DatabaseFolder"; String: "{app}\Database";  Flags: createkeyifdoesntexist; Components: server;
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "DataFolder"; String: "{app}\Data";  Flags: createkeyifdoesntexist; Components: server;
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "LogFolder"; String: "{app}\Logs"; Flags: createkeyifdoesntexist; Components: server;
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "TempFolder"; String: "{app}\Temp"; Flags: createkeyifdoesntexist; Components: server;
Filename: "{app}\Bin\hMailServer.INI"; Section: "Directories"; Key: "EventFolder"; String: "{app}\Events"; Flags: createkeyifdoesntexist; Components: server;

; Languages
Filename: "{app}\Bin\hMailServer.INI"; Section: "GUILanguages"; Key: "ValidLanguages"; String: "english,swedish";
Filename: "{app}\Bin\hMailServer.INI"; Section: "Security"; Key: "AdministratorPassword"; String: "{code:GetHashedPassword}"; Flags: createkeyifdoesntexist; Components: server;
