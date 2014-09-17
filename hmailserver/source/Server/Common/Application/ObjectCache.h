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
      std::shared_ptr<DomainAliases> GetDomainAliases();

      void SetGlobalRulesNeedsReload();
      std::shared_ptr<Rules> GetGlobalRules();

      void SetAccountRulesNeedsReload(__int64 iAccountID);
      std::shared_ptr<Rules> GetAccountRules(__int64 iAccountID);

      void ClearRuleCaches();

   private:

      std::shared_ptr<DomainAliases> domain_aliases_;
      bool domain_aliases_needs_reload_;

      std::shared_ptr<Rules> global_rules_;
      bool global_rules_needs_reload_;

      std::map<__int64, std::shared_ptr<Rules> > account_rules_;
      std::set<__int64> account_rules_to_refresh_;

      boost::recursive_mutex domain_aliases_mutex_;
      boost::recursive_mutex global_rules_mutex_;
      boost::recursive_mutex account_rules_mutex_;

   };
}