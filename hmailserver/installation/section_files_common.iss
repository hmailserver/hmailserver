[Files]
Source: isxdl.dll; DestDir: {tmp}; Flags: dontcopy
Source: "License.rtf"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server admintools;

; 3'rd party dependencies
; 7-Zip is fetched by libraries\build-7zip.ps1 rather than kept in the repository. It is
; LGPL, so its license text is installed beside the executable.
Source: "{#SEVENZIP_PATH}\7za.exe"; DestDir: "{app}\Bin"; Flags: ignoreversion; Components: server;
Source: "{#SEVENZIP_PATH}\License.txt"; DestDir: "{app}\Bin"; DestName: "7za.exe.license.txt"; Flags: ignoreversion; Components: server;

; Database scripts
Source: "..\source\DBScripts\*.sql"; DestDir: "{app}\DBScripts";Flags: ignoreversion recursesubdirs; Components: server;

Source: "..\source\Addons\*.*"; DestDir: "{app}\Addons"; Flags: onlyifdoesntexist recursesubdirs; Excludes: "Events";  Components: server;
Source: "..\source\Addons\Events\*.*"; DestDir: "{app}\Events"; Flags: onlyifdoesntexist;Components: server;

Source: "..\source\WebAdmin\*.*"; DestDir: "{app}\PHPWebAdmin"; Flags: recursesubdirs; Components: admintools;
Source: "..\source\Translations\*"; Excludes: "CVS,.cvsignore,.#*"; DestDir: "{app}\Languages"; Components: server admintools;

Source: ISC.dll; Flags: dontcopy
Source: ".\Extras\public_suffix_list.dat"; DestDir: "{app}\Bin";  Flags: ignoreversion; Components: server;
Source: ".\Extras\dh2048.pem"; DestDir: "{app}\Bin";  Flags: ignoreversion; Components: server;
