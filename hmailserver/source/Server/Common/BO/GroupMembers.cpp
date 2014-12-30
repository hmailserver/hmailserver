// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "GroupMembers.h"

#include "../BO/Account.h"
#include "../Cache/CacheContainer.h"
#include "../Persistence/PersistentGroupMember.h"
#include "../Persistence/PersistentAccount.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   GroupMembers::GroupMembers(__int64 iGroupID) :
      group_id_(iGroupID)
   {

   }

   GroupMembers::~GroupMembers(void)
   {
   }

   void 
   GroupMembers::Refresh()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Reads all group members from the database
   //---------------------------------------------------------------------------()
   {
      String sSQL;
      sSQL.Format(_T("select * from hm_group_members where membergroupid = %I64d order by memberid asc"), group_id_);

      DBLoad_(sSQL);      
   }

   bool 
   GroupMembers::UserIsMember(__int64 iAccountID)
   {
      auto iter = vecObjects.begin();
      auto iterEnd = vecObjects.end();

      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<GroupMember> pMember = (*iter);

         if (pMember->GetAccountID() == iAccountID)
            return true;
      }

      return false;
   }

   void 
   GroupMembers::PostStoreObject(std::shared_ptr<GroupMember> pGroupMember, XNode *pChildNode)
   {
      std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(pGroupMember->GetAccountID());
      if (!pAccount)
         return;

      String sAddress = pAccount->GetAddress();

      pChildNode->AppendAttr(_T("Name"), sAddress);
   }

   bool
   GroupMembers::PreSaveObject(std::shared_ptr<GroupMember> pGroupMember, XNode *pNode)
   {
      String sAddress = pNode->GetAttrValue(_T("Name"));
      std::shared_ptr<const Account> pAccount = CacheContainer::Instance()->GetAccount(sAddress);

      if (!pAccount)
      {
         // The group member is connected to an account which no longer exists.
         return false;
      }

      pGroupMember->SetGroupID(group_id_);
      pGroupMember->SetAccountID(pAccount->GetID());

      return true;
   }

}