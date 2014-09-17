// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandFetch.h"
#include "IMAPFetch.h"
#include "IMAPConnection.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommandFETCH::IMAPCommandFETCH()
   {

   }

   IMAPCommandFETCH::~IMAPCommandFETCH()
   {

   }


   IMAPResult
   IMAPCommandFETCH::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {
      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      String sTag = pArgument->Tag();
      String sCommand = pArgument->Command();

      if (!pConnection->GetCurrentFolder())
         return IMAPResult(IMAPResult::ResultNo, "No folder selected.");

      std::shared_ptr<IMAPFetch> pFetch = std::shared_ptr<IMAPFetch>(new IMAPFetch());
      pFetch->SetIsUID(false);

      String sResponse; 
      long lMailNoStart = 6;
      long lMailNoEnd = sCommand.Find(_T(" "), lMailNoStart);
      long lMailNoLen = lMailNoEnd - lMailNoStart;
      String sMailNo = sCommand.Mid(lMailNoStart, lMailNoLen);
      String sShowPart = sCommand.Mid(lMailNoEnd);

      // Set a new command
      pArgument->Command(sShowPart);

      if (sMailNo.IsEmpty())
         return IMAPResult(IMAPResult::ResultBad, "No mail number specified");

      if (!StringParser::ValidateString(sMailNo, "01234567890,.:*"))
         return IMAPResult(IMAPResult::ResultBad, "Incorrect mail number");
      
      IMAPResult result = pFetch->DoForMails(pConnection, sMailNo, pArgument);

      if (result.GetResult() == IMAPResult::ResultOK)
         pConnection->SendAsciiData(pArgument->Tag() + " OK FETCH completed\r\n");

      return result;
   }
}