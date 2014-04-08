// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Group;

   class PersistentGroup
   {
   public:
      PersistentGroup(void);
      ~PersistentGroup(void);
      
      static bool DeleteObject(shared_ptr<Group> pObject);
      static bool SaveObject(shared_ptr<Group> pObject);
      static bool SaveObject(shared_ptr<Group> pObject, String &sErrorMessage);
      
      static bool ReadObject(shared_ptr<Group> pObject, shared_ptr<DALRecordset> pRS);
      static bool ReadObject(shared_ptr<Group> pAlias, const String & sName);
      static bool ReadObject(shared_ptr<Group> pGroup, __int64 ObjectID);
      static bool ReadObject(shared_ptr<Group> pGroup, const SQLCommand &command);
   };
}