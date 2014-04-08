// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class WhiteListAddress;

   class PersistentWhiteListAddress
   {
   public:
      PersistentWhiteListAddress(void);
      ~PersistentWhiteListAddress(void);
      
      static bool DeleteObject(shared_ptr<WhiteListAddress> pObject);
      static bool SaveObject(shared_ptr<WhiteListAddress> pObject, String &errorMessage);
      static bool SaveObject(shared_ptr<WhiteListAddress> pObject);
      static bool ReadObject(shared_ptr<WhiteListAddress> pObject, shared_ptr<DALRecordset> pRS);
   };
}