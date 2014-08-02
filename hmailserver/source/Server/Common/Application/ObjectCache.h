// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class DomainAliases;
   class Rules;

   class ObjectCache : public Singleton<ObjectCache>
   {
   public:
      ObjectCache(void);
      ~ObjectCache(void);

      void SetDomainAliasesNeedsReload();
      shared_ptr<DomainAliases> GetDomainAliases();

      void SetGlobalRulesNeedsReload();
      shared_ptr<Rules> GetGlobalRules();

      void SetAccountRulesNeedsReload(__int64 iAccountID);
      shared_ptr<Rules> GetAccountRules(__int64 iAccountID);

      void ClearRuleCaches();

   private:

      shared_ptr<DomainAliases> domain_aliases_;
      bool m_bDomainAliasesNeedsReload;

      shared_ptr<Rules> global_rules_;
      bool m_bGlobalRulesNeedsReload;

      map<__int64, shared_ptr<Rules> > m_mapAccountRules;
      set<__int64> m_setAccountRulesToRefresh;

      boost::recursive_mutex _domainAliasesMutex;
      boost::recursive_mutex _globalRulesMutex;
      boost::recursive_mutex _accountRulesMutex;

   };
}