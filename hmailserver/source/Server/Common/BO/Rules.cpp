// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "Rules.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   Rules::Rules(__int64 iAccountID) :
      m_iAccountID(iAccountID)
   {
      
   }

   Rules::~Rules(void)
   {
   }

   void 
   Rules::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Refreshes this collection from the database.
   //---------------------------------------------------------------------------()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_rules where ruleaccountid = %I64d order by rulesortorder asc"), m_iAccountID);
      _DBLoad(sSQL);
   }

   vector<shared_ptr<Rule> >::iterator 
   Rules::_GetRuleIterator(__int64 iRuleID)
   {
      vector<shared_ptr<Rule> >::iterator iter = vecObjects.begin();
      vector<shared_ptr<Rule> >::iterator iterEnd = vecObjects.end();

      int iCurrentSortOrder = -1;
      shared_ptr<Rule> pRule;
      for (; iter != iterEnd; iter++)
      {
         pRule = (*iter);

         if (pRule->GetID() == iRuleID)
         {
            return iter;
         }
      }

      return iterEnd;
   }

   void
   Rules::MoveUp(__int64 iRuleID)
   {
      vector<shared_ptr<Rule> >::iterator iter = _GetRuleIterator(iRuleID);
      vector<shared_ptr<Rule> >::iterator iterEnd = vecObjects.end();

      if (iter == iterEnd || iter == vecObjects.begin())
         return;

      vector<shared_ptr<Rule> >::iterator iterPrevious = iter - 1;
         
      // Move the rule to the previous position in the vector.
      shared_ptr<Rule> pRule = (*iter);
      
      // Delete it from the current position
      vecObjects.erase(iter);

      // Insert the rule one step back
      vecObjects.insert(iterPrevious, pRule);

      // Check that sort order is correct.
      _UpdateSortOrder();

      Refresh();
   }

   void
   Rules::MoveDown(__int64 iRuleID)
   {
      vector<shared_ptr<Rule> >::iterator iter = _GetRuleIterator(iRuleID);
      vector<shared_ptr<Rule> >::iterator iterEnd = vecObjects.end();
     
      vector<shared_ptr<Rule> >::iterator iterNext = iter +1;

      if (iter == iterEnd || iterNext == vecObjects.end())
         return;

      // Move the rule to the next position in the vector.
      shared_ptr<Rule> pNextRule = (*iterNext);

      // Delete the next rule from the current position.
      vecObjects.erase(iterNext);

      // Insert the next rule before this one.
      vecObjects.insert(iter, pNextRule);

      // Check that sort order is correct.
      _UpdateSortOrder();

      Refresh();
   }

   void 
   Rules::_UpdateSortOrder()
   {
      vector<shared_ptr<Rule> >::iterator iter = vecObjects.begin();
      vector<shared_ptr<Rule> >::iterator iterEnd = vecObjects.end();

      int iSortOrder = 1;

      for (; iter != iterEnd; iter++, iSortOrder ++)
      {
         shared_ptr<Rule> pRule = (*iter);

         if (pRule->GetSortOrder() != iSortOrder)
         {
            // We need to update the sort order of this rule.
            pRule->SetSortOrder(iSortOrder);
            PersistentRule::SaveObject(pRule);
         }
         
      }

   }

   bool
   Rules::PreSaveObject(shared_ptr<Rule> pRule, XNode *node)
   {
      pRule->SetAccountID(m_iAccountID);
      return true;
   }
}