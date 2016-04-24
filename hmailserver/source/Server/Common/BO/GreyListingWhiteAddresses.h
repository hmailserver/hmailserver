// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "GreyListingWhiteAddress.h"
#include "../Persistence/PersistentGreyListingWhiteAddress.h"

namespace HM
{
   class GreyListingWhiteAddresses : public Collection<GreyListingWhiteAddress, PersistentGreyListingWhiteAddress>
   {
   public:
      GreyListingWhiteAddresses();
      ~GreyListingWhiteAddresses(void);

      // Refreshes this collection from the database.
      void Refresh();

   protected:
      virtual String GetCollectionName() const {return "GreyListingWhiteAddresses"; }

   private:
     
   };
}