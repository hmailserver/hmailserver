[code]
// GLOBAL
var
  g_pageAccessKey: TInputQueryWizardPage;
  g_szAdminPassword: String;
  
  g_pageDBType: TWizardPage;
  g_bUseInternal : Boolean;

  rdoUseInternal : TRadioButton;
  rdoUseExternal : TRadioButton;

// The NT-service specific parts of the scrit below is taken
// from the innosetup extension knowledgebase.
// Author: Silvio Iaccarino silvio.iaccarino(at)de.adp.com
// Article created: 6 November 2002
// Article updated: 6 November 2002
// http://www13.brinkster.com/vincenzog/isxart.asp?idart=31

type
	SERVICE_STATUS = record
    	dwServiceType				: cardinal;
    	dwCurrentState				: cardinal;
    	dwControlsAccepted			: cardinal;
    	dwWin32ExitCode				: cardinal;
    	dwServiceSpecificExitCode	: cardinal;
    	dwCheckPoint				: cardinal;
    	dwWaitHint					: cardinal;
	end;
	HANDLE = cardinal;
	
const
	SERVICE_QUERY_CONFIG		= $1;
	SERVICE_CHANGE_CONFIG		= $2;
	SERVICE_QUERY_STATUS		= $4;
	SERVICE_START				= $10;
	SERVICE_STOP				= $20;
	SERVICE_ALL_ACCESS			= $f01ff;
	SC_MANAGER_ALL_ACCESS		= $f003f;
	SERVICE_WIN32_OWN_PROCESS	= $10;
	SERVICE_WIN32_SHARE_PROCESS	= $20;
	SERVICE_WIN32				= $30;
	SERVICE_INTERACTIVE_PROCESS = $100;
	SERVICE_BOOT_START          = $0;
	SERVICE_SYSTEM_START        = $1;
	SERVICE_AUTO_START          = $2;
	SERVICE_DEMAND_START        = $3;
	SERVICE_DISABLED            = $4;
	SERVICE_DELETE              = $10000;
	SERVICE_CONTROL_STOP		= $1;
	SERVICE_CONTROL_PAUSE		= $2;
	SERVICE_CONTROL_CONTINUE	= $3;
	SERVICE_CONTROL_INTERROGATE = $4;
	SERVICE_STOPPED				= $1;
	SERVICE_START_PENDING       = $2;
	SERVICE_STOP_PENDING        = $3;
	SERVICE_RUNNING             = $4;
	SERVICE_CONTINUE_PENDING    = $5;
	SERVICE_PAUSE_PENDING       = $6;
	SERVICE_PAUSED              = $7;

  // BEGIN .NET INSTALLER	
  mdacURL = 'http://download.microsoft.com/download/4/a/a/4aafff19-9d21-4d35-ae81-02c48dcbbbff/MDAC_TYP.EXE';
  dotnet20URL = 'http://download.microsoft.com/download/5/6/7/567758a3-759e-473e-bf8f-52154438565a/dotnetfx.exe';
  // END .NET INSTALLER	

function ControlService(hService :HANDLE; dwControl :cardinal;var ServiceStatus :SERVICE_STATUS) : boolean;
external 'ControlService@advapi32.dll stdcall';		

function CloseServiceHandle(hSCObject :HANDLE): boolean;
external 'CloseServiceHandle@advapi32.dll stdcall';

function OpenService(hSCManager :HANDLE;lpServiceName: AnsiString; dwDesiredAccess :cardinal): HANDLE;
external 'OpenServiceA@advapi32.dll stdcall';

function OpenSCManager(lpMachineName, lpDatabaseName: AnsiString; dwDesiredAccess :cardinal): HANDLE;
external 'OpenSCManagerA@advapi32.dll stdcall';

function QueryServiceStatus(hService :HANDLE;var ServiceStatus :SERVICE_STATUS) : boolean;
external 'QueryServiceStatus@advapi32.dll stdcall';

function CheckPorts(): Integer;
external 'CheckPorts@files:ISC.DLL stdcall';


function isxdl_Download(hWnd: Integer; URL, Filename: PAnsiChar): Integer;
external 'isxdl_Download@files:isxdl.dll stdcall';

