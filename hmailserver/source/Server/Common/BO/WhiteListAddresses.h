// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "WhiteListAddress.h"
#include "../Persistence/PersistentWhiteListAddress.h"

namespace HM
{
   class WhiteListAddresses : public Collection<WhiteListAddress, PersistentWhiteListAddress>
   {
   public:
      WhiteListAddresses();
      ~WhiteListAddresses(void);

      // Refreshes this collection from the database.
      void Refresh();

   protected:
   
      virtual String GetCollectionName() const {return "WhiteListAddresses"; }

   private:
     
   };
}