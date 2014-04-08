// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "../Persistence/PersistentDomain.h"
#include "Domain.h"

namespace HM
{
   class Domains : public Collection<Domain, PersistentDomain>
   {
   public:
	   Domains();
	   virtual ~Domains();

      void Refresh();
      void Refresh(__int64 iDomainID);
      String GetNames();

   protected:
      virtual String GetCollectionName() const {return "Domains"; }
   private:

   };
}
