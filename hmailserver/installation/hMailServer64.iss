#define HMAILSERVERLIBS = GetEnv("hMailServerLibs")
#define OPENSSL_LIBS_PATH HMAILSERVERLIBS + "\openssl-3.5.8\out64\bin"
#define POSTGRESQL_LIBPQ_PATH HMAILSERVERLIBS + "\postgresql-15.19\Release\libpq"
#define MARIADB_LIBS_PATH HMAILSERVERLIBS + "\libmariadb-3.4.9\build64\libmariadb\RelWithDebInfo"
#define SEVENZIP_PATH HMAILSERVERLIBS + "\7zip-26.03"

; The Visual C++ runtime shipped in Bin comes from the toolset that compiled hMailServer.exe,
; so the two cannot drift apart. build\Get-VCRedistPath.ps1 locates it: the build workflow
; passes the result as /DVCREDIST_PATH, a local build sets VCRedistPath instead.
#ifndef VCREDIST_PATH
  #define VCREDIST_PATH GetEnv("VCRedistPath")
#endif
#if VCREDIST_PATH == ""
  #error "VCREDIST_PATH is not set. Set the VCRedistPath environment variable to the output of build\Get-VCRedistPath.ps1, or pass /DVCREDIST_PATH to ISCC."
#endif

; The Universal CRT, for the Windows versions older than 10 that have none of their own.
; 10.0.14393 is the newest one Microsoft supports on Windows Vista through 8.1 - see
; "Universal CRT deployment" on Microsoft Learn - so it is checked in here rather than taken
; from whichever Windows SDK the build machine happens to carry. A newer one breaks Vista:
; its loader cannot resolve the "kernel32.dll.VirtualAlloc" forwarders they use, and
; hMailServer.exe then dies at startup. build\Test-DownlevelForwarders.ps1 guards that.
; Taken from the Windows 10 SDK version 1607, Redist\ucrt\DLLs\x64:
; https://learn.microsoft.com/en-us/windows/apps/windows-sdk/downloads-archive
#define UCRT_PATH "Microsoft.UCRT.WindowsSDK14393"

#include "section_setup.iss"
#include "section_setup_64.iss"
#include "section_custom_messages.iss"
#include "section_languages.iss"
#include "section_istool.iss"
#include "section_types.iss"
#include "section_components.iss"

#include "section_files_common.iss"

#include "section_files_64.iss"

#include "section_messages.iss"
#include "section_ini.iss"
#include "section_dirs.iss"
#include "section_run.iss"
#include "section_uninstallrun.iss"

#include "section_icons.iss"

#include "hMailServerInnoExtension.iss"

