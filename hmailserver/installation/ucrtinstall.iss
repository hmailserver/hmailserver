[Code]
// Module to handle VC142 Redist issues with MS Universal C Runtime

// Forwarders
function Is_Win7_SP1_or_Win2008_R2_SP1(): boolean; forward;
procedure UCrt_Win7_SP1_or_Win2008_R2_SP1_Update(); forward;

function Is_Win8_or_Win2012(): boolean; forward;
procedure UCrt_Win8_or_Win2012_Update(); forward;

function Is_Win81_or_Win2012_R2(): boolean; forward;
procedure UCrt_Win81_or_Win2012_R2_Update(); forward;

function IsKBInstalled(KB: string): Boolean; forward;
procedure ExitProcess(exitCode:integer);  external 'ExitProcess@kernel32.dll stdcall';

// Implementation
function IsKBInstalled(KB: string): Boolean;
var
  WbemLocator: Variant;
  WbemServices: Variant;
  WQLQuery: string;
  WbemObjectSet: Variant;
begin
  WbemLocator := CreateOleObject('WbemScripting.SWbemLocator');
  WbemServices := WbemLocator.ConnectServer('', 'root\CIMV2');

  WQLQuery := 'select * from Win32_QuickFixEngineering where HotFixID = ''' + KB + '''';

  WbemObjectSet := WbemServices.ExecQuery(WQLQuery);
  Result := (not VarIsNull(WbemObjectSet)) and (WbemObjectSet.Count > 0);
end;

// Returns true if we are running on Win7-SP1 or Win2008_R2-SP1
function Is_Win7_SP1_or_Win2008_R2_SP1(): boolean;
 var
    Version: TWindowsVersion;
begin
	GetWindowsVersionEx(Version);
  
	if ((Version.Major = 6) and (Version.Minor = 1))  then    
      Result := True     
	else  
	  Result := False;      
end;

// Installs UCrt Update for Win7+Win2008_R2-SP1
procedure UCrt_Win7_SP1_or_Win2008_R2_SP1_Update();

 var ResultCode : Integer;
     update_installer, patch_folder, patch_file : String;	 
begin

    if (Is_Win7_SP1_or_Win2008_R2_SP1) then 
	begin
		patch_file       := 'Windows6.1-KB2999226-x64.msu /quiet /norestart';		
		patch_folder     := ExpandConstant('{app}\Temp');
		update_installer := ExpandConstant('{sys}\wusa.exe');
		
		if not(IsKBInstalled('KB2999226')) then 
		begin
			Exec(update_installer, patch_file, patch_folder, SW_SHOW,
				ewWaitUntilTerminated, ResultCode) 
	
			if not(ResultCode = 0) then 
			   MsgBox('KB2999226 could not be installed', mbError, MB_OK);
		end;		   
	end;
end;

// Returns true if we are running on Win8 or Win2012
function Is_Win8_or_Win2012(): boolean;
 var
    Version: TWindowsVersion;
begin
	GetWindowsVersionEx(Version);
  
	if ((Version.Major = 6) and (Version.Minor = 2))  then    
      Result := True     
	else  
	  Result := False;      
end;

// Installs UCrt Update for Win8+Win2012
procedure UCrt_Win8_or_Win2012_Update();

 var ResultCode : Integer;
     update_installer, patch_folder, patch_file : String;	 
begin

    if (Is_Win8_or_Win2012) then 
	begin
		patch_file       := 'Windows8-RT-KB2999226-x64.msu /quiet /norestart';		
		patch_folder     := ExpandConstant('{app}\Temp');
		update_installer := ExpandConstant('{sys}\wusa.exe');
		
		if not(IsKBInstalled('KB2999226')) then 
		begin
			Exec(update_installer, patch_file, patch_folder, SW_SHOW,
				ewWaitUntilTerminated, ResultCode) 
	
			if not(ResultCode = 0) then 
			   MsgBox('KB2999226 could not be installed', mbError, MB_OK);
		end;
	end;
end;

// Returns true if we are running on Win8.1 or Win2012_R2
function Is_Win81_or_Win2012_R2(): boolean;
 var
    Version: TWindowsVersion;
begin
	GetWindowsVersionEx(Version);
  
	if ((Version.Major = 6) and (Version.Minor = 3))  then    
      Result := True     
	else  
	  Result := False;      
end;

// Installs UCrt Update for Win81+Win2012_R2
procedure UCrt_Win81_or_Win2012_R2_Update();

 var ResultCode : Integer;
     update_installer, patch_folder, patch_file : String;	 
begin

    if (Is_Win81_or_Win2012_R2) then 
	begin
		patch_file       := 'Windows8.1-KB2999226-x64.msu /quiet /norestart';		
		patch_folder     := ExpandConstant('{app}\Temp');
		update_installer := ExpandConstant('{sys}\wusa.exe');
		
		if not(IsKBInstalled('KB2999226')) then 
		begin
			Exec(update_installer, patch_file, patch_folder, SW_SHOW,
				ewWaitUntilTerminated, ResultCode) 
	
			if not(ResultCode = 0) then 
			   MsgBox('KB2999226 could not be installed', mbError, MB_OK);
		end;
	end;
end;