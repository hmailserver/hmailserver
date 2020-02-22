#include "StdAfx.h"

#include "AccountCache.h"
#include "..\BO\Account.h"

namespace HM
{
   AccountCache::AccountCache(void)
   {
      cache_ttl_ = 5;
      no_of_misses_ = 0;
      no_of_hits_ = 0;
   }

   AccountCache::~AccountCache(void)
   {
   }

   void
   AccountCache::SetTTL(int iNewVal)
   {
      CriticalSectionLeaver oLeaver(account_crit_sec_);
      account_crit_sec_.Enter();

      cache_ttl_ = iNewVal;
   }


   void 
   AccountCache::AddAccount(shared_ptr<Account> pAccount)
   {
      CriticalSectionLeaver oLeaver(account_crit_sec_);
      account_crit_sec_.Enter();

      // If we've gotten here, it means that we
      // have missed a cache. (otherwise we wouldn't
      // be adding an object to the cache)
      no_of_misses_++;

      // First check that someone else hasn't already added it.
      if (accounts_.find(pAccount->GetAddress()) != accounts_.end())
         return;

      accounts_[pAccount->GetAddress()] = pAccount;
   }

   shared_ptr<Account>
   AccountCache::GetAccount(const String &sAccountAddress, uint64 iAccountID )
   {
      CriticalSectionLeaver oLeaver(account_crit_sec_);
      account_crit_sec_.Enter();
      std::map<String, shared_ptr<Account> >::iterator iterAccount;
      if (iAccountID > 0)
      {
         // Find the Account using the ID
         iterAccount = accounts_.begin();
         while (iterAccount != accounts_.end())
         {
            if ((*iterAccount).second->GetID() == iAccountID)
               break;

            iterAccount++;
         }
      }
      else
      {
         // Find Account using it's name.
         iterAccount = accounts_.find(sAccountAddress);
      }

      if (iterAccount != accounts_.end())
      {

         shared_ptr<Account> pAccount = (*iterAccount).second;

         if (pAccount->Seconds() < cache_ttl_)
         {
            // A somewhat fresh Account was found in the 
            // cache. Use this.
            no_of_hits_++;
            return pAccount;
         }

         // Account is old. Delete it from the cache
         accounts_.erase(iterAccount);
      }

      shared_ptr<Account> pEmpty;
      return pEmpty;

   }

   void
   AccountCache::RemoveAccount(shared_ptr<Account> pAccount)
   {
      CriticalSectionLeaver oLeaver(account_crit_sec_);
      account_crit_sec_.Enter();

      std::map<String, shared_ptr<Account> >::iterator iterAccount = accounts_.find(pAccount->GetName());

      if (iterAccount != accounts_.end())
      {
         accounts_.erase(iterAccount);
      }
   }

   int 
   AccountCache::GetHitRate()
   {
      CriticalSectionLeaver oLeaver(account_crit_sec_);
      account_crit_sec_.Enter();

      if (no_of_hits_ == 0)
         return 0;

      int iHitRate = (int) (((float) no_of_hits_ / (float) (no_of_hits_ + no_of_misses_)) * 100);

      return iHitRate;
   }

   void 
   AccountCache::ClearCache()
   {
      CriticalSectionLeaver oLeaver(account_crit_sec_);
      account_crit_sec_.Enter();

      accounts_.clear();
      no_of_hits_ = 0;
      no_of_misses_ = 0;

   }
}