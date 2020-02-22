// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

#include "../Persistence/PersistentDomainAlias.h"
#include "DomainAlias.h"

namespace HM
{
   class DomainAliases : public Collection<DomainAlias, PersistentDomainAlias> 
   {
   public:
      DomainAliases(__int64 iDomainID);
      ~DomainAliases(void);

      void Refresh();

      String ApplyAliasesOnAddress(const String &sAddress);
      __int64 GetDomainID() {return domain_id_; }

   protected:
      virtual bool PreSaveObject(std::shared_ptr<DomainAlias> pDA, XNode *node);
      virtual String GetCollectionName() const {return "DomainAliases"; }
   private:

      __int64 domain_id_;
   };
}