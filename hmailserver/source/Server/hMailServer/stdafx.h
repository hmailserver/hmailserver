// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#pragma warning( disable : 4180 )


#define STRICT
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define NOMINMAX

// Following define is to solve this compilation warning:
//    C:\Dev\hMailLibs\VS2013\boost_1_56_0\boost/asio/detail/impl/socket_ops.ipp(2315) : error C2220 : warning treated as error - no 'object' file generated
//    C:\Dev\hMailLibs\VS2013\boost_1_56_0\boost/asio/detail/impl/socket_ops.ipp(2315) : warning C4996 : 'gethostbyaddr' : Use getnameinfo() or GetNameInfoW() instead or define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings
#define _WINSOCK_DEPRECATED_NO_WARNINGS


// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.

#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define _WIN32_WINDOWS 0x0501

// START: ATL settings
	#define _ATL_FREE_THREADEDLPCWSTR
	#define _ATL_NO_AUTOMATIC_NAMESPACE
	// turns off ATL's hiding of some common and often safely ignored warning messages
	#define _ATL_ALL_WARNINGS
// END: ATL settings

// Needed for rand_s() function
#define _CRT_RAND_S

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
//
#define BOOST_USE_WINAPI_VERSION 0x0501
#include <boost/winapi/config.hpp>
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