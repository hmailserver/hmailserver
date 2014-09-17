// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandCheck.h"

#include "IMAPConnection.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPResult
   IMAPCommandCHECK::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      String sResponse; 
      sResponse += pArgument->Tag() + " OK CHECK completed\r\n";
      pConnection->SendAsciiData(sResponse);   

      return IMAPResult();
   }
}