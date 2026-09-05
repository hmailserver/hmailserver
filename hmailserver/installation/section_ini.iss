[INI]
Filename: "{code:GetIniPath}"; Section: "Directories"; Key: "ProgramFolder"; String: "{app}";
Filename: "{code:GetIniPath}"; Section: "Directories"; Key: "DatabaseFolder"; String: "{code:GetDefaultDir|Database}";  Flags: createkeyifdoesntexist; Components: server;
Filename: "{code:GetIniPath}"; Section: "Directories"; Key: "DataFolder"; String: "{code:GetDefaultDir|Data}";  Flags: createkeyifdoesntexist; Components: server;
Filename: "{code:GetIniPath}"; Section: "Directories"; Key: "LogFolder"; String: "{code:GetDefaultDir|Logs}"; Flags: createkeyifdoesntexist; Components: server;
Filename: "{code:GetIniPath}"; Section: "Directories"; Key: "TempFolder"; String: "{code:GetDefaultDir|Temp}"; Flags: createkeyifdoesntexist; Components: server;
Filename: "{code:GetIniPath}"; Section: "Directories"; Key: "EventFolder"; String: "{code:GetDefaultDir|Events}"; Flags: createkeyifdoesntexist; Components: server;

; Languages
Filename: "{code:GetIniPath}"; Section: "GUILanguages"; Key: "ValidLanguages"; String: "english,swedish";
Filename: "{code:GetIniPath}"; Section: "Security"; Key: "AdministratorPassword"; String: "{code:GetHashedPassword}"; Flags: createkeyifdoesntexist; Components: server;