procedure isxdl_AddFile(URL, Filename: PAnsiChar);
external 'isxdl_AddFile@files:isxdl.dll stdcall';

procedure isxdl_AddFileSize(URL, Filename: PAnsiChar; Size: Cardinal);
external 'isxdl_AddFileSize@files:isxdl.dll stdcall';

function isxdl_DownloadFiles(hWnd: Integer): Integer;
external 'isxdl_DownloadFiles@files:isxdl.dll stdcall';

procedure isxdl_ClearFiles;
external 'isxdl_ClearFiles@files:isxdl.dll stdcall';

function isxdl_IsConnected: Integer;
external 'isxdl_IsConnected@files:isxdl.dll stdcall';

function isxdl_SetOption(Option, Value: PAnsiChar): Integer;
external 'isxdl_SetOption@files:isxdl.dll stdcall';

function isxdl_GetFileName(URL: PAnsiChar): PAnsiChar;
external 'isxdl_GetFileName@files:isxdl.dll stdcall';

// get Windows Installer version
procedure DecodeVersion(const Version: cardinal; var a, b : word);
begin
  a := word(Version shr 16);
  b := word(Version and not $ffff0000);
end;


function OpenServiceManager() : HANDLE;
begin
	if UsingWinNT() = true then begin
		Result := OpenSCManager('','ServicesActive',SC_MANAGER_ALL_ACCESS);
		if Result = 0 then
			MsgBox('the servicemanager is not available', mbError, MB_OK)
	end
	else begin
			MsgBox('only nt based systems support services', mbError, MB_OK)
			Result := 0;
	end
end;

function IsServiceInstalled(ServiceName: AnsiString) : boolean;
var
	hSCM	: HANDLE;
	hService: HANDLE;
begin
	hSCM := OpenServiceManager();
	Result := false;
	if hSCM <> 0 then begin
		hService := OpenService(hSCM,ServiceName,SERVICE_QUERY_CONFIG);
        if hService <> 0 then begin
            Result := true;
            CloseServiceHandle(hService)
		end;
        CloseServiceHandle(hSCM)
	end
end;

function StopService(ServiceName: AnsiString) : boolean;
var
	hSCM	: HANDLE;
	hService: HANDLE;
	Status	: SERVICE_STATUS;
begin
	hSCM := OpenServiceManager();
	Result := false;
	if hSCM <> 0 then begin
		hService := OpenService(hSCM,ServiceName,SERVICE_STOP);
        if hService <> 0 then begin
        	Result := ControlService(hService,SERVICE_CONTROL_STOP,Status);
            CloseServiceHandle(hService)
		end;
        CloseServiceHandle(hSCM)
	end;
end;

function IsServiceRunning(ServiceName: AnsiString) : boolean;
var
	hSCM	: HANDLE;
	hService: HANDLE;
	Status	: SERVICE_STATUS;
begin
	hSCM := OpenServiceManager();
	Result := false;
	if hSCM <> 0 then begin
		hService := OpenService(hSCM,ServiceName,SERVICE_QUERY_STATUS);
    	if hService <> 0 then begin
			if QueryServiceStatus(hService,Status) then begin
				Result :=(Status.dwCurrentState = SERVICE_RUNNING)
        	end;
            CloseServiceHandle(hService)
		    end;
        CloseServiceHandle(hSCM)
	end
end;

function IsServiceStopped(ServiceName: AnsiString) : boolean;
var
	hSCM	: HANDLE;
	hService: HANDLE;
	Status	: SERVICE_STATUS;
begin
	hSCM := OpenServiceManager();
	Result := false;
	if hSCM <> 0 then begin
		hService := OpenService(hSCM,ServiceName,SERVICE_QUERY_STATUS);
    	if hService <> 0 then begin
			if QueryServiceStatus(hService,Status) then begin
				Result :=(Status.dwCurrentState = SERVICE_STOPPED)
        	end;
            CloseServiceHandle(hService)
		    end;
        CloseServiceHandle(hSCM)
	end
end;

function GetInifile() : AnsiString;
var
   szInifile : String;
