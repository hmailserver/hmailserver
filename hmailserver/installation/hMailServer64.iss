#define HMAILSERVERLIBS "C:\Dev\hMailLibs\Shared\"

; OPENSSL_LIBS_PATH is initialized from main.yml when running under GitHub Actions
#ifndef OPENSSL_LIBS_PATH
	#define OPENSSL_LIBS_PATH HMAILSERVERLIBS + "openssl-1.1.1i\out64\bin"
#endif



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

