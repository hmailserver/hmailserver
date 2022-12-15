// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\ExternalFetchManager.h"

#include "../Common/BO/FetchAccounts.h"
#include "../Common/BO/FetchAccount.h"
#include "../Common/Persistence/PersistentFetchAccount.h"

#include "ExternalFetchTask.h"

#include "../COmmon/Threading/WorkQueueManager.h"

#include "../COmmon/BO/Account.h"

#include "../COmmon/BO/Domain.h"

#include "../COmmon/Cache/CacheContainer.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   ExternalFetchManager::ExternalFetchManager(void) :
      Task("ExternalFetchManager"),
      queue_name_("External fetch queue")
   {
      int iMaxNumberOfSimultaneousTasks = IniFileSettings::Instance()->GetMaxNumberOfExternalFetchThreads();

      queue_id_ = WorkQueueManager::Instance()->CreateWorkQueue(iMaxNumberOfSimultaneousTasks, queue_name_);

   }

   ExternalFetchManager::~ExternalFetchManager(void)
   {
      try
      {
         LOG_DEBUG("ExternalFetchManager::DoWork() - Exiting")
         WorkQueueManager::Instance()->RemoveQueue(queue_name_);
         LOG_DEBUG("ExternalFetchManager::DoWork() - Removed queue")
      }
      catch (...)
      {

      }

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


      Logger::Instance()->LogDebug("ExternalFetchManager::DoWork()");

      PersistentFetchAccount::UnlockAll();

      fetch_accounts_ = std::shared_ptr<FetchAccounts>(new FetchAccounts(0));


      while (1)
      {
         fetch_accounts_->RefreshPendingList();

         std::vector<std::shared_ptr<FetchAccount> > &vecAccounts = fetch_accounts_->GetVector();
         auto iterFA = vecAccounts.begin();

         while (iterFA != vecAccounts.end())
         {
            // Create a fetch task that will do the actual work, and
            // add this task to the queue.
            std::shared_ptr<FetchAccount> pFA = (*iterFA);

            if (FetchIsAllowed_(pFA))
            {
               // We're allowed to fetch. Lock fetchaccount and start the fetcher.
               PersistentFetchAccount::Lock(pFA->GetID());
               std::shared_ptr<ExternalFetchTask> pTask = std::shared_ptr<ExternalFetchTask>(new ExternalFetchTask(pFA));
               WorkQueueManager::Instance()->AddTask(queue_id_, pTask);
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
         check_now_.WaitFor(boost::chrono::minutes(1));
      }
   }

   bool 
   ExternalFetchManager::FetchIsAllowed_(std::shared_ptr<FetchAccount> pFA)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Checks whether hMailServer should fetch messages for this fetch account.
   //---------------------------------------------------------------------------() 
   {
      __int64 iAccountID = pFA->GetAccountID();

      std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(iAccountID);

      if (!pAccount || !pAccount->GetActive())
      {
         // The account is not active, so we should not
         // fetch messages to it.
         return false;
      }

      __int64 iDomainID = pAccount->GetDomainID();

      std::shared_ptr<const Domain> pDomain = CacheContainer::Instance()->GetDomain(iDomainID);

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