[Run]
Filename: {ini:{tmp}\dep.ini,install,msi20}; Description: {cm:MSI20Title}; StatusMsg: {cm:MSI20InstallMsg}; Parameters: "/q"; Flags: skipifdoesntexist
Filename: {ini:{tmp}\dep.ini,install,msi31}; Description: {cm:MSI31Title}; StatusMsg: {cm:MSI31InstallMsg}; Parameters: "/quiet"; Flags: skipifdoesntexist
Filename: {ini:{tmp}\dep.ini,install,ie}; Description: {cm:IE6Title}; StatusMsg: {cm:IE6InstallMsg}; Parameters: "/Q /C:""ie6wzd /QU /R:N /S:#e"""; Flags: skipifdoesntexist
Filename: {ini:{tmp}\dep.ini,install,mdac}; Description: {cm:MDACTitle}; StatusMsg: {cm:MDACInstallMsg}; Parameters: "/Q /C:""setup /QNT"""; Flags: skipifdoesntexist
Filename: {ini:{tmp}\dep.ini,install,jet}; Description: {cm:JETTitle}; StatusMsg: {cm:JETInstallMsg}; Parameters: /Q; Flags: skipifdoesntexist
Filename: {ini:{tmp}\dep.ini,install,dotnet20}; Description: {cm:DOTNET20Title}; StatusMsg: {cm:DOTNET20InstallMsg}; Parameters: "/Q /T:{tmp}\dotnetfx /C:""install /q"""; Flags: skipifdoesntexist
Filename: "{app}\Bin\hMailAdmin.exe";  Flags: skipifsilent postinstall nowait; Description: Run hMailServer Administrator; Components: admintools;
