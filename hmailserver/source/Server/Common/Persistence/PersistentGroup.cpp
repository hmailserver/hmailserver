// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "PersistentGroup.h"
#include "PersistentACLPermission.h"

#include "PreSaveLimitationsCheck.h"

#include "..\BO\Group.h"
#include "..\SQL\SQLStatement.h"
#include "../Cache/Cache.h"

#include "PersistenceMode.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentGroup::PersistentGroup(void)
   {
   }

   PersistentGroup::~PersistentGroup(void)
   {
   }

   bool
   PersistentGroup::DeleteObject(std::shared_ptr<Group> pObject)
   {
      SQLCommand command("delete from hm_groups where groupid = @GROUPID");
      command.AddParameter("@GROUPID", pObject->GetID());

      bool bResult = Application::Instance()->GetDBManager()->Execute(command);

      PersistentACLPermission::DeleteOwnedByGroup(pObject->GetID());

      return bResult;
   }

   bool 
   PersistentGroup::ReadObject(std::shared_ptr<Group> pObject, std::shared_ptr<DALRecordset> pRS)
   {
      pObject->SetID(pRS->GetInt64Value("groupid"));
      pObject->SetName(pRS->GetStringValue("groupname"));

      return true;
   }

   bool
   PersistentGroup::ReadObject(std::shared_ptr<Group> pGroup, const String & sName)
   {
      SQLStatement statement;

      statement.SetStatementType(SQLStatement::STSelect);
      statement.SetTable("hm_groups");
      statement.AddWhereClauseColumn("groupname", sName);

      return ReadObject(pGroup, statement.GetCommand());
   }

   bool
   PersistentGroup::ReadObject(std::shared_ptr<Group> pGroup, __int64 ObjectID)
   {
      SQLCommand command("select * from hm_groups where groupid = @GROUPID");
      command.AddParameter("@GROUPID", ObjectID);

      return ReadObject(pGroup, command);

   }

   bool
   PersistentGroup::ReadObject(std::shared_ptr<Group> pGroup, const SQLCommand &command)
   {
      std::shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return false;

      bool bRetVal = false;
      if (!pRS->IsEOF())
      {
         bRetVal = ReadObject(pGroup, pRS);
      }

      return bRetVal;
   }

   bool 
   PersistentGroup::SaveObject(std::shared_ptr<Group> pGroup)
   {
      String sErrorMessage;
      return SaveObject(pGroup, sErrorMessage, PersistenceModeNormal);
   }

   bool 
   PersistentGroup::SaveObject(std::shared_ptr<Group> pGroup, String &sErrorMessage, PersistenceMode mode)
   {
      if (!PreSaveLimitationsCheck::CheckLimitations(mode, pGroup, sErrorMessage))
         return false;

      SQLStatement oStatement;

      oStatement.AddColumn("groupname", pGroup->GetName());

      oStatement.SetTable("hm_groups");


      if (pGroup->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("groupid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);

         String sWhere;
         sWhere.Format(_T("groupid = %I64d"), pGroup->GetID());

         oStatement.SetWhereClause(sWhere);

      }

      bool bNewObject = pGroup->GetID() == 0;

      // Save and fetch ID
      __int64 iDBID = 0;
      bool bRetVal = Application::Instance()->GetDBManager()->Execute(oStatement, bNewObject ? &iDBID : 0);
      if (bRetVal && bNewObject)
         pGroup->SetID((long) iDBID);

      return bRetVal;      
   }
}