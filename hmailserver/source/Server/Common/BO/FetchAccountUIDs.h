// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"
#include "FetchAccountUID.h"
#include "../Persistence/PersistentFetchAccountUID.h"

namespace HM
{
   class FetchAccountUID;

   class FetchAccountUIDs : public Collection<FetchAccountUID, PersistentFetchAccountUID>
   {
   public:
      FetchAccountUIDs(void);
      ~FetchAccountUIDs(void);

      void Refresh(__int64 iFAID);

      bool PreSaveObject(shared_ptr<FetchAccountUID> pUID, XNode *node);

   protected:
      virtual String GetCollectionName() const {return "FetchAccountUIDs"; }

   private:

      std::map<String, shared_ptr<FetchAccountUID> > _fetchedUIDs;

      __int64 m_iFAID;
   };
}
