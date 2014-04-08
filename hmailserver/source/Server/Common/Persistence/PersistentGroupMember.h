// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class GroupMember;

   class PersistentGroupMember
   {
   public:
      PersistentGroupMember(void);
      ~PersistentGroupMember(void);
      
      static bool DeleteByAccount(__int64 iAccountID);
      static bool DeleteObject(shared_ptr<GroupMember> pObject);
      static bool SaveObject(shared_ptr<GroupMember> pObject);
      static bool SaveObject(shared_ptr<GroupMember> pObject, String &errorMessage);
      static bool ReadObject(shared_ptr<GroupMember> pObject, shared_ptr<DALRecordset> pRS);

   };
}