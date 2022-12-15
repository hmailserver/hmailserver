// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPACLHelper.h"

#include "../Common/BO/Account.h"
#include "../Common/BO/ACLPermission.h"
#include "../Common/BO/ACLPermissions.h"
#include "../Common/BO/IMAPFolder.h"
#include "../Common/BO/IMAPFolders.h"
#include "../Common/Cache/CacheContainer.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   IMAPACLHelper::IMAPACLHelper()
   {

   }

   IMAPACLHelper::~IMAPACLHelper()
   {

   }

   String
   IMAPACLHelper::CreateACLList(std::shared_ptr<IMAPFolder> pFolder, const String &sEscapedFolderName)
   {
      // Retrieve a list of all rights for this folder.
      std::shared_ptr<ACLPermissions> pPermissions = pFolder->GetPermissions();

      String sACLList = "* ACL \"" + sEscapedFolderName + "\"";
      for (int i = 0; i < pPermissions->GetCount(); i++)
      {
         std::shared_ptr<ACLPermission> pPermission = pPermissions->GetItem(i);

         // Determine the name of this account
         __int64 iAccountID = pPermission->GetPermissionAccountID();

         std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(iAccountID);

         String sIdentifier = pAccount->GetAddress();
         String sRights = pPermission->GetRights();

         sACLList += " " + sIdentifier + " " + sRights;
      }

      return sACLList + "\r\n";
   }

   bool 
   IMAPACLHelper::IsValidPermissionString(const String &sPermissions)
   {
      for (int i = 0; i < sPermissions.GetLength(); i++)
      {
         wchar_t w = sPermissions.GetAt(i);

         if (w == '+' || w == '-')
            continue;

         if (ACLPermission::GetPermission(w) == ACLPermission::PermissionNone)
            return false;
      }

      return true;
   }




}
