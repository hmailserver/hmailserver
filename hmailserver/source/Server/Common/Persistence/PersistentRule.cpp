// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "PersistentRule.h"
#include "PersistentRuleCriteria.h"
#include "PersistentRuleAction.h"

#include "..\BO\Rule.h"

#include "..\BO\RuleActions.h"
#include "..\BO\RuleAction.h"
#include "..\BO\RuleCriterias.h"
#include "..\BO\RuleCriteria.h"

#include "..\Application\ObjectCache.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentRule::PersistentRule(void)
   {
   }

   PersistentRule::~PersistentRule(void)
   {
   }

   bool
   PersistentRule::ReadObject(std::shared_ptr<Rule> pRule, const SQLCommand& sSQL)
   {
      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(sSQL);
      if (!pRS)
         return false;

      bool bRetVal = false;
      if (!pRS->IsEOF())
      {
         bRetVal = ReadObject(pRule, pRS);
      }

      return bRetVal;
   }

   bool
   PersistentRule::ReadObject(std::shared_ptr<Rule> pRule, std::shared_ptr<DALRecordset> pRS)
   {
      if (pRS->IsEOF())
         return false;

      pRule->SetID(pRS->GetLongValue("ruleid"));
      pRule->SetAccountID(pRS->GetLongValue("ruleaccountid"));
      pRule->SetName(pRS->GetStringValue("rulename"));
      pRule->SetActive(pRS->GetLongValue("ruleactive") ? true : false);
      pRule->SetUseAND(pRS->GetLongValue("ruleuseand") ? true : false);
      pRule->SetSortOrder(pRS->GetLongValue("rulesortorder"));

      // Read actions
      pRule->GetActions();

      // Read criterias
      pRule->GetCriterias();

      return true;
   }

   bool
   PersistentRule::SaveObject(std::shared_ptr<Rule> pRule, String &errorMessage, PersistenceMode mode)
   {
      // errorMessage not supported yet.
      return SaveObject(pRule);
   }


   bool
   PersistentRule::SaveObject(std::shared_ptr<Rule> pRule)
   {
      SQLStatement oStatement;
      oStatement.SetTable("hm_rules");
      
      bool bNewObject = pRule->GetID() == 0;

      if (bNewObject)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("ruleid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);
         String sWhere;
         sWhere.Format(_T("ruleid = %I64d"), pRule->GetID());
         oStatement.SetWhereClause(sWhere);
      }

      oStatement.AddColumnInt64("ruleaccountid", pRule->GetAccountID());
      oStatement.AddColumn("rulename", pRule->GetName());
      oStatement.AddColumn("ruleactive", 0);
      oStatement.AddColumn("ruleuseand", pRule->GetUseAND());
      oStatement.AddColumn("rulesortorder", pRule->GetSortOrder());

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pRule->SetID((int) iDBID);

      // Save criterias.
      __int64 iRuleID = pRule->GetID();
      std::shared_ptr<RuleCriterias> pRuleCriterias = pRule->GetCriterias();
      for (int i = 0; i < pRuleCriterias->GetCount(); i++)
      {
         std::shared_ptr<RuleCriteria> pRuleCriteria = pRuleCriterias->GetItem(i);
         pRuleCriteria->SetRuleID(iRuleID);
         PersistentRuleCriteria::SaveObject(pRuleCriteria);
      }
      
      // Save actions
      std::shared_ptr<RuleActions> pRuleActions = pRule->GetActions();
      for (int i = 0; i < pRuleActions->GetCount(); i++)
      {
         std::shared_ptr<RuleAction> pRuleAction = pRuleActions->GetItem(i);
         pRuleAction->SetRuleID(iRuleID);
         PersistentRuleAction::SaveObject(pRuleAction);
      }

      // Set the rule to active again.
      SQLCommand command("update hm_rules set ruleactive = @ACTIVE where ruleid = @RULEID");
      command.AddParameter("@ACTIVE", pRule->GetActive());
      command.AddParameter("@RULEID", iRuleID);

      bRetVal = Application::Instance()->GetDBManager()->Execute(command);

      NotifyReload_(pRule);

      return bRetVal;

   }

   void
   PersistentRule::DeleteByAccountID(__int64 iAccountID)
   {
      SQLCommand selectCommand("select * from hm_rules where ruleaccountid = @ACCOUNTID");
      selectCommand.AddParameter("@ACCOUNTID", iAccountID);

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(selectCommand);
      if (!pRS)
         return ;

      bool bRetVal = false;
      while (!pRS->IsEOF())
      {
         // Create and read the fetch account.
         std::shared_ptr<Rule> oRule = std::shared_ptr<Rule>(new Rule);

         if (ReadObject(oRule, pRS))
         {
            // Delete this fetch account and all the 
            // UID's connected to it.
            DeleteObject(oRule);
         }

         pRS->MoveNext();
      }

      // All the fetch accounts have been deleted.

   }

   bool
   PersistentRule::DeleteObject(std::shared_ptr<Rule> pRule)
   {
      SQLCommand command("delete from hm_rules where ruleid = @RULEID");
      command.AddParameter("@RULEID", pRule->GetID());

      Application::Instance()->GetDBManager()->Execute(command);

      PersistentRuleAction::DeleteObjects(pRule->GetID());
      PersistentRuleCriteria::DeleteObjects(pRule->GetID());

      NotifyReload_(pRule);

      return true;
   }

   void
   PersistentRule::NotifyReload_(std::shared_ptr<Rule> pRule)
   {
      if (pRule->GetAccountID() == 0)
         ObjectCache::Instance()->SetGlobalRulesNeedsReload();
      else
         ObjectCache::Instance()->SetAccountRulesNeedsReload(pRule->GetAccountID());
   }
}