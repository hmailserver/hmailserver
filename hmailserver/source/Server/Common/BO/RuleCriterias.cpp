// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "RuleCriterias.h"
#include "RuleCriteria.h"
#include "../Persistence/PersistentRuleCriteria.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   RuleCriterias::RuleCriterias(__int64 iRuleID) :
      m_iRuleID(iRuleID)
   {
   }

   RuleCriterias::~RuleCriterias(void)
   {

   }

   void 
   RuleCriterias::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_rule_criterias where criteriaruleid = %I64d order by criteriaid asc"), m_iRuleID);

      _DBLoad(sSQL);
   }


   bool
   RuleCriterias::PreSaveObject(shared_ptr<RuleCriteria> pRuleCriteria, XNode *node)
   {
      pRuleCriteria->SetRuleID(m_iRuleID);
      return true;
   }   
}