// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class WhiteListAddress;
   enum PersistenceMode;

   class PersistentWhiteListAddress
   {
   public:
      PersistentWhiteListAddress(void);
      ~PersistentWhiteListAddress(void);
      
      static bool DeleteObject(std::shared_ptr<WhiteListAddress> pObject);
      static bool SaveObject(std::shared_ptr<WhiteListAddress> pObject, String &errorMessage, PersistenceMode mode);
      static bool SaveObject(std::shared_ptr<WhiteListAddress> pObject);
      static bool ReadObject(std::shared_ptr<WhiteListAddress> pObject, std::shared_ptr<DALRecordset> pRS);
   };
}