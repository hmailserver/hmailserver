// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "Collection.h"

namespace HM
{
   class DistributionListRecipient;
   class PersistentDistributionListRecipient;

   class DistributionListRecipients : public Collection<DistributionListRecipient, PersistentDistributionListRecipient>
   {
   public:
      DistributionListRecipients(__int64 iListID);
      ~DistributionListRecipients(void);

      void Refresh();

   protected:

      virtual String GetCollectionName() const {return "DistributionList"; }
      virtual bool PreSaveObject(std::shared_ptr<DistributionListRecipient> pListRecipient, XNode *node);

   private:

      __int64 list_id_;
   };
}
