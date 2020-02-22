// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\persistentruleaction.h"
#include "..\BO\RuleAction.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentRuleAction::PersistentRuleAction(void)
   {
   }

   PersistentRuleAction::~PersistentRuleAction(void)
   {
   }

   bool
   PersistentRuleAction::ReadObject(std::shared_ptr<RuleAction> pRuleAction, const SQLCommand& command)
   {

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);

      bool bRetVal = false;
      if (!pRS->IsEOF())
      {
         bRetVal = ReadObject(pRuleAction, pRS);
      }

      return bRetVal;
   }

   bool
   PersistentRuleAction::ReadObject(std::shared_ptr<RuleAction> pRuleAction, std::shared_ptr<DALRecordset> pRS)
   {
      if (pRS->IsEOF())
         return false;

      pRuleAction->SetID(pRS->GetLongValue("actionid"));
      pRuleAction->SetRuleID(pRS->GetLongValue("actionruleid"));
      pRuleAction->SetType((RuleAction::Type)pRS->GetLongValue("actiontype"));
      pRuleAction->SetIMAPFolder(pRS->GetStringValue("actionimapfolder"));
      pRuleAction->SetSubject(pRS->GetStringValue("actionsubject"));
      pRuleAction->SetFromAddress(pRS->GetStringValue("actionfromaddress"));
      pRuleAction->SetFromName(pRS->GetStringValue("actionfromname"));
      pRuleAction->SetBody(pRS->GetStringValue("actionbody"));
      pRuleAction->SetFilename(pRS->GetStringValue("actionfilename"));
      pRuleAction->SetTo(pRS->GetStringValue("actionto"));
      pRuleAction->SetScriptFunction(pRS->GetStringValue("actionscriptfunction"));
      pRuleAction->SetSortOrder(pRS->GetLongValue("actionsortorder"));
      pRuleAction->SetHeaderName(pRS->GetStringValue("actionheader"));
      pRuleAction->SetValue(pRS->GetStringValue("actionvalue"));
      pRuleAction->SetRouteID(pRS->GetLongValue("actionrouteid"));

      return true;
   }

   bool
   PersistentRuleAction::SaveObject(std::shared_ptr<RuleAction> pRuleAction, String &errorMessage, PersistenceMode mode)
   {
      // errorMessage - not supported yet.
      return SaveObject(pRuleAction);
   }

   bool
   PersistentRuleAction::SaveObject(std::shared_ptr<RuleAction> pRuleAction)
   {
      SQLStatement oStatement;
      oStatement.SetTable("hm_rule_actions");

      bool bNewObject = pRuleAction->GetID() == 0;

      if (bNewObject)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("actionid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);

         String sWhere;
         sWhere.Format(_T("actionid = %I64d"), pRuleAction->GetID());
         oStatement.SetWhereClause(sWhere);

      }

      oStatement.AddColumnInt64("actionruleid", pRuleAction->GetRuleID());
      oStatement.AddColumn("actiontype", pRuleAction->GetType());
      oStatement.AddColumn("actionimapfolder", pRuleAction->GetIMAPFolder());
      oStatement.AddColumn("actionfromname", pRuleAction->GetFromName());
      oStatement.AddColumn("actionfromaddress", pRuleAction->GetFromAddress());
      oStatement.AddColumn("actionsubject", pRuleAction->GetSubject());
      oStatement.AddColumn("actionbody", pRuleAction->GetBody());
      oStatement.AddColumn("actionfilename", pRuleAction->GetFilename());
      oStatement.AddColumn("actionto", pRuleAction->GetTo());
      oStatement.AddColumn("actionsortorder", pRuleAction->GetSortOrder());
      oStatement.AddColumn("actionscriptfunction", pRuleAction->GetScriptFunction());
      oStatement.AddColumn("actionheader", pRuleAction->GetHeaderName());
      oStatement.AddColumn("actionvalue", pRuleAction->GetValue());
      oStatement.AddColumnInt64("actionrouteid", pRuleAction->GetRouteID());

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pRuleAction->SetID((int) iDBID);

      return bRetVal;

   }

   bool
   PersistentRuleAction::DeleteObject(std::shared_ptr<RuleAction> pRuleAction)
   {
      SQLCommand command("delete from hm_rule_actions where actionid = @ACTIONID");
      command.AddParameter("@ACTIONID", pRuleAction->GetID());

      bool bRet = Application::Instance()->GetDBManager()->Execute(command);
      return bRet; 
   }

   bool
   PersistentRuleAction::DeleteObjects(__int64 iRuleID)
   {
      SQLCommand command("delete from hm_rule_actions where actionruleid = @RULEID");
      command.AddParameter("@RULEID", iRuleID);

      bool bRet = Application::Instance()->GetDBManager()->Execute(command);
      return bRet; 
   }

   bool 
   PersistentRuleAction::GetExistsFolderReferenceContainingCharacter(String theChar)
   {
      theChar = SQLStatement::Escape(theChar);

      String sql;
      sql.Format(_T("select count(*) as c from hm_rule_actions where actionimapfolder like '%" + theChar + "%'"));

      SQLCommand command(sql);

      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return false;

      long count = pRS->GetLongValue("c");

      return count > 0;
   }

   bool
   PersistentRuleAction::UpdateHierarchyDelimiter(String oldDelimiter, String newDelimiter)
   {
      SQLCommand command(_T("update hm_rule_actions set actionimapfolder = replace(actionimapfolder, @OLDDELIMIETER, @NEWDELIMITER)"));
      command.AddParameter("@OLDDELIMIETER", oldDelimiter);
      command.AddParameter("@NEWDELIMITER", newDelimiter);

      return Application::Instance()->GetDBManager()->Execute(command); 
   }
}