// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "SURBLServer.h"
#include "../Persistence/PersistentSURBLServer.h"

namespace HM
{

   class SURBLServers : public Collection<SURBLServer, PersistentSURBLServer>
   {
   public:
      SURBLServers();
      ~SURBLServers(void);

      // Refreshes this collection from the database.
      void Refresh();

   protected:
      virtual String GetCollectionName() const {return String("SURBLServers"); }
   private:
     
   };
}