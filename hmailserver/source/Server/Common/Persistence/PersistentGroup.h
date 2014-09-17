// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Group;
   enum PersistenceMode;

   class PersistentGroup
   {
   public:
      PersistentGroup(void);
      ~PersistentGroup(void);
      
      static bool DeleteObject(std::shared_ptr<Group> pObject);
      static bool SaveObject(std::shared_ptr<Group> pObject);
      static bool SaveObject(std::shared_ptr<Group> pObject, String &sErrorMessage, PersistenceMode mode);
      
      static bool ReadObject(std::shared_ptr<Group> pObject, std::shared_ptr<DALRecordset> pRS);
      static bool ReadObject(std::shared_ptr<Group> pAlias, const String & sName);
      static bool ReadObject(std::shared_ptr<Group> pGroup, __int64 ObjectID);
      static bool ReadObject(std::shared_ptr<Group> pGroup, const SQLCommand &command);
   };
}