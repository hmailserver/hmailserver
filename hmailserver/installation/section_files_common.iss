[Files]
Source: isxdl.dll; DestDir: {tmp}; Flags: dontcopy
Source: "License.rtf"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; 3'rd party dependencies
Source: ".\Extras\7za.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;

; Database scripts
Source: "..\source\DBScripts\*.sql"; DestDir: "{app}\DBScripts";Flags: ignoreversion recursesubdirs; Components: server;

Source: "..\source\Addons\*.*"; DestDir: "{app}\Addons"; Flags: onlyifdoesntexist recursesubdirs; Excludes: "Events";  Components: server;
Source: "..\source\Addons\Events\*.*"; DestDir: "{app}\Events"; Flags: onlyifdoesntexist;Components: server;

Source: "..\source\WebAdmin\*.*"; DestDir: "{app}\PHPWebAdmin"; Flags: recursesubdirs; Components: admintools;
Source: "..\source\Translations\*"; Excludes: "CVS,.cvsignore,.#*"; DestDir: "{app}\Languages"; Components: server admintools;

Source: ISC.dll; Flags: dontcopy
Source: ".\Extras\tlds.txt"; DestDir: "{app}\Bin";  Flags: ignoreversion; Components: server;
Source: ".\Extras\dh2048.pem"; DestDir: "{app}\Bin";  Flags: ignoreversion; Components: server;
