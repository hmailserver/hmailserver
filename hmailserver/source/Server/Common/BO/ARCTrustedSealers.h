// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "ARCTrustedSealer.h"
#include "../Persistence/PersistentARCTrustedSealer.h"

namespace HM
{
   class ARCTrustedSealers : public Collection<ARCTrustedSealer, PersistentARCTrustedSealer>
   {
   public:
      ARCTrustedSealers();
      ~ARCTrustedSealers(void);

      // Refreshes this collection from the database.
      void Refresh();

      // True if the domain is in the collection. An exact match: subdomains of a
      // trusted domain are not trusted.
      bool IsTrusted(const String &domain) const;

   protected:
      virtual String GetCollectionName() const {return "ARCTrustedSealers"; }
   };
}
