// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\externalfetchtask.h"

#include "ExternalFetch.h"
#include "..\Common\BO\FetchAccount.h"
#include "../Common/Persistence/PersistentFetchAccount.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ExternalFetchTask::ExternalFetchTask(shared_ptr<FetchAccount> pFA) : 
      m_pFA(pFA)
   {
   }

   ExternalFetchTask::~ExternalFetchTask(void)
   {
   }

   void 
   ExternalFetchTask::DoWork()
   {
      try
      {
         // Do the actual delivery of the message.
         ExternalFetch oFetcher;
         oFetcher.Start(m_pFA);
      }
      catch (boost::system::system_error error)
      {
         String sErrorMessage;
         sErrorMessage.Format(_T("An error occurred while download messages from external account. Error number: %d, Description: %s"), error.code().value(), String(error.what()));
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5316, "DeliveryTask::DoWork", sErrorMessage);
      }
      catch (...)
      {
         String sErrorMessage = _T("An error occurred while download messages from external account.");
         ErrorManager::Instance()->ReportError(ErrorManager::High, 5317, "DeliveryTask::DoWork", sErrorMessage);
      }

      // Set next fetch time 
      PersistentFetchAccount::SetNextTryTime(m_pFA);

      // Unlock the account
      PersistentFetchAccount::Unlock(m_pFA->GetID());
   }

   void 
   ExternalFetchTask::StopWork()
   {
      
   }

}