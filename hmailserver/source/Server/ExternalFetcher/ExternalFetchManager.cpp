// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\ExternalFetchManager.h"

#include "../Common/BO/FetchAccounts.h"
#include "../Common/BO/FetchAccount.h"
#include "../Common/Persistence/PersistentFetchAccount.h"

#include "ExternalFetch.h"
#include "ExternalFetchTask.h"

#include "../COmmon/Threading/WorkQueueManager.h"

#include "../COmmon/BO/Account.h"

#include "../COmmon/BO/Domain.h"

#include "../COmmon/Persistence/PersistentDomain.h"
#include "../COmmon/Persistence/PersistentAccount.h"

#include "../COmmon/Cache/Cache.h"



#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   ExternalFetchManager::ExternalFetchManager(void) :
      Task("ExternalFetchManager"),
      m_sQueueName("External fetch queue")
   {
      int iMaxNumberOfSimultaneousTasks = IniFileSettings::Instance()->GetMaxNumberOfExternalFetchThreads();
      
      m_iQueueID = WorkQueueManager::Instance()->CreateWorkQueue(iMaxNumberOfSimultaneousTasks, m_sQueueName);
   }

   ExternalFetchManager::~ExternalFetchManager(void)
   {
      LOG_DEBUG("ExternalFetchManager::~ExternalFetchManager")
      WorkQueueManager::Instance()->RemoveQueue(m_sQueueName);
      LOG_DEBUG("ExternalFetchManager::~ExternalFetchManager - Removed queue")
   }

   void
   ExternalFetchManager::DoWork()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Responsible for creating threads that downloads messages from external
   // servers.
   //---------------------------------------------------------------------------()
   {
      SetIsStarted();

      Logger::Instance()->LogDebug("ExternalFetchManager::Start()");

      PersistentFetchAccount::UnlockAll();

      fetch_accounts_ = shared_ptr<FetchAccounts> (new FetchAccounts(0));


      while (1)
      {
         fetch_accounts_->RefreshPendingList();

         vector<shared_ptr<FetchAccount> > &vecAccounts = fetch_accounts_->GetVector();
         vector<shared_ptr<FetchAccount> >::iterator iterFA = vecAccounts.begin();

         while (iterFA != vecAccounts.end())
         {
            // Create a fetch task that will do the actual work, and
            // add this task to the queue.
            shared_ptr<FetchAccount> pFA = (*iterFA);

            if (_FetchIsAllowed(pFA))
            {
               // We're allowed to fetch. Lock fetchaccount and start the fetcher.
               PersistentFetchAccount::Lock(pFA->GetID());
               shared_ptr<ExternalFetchTask> pTask = shared_ptr<ExternalFetchTask>(new ExternalFetchTask(pFA));
               WorkQueueManager::Instance()->AddTask(m_iQueueID, pTask);
            }
            else
            {
               // We should not fetch for this account now. Update
               // fetch account.
               PersistentFetchAccount::SetNextTryTime(pFA);
            }

            iterFA++;
         }

         // We are currently not fetching anything
         // Sit here and wait a minute 
         check_now_.WaitFor(chrono::minutes(1));
      }


   }

   bool 
   ExternalFetchManager::_FetchIsAllowed(shared_ptr<FetchAccount> pFA)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Checks whether hMailServer should fetch messages for this fetch account.
   //---------------------------------------------------------------------------() 
   {
      __int64 iAccountID = pFA->GetAccountID();

      shared_ptr<const Account> pAccount = Cache<Account, PersistentAccount>::Instance()->GetObject(iAccountID);

      if (!pAccount || !pAccount->GetActive())
      {
         // The account is not active, so we should not
         // fetch messages to it.
         return false;
      }

      __int64 iDomainID = pAccount->GetDomainID();

      shared_ptr<const Domain> pDomain = Cache<Domain, PersistentDomain>::Instance()->GetObject(iDomainID);

      if (!pDomain || !pDomain->GetIsActive())
      {
         // The domain is not active, so we should not 
         // fetch messages for it.
         return false;
      }

      // Both domain and account is active. 
      // This means it's OK to fetch.
      return true;
   }

   void 
   ExternalFetchManager::SetCheckNow()
   {
      check_now_.Set();
   }

}