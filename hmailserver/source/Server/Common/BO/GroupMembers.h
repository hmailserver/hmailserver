// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "../Persistence/PersistentGroupMember.h"

#include "GroupMember.h"

namespace HM
{
   class GroupMembers : public Collection<GroupMember, PersistentGroupMember>
   {
   public:
      GroupMembers(__int64 iGroupID);
      ~GroupMembers(void);

      // Refreshes this collection from the database.
      void Refresh();

      __int64 GetGroupID() {return m_iGroupID; }
      bool UserIsMember(__int64 iAccountID);

   protected:

      virtual String GetCollectionName() const {return "GroupMembers"; }

      void PostStoreObject(shared_ptr<GroupMember> pGroupMember, XNode *pChildNode);
      bool PreSaveObject(shared_ptr<GroupMember> pGroupMember, XNode *pNode);

   private:
 
      __int64 m_iGroupID;
   };
}