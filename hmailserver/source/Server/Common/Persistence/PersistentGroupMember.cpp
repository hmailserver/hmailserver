// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "PersistentGroupMember.h"
#include "PersistentGroup.h"
#include "..\BO\GroupMember.h"
#include "..\BO\Group.h"
#include "..\SQL\SQLStatement.h"
#include "..\Cache\Cache.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentGroupMember::PersistentGroupMember(void)
   {
   }

   PersistentGroupMember::~PersistentGroupMember(void)
   {
   }

   bool
   PersistentGroupMember::DeleteByAccount(__int64 iAccountID)
   {
      SQLCommand command("delete from hm_group_members where memberaccountid = @ACCOUNTID");
      command.AddParameter("@ACCOUNTID", iAccountID);

      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool
   PersistentGroupMember::DeleteObject(std::shared_ptr<GroupMember> pObject)
   {
      SQLCommand command("delete from hm_group_members where memberid = @MEMBERID");
      command.AddParameter("@MEMBERID", pObject->GetID());
     
      return Application::Instance()->GetDBManager()->Execute(command);
   }

   bool 
   PersistentGroupMember::ReadObject(std::shared_ptr<GroupMember> pObject, std::shared_ptr<DALRecordset> pRS)
   {
      pObject->SetID(pRS->GetInt64Value("memberid"));
      pObject->SetGroupID(pRS->GetInt64Value("membergroupid"));
      pObject->SetAccountID(pRS->GetInt64Value("memberaccountid"));

      return true;
   }

   bool 
   PersistentGroupMember::SaveObject(std::shared_ptr<GroupMember> pObject, String &errorMessage, PersistenceMode mode)
   {
      // errorMessage - not supported yet.
      return SaveObject(pObject);
   }

   bool 
   PersistentGroupMember::SaveObject(std::shared_ptr<GroupMember> pObject)
   {
      SQLStatement oStatement;

      oStatement.AddColumnInt64("membergroupid", pObject->GetGroupID());
      oStatement.AddColumnInt64("memberaccountid", pObject->GetAccountID());

      oStatement.SetTable("hm_group_members");


      if (pObject->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("memberid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);

         String sWhere;
         sWhere.Format(_T("memberid = %I64d"), pObject->GetID());

         oStatement.SetWhereClause(sWhere);

      }

      bool bNewObject = pObject->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pObject->SetID((long) iDBID);

      return bRetVal;     
   }
}