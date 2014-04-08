// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "../Persistence/PersistentSecurityRange.h"
#include "../BO/SecurityRange.h"

namespace HM
{
   class SecurityRanges : public Collection<SecurityRange, PersistentSecurityRange> 
   {
   public:
	   SecurityRanges();
	   virtual ~SecurityRanges();

      void Refresh();

      void SetDefault();

   protected:
      virtual String GetCollectionName() const {return "SecurityRanges"; } 
   private:

   };

}