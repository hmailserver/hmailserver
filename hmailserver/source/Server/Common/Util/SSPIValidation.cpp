// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "SSPIValidation.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool 
   SSPIValidation::ValidateUser(const String &sDomain, const String &sUsername, const String &sPassword)
   {
	  HANDLE token;
      BOOL result = LogonUser(sUsername, sDomain, sPassword, LOGON32_LOGON_NETWORK, LOGON32_PROVIDER_DEFAULT, &token);
	  CloseHandle(token);

	  return result != 0;
   }
}