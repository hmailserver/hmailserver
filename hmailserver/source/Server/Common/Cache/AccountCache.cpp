#include "StdAfx.h"

#include "AccountCache.h"
#include "..\BO\Account.h"

namespace HM
{
   AccountCache::AccountCache(void)
   {
      m_iCacheTTL = 5;
      m_iNoOfMisses = 0;
      m_iNoOfHits = 0;
   }

   AccountCache::~AccountCache(void)
   {
   }

   void
   AccountCache::SetTTL(int iNewVal)
   {
      CriticalSectionLeaver oLeaver(m_oAccountCritSec);
      m_oAccountCritSec.Enter();

      m_iCacheTTL = iNewVal;
   }


   void 
   AccountCache::AddAccount(shared_ptr<Account> pAccount)
   {
      CriticalSectionLeaver oLeaver(m_oAccountCritSec);
      m_oAccountCritSec.Enter();

      // If we've gotten here, it means that we
      // have missed a cache. (otherwise we wouldn't
      // be adding an object to the cache)
      m_iNoOfMisses++;

      // First check that someone else hasn't already added it.
      if (m_mapAccounts.find(pAccount->GetAddress()) != m_mapAccounts.end())
         return;

      m_mapAccounts[pAccount->GetAddress()] = pAccount;
   }

   shared_ptr<Account>
   AccountCache::GetAccount(const String &sAccountAddress, uint64 iAccountID )
   {
      CriticalSectionLeaver oLeaver(m_oAccountCritSec);
      m_oAccountCritSec.Enter();
      std::map<String, shared_ptr<Account> >::iterator iterAccount;
      if (iAccountID > 0)
      {
         // Find the Account using the ID
         iterAccount = m_mapAccounts.begin();
         while (iterAccount != m_mapAccounts.end())
         {
            if ((*iterAccount).second->GetID() == iAccountID)
               break;

            iterAccount++;
         }
      }
      else
      {
         // Find Account using it's name.
         iterAccount = m_mapAccounts.find(sAccountAddress);
      }

      if (iterAccount != m_mapAccounts.end())
      {

         shared_ptr<Account> pAccount = (*iterAccount).second;

         if (pAccount->Seconds() < m_iCacheTTL)
         {
            // A somewhat fresh Account was found in the 
            // cache. Use this.
            m_iNoOfHits++;
            return pAccount;
         }

         // Account is old. Delete it from the cache
         m_mapAccounts.erase(iterAccount);
      }

      shared_ptr<Account> pEmpty;
      return pEmpty;

   }

   void
   AccountCache::RemoveAccount(shared_ptr<Account> pAccount)
   {
      CriticalSectionLeaver oLeaver(m_oAccountCritSec);
      m_oAccountCritSec.Enter();

      std::map<String, shared_ptr<Account> >::iterator iterAccount = m_mapAccounts.find(pAccount->GetName());

      if (iterAccount != m_mapAccounts.end())
      {
         m_mapAccounts.erase(iterAccount);
      }
   }

   int 
   AccountCache::GetHitRate()
   {
      CriticalSectionLeaver oLeaver(m_oAccountCritSec);
      m_oAccountCritSec.Enter();

      if (m_iNoOfHits == 0)
         return 0;

      int iHitRate = (int) (((float) m_iNoOfHits / (float) (m_iNoOfHits + m_iNoOfMisses)) * 100);

      return iHitRate;
   }

   void 
   AccountCache::ClearCache()
   {
      CriticalSectionLeaver oLeaver(m_oAccountCritSec);
      m_oAccountCritSec.Enter();

      m_mapAccounts.clear();
      m_iNoOfHits = 0;
      m_iNoOfMisses = 0;

   }
}