begin

   // Check if the file exists in the selected installation directory.
   szInifile := ExpandConstant('{app}\Bin\hMailServer.ini');

   if (FileExists(szInifile) = False) then
   begin

      if RegQueryStringValue(HKLM32, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\hMailServer_is1','InstallLocation', szInifile) then
      begin
         szInifile := szInifile + 'Bin\hMailServer.ini';
      end;

      if (FileExists(szInifile) = False) then
      begin
        // File doesn't exist in the old installation directory.
        szInifile := ExpandConstant('{win}\hMailServer.ini');

        if (FileExists(szInifile) = False) then
        begin
           szInifile := '';
        end;
      end;

   end;

   Result := szInifile;

end;

function GetHashedPassword(Param: String) : String;
begin
  Result := GetMD5OfString(g_szAdminPassword);
end;

function GetCurrentDatabaseType() : AnsiString;
var
   szInifile : AnsiString;
   szDatabaseType : AnsiString;
begin

   // Locate the ini file.
   szInifile := GetInifile();

   // Read the database settings...
   szDatabaseType := GetIniString('Database', 'Type', '', szIniFile);
   Result := Lowercase(szDatabaseType);
end;


function GetAdministratorPassword() : AnsiString;
	var szIniFile : AnsiString;
	var sKey : AnsiString;
begin
	szIniFile := GetInifile();
	
	sKey := GetIniString('Security', 'AdministratorPassword', '', szIniFile);
	
	Result := sKey;
end;

function IsNetFrameworkInstalled() : boolean;
	var release: cardinal;
begin
	Result := True;
	if (not RegKeyExists(HKLM, 'SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Full')) then begin
		// No 4.x-version is installed.
		Result := false;
		Exit;
	end;
	
	// Check for at least 4.5:
	if (not RegQueryDWordValue(HKLM, 'SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Full', 'Release', release)) then begin
		// Unable to determine 4.5-version
		Result := false;
		Exit;
	end;
	
	// https://docs.microsoft.com/en-us/dotnet/framework/migration-guide/how-to-determine-which-versions-are-installed
	// 378389 = .NET Framework 4.5
	if (release < 378389) then begin
		// Only .NET 4.0 installed
		Result := false;
		Exit;
	end;
	
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
   Result := false;

   if (WizardSilent() = false) then
   begin
	   // Check if we should skip the password dialog.
	   if (PageID = g_pageAccessKey.ID) then
	   begin
	
	      // If server installation has not been selected, we should skip it,
	      // since the password is for the server...
	      if (IsComponentSelected('server') = false) then
	      begin
	         Result := true;
	      end;
	
	      // If a password has already been specified, we should skip it as well,
	      // It's not possible to change an existing password.
	   	if Length(GetAdministratorPassword()) > 0 then
	   	begin
		   	// Password already specified - skip page.
		   	  Result:= true;
		   end;
	   end;
	
     if (PageID = g_pageDBType.ID) then
     begin
       if (GetCurrentDatabaseType() <> '') then
       begin
          // we have already selected database engine. don't ask for it again.
          Result := true;
       end;

       if (IsComponentSelected('server') = false) then
       begin
          Result := true;
       end;
     end;

  end;

end;


procedure rdoUseInternal_Click(Sender: TObject);
begin
   g_bUseInternal := true;
end;

procedure rdoUseExternal_Click(Sender: TObject);
begin
   g_bUseInternal := false;
end;

procedure moreInfoLink_Click(Sender: TObject);
var
  ErrorCode: Integer;
begin
  ShellExec('', 'http://www.hmailserver.com/documentation/?page=choosing_database_engine', '', '', SW_SHOW, ewNoWait, ErrorCode);

end;

procedure CreateWizardPages();
var
   moreInfoLink : TLabel;
   moreInfoFont : TFont;

begin
   g_pageDBType := CreateCustomPage(wpSelectComponents, 'Select database server type', 'Database type');

    { useMySQL }
   rdoUseInternal := TRadioButton.Create(g_pageDBType);
   with rdoUseInternal do
   begin
     Parent := g_pageDBType.Surface;
     Left := 32;
     Top := 40;
     Width := g_pageDBType.Surface.Width;
     Height := 40;
     Caption := 'Use built-in database engine (Microsoft SQL Compact)';
     TabOrder := 0;
     if (g_bUseInternal) then
        Checked := True
     else
        Checked := False;
     OnClick := @rdoUseInternal_Click;
   end;

   { useExternalDB }
   rdoUseExternal := TRadioButton.Create(g_pageDBType);
   with rdoUseExternal do
   begin
     Parent := g_pageDBType.Surface;
     Left := 32;
     Top := 90;
     Width := g_pageDBType.Surface.Width
	 Height := 40;
     Caption := 'Use external database engine (MSSQL, MySQL or PostgreSQL)';
     TabOrder := 1;
     if (g_bUseInternal) then
        Checked := False
     else
        Checked := True;
     OnClick := @rdoUseExternal_Click;
   end;

   moreInfoFont := TFont.Create();
   moreInfoFont.Style := [fsUnderline];
   moreInfoFont.Color := clBlue;

   moreInfoLink := TLabel.Create(g_pageDBType);
   with moreInfoLink do
   begin
     Parent := g_pageDBType.Surface;
     Left := 32;
     Top := 140;
     Width := 329;
     Height := 40;
     Caption := 'More information...';
     OnClick := @moreInfoLink_Click;
     Font := moreInfoFont;
   end;

 	 // Create key page
   g_pageAccessKey := CreateInputQueryPage(wpSelectTasks, 'hMailServer Security', 'Specify main password','The installation program will now create a hMailServer user with administration rights. Please enter a password below. You will need this password to be able to manage your hMailServer installation, so please remember it.');	

   g_pageAccessKey.Add('Password:', True);
   g_pageAccessKey.Add('Confirm password:', True);

end;


procedure OverrideInstallationFolder();
var
  installFolder : String;
  
begin
    // If hMailServer has been previously installed, and there's a reg key
    // specifying where it was installed, we should install into the same
    // folder as before. Normally InnoSetup keeps track of this automatically,
    // but not when switching between x86 and x64 installs.
    if RegQueryStringValue(HKLM32, 'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\hMailServer_is1','InstallLocation', installFolder) then
    begin
       if (Length(installFolder) > 2) then
          WizardForm.DirEdit.Text := installFolder;
    end;
    
end;

procedure InitializeWizard();
begin

   if ExpandConstant('{param:useinternaldbms|true}') = 'true' then
      g_bUseInternal := true
   else
      g_bUseInternal := false;

   OverrideInstallationFolder();

   if (WizardSilent() = false) then
   begin
      CreateWizardPages();
   end;

end;


function InitializeSetup(): Boolean;
	var
		sMessage : AnsiString;
		SoftwareVersion: AnsiString;
begin
	Result := true;
			
	if not IsNetFrameworkInstalled() then begin
        MsgBox('hMailServer requires .NET Framework 4.5'#13#13
            'Please install this version and then re-run the hMailServer setup program.', mbInformation, MB_OK);
        result := false;
		Exit;
	end;
	
	if (FindWindowByWindowName('hMailServer Administrator') > 0) then
	begin
		MsgBox('hMailServer Administrator is started. You must close down this application before starting the installation.',mbInformation, MB_OK);	
		Result := false;
		Exit;
	end;
	
	if (Result = true) then
	begin
		if (FindWindowByWindowName('hMailServer Database Setup') > 0) then
		begin
			MsgBox('hMailServer Database Setup is started. You must close down this application before starting the installation.',mbInformation, MB_OK);	
			Result := false;
			Exit;
		end;	
	end;
	
	if (Result = true) then
	begin
		if (FindWindowByWindowName('hMailServer Database Upgrader') > 0) then
		begin
			MsgBox('hMailServer Database Upgrader is started. You must close down this application before starting the installation.',mbInformation, MB_OK);	
			Result := false;
			Exit;			
		end;	
	end;	
	
	if (Result = true) then
	begin
		if (FindWindowByWindowName('DBSetup') > 0) then
		begin
			MsgBox('hMailServer DBSetup is started. You must close down this application before starting the installation.',mbInformation, MB_OK);	
			Result := false;
			Exit;			
		end;	
	end;	
	

	// Check so that there isn't already a server running
	// on one of hour ports.
	if (Result = true) then
	begin
		if (IsServiceRunning('hMailServer') <> True) And (CheckPorts() < 0) then
		begin
			// The hMailServer isn't running, but someone is blocking the ports.
			//
			sMessage := 'The hMailServer Setup has detected that one or several of the TCP/IP ports 25, 110 and 143 are already in use.' + Chr(13) + Chr(10) + 'This indicates that there already is an email server running on this computer.' + Chr(13) + Chr(10) + 'If you plan to use any of these ports with hMailServer, the already existing server must be stopped.';
			MsgBox(sMessage, mbInformation, MB_OK);	
		end;
	end;	

	Result := true;

end;



function RegisterTypeLib() : Boolean;
var
  ResultCode: Integer;
begin
   // Register hMaiLlServer service
   if (Exec(ExpandConstant('{app}\Bin\hMailServer.exe'), '/RegisterTypeLib', '',  SW_HIDE, ewWaitUntilTerminated, ResultCode) = False) then
      MsgBox(SysErrorMessage(ResultCode), mbError, MB_OK);

   Result := true;
end;


function DeleteOldFiles() : Boolean;
begin

   DeleteFile(ExpandConstant('{app}\Bin\Copyright.txt'));
   DeleteFile(ExpandConstant('{app}\Bin\License.txt'));
   DeleteFile(ExpandConstant('{app}\Bin\Sourcecode.txt'));

   Result := true;
end;

function InstallSQLCE() : boolean;
var
   ResultCode: Integer;
   szInstallApp: AnsiString;
   szParams: AnsiString;

   szIniFile : AnsiString;
   szDatabaseType : AnsiString;

   bNewInstallationWithSQLCE : Boolean;
   bUpgradeWithSQLCE : Boolean;
begin

   szIniFile := ExpandConstant('{app}\Bin\hMailServer.ini');
   szDatabaseType := GetIniString('Database', 'Type', '', szIniFile);
   szDatabaseType := Lowercase(szDatabaseType);

   bNewInstallationWithSQLCE := (szDatabaseType = '') and g_bUseInternal;
   bUpgradeWithSQLCE := (szDatabaseType = 'mssqlce');


   // Only install SQL CE if we haven't already choosen another
   // database, or if this is a fresh installation. No point in
   // installing SQL CE if MySQL is used.

   if ( bNewInstallationWithSQLCE or bUpgradeWithSQLCE) then
   begin
      // Register SQL CE
      
      szParams := 'msiexec';
      
      if (IsWin64) then
      begin
        szParams := ExpandConstant('/I {tmp}\SSCERuntime_x64-ENU.msi /quiet');
      end
      else
      begin
        szParams := ExpandConstant('/I {tmp}\SSCERuntime_x86-ENU.msi /quiet');
      end;
      
      if (ShellExec('', 'msiexec', szParams, '', SW_SHOW, ewWaitUntilTerminated, ResultCode) = True) then
      begin
         Result:= true;
      end
      else
      begin
		    MsgBox('The installation of SQL Server 2005 Compact Edition (x86) failed.', mbError, MB_OK)
		   	Result := false;
      end;
   end;
end;


function RunPostInstallTasks() : Boolean;
   var
      ResultCode: Integer;
      ProgressPage : TOutputProgressWizardPage;
      szParameters: AnsiString;
begin
   try

      ProgressPage := CreateOutputProgressPage('Finalizing installation','Please wait while the setup performs post-installation tasks');
      ProgressPage.Show();

      ProgressPage.SetText('Starting...', '');
      ProgressPage.SetProgress(1,6);

      ProgressPage.SetText('Initializing database backend...', '');
      ProgressPage.SetProgress(2,6);

  	  // Install
      InstallSQLCE();

      ProgressPage.SetText('Creating the hMailServer service...', '');
      ProgressPage.SetProgress(3,6);

      // Register hMaillServer service
      if (Exec(ExpandConstant('{app}\Bin\hMailServer.exe'), '/Register', '',  SW_HIDE, ewWaitUntilTerminated, ResultCode) = False) then
         MsgBox(SysErrorMessage(ResultCode), mbError, MB_OK);

      ProgressPage.SetText('Initializing hMailServer database...', '');
      ProgressPage.SetProgress(4,6);

      if (WizardSilent() = true) then
      begin
          szParameters:= '/silent';
      end;

	  // Add the password as well, so that the administrator doesn't have to type it in again
      //  if he have just entered it. If this is an upgrade, he'll have to enter it again though.
      if (Length(g_szAdminPassword) > 0) then
         szParameters := szParameters + ' password:' + g_szAdminPassword;
		 
      if ((GetCurrentDatabaseType() <> '') or g_bUseInternal) then
      begin
         if (Exec(ExpandConstant('{app}\Bin\DBSetupQuick.exe'), szParameters, '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode) = False) then
            MsgBox(SysErrorMessage(ResultCode), mbError, MB_OK);
      end
      else
      begin
         if (Exec(ExpandConstant('{app}\Bin\DBSetup.exe'), szParameters, '', SW_SHOWNORMAL, ewWaitUntilTerminated, ResultCode) = False) then
            MsgBox(SysErrorMessage(ResultCode), mbError, MB_OK);

      end;

      ProgressPage.SetText('Starting the hMailServer service...', '');
      ProgressPage.SetProgress(5,6);

      // Start hMailServer
      if (Exec(ExpandConstant('{sys}\net.exe'), 'START hMailServer', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) = False) then
         MsgBox(SysErrorMessage(ResultCode), mbError, MB_OK);

      ProgressPage.SetText('Completed', '');
      ProgressPage.SetProgress(6,6);

   finally
     ProgressPage.Hide();
   end;

   Result := true;

end;

function MoveIni() : Boolean;
  var sOldFile : AnsiString;
  var sNewFile : AnsiString;
begin

   CreateDir(ExpandConstant('{app}\Bin'));
   sOldFile := ExpandConstant('{win}\hMailServer.ini');
   sNewFile := ExpandConstant('{app}\Bin\hMailServer.ini');

   // Copy the file from the Windows directory
   // to the Bin directory. hMailServer uses the
   // file located in the Bin directory.
   if (FileCopy(sOldFile, sNewFile, True) = True) then
   begin
      // Rename the old hmailserver.ini
      sNewFile := sOldFile + '.old';
      if (FileCopy(sOldFile, sNewFile, True) = True) then
      begin
        // We've managed to backup hMailServer.ini in the
        // windows directory. Now delete the original.
        DeleteFile(sOldFile);
      end;
   end;
   Result := true;

end;

function CheckIsOldMySQLInstallation(szIniFile: AnsiString) : boolean;
var
   szDatabasePort : AnsiString;
   szProgramFolder: AnsiString;
   szMySQLExecutable : AnsiString;
   iFileSize: Integer;
   szMessage : AnsiString;
   szDatabase : AnsiString;
   szDatabaseHost : AnsiString;
   szDatabaseUsername : AnsiString;
begin

  szDatabasePort := GetIniString('Database', 'Port', '', szIniFile);
  szDatabase := GetIniString('Database', 'Database', '', szIniFile);
  szDatabaseHost := GetIniString('Database', 'Server', '', szIniFile);
  szDatabaseUsername := GetIniString('Database', 'Username', '', szIniFile);
  szProgramFolder := RemoveBackslash(GetIniString('Directories', 'ProgramFolder', '', szIniFile));

  if (GetCurrentDatabaseType() = 'mysql') and
     (szDatabasePort = '3307') and
     (szDatabase = 'hMailServer') and
     (szDatabaseHost = 'localhost') and
     (szDatabaseUsername = 'root') then begin

    // We're using an internal MySQL database.
    szMySQLExecutable := szProgramFolder + '\MySQL\Bin\mysqld-nt.exe';

    // Check the size of MySQL.
    iFileSize := 0;
    if (FileSize(szMySQLExecutable, iFileSize)) then begin
       // MySQL in 4.4.3 is larger than 3500000 bytes.
       if (iFileSize < 3500000) then begin
          // MySQL is too old.

          szMessage := 'This version of hMailServer does not include MySQL. hMailServer can still' + #13 +
                       'use MySQL as backend though, assuming it is already installed on the system.' + #13 +
                       '' + #13 +
                       'However, the MySQL version hMailServer is configured to use is'  + #13 +
                       'too old for this version of hMailServer.' + #13 +
                       ''+ #13 +
                       'To solve this issue you must install the latest hMailServer 4 version' + #13 +
                       'before upgrading to hMailServer 5.' + #13
                       ''+ #13 +
                       'The latest hMailServer 4 version will upgrade MySQL to an version' + #13 +
                       'which is compatible with hMailServer 5.'

  		  	MsgBox(szMessage, mbError, MB_OK)
          Result := true;
       end;
    end
    else
    begin
          szMessage := 'hMailServer 5 and later does not include MySQL. hMailServer 5 can still' + #13 +
                       'use MySQL as backend though, assuming it is already installed on the system.' + #13 +
                       '' + #13 +
                       'You have configured hMailServer 4 to use the bundled MySQL installation. However'+ #13 +
                       'hMailServer 4 with MySQL appears to have been uninstalled prior to running this' + #13 +
                       'hMailServer 5 installation. Hence, the MySQL installation hMailServer needs is' + #13 +
                       'no longer available.' + #13 +
                       '' + #13 +
                       'To solve this problem, reinstall the same hMailServer 4 version as before and then' + #13 +
                       'upgrade to version 5 without first uninstalling version 4.' + #13 +
                       '' + #13 +
                       'As an alternative, you can cancel this installation, delete the entire hMailServer ' + #13 +
                       'directory and then run this installation program again. Using this method, your configuration' + #13 +
                       'and email messages will be lost.';

  		  	MsgBox(szMessage, mbError, MB_OK)
          Result := true;
    end;

  end;


end;

function NextButtonClick(CurPage : Integer): boolean;
var
   hWnd: Integer;
   szIniFile : AnsiString;

begin
	// We default to true.
	Result := true;

	if (CurPage = wpSelectDir) then
	begin
		szIniFile := GetIniFile();

		// Check if this folder contains an old MySQL installation, or if
		// the old MySQL installation has been uninstalled.
		if CheckIsOldMySQLInstallation(szIniFile) = true then 
		begin
			Result := false;
		end;
	end
	else if CurPage = wpReady then
	begin
		// Start hMailServer and MySQL, if they are running.
		if IsServiceRunning('hMailServer') = true then
		begin
		 	 StopService('hMailServer');
		
		   while (IsServiceStopped('hMailServer') = false) do
		   begin
		      Sleep(250);
		   end;
		end;
    end;
	
    hWnd := StrToInt(ExpandConstant('{wizardhwnd}'));

	if WizardSilent() = false then
	begin
		if CurPage = g_pageAccessKey.ID then
		begin
			// Check that passwords matches.
			if (Length(g_pageAccessKey.Values[0]) < 5) or (g_pageAccessKey.Values[0] <> g_pageAccessKey.Values[1]) then
			begin
				MsgBox('The two passwords must match and be at least 5 characters long.', mbError, MB_OK)
				Result := false;
			end;

			g_szAdminPassword := g_pageAccessKey.Values[0];
		end;
	end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  szIniFile  : AnsiString;
begin

	if CurStep = ssInstall then
	begin
	   // Move hMailServer.ini before files are copied
	   MoveIni();
	end;
	
	if CurStep = ssPostInstall then
	begin
   // Create a registry key that tell
	  // other apps where we're installed.
	  RegWriteStringValue(HKLM32, 'Software\hMailServer', 'InstallLocation', ExpandConstant('{app}'));
   	
	  // Write db location to hMailServer.ini.
	  szIniFile := ExpandConstant('{app}\Bin\hMailServer.ini');

  	// Create the hMailServer database
 	  if (IsComponentSelected('server')) then
	  begin
	    RunPostInstallTasks();
	  end
	 else
	 begin
	   if (IsComponentSelected('admintools')) then
	   begin
	      RegisterTypeLib();
	   end;
	 end;

   DeleteOldFiles();

	end;

end;


