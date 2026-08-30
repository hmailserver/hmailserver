// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The TXT lookups DMARC needs. Abstract so that the policy lookup can be
   // tested without depending on DNS.
   class DMARCTxtLookup
   {
   public:

      virtual ~DMARCTxtLookup() {}

      // Returns false if the lookup failed. An empty result means no records exist.
      virtual bool GetTXTRecords(const String &domain, std::vector<String> &records) = 0;
   };

   class DMARCDnsTxtLookup : public DMARCTxtLookup
   {
   public:

      virtual bool GetTXTRecords(const String &domain, std::vector<String> &records);
   };
}
