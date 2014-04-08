// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ACLPermissions.h"
#include "../Application/ACLManager.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ACLPermissions::ACLPermissions() :
      m_iFolderID(0)
   {

   }

      ACLPermissions::ACLPermissions(__int64 iFolderID) :
      m_iFolderID(iFolderID)
   {

   }

   ACLPermissions::~ACLPermissions(void)
   {
   }

   void 
   ACLPermissions::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all groups form the database
   //---------------------------------------------------------------------------()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_acl where aclsharefolderid = %I64d"), m_iFolderID);

      _DBLoad(sSQL);
   }

   shared_ptr<ACLPermission> 
   ACLPermissions::GetPermissionForAccount(__int64 iAccountID)
   {
      std::vector<shared_ptr<ACLPermission> >::iterator iter = vecObjects.begin();
      std::vector<shared_ptr<ACLPermission> >::iterator iterEnd = vecObjects.end();

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<ACLPermission> pPermission = (*iter);
         if (pPermission->GetPermissionType() == 0 && pPermission->GetPermissionAccountID() == iAccountID)
         {
            return pPermission;
         }
      }

      shared_ptr<ACLPermission> pEmpty;
      return pEmpty;
   }

   shared_ptr<ACLPermission> 
   ACLPermissions::GetPermissionForGroup(__int64 iGroupID)
   {
      std::vector<shared_ptr<ACLPermission> >::iterator iter = vecObjects.begin();
      std::vector<shared_ptr<ACLPermission> >::iterator iterEnd = vecObjects.end();

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<ACLPermission> pPermission = (*iter);
         if (pPermission->GetPermissionType() == ACLPermission::PTGroup && pPermission->GetPermissionGroupID() == iGroupID)
         {
            return pPermission;
         }
      }

      shared_ptr<ACLPermission> pEmpty;
      return pEmpty;
   }

   // Called before save in DB
   bool
   ACLPermissions::PreSaveObject(shared_ptr<ACLPermission> pPermission, XNode *pNode)
   {
      pPermission->SetShareFolderID(m_iFolderID);

      return true;
   }

   bool 
   ACLPermissions::DeletePermissionsForAccount(__int64 iAccountID)
   {
      std::vector<shared_ptr<ACLPermission> >::iterator iter = vecObjects.begin();

      while (iter != vecObjects.end())
      {
         shared_ptr<ACLPermission> pPermission = (*iter);
         if (pPermission->GetPermissionType() == 0 && pPermission->GetPermissionAccountID() == iAccountID)
         {
            if (!PersistentACLPermission::DeleteObject(pPermission))
               return false;

            iter = vecObjects.erase(iter);
         }
         else
            iter++;
      }

      return true;
   }
}