#pragma once
#define STRICT
#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef _WIN64
   // Windows Server 2003
   #define WINVER 0x0502	
   #define _WIN32_WINNT 0x0502	// Change this to the appropriate value to target Windows Server 2008 or later.
   #define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.

#else
   #define WINVER 0x0400		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
   #define _WIN32_WINNT 0x0500	// Change this to the appropriate value to target Windows 2000 or later.
   #define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.

#endif


// START: ATL settings
	#define _ATL_FREE_THREADEDLPCWSTR
	#define _ATL_NO_AUTOMATIC_NAMESPACE
	// turns off ATL's hiding of some common and often safely ignored warning messages
	#define _ATL_ALL_WARNINGS
// END: ATL settings

// Needed for rand_s() function
#define _CRT_RAND_S
#include <stdlib.h>

#include "WinSock2.h"
#include "Windows.h"

// ADO
#if _WIN64
   #import "..\..\..\..\libraries\msado28\msado28-x64.tlb" \
      rename("EOF","adoEOF") \
      no_namespace
#else
   #import "..\..\..\..\libraries\msado28\msado28-x32.tlb" \
      rename("EOF","adoEOF") \
      no_namespace
#endif

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>


//
// STL INCLUDES
//
#include <map>
#include <vector>
#include <set> 
#include <list> 
#include <queue>
#include <functional>
#include <memory>


//
// BOOST INCLUDES

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/signals2/signal.hpp>

#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC
   #include <stdlib.h>
   #include <crtdbg.h>
#endif


// Start: Common files
   #include "..\Common\Util\StdString.h"

   #include "..\Common\Util\XMLite.h"
   #include "..\Common\Util\Singleton.h"
   #include "..\Common\Application\Constants.h"
   #include "..\Common\Application\PropertySet.h"
   #include "..\Common\Application\Configuration.h"
   #include "..\Common\Application\IniFileSettings.h"
   #include "..\Common\Application\Application.h"
   #include "..\Common\Application\Logger.h"
   #include "..\Common\Application\ErrorManager.h"
   #include "..\Common\SQL\SQLParameter.h"
   #include "..\Common\SQL\SQLStatement.h"
   #include "..\Common\SQL\DatabaseConnectionManager.h"
   #include "..\Common\SQL\DALRecordset.h"
   #include "..\Common\SQL\DALRecordsetFactory.h"
   #include "..\Common\SQL\SQLCommand.h"
   #include "..\Common\Util\Parsing\StringParser.h"
   #include "..\Common\Util\FileUtilities.h"
   #include "..\Common\Util\HeapChecker.h"

   #include "..\Common\BO\BusinessObject.h"
   #include "..\COM\COMAuthentication.h"
   #include "..\COM\COMAuthenticator.h"
   #include "..\IMAP\IMAPResult.h"
   #include "..\Common\TCPIP\IPAddress.h"
   #include "../Common/Util/Strings/FormatArgument.h"
   #include "../Common/Util/Strings/Formatter.h"
// End: Common files.
#define COMPILE_NEWAPIS_STUBS
using namespace ATL;