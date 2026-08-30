// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "DMARCRecord.h"

namespace HM
{
   // Identifier alignment and policy selection. See RFC 7489, sections 3.1 and 6.6.3.
   class DMARCEvaluator
   {
   public:

      // The domain the DMARC policy is published for, for example example.com for
      // mail.example.com. Falls back to the domain itself when it can't be determined.
      static String GetOrganizationalDomain(const String &domain);

      // True if an SPF- or DKIM-authenticated domain aligns with the From header domain.
      static bool IsAligned(const String &authenticatedDomain, const String &headerFromDomain,
                            DMARCRecord::Alignment alignment);

      // The subdomain policy applies when the record was found at the organizational
      // domain rather than at the domain in the From header.
      static DMARCRecord::Policy GetApplicablePolicy(const DMARCRecord &record,
                                                     const String &headerFromDomain,
                                                     const String &policyDomain);
   };
}
