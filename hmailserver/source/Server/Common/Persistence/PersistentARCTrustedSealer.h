// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class ARCTrustedSealer;
   enum PersistenceMode;

   class PersistentARCTrustedSealer
   {
   public:
      static bool DeleteObject(std::shared_ptr<ARCTrustedSealer> pObject);
      static bool SaveObject(std::shared_ptr<ARCTrustedSealer> pObject, String &errorMessage, PersistenceMode mode);
      static bool SaveObject(std::shared_ptr<ARCTrustedSealer> pObject);
      static bool ReadObject(std::shared_ptr<ARCTrustedSealer> pObject, std::shared_ptr<DALRecordset> pRS);
   };
}
