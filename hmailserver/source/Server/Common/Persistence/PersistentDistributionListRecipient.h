// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../BO/DistributionListRecipient.h"

namespace HM
{
   class PersistentDistributionListRecipient
   {
   public:
      PersistentDistributionListRecipient(void);
      ~PersistentDistributionListRecipient(void);

      static bool DeleteByListID(__int64 iListID);
      static bool DeleteObject(shared_ptr<DistributionListRecipient> pRecipient);
      
      static bool SaveObject(shared_ptr<DistributionListRecipient> pRecipient);
      static bool SaveObject(shared_ptr<DistributionListRecipient> pRecipient, String &sErrorMessage);

      static bool ReadObject(shared_ptr<DistributionListRecipient> pRecipient, long ObjectID);
      static bool ReadObject(shared_ptr<DistributionListRecipient> pRecipient, const SQLCommand &command);
      static bool ReadObject(shared_ptr<DistributionListRecipient> pRecipient, shared_ptr<DALRecordset> pRS);

   };
}
