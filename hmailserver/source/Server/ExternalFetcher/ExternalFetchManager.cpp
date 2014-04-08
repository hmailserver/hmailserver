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
      m_sQueueName("External fetch queue")
   {
      int iMaxNumberOfSimultaneousTasks = IniFileSettings::Instance()->GetMaxNumberOfExternalFetchThreads();
      
      m_iQueueID = WorkQueueManager::Instance()->CreateWorkQueue(iMaxNumberOfSimultaneousTasks, m_sQueueName, WorkQueue::eQTRandom);
   }

   ExternalFetchManager::~ExternalFetchManager(void)
   {
      WorkQueueManager::Instance()->RemoveQueue(m_sQueueName);
   }

   void
   ExternalFetchManager::DoWork()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Responsible for creating threads that downloads messages from external
   // servers.
   //---------------------------------------------------------------------------()
   {
      Logger::Instance()->LogDebug("ExternalFetchManager::Start()");

      

      PersistentFetchAccount::UnlockAll();

      m_pFetchAccounts = shared_ptr<FetchAccounts> (new FetchAccounts(0));

      while (1)
      {
         m_pFetchAccounts->RefreshPendingList();

         vector<shared_ptr<FetchAccount> > &vecAccounts = m_pFetchAccounts->GetVector();
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
         const int iSize = 2;
         HANDLE handles[iSize];

         handles[0] = m_hStopTask.GetHandle();
         handles[1] = m_hCheckNow.GetHandle();
         // CheckNow is used if a check is triggered by the COM API. Then we don't
         // want to wait 60 seconds to check.
         

         DWORD dwWaitResult = WaitForMultipleObjects(iSize, handles, FALSE, 60000);

         int iEvent = dwWaitResult - WAIT_OBJECT_0;

         switch (iEvent)
         {
         case 0:
            m_hStopTask.Reset();
            return;
         case 1:
            m_hCheckNow.Reset();
            continue;
         }
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
   ExternalFetchManager::StopWork()
   {
      m_hStopTask.Set();
   }

   void 
   ExternalFetchManager::SetCheckNow()
   {
      m_hCheckNow.Set();
   }

}