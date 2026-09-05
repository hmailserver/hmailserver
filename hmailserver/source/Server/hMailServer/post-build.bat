@echo off
setlocal

REM Copies the runtime dependencies next to hMailServer.exe and registers the COM
REM server. The mode argument splits the two, because only the registration needs
REM administrator rights:
REM
REM   COPY       copy the dependencies only
REM   REGISTER   register the COM server and the service only
REM   ALL        both (the default, used by the Visual Studio post-build event)
REM
REM Starting and stopping the service is left to the caller.

set HMS_LIBS=%~1
set OUT_DIR=%~2
set TARGET=%~3
set MODE=%~4
set SCRIPT_DIR=%~dp0

if "%MODE%"=="" set MODE=ALL

if /I "%MODE%"=="COPY" goto copy
if /I "%MODE%"=="REGISTER" goto register
if /I "%MODE%"=="ALL" goto copy

echo Unknown mode "%MODE%". Expected COPY, REGISTER or ALL.
exit /b 1

:copy

xcopy /F /Y "%HMS_LIBS%\openssl-3.5.8\out64\bin\libcrypto-3-x64.dll" "%OUT_DIR%"
if errorlevel 1 exit /b 1

xcopy /F /Y "%HMS_LIBS%\openssl-3.5.8\out64\bin\libssl-3-x64.dll" "%OUT_DIR%"
if errorlevel 1 exit /b 1

xcopy /F /Y "%HMS_LIBS%\postgresql-15.19\Release\libpq\*.dll" "%OUT_DIR%"
if errorlevel 1 exit /b 1

xcopy /F /Y "%HMS_LIBS%\libmariadb-3.4.9\build64\libmariadb\RelWithDebInfo\libmariadb.dll" "%OUT_DIR%"
if errorlevel 1 exit /b 1

REM The backup/restore feature launches this; libraries\build-7zip.ps1 fetches it.
xcopy /F /Y "%HMS_LIBS%\7zip-26.03\7za.exe" "%OUT_DIR%"
if errorlevel 1 exit /b 1

xcopy /F /Y "%SCRIPT_DIR%..\..\..\installation\Extras\public_suffix_list.dat" "%OUT_DIR%"
if errorlevel 1 exit /b 1

if /I "%MODE%"=="COPY" exit /b 0

:register

"%TARGET%" /Register
if errorlevel 1 exit /b 1

exit /b 0
