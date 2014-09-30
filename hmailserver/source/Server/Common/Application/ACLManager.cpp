// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ACLManager.h"

#include "../Cache/CacheContainer.h"
#include "../BO/IMAPFolders.h"
#include "../BO/ACLPermissions.h"
#include "../BO/Groups.h"
#include "../BO/Account.h"
#include "../../IMAP/IMAPConfiguration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif


namespace HM
{
   ACLManager::ACLManager(void)
   {

   }

   ACLManager::~ACLManager(void)
   {

   }

   // Sorts a list of ACL permissions based on their ID. Low ID will come before High ID.
   class ACLSortID {
   public:
      //Return true if s1 < s2; otherwise, return false.
      bool operator()(const std::shared_ptr<ACLPermission> p1, const std::shared_ptr<ACLPermission>  p2)
      {
         return p1->GetID() < p2->GetID();
      }

   private:
   };   

   /* 
      Example of folder structure:

      Folder A <- Shared
         Folder A1 <- Inherited
         Folder A2 <- Inherited
         Folder A3 <- Shared with separate permissions
      Folder B
         Folder B1 <- Shared
         Folder B2
   */


   std::shared_ptr<ACLPermission> 
   ACLManager::GetPermissionForFolder(__int64 iAccountID, std::shared_ptr<IMAPFolder> pFolder)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Input:
   // iAccountID - The account which wants access to the folder
   // pFolder    - The folder the account wants access to
   //---------------------------------------------------------------------------()
   {
      if (pFolder->GetAccountID() == iAccountID)
      {
         // Folder is owned by requester. Full access.
         std::shared_ptr<ACLPermission> pFullPermissions = std::shared_ptr<ACLPermission>(new ACLPermission);
         pFullPermissions->GrantAll();
         return pFullPermissions;         
      }


      std::shared_ptr<IMAPFolders> pPublicFolders = Configuration::Instance()->GetIMAPConfiguration()->GetPublicFolders();

      // The user is trying to access a public folder. Determine the permissions for this one.
      // We have a list containing Folder A and Folder B1. Since not all folders may have permissions
      // we need to locate a parent folder in the structure which has a permission and then
      // inherit that one.
      
      std::shared_ptr<IMAPFolder> pCheckFolder = pFolder;

      int maxRecursions = 250;
      while (pCheckFolder && maxRecursions > 0)
      {
         maxRecursions--;

         // Check if permissions is set for this folder. If it is, we need to check
         // if we have permissions to it.
         std::shared_ptr<ACLPermissions> pPermissions = pCheckFolder->GetPermissions();

         if (pPermissions && pPermissions->GetCount() > 0)
         {
            // We found permissions for this folder. Locate the permission for the given user.
            std::shared_ptr<ACLPermission> pPermission = GetPermissionForAccount_(pPermissions, iAccountID);

            return pPermission;
         }

         // Locate parent folder.
         __int64 iParentFolderID = pCheckFolder->GetParentFolderID();

         pCheckFolder = pPublicFolders->GetItemByDBIDRecursive(iParentFolderID);
      }

      std::shared_ptr<ACLPermission> pNoPermission;
      return pNoPermission;

   }
   
   std::shared_ptr<ACLPermission> 
   ACLManager::GetPermissionForAccount_(std::shared_ptr<ACLPermissions> pPermissions, __int64 iAccountID)
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Goes through the list of permissions (typically a list of permissions connected
   // to a specific IMAP folder) and returns the "best" permission which matches
   // the account ID. If the user has been given specific rights, these are used.
   // If not, we check if the user is a member of a group and if so uses the
   // rights for this group.
   // If not, we check whether "anyone" has been given permission to the folder. In that
   // case, we use those.
   //---------------------------------------------------------------------------()
   {
      std::vector<std::shared_ptr<ACLPermission> > vecObjects = pPermissions->GetVector();

      // Sort the list of permissions. If a user is a member of two groups set up in
      // separate ACL records, we should use the permissions from the last record.
      std::sort(vecObjects.begin(), vecObjects.end(), ACLSortID());

      auto iter = vecObjects.begin();
      auto iterEnd = vecObjects.end();

      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<ACLPermission> pPermission = (*iter);

         if (pPermission->GetPermissionType() == 0 && pPermission->GetPermissionAccountID() == iAccountID)
            return pPermission;
      }

      // Check which groups have been given access.
      iter = vecObjects.begin();
      iterEnd = vecObjects.end();

      std::list<std::pair<__int64, std::shared_ptr<ACLPermission> > > listGroupPermissions;
      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<ACLPermission> pPermission = (*iter);

         if (pPermission->GetPermissionType() == 1)
         {
            listGroupPermissions.push_back(std::make_pair(pPermission->GetPermissionGroupID(), pPermission));
         }
      }

