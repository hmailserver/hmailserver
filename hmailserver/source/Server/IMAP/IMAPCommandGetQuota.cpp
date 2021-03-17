// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "IMAPCommandGetQuota.h"
#include "IMAPConnection.h"
#include "IMAPConfiguration.h"
#include "IMAPSimpleCommandParser.h"
#include "../Common/BO/IMAPFolders.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/Account.h"
#include "../Common/Persistence/PersistentAccount.h"
#include "../Common/Cache/CacheContainer.h"
#include "../Common/Cache/AccountSizeCache.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPCommandGetQuota::IMAPCommandGetQuota(void)
   {
   }

   IMAPCommandGetQuota::~IMAPCommandGetQuota(void)
   {

   }
   
   IMAPResult 
   IMAPCommandGetQuota::ExecuteCommand(std::shared_ptr<IMAPConnection> pConnection, std::shared_ptr<IMAPCommandArgument> pArgument)
   {

      if (!pConnection->IsAuthenticated())
         return IMAPResult(IMAPResult::ResultNo, "Authenticate first");

      if (!Configuration::Instance()->GetIMAPConfiguration()->GetUseIMAPQuota())
         return IMAPResult(IMAPResult::ResultNo, "IMAP QUOTA is not enabled.");

      std::shared_ptr<IMAPSimpleCommandParser> pParser = std::shared_ptr<IMAPSimpleCommandParser>(new IMAPSimpleCommandParser());
      pParser->Parse(pArgument);
      if (pParser->WordCount() < 2)
         return IMAPResult(IMAPResult::ResultBad, "GETQUOTA Command requires at least 1 parameter.");

      std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(pConnection->GetAccount()->GetID());

      // According to the RFC, these values should be counted in kilobytes.
      __int64 iCurrentSize = AccountSizeCache::Instance()->GetSize(pAccount->GetID()) / 1024;
      __int64 iMaxSize = ((__int64) pAccount->GetAccountMaxSize()) * 1024; // Convert from MB to KB

      String sResponse;
      if (iMaxSize > 0)
      {
         sResponse.Format(_T("* QUOTA \"\" (STORAGE %I64d %I64d)\r\n")
                        _T("%s OK GETQUOTA completed\r\n"), 
                        iCurrentSize, iMaxSize, pArgument->Tag().c_str());
      }
      else
      {
         // NO QUOTA DEFINED
         sResponse.Format(_T("* QUOTA \"\" (STORAGE)\r\n")
            _T("%s OK GETQUOTA completed\r\n"), 
            pArgument->Tag().c_str());

      }
      pConnection->SendAsciiData(sResponse);   

      return IMAPResult();
   }
}