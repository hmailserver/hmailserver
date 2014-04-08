// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "../TCPIP/IPAddress.h"

namespace HM
{
   class SecurityRange;

   class PersistentSecurityRange
   {
   public:
	   PersistentSecurityRange();
	   virtual ~PersistentSecurityRange();

      static bool DeleteObject(shared_ptr<SecurityRange> pSR);
      static bool SaveObject(shared_ptr<SecurityRange> pSR);
      static bool SaveObject(shared_ptr<SecurityRange> pSR, String &result);

      static bool ReadObject(shared_ptr<SecurityRange> pSR, shared_ptr<DALRecordset> pRS);
      static bool ReadObject(shared_ptr<SecurityRange> pSR, const SQLCommand &command);
      static bool ReadObject(shared_ptr<SecurityRange> pSR, __int64 ObjectID);

      static shared_ptr<SecurityRange> ReadMatchingIP(const IPAddress &ipaddress);

      static bool DeleteExpired();

      static bool Exists(const String &name);

   private:

      static bool Validate(shared_ptr<SecurityRange> pSR, String &result);
   };

}
