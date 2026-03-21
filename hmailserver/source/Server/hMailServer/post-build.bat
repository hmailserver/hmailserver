@echo off
setlocal

set HMS_LIBS=%~1
set OUT_DIR=%~2
set TARGET=%~3
set SCRIPT_DIR=%~dp0

NET STOP hMailServer

xcopy /F /Y "%HMS_LIBS%\openssl-3.5.5\out64\bin\libcrypto-3-x64.dll" "%OUT_DIR%"
if errorlevel 1 exit /b 1

xcopy /F /Y "%HMS_LIBS%\openssl-3.5.5\out64\bin\libssl-3-x64.dll" "%OUT_DIR%"
if errorlevel 1 exit /b 1

xcopy /F /Y "%HMS_LIBS%\postgresql-18.3\builddir\src\interfaces\libpq\*.dll" "%OUT_DIR%"
if errorlevel 1 exit /b 1

"%TARGET%" /Register
if errorlevel 1 exit /b 1
