// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class GreyListingWhiteAddress;

   class PersistentGreyListingWhiteAddress
   {
   public:
      PersistentGreyListingWhiteAddress(void);
      ~PersistentGreyListingWhiteAddress(void);
      
      static bool DeleteObject(shared_ptr<GreyListingWhiteAddress> pObject);
      static bool SaveObject(shared_ptr<GreyListingWhiteAddress> pObject, String &errorMessage);
      static bool SaveObject(shared_ptr<GreyListingWhiteAddress> pObject);
      static bool ReadObject(shared_ptr<GreyListingWhiteAddress> pObject, shared_ptr<DALRecordset> pRS);

      static bool IsSenderWhitelisted(const String &address);
   };
}