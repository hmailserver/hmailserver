#define HMAILSERVERLIBS = GetEnv("hMailServerLibs")
#define OPENSSL_LIBS_PATH HMAILSERVERLIBS + "\openssl-3.5.8\out64\bin"
#define POSTGRESQL_LIBPQ_PATH HMAILSERVERLIBS + "\postgresql-15.19\Release\libpq"
#define MARIADB_LIBS_PATH HMAILSERVERLIBS + "\libmariadb-3.4.9\build64\libmariadb\RelWithDebInfo"
#define SEVENZIP_PATH HMAILSERVERLIBS + "\7zip-26.03"

; The Visual C++ runtime shipped in Bin, in two versions, because the redistributable version
; is also an operating system floor. Microsoft's advice is to ship the latest available
; runtime, and the latest one runs on Windows 10 and later only - it imports CopyFile2,
; CreateFile2 and GetLogicalProcessorInformationEx, which Vista does not have. So Windows 10
; and later get the newest, and everything below it gets the newest that still runs there,
; which is the 14.29 redistributable of the v142 toolset that compiled hMailServer.exe.
; Neither may be older than that toolset. build\Get-VCRedistPath.ps1 locates both: the build
; workflow passes them as /DVCREDIST_PATH_MODERN and /DVCREDIST_PATH_LEGACY, a local build
; sets VCRedistPathModern and VCRedistPathLegacy instead.
#ifndef VCREDIST_PATH_MODERN
  #define VCREDIST_PATH_MODERN GetEnv("VCRedistPathModern")
#endif
#if VCREDIST_PATH_MODERN == ""
  #error "VCREDIST_PATH_MODERN is not set. Set the VCRedistPathModern environment variable to the output of 'Get-VCRedistPath -Newest', or pass /DVCREDIST_PATH_MODERN to ISCC."
#endif

#ifndef VCREDIST_PATH_LEGACY
  #define VCREDIST_PATH_LEGACY GetEnv("VCRedistPathLegacy")
#endif
#if VCREDIST_PATH_LEGACY == ""
  #error "VCREDIST_PATH_LEGACY is not set. Set the VCRedistPathLegacy environment variable to the output of build\Get-VCRedistPath.ps1, or pass /DVCREDIST_PATH_LEGACY to ISCC."
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

