// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "DMARCRecord.h"

namespace HM
{
   class DMARCTxtLookup;

   // Locates the DMARC policy for a domain, falling back to the organizational
   // domain when the domain itself doesn't publish one. See RFC 7489, section 6.6.3.
   class DMARCPolicyLocator
   {
   public:

      enum class Result
      {
         NoRecord = 0,
         Found = 1,
         TemporaryError = 2
      };

      DMARCPolicyLocator(std::shared_ptr<DMARCTxtLookup> lookup);

      Result Locate(const String &headerFromDomain, DMARCRecord &record, String &policyDomain);

   private:

      Result LocateAtDomain_(const String &domain, DMARCRecord &record);

      std::shared_ptr<DMARCTxtLookup> lookup_;
   };
}