      // Check if user is member of any of these groups.
      auto iterGroup = listGroupPermissions.begin();
      auto iterGroupEnd = listGroupPermissions.end();

      for (; iterGroup != iterGroupEnd; iterGroup++)
      {
         __int64 iGroupID = (*iterGroup).first;
         std::shared_ptr<ACLPermission> pPermission = (*iterGroup).second;

         // Fetch this group
         std::shared_ptr<Group> pGroup = Configuration::Instance()->GetIMAPConfiguration()->GetGroups()->GetItemByDBID(iGroupID);

         if (!pGroup)
         {
            String sMessage;
            sMessage.Format(_T("The group referenced by ACL ID %I64d (Group ID %I64d, Folder ID %I64d) does not exist. "), 
               pPermission->GetID(), iGroupID, pPermission->GetShareFolderID());

            ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5002, "ACLManager::GetPermissionForAccount_", sMessage);

            continue;
         }

         // Check if the user is a member of this group
         if (pGroup->UserIsMember(iAccountID))
            return pPermission;
      }

      // Check if anyone has been given permission to this group
      iter = vecObjects.begin();
      iterEnd = vecObjects.end();

      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<ACLPermission> pPermission = (*iter);

         if (pPermission->GetPermissionType() == ACLPermission::PTAnyone)
            return pPermission;
      }


      std::shared_ptr<ACLPermission> pEmpty;
      return pEmpty;
   }

   bool 
   ACLManager::SetACL(std::shared_ptr<IMAPFolder> pFolder, const String& sIdentifier, const String &sPermissions)
   {
      std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(sIdentifier);
      std::shared_ptr<Group> pGroup;

      if (!pAccount)
      {
         // No account was found. Check if it's a group.
         std::shared_ptr<Group> pGroup = Configuration::Instance()->GetIMAPConfiguration()->GetGroups()->GetItemByName(sIdentifier);

         if (!pGroup)
         {
            // Identifier was not found.
            return false;
         }
      }

      if (pAccount && pAccount->GetID() == pFolder->GetAccountID())
      {
         // Should we ever come here? A user should not be able to modify
         // his own right on his own folder.
         assert(0);       
         return false;
      }

      std::shared_ptr<ACLPermissions> pFolderPermissions = pFolder->GetPermissions();
      
      std::shared_ptr<ACLPermission> pPermission;
      
      if (pAccount)
         pPermission = pFolderPermissions->GetPermissionForAccount(pAccount->GetID());
      else
         pPermission = pFolderPermissions->GetPermissionForGroup(pGroup->GetID());

      if (!pPermission)
      {
         pPermission = std::shared_ptr<ACLPermission>(new ACLPermission);

         pPermission->SetShareFolderID(pFolder->GetID());

         if (pAccount)
         {
            pPermission->SetPermissionType(ACLPermission::PTUser);
            pPermission->SetPermissionAccountID(pAccount->GetID());
         }
         else if (pGroup)
         {
            pPermission->SetPermissionType(ACLPermission::PTGroup);
            pPermission->SetPermissionGroupID(pGroup->GetID());
         }
         else
            return false;

      }

      pPermission->AppendPermissions(sPermissions);

      if (!PersistentACLPermission::SaveObject(pPermission))
         return false;
      
      return true;
   }


  


}