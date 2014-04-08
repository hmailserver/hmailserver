// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include "RuleActions.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   RuleActions::RuleActions(__int64 iRuleID) :
      m_iRuleID(iRuleID)
   {
   }

   RuleActions::~RuleActions(void)
   {
   }

   void 
   RuleActions::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_rule_actions where actionruleid = %I64d order by actionsortorder asc"), m_iRuleID);

      _DBLoad(sSQL);
   }



   bool
   RuleActions::PreSaveObject(shared_ptr<RuleAction> pRuleAction, XNode *node)
   {
      pRuleAction->SetRuleID(m_iRuleID);
      return true;
   }   

   void
   RuleActions::MoveUp(shared_ptr<RuleAction> pRuleToMove)
   {
      vector<shared_ptr<RuleAction> >::iterator iter = _GetRuleActionIterator(pRuleToMove);
      vector<shared_ptr<RuleAction> >::iterator iterEnd = vecObjects.end();

      if (iter == iterEnd || iter == vecObjects.begin())
         return;

      vector<shared_ptr<RuleAction> >::iterator iterPrevious = iter - 1;

      // Move the rule to the previous position in the vector.
      shared_ptr<RuleAction> pRuleAction = (*iter);

      // Delete it from the current position
      vecObjects.erase(iter);

      // Insert the rule one step back
      vecObjects.insert(iterPrevious, pRuleAction);

      // Check that sort order is correct.
      _UpdateSortOrder();
   }

   void
   RuleActions::MoveDown(shared_ptr<RuleAction> pRuleAction)
   {
      vector<shared_ptr<RuleAction> >::iterator iter = _GetRuleActionIterator(pRuleAction);
      vector<shared_ptr<RuleAction> >::iterator iterEnd = vecObjects.end();

      vector<shared_ptr<RuleAction> >::iterator iterNext = iter +1;

      if (iter == iterEnd || iterNext == vecObjects.end())
         return;

      // Move the rule to the next position in the vector.
      shared_ptr<RuleAction> pNextRuleAction = (*iterNext);

      // Delete the next rule from the current position.
      vecObjects.erase(iterNext);

      // Insert the next rule before this one.
      vecObjects.insert(iter, pNextRuleAction);

      // Check that sort order is correct.
      _UpdateSortOrder();
   }

   void 
   RuleActions::_UpdateSortOrder()
   {
      vector<shared_ptr<RuleAction> >::iterator iter = vecObjects.begin();
      vector<shared_ptr<RuleAction> >::iterator iterEnd = vecObjects.end();

      int iSortOrder = 1;

      for (; iter != iterEnd; iter++, iSortOrder ++)
      {
         shared_ptr<RuleAction> pRuleAction = (*iter);

         if (pRuleAction->GetSortOrder() != iSortOrder)
         {
            // We need to update the sort order of this rule.
            pRuleAction->SetSortOrder(iSortOrder);

            // If it's a new rule action, we should not save it now.
            if (pRuleAction->GetID() > 0)
            {
               PersistentRuleAction::SaveObject(pRuleAction);
            }
         }
      }
   }

   vector<shared_ptr<RuleAction> >::iterator 
   RuleActions::_GetRuleActionIterator(shared_ptr<RuleAction> pRuleAction)
   {
      vector<shared_ptr<RuleAction> >::iterator iter = vecObjects.begin();
      vector<shared_ptr<RuleAction> >::iterator iterEnd = vecObjects.end();

      int iCurrentSortOrder = -1;

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<RuleAction> pCurRuleAction = (*iter);

         if (pCurRuleAction == pRuleAction)
         {
            return iter;
         }
      }

      return iterEnd;
   }


}