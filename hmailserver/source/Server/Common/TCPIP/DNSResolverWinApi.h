// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

#include "DNSRecord.h"

namespace HM
{


   class DNSResolverWinApi
   {
   public:
      DNSResolverWinApi();
      virtual ~DNSResolverWinApi();

      bool Query(const String &query, int resourceType, std::vector<DNSRecord> &foundRecords);

      static bool NameMatchesQuery(const String &query, const String &recordName);

   private:

      bool IsDNSError_(int iErrorMessage);
   };

   class DNSResolverWinApiTester
   {
   public:
      void Test();
   };

}
