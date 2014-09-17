// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "ACLPermission.h"
#include "../BO/Groups.h"
#include "../BO/Account.h"
#include "../Cache/CacheContainer.h"
#include "../Application/ACLManager.h"
#include "../../IMAP/IMAPConfiguration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   ACLPermission::ACLPermission(void) :
      share_folder_id_(0),
      permission_type_(PTUser),
      permission_group_id_(0),
      permission_account_id_(0),
      value_(0)
   {
      
   }

   ACLPermission::~ACLPermission(void)
   {
   }

   void
   ACLPermission::GrantAll()
   {
      value_ = value_ | 2047;
   }

   __int64 
   ACLPermission::GetShareFolderID()
   {
      return share_folder_id_;
   }

   void 
   ACLPermission::SetShareFolderID(__int64 iNewVal)
   {
      share_folder_id_ = iNewVal;
   }

   __int64 
   ACLPermission::GetPermissionGroupID()
   {
      return permission_group_id_;
   }

   void 
   ACLPermission::SetPermissionGroupID(__int64 iNewVal)
   {
      permission_group_id_ = iNewVal;
   }

   __int64 
   ACLPermission::GetPermissionAccountID()
   {
      return permission_account_id_;
   }

   void 
   ACLPermission::SetPermissionAccountID(__int64 iNewVal)
   {
      permission_account_id_ = iNewVal;
   }

   ACLPermission::ePermissionType
   ACLPermission::GetPermissionType()
   {
      return permission_type_;
   }

   void 
   ACLPermission::SetPermissionType(ePermissionType iNewVal)
   {
      permission_type_ = iNewVal;
   }

   __int64 
   ACLPermission::GetValue()
   {
      return value_;
   }

   void 
   ACLPermission::SetValue(__int64 iNewVal)
   {
      value_ = iNewVal;
   }

   bool 
   ACLPermission::GetAllow(ACLPermission::ePermission p) const
   {
      return (value_ & p) ? true : false;
   }

   void 
   ACLPermission::SetAllow(ACLPermission::ePermission p, bool bValue)
   {
      if (bValue)
         value_ = value_ | p;
      else
         value_ = value_ & ~p;
   }

   String 
   ACLPermission::GetRights() const
   {
      String sRetVal;

      if (GetAllow(PermissionLookup))
         sRetVal += "l";
      if (GetAllow(PermissionRead))
         sRetVal += "r";
      if (GetAllow(PermissionWriteSeen))
         sRetVal += "s";
      if (GetAllow(PermissionWriteOthers))
         sRetVal += "w";
      if (GetAllow(PermissionInsert))
         sRetVal += "i";
      if (GetAllow(PermissionPost))
         sRetVal += "p";
      if (GetAllow(PermissionCreate))
         sRetVal += "k";
      if (GetAllow(PermissionDeleteMailbox))
         sRetVal += "x";
      if (GetAllow(PermissionWriteDeleted))
         sRetVal += "t";
      if (GetAllow(PermissionExpunge))
         sRetVal += "e";
      if (GetAllow(PermissionAdminister))
         sRetVal += "a";

      return sRetVal;
   }

   ACLPermission::ePermission 
   ACLPermission::GetPermission(wchar_t w)
   {
      switch (w)
      {
      case 'l':
         return PermissionLookup;
      case 'r':
         return PermissionRead;
      case 's':
         return PermissionWriteSeen;
      case 'w':
         return PermissionWriteOthers;
      case 'i':
         return PermissionInsert;
      case 'p':
         return PermissionPost;
      case 'k':
         return PermissionCreate;
      case 'x':
         return PermissionDeleteMailbox;
      case 't':
         return PermissionWriteDeleted;
      case 'e':
         return PermissionExpunge;
      case 'a':
         return PermissionAdminister;
      }

      return PermissionNone;
   }

   void 
   ACLPermission::AppendPermissions(const String &sRights)
   {
      int iAction = 0; // 1 - append, 2 = subtract, 3 - replace

      if (sRights.GetAt(0) == '+')
         iAction = 1;
      else if (sRights.GetAt(0) == '-')
         iAction = 2;
      else
         iAction = 3;

      if (iAction == 3)
      {
         // Reset any existing right.
         SetValue(0);

         // And now append instead
         iAction = 1;
      }

      for (int i = 0; i < sRights.GetLength(); i++)
      {
         wchar_t sChar = sRights.GetAt(i);

         if (sChar == '+' || sChar == '-')
            continue;
         
         ePermission p = GetPermission(sChar);

         if (iAction == 1)
            SetAllow(p, true);
         else
            SetAllow(p, false);


      }
   }

   bool 
   ACLPermission::XMLStore(XNode *pParentNode, int iOptions)
   {
      XNode *pNode = pParentNode->AppendChild(_T("Permission"));

      pNode->AppendAttr(_T("Type"), StringParser::IntToString(permission_type_));
      pNode->AppendAttr(_T("Rights"), StringParser::IntToString(value_));
      pNode->AppendAttr(_T("Holder"), GetPermissionHolderName_(this));

      return true;
   }

   bool
   ACLPermission::XMLLoad(XNode *pNode, int iOptions)
   {
      permission_type_ = (ePermissionType) _ttoi(pNode->GetAttrValue(_T("Type")));
      value_ = _ttoi(pNode->GetAttrValue(_T("Rights")));

      String sPermissionHolder = pNode->GetAttrValue(_T("Holder"));

      switch (permission_type_)
      {
      case ACLPermission::PTGroup:
         {
            std::shared_ptr<Group> pGroup = Configuration::Instance()->GetIMAPConfiguration()->GetGroups()->GetItemByName(sPermissionHolder);

            if (pGroup)
            {
               SetPermissionGroupID(pGroup->GetID());
            }

            break;
         }
      case ACLPermission::PTUser:
         {
            std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(sPermissionHolder);

            if (pAccount)
            {
               SetPermissionAccountID(pAccount->GetID());
            }

            break;
         }
      }


      return true;

   }

   String
   ACLPermission::GetPermissionHolderName_(ACLPermission *pPermission)  const
   {
      switch (pPermission->GetPermissionType())
      {
      case ACLPermission::PTGroup:
         {
            std::shared_ptr<Group> pGroup = Configuration::Instance()->GetIMAPConfiguration()->GetGroups()->GetItemByDBID(pPermission->GetPermissionGroupID());
            return pGroup->GetName();
         }
      case ACLPermission::PTUser:
         {
            std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(pPermission->GetPermissionAccountID());
            return pAccount->GetAddress();
         }
      case ACLPermission::PTAnyone:
         {
            return "Anyone";
         }
      }

      return "Unknown";
   }

}