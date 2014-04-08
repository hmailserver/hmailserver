// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class FetchAccountUID;

   class PersistentFetchAccountUID
   {
   public:
      PersistentFetchAccountUID(void);
      ~PersistentFetchAccountUID(void);

      static __int64 AddUID(__int64 iFAID, const String &sValue);
      static bool DeleteUID(__int64 iUIDID);
      
      static bool SaveObject(shared_ptr<FetchAccountUID> pUID, String &result);
      static bool DeleteObject(shared_ptr<FetchAccountUID> fetchAccountUID);
   };
}