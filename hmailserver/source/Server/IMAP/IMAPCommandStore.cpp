// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "IMAPCommandStore.h"
#include "IMAPStore.h"
#include "IMAPConnection.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommandStore::IMAPCommandStore()
   {

   }

   IMAPCommandStore::~IMAPCommandStore()
   {

   }

   IMAPResult
   IMAPCommandStore::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {


      String sTag = pArgument->Tag();
      String sCommand = pArgument->Command();

      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      if (!pConnection->GetCurrentFolder())
         return IMAPResult(IMAPResult::ResultNo, "No folder selected.");


      std::shared_ptr<IMAPStore> pStore = std::shared_ptr<IMAPStore>(new IMAPStore());
      pStore->SetIsUID(false);

      String sResponse; 
      long lMailNoStart = 6;
      long lMailNoEnd = sCommand.Find(_T(" "), lMailNoStart);
      long lMailNoLen = lMailNoEnd - lMailNoStart;
      String sMailNo = sCommand.Mid(lMailNoStart, lMailNoLen);
      String sShowPart = sCommand.Mid(lMailNoEnd);

      pArgument->Command(sShowPart);

      IMAPResult result = pStore->DoForMails(pConnection, sMailNo, pArgument);

      if (result.GetResult() == IMAPResult::ResultOK)
         pConnection->SendAsciiData(pArgument->Tag() + " OK STORE completed\r\n");

      return result;
   }
}
