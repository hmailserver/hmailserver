// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "GroupMembers.h"

#include "../BO/Account.h"
#include "../Cache/Cache.h"
#include "../Persistence/PersistentGroupMember.h"
#include "../Persistence/PersistentAccount.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   GroupMembers::GroupMembers(__int64 iGroupID) :
      m_iGroupID(iGroupID)
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
      sSQL.Format(_T("select * from hm_group_members where membergroupid = %I64d order by memberid asc"), m_iGroupID);

      _DBLoad(sSQL);      
   }

   bool 
   GroupMembers::UserIsMember(__int64 iAccountID)
   {
      vector<shared_ptr<GroupMember> >::iterator iter = vecObjects.begin();
      vector<shared_ptr<GroupMember> >::iterator iterEnd = vecObjects.end();

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<GroupMember> pMember = (*iter);

         if (pMember->GetAccountID() == iAccountID)
            return true;
      }

      return false;
   }

   void 
   GroupMembers::PostStoreObject(shared_ptr<GroupMember> pGroupMember, XNode *pChildNode)
   {
      shared_ptr<const Account> pAccount = Cache<Account, PersistentAccount>::Instance()->GetObject(pGroupMember->GetAccountID());
      if (!pAccount)
         return;

      String sAddress = pAccount->GetAddress();

      pChildNode->AppendAttr(_T("Name"), sAddress);
   }

   bool
   GroupMembers::PreSaveObject(shared_ptr<GroupMember> pGroupMember, XNode *pNode)
   {
      String sAddress = pNode->GetAttrValue(_T("Name"));
      shared_ptr<const Account> pAccount = Cache<Account, PersistentAccount>::Instance()->GetObject(sAddress);

      if (!pAccount)
      {
         // The group member is connected to an account which no longer exists.
         return false;
      }

      pGroupMember->SetGroupID(m_iGroupID);
      pGroupMember->SetAccountID(pAccount->GetID());

      return true;
   }

}