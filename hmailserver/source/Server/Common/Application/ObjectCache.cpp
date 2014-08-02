// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\ObjectCache.h"

#include "..\BO\DomainAliases.h"
#include "..\BO\Rules.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   ObjectCache::ObjectCache(void)
   {
      domain_aliases_needs_reload_ = true;
      global_rules_needs_reload_ = true;
   }

   ObjectCache::~ObjectCache(void)
   {

   }

   void 
   ObjectCache::SetDomainAliasesNeedsReload()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_domainAliasesMutex);

      domain_aliases_needs_reload_ = true;
   }

   shared_ptr<DomainAliases> 
   ObjectCache::GetDomainAliases()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_domainAliasesMutex);

      if (!domain_aliases_ || domain_aliases_needs_reload_)
      {
         domain_aliases_ = shared_ptr<DomainAliases>(new DomainAliases(0));
         domain_aliases_->Refresh();

         domain_aliases_needs_reload_ = false;
      }

      return domain_aliases_;
   }

   void 
   ObjectCache::SetGlobalRulesNeedsReload()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_globalRulesMutex);

      global_rules_needs_reload_ = true;
   }


   shared_ptr<Rules> 
   ObjectCache::GetGlobalRules()
   {
      boost::lock_guard<boost::recursive_mutex> guard(_globalRulesMutex);

      if (!global_rules_ || global_rules_needs_reload_)
      {
         global_rules_ = shared_ptr<Rules>(new Rules(0));
         global_rules_->Refresh();

         global_rules_needs_reload_ = false;
      }

      return global_rules_;
   }

   void 
   ObjectCache::SetAccountRulesNeedsReload(__int64 iAccountID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_accountRulesMutex);

      set<__int64>::iterator iterRefresh = account_rules_to_refresh_.find(iAccountID);      
      if (iterRefresh == account_rules_to_refresh_.end())
         account_rules_to_refresh_.insert(iAccountID);
   }

   shared_ptr<Rules> 
   ObjectCache::GetAccountRules(__int64 iAccountID)
   {
      boost::lock_guard<boost::recursive_mutex> guard(_accountRulesMutex);

      // First find the rules.
      map<__int64, shared_ptr<Rules> >::iterator iterRules = account_rules_.find(iAccountID);
      shared_ptr<Rules> pRules;

      if (iterRules == account_rules_.end())
      {
         pRules = shared_ptr<Rules>(new Rules(iAccountID));
         account_rules_[iAccountID] = pRules;
      
         // We need to refresh this one.
         account_rules_to_refresh_.insert(iAccountID);
      }
      else
      {
         pRules = (*iterRules).second;
      }

      set<__int64>::iterator iterRefresh = account_rules_to_refresh_.find(iAccountID);
      if (iterRefresh != account_rules_to_refresh_.end())
      {
         pRules->Refresh();

         account_rules_to_refresh_.erase(iterRefresh);
      }

      return pRules;
   }

   void 
   ObjectCache::ClearRuleCaches()
   {
      boost::lock_guard<boost::recursive_mutex> globalRulesGuard(_globalRulesMutex);
      boost::lock_guard<boost::recursive_mutex> accontRulesGuard(_accountRulesMutex);

      global_rules_.reset();
      account_rules_.clear();
   }
}