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
      rule_id_(iRuleID)
   {
   }

   RuleActions::~RuleActions(void)
   {
   }

   void 
   RuleActions::Refresh()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_rule_actions where actionruleid = %I64d order by actionsortorder asc"), rule_id_);

      DBLoad_(sSQL);
   }



   bool
   RuleActions::PreSaveObject(std::shared_ptr<RuleAction> pRuleAction, XNode *node)
   {
      pRuleAction->SetRuleID(rule_id_);
      return true;
   }   

   void
   RuleActions::MoveUp(std::shared_ptr<RuleAction> pRuleToMove)
   {
      auto iter = GetRuleActionIterator_(pRuleToMove);
      auto iterEnd = vecObjects.end();

      if (iter == iterEnd || iter == vecObjects.begin())
         return;

      auto iterPrevious = iter - 1;

      // Move the rule to the previous position in the vector.
      std::shared_ptr<RuleAction> pRuleAction = (*iter);

      // Delete it from the current position
      vecObjects.erase(iter);

      // Insert the rule one step back
      vecObjects.insert(iterPrevious, pRuleAction);

      // Check that sort order is correct.
      UpdateSortOrder_();
   }

   void
   RuleActions::MoveDown(std::shared_ptr<RuleAction> pRuleAction)
   {
      auto iter = GetRuleActionIterator_(pRuleAction);
      auto iterEnd = vecObjects.end();

      auto iterNext = iter +1;

      if (iter == iterEnd || iterNext == vecObjects.end())
         return;

      // Move the rule to the next position in the vector.
      std::shared_ptr<RuleAction> pNextRuleAction = (*iterNext);

      // Delete the next rule from the current position.
      vecObjects.erase(iterNext);

      // Insert the next rule before this one.
      vecObjects.insert(iter, pNextRuleAction);

      // Check that sort order is correct.
      UpdateSortOrder_();
   }

   void 
   RuleActions::UpdateSortOrder_()
   {
      auto iter = vecObjects.begin();
      auto iterEnd = vecObjects.end();

      int iSortOrder = 1;

      for (; iter != iterEnd; iter++, iSortOrder ++)
      {
         std::shared_ptr<RuleAction> pRuleAction = (*iter);

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

   std::vector<std::shared_ptr<RuleAction> >::iterator 
   RuleActions::GetRuleActionIterator_(std::shared_ptr<RuleAction> pRuleAction)
   {
      auto iter = vecObjects.begin();
      auto iterEnd = vecObjects.end();

      int iCurrentSortOrder = -1;

      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<RuleAction> pCurRuleAction = (*iter);

         if (pCurRuleAction == pRuleAction)
         {
            return iter;
         }
      }

      return iterEnd;
   }


}