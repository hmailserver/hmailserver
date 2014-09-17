// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class GroupMember;
   enum PersistenceMode;

   class PersistentGroupMember
   {
   public:
      PersistentGroupMember(void);
      ~PersistentGroupMember(void);
      
      static bool DeleteByAccount(__int64 iAccountID);
      static bool DeleteObject(std::shared_ptr<GroupMember> pObject);
      static bool SaveObject(std::shared_ptr<GroupMember> pObject);
      static bool SaveObject(std::shared_ptr<GroupMember> pObject, String &errorMessage, PersistenceMode mode);
      static bool ReadObject(std::shared_ptr<GroupMember> pObject, std::shared_ptr<DALRecordset> pRS);

   };
}