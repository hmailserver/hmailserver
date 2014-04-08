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
      m_bDomainAliasesNeedsReload = true;
      m_bGlobalRulesNeedsReload = true;
   }

   ObjectCache::~ObjectCache(void)
   {

   }

   void 
   ObjectCache::SetDomainAliasesNeedsReload()
   {
      CriticalSectionScope scope(m_oDACritSec);

      m_bDomainAliasesNeedsReload = true;
   }

   shared_ptr<DomainAliases> 
   ObjectCache::GetDomainAliases()
   {
      CriticalSectionScope scope(m_oDACritSec);

      if (!m_pDomainAliases || m_bDomainAliasesNeedsReload)
      {
         m_pDomainAliases = shared_ptr<DomainAliases>(new DomainAliases(0));
         m_pDomainAliases->Refresh();

         m_bDomainAliasesNeedsReload = false;
      }

      return m_pDomainAliases;
   }

   void 
   ObjectCache::SetGlobalRulesNeedsReload()
   {
      CriticalSectionScope scope(m_oGRCritSec);

      m_bGlobalRulesNeedsReload = true;
   }


   shared_ptr<Rules> 
   ObjectCache::GetGlobalRules()
   {
      CriticalSectionScope scope(m_oGRCritSec);

      if (!m_pGlobalRules || m_bGlobalRulesNeedsReload)
      {
         m_pGlobalRules = shared_ptr<Rules>(new Rules(0));
         m_pGlobalRules->Refresh();

         m_bGlobalRulesNeedsReload = false;
      }

      return m_pGlobalRules;
   }

   void 
   ObjectCache::SetAccountRulesNeedsReload(__int64 iAccountID)
   {
      CriticalSectionScope scope(m_oARCritSec);

      set<__int64>::iterator iterRefresh = m_setAccountRulesToRefresh.find(iAccountID);      
      if (iterRefresh == m_setAccountRulesToRefresh.end())
         m_setAccountRulesToRefresh.insert(iAccountID);
   }

   shared_ptr<Rules> 
   ObjectCache::GetAccountRules(__int64 iAccountID)
   {
      CriticalSectionScope scope(m_oARCritSec);

      // First find the rules.
      map<__int64, shared_ptr<Rules> >::iterator iterRules = m_mapAccountRules.find(iAccountID);
      shared_ptr<Rules> pRules;

      if (iterRules == m_mapAccountRules.end())
      {
         pRules = shared_ptr<Rules>(new Rules(iAccountID));
         m_mapAccountRules[iAccountID] = pRules;
      
         // We need to refresh this one.
         m_setAccountRulesToRefresh.insert(iAccountID);
      }
      else
      {
         pRules = (*iterRules).second;
      }

      set<__int64>::iterator iterRefresh = m_setAccountRulesToRefresh.find(iAccountID);
      if (iterRefresh != m_setAccountRulesToRefresh.end())
      {
         pRules->Refresh();

         m_setAccountRulesToRefresh.erase(iterRefresh);
      }

      return pRules;
   }

   void 
   ObjectCache::ClearRuleCaches()
   {
      CriticalSectionScope scopeAccountRules(m_oARCritSec);
      CriticalSectionScope scopeGlobalRules(m_oGRCritSec);

      m_pGlobalRules.reset();
      m_mapAccountRules.clear();
   }
}