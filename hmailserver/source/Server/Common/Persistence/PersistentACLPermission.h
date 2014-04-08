// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ACLPermission;

   class PersistentACLPermission
   {
   public:
      PersistentACLPermission(void);
      ~PersistentACLPermission(void);
      
      static bool Validate(shared_ptr<ACLPermission> pObject);
      static bool DeleteOwnedByAccount(__int64 iAccountID);
      static bool DeleteOwnedByGroup(__int64 groupID);
      static bool DeleteObject(shared_ptr<ACLPermission> pObject);
      static bool SaveObject(shared_ptr<ACLPermission> pObject);
      static bool SaveObject(shared_ptr<ACLPermission> pObject, String &errorMessage);
      static bool ReadObject(shared_ptr<ACLPermission> pObject, shared_ptr<DALRecordset> pRS);

   };
}