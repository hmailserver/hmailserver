// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPCommandStartTls.h"
#include "IMAPConnection.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommandStartTls::IMAPCommandStartTls()
   {

   }

   IMAPCommandStartTls::~IMAPCommandStartTls()
   {

   }

   IMAPResult
   IMAPCommandStartTls::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (pConnection->GetConnectionSecurity() == CSSTARTTLSOptional ||
         pConnection->GetConnectionSecurity() == CSSTARTTLSRequired)
      {
         pConnection->SendAsciiData(pArgument->Tag() + " OK Begin TLS negotiation now\r\n");

         pConnection->StartHandshake();

         return IMAPResult(IMAPResult::ResultOKSupressRead, "");
      }
      else
      {
         return IMAPResult(IMAPResult::ResultBad, "Unknown or NULL command");
      }
      
   }
}
