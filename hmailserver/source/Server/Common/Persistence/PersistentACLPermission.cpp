// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "PersistentACLPermission.h"

#include "..\Application\ACLManager.h"
#include "..\BO\ACLPermission.h"
#include "..\BO\ACLPermissions.h"
#include "..\SQL\SQLStatement.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   PersistentACLPermission::PersistentACLPermission(void)
   {
   }

   PersistentACLPermission::~PersistentACLPermission(void)
   {
   }

   bool
   PersistentACLPermission::DeleteObject(std::shared_ptr<ACLPermission> pObject)
   {
      SQLCommand command("delete from hm_acl where aclid = @ACLID");
      command.AddParameter("@ACLID", pObject->GetID());

      bool bRetVal = Application::Instance()->GetDBManager()->Execute(command);

      return bRetVal;
   }

   bool
   PersistentACLPermission::DeleteOwnedByAccount(__int64 iAccountID)
   {
      SQLCommand command("delete from hm_acl where aclpermissiontype = 0 and aclpermissionaccountid = @ACCOUNTID");
      command.AddParameter("@ACCOUNTID", iAccountID);

      bool bRetVal = Application::Instance()->GetDBManager()->Execute(command);

      return bRetVal;
   }

   bool
   PersistentACLPermission::DeleteOwnedByGroup(__int64 groupID)
   {
      SQLCommand command("delete from hm_acl where aclpermissiontype = 1 and aclpermissiongroupid = @GROUPID");
      command.AddParameter("@GROUPID", groupID);

      bool bRetVal = Application::Instance()->GetDBManager()->Execute(command);

      return bRetVal;
   }

   bool 
   PersistentACLPermission::ReadObject(std::shared_ptr<ACLPermission> pObject, std::shared_ptr<DALRecordset> pRS)
   {
      pObject->SetID(pRS->GetInt64Value("aclid"));
      pObject->SetShareFolderID(pRS->GetInt64Value("aclsharefolderid"));
            
      pObject->SetPermissionType((ACLPermission::ePermissionType) pRS->GetLongValue("aclpermissiontype"));
      pObject->SetPermissionGroupID(pRS->GetInt64Value("aclpermissiongroupid"));
      pObject->SetPermissionAccountID(pRS->GetInt64Value("aclpermissionaccountid"));
      
      pObject->SetValue(pRS->GetInt64Value("aclvalue"));

      return true;
   }

   bool
   PersistentACLPermission::Validate(std::shared_ptr<ACLPermission> pObject)
   {
      // Do some sanity checking
      switch (pObject->GetPermissionType())
      {
      case ACLPermission::PTUser:
         {
            if (pObject->GetPermissionAccountID() == 0 || pObject->GetPermissionGroupID() != 0)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5119, "PersistentACLPermission::SaveObject", "ACL item of type PTUser not initialized correctly.");
               return false;
            }
         }
         break;
      case ACLPermission::PTGroup:
         {
            if (pObject->GetPermissionAccountID() != 0 || pObject->GetPermissionGroupID() == 0)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5120, "PersistentACLPermission::SaveObject", "ACL item of type PTGroup not initialized correctly.");
               return false;
            }
         }
         break;
      case ACLPermission::PTAnyone:
         {
            if (pObject->GetPermissionAccountID() != 0 || pObject->GetPermissionGroupID() != 0)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5121, "PersistentACLPermission::SaveObject", "ACL item of type PTAnyone not initialized correctly.");
               return false;
            }
         }
         break;
      }

      return true;
   }

   bool 
   PersistentACLPermission::SaveObject(std::shared_ptr<ACLPermission> pObject, String &errorMessage, PersistenceMode mode)
   {
      return SaveObject(pObject);
   }

   bool 
   PersistentACLPermission::SaveObject(std::shared_ptr<ACLPermission> pObject)
   {
      if (!Validate(pObject))
         return false;

      SQLStatement oStatement;

      oStatement.AddColumnInt64("aclsharefolderid", pObject->GetShareFolderID());
      
      oStatement.AddColumn("aclpermissiontype", pObject->GetPermissionType());
      
      oStatement.AddColumnInt64("aclpermissiongroupid", pObject->GetPermissionGroupID());
      oStatement.AddColumnInt64("aclpermissionaccountid", pObject->GetPermissionAccountID());

      oStatement.AddColumnInt64("aclvalue", pObject->GetValue());

      oStatement.SetTable("hm_acl");


      if (pObject->GetID() == 0)
      {
         oStatement.SetStatementType(SQLStatement::STInsert);
         oStatement.SetIdentityColumn("aclid");
      }
      else
      {
         oStatement.SetStatementType(SQLStatement::STUpdate);

         String sWhere;
         sWhere.Format(_T("aclid = %I64d"), pObject->GetID());

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