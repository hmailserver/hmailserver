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

      CriticalSection m_oAccountCritSec;
      std::map<String, shared_ptr<Account> > m_mapAccounts;

      int m_iNoOfHits;
      int m_iNoOfMisses;

      int m_iCacheTTL;

   };
}