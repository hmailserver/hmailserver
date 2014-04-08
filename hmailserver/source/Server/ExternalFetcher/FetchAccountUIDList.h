// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../Common/BO/FetchAccountUID.h"
#include "../Common/Persistence/PersistentFetchAccountUID.h"

namespace HM
{
   class FetchAccountUIDList
   {
   public:
      FetchAccountUIDList(void);
      ~FetchAccountUIDList(void);

      void Refresh(__int64 iFAID);

      bool IsUIDInList(const String&sUID) const;
      void DeleteUID(const String &sUID);
      void DeleteUIDsNotInSet(set<String> &vecUIDs);
      void AddUID(const String &sUIDValue);

      shared_ptr<FetchAccountUID> GetUID(const String &sUID);
   private:

      std::map<String, shared_ptr<FetchAccountUID> > _fetchedUIDs;

      __int64 m_iFAID;
   };
}
