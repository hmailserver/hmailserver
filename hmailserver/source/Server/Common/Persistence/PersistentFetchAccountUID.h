// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class FetchAccountUID;
   enum PersistenceMode;

   class PersistentFetchAccountUID
   {
   public:
      PersistentFetchAccountUID(void);
      ~PersistentFetchAccountUID(void);

      static __int64 AddUID(__int64 iFAID, const String &sValue);
      static bool DeleteUID(__int64 iUIDID);
      
      static bool SaveObject(std::shared_ptr<FetchAccountUID> pUID, String &result, PersistenceMode mode);
      static bool DeleteObject(std::shared_ptr<FetchAccountUID> fetchAccountUID);
   };
}