// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../BO/DistributionListRecipient.h"

namespace HM
{  
   enum PersistenceMode;

   class PersistentDistributionListRecipient
   {
   public:
      PersistentDistributionListRecipient(void);
      ~PersistentDistributionListRecipient(void);

      static bool DeleteByListID(__int64 iListID);
      static bool DeleteObject(std::shared_ptr<DistributionListRecipient> pRecipient);
      
      static bool SaveObject(std::shared_ptr<DistributionListRecipient> pRecipient);
      static bool SaveObject(std::shared_ptr<DistributionListRecipient> pRecipient, String &sErrorMessage, PersistenceMode mode);

      static bool ReadObject(std::shared_ptr<DistributionListRecipient> pRecipient, long ObjectID);
      static bool ReadObject(std::shared_ptr<DistributionListRecipient> pRecipient, const SQLCommand &command);
      static bool ReadObject(std::shared_ptr<DistributionListRecipient> pRecipient, std::shared_ptr<DALRecordset> pRS);

   };
}
