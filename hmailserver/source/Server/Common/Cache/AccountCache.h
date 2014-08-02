#pragma once

namespace HM
{
   class Account;

   class AccountCache
   {
   public:
      AccountCache(void);
      virtual ~AccountCache(void);

      void AddAccount(shared_ptr<Account> pAccount);
      void RemoveAccount(shared_ptr<Account> pAccount);
      shared_ptr<Account> GetAccount(const String &sAccountAddress, uint64 iAccountID);

      void ClearCache();
      void SetTTL(int iNewVal);
      int GetHitRate();

   private:

      CriticalSection account_crit_sec_;
      std::map<String, shared_ptr<Account> > accounts_;

      int no_of_hits_;
      int no_of_misses_;

      int cache_ttl_;

   };
}