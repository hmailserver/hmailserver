// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DMARCPolicyLocator.h"

#include "DMARCEvaluator.h"
#include "DMARCTxtLookup.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   DMARCPolicyLocator::DMARCPolicyLocator(std::shared_ptr<DMARCTxtLookup> lookup) :
      lookup_(lookup)
   {

   }

   DMARCPolicyLocator::Result
   DMARCPolicyLocator::Locate(const String &headerFromDomain, DMARCRecord &record, String &policyDomain)
   {
      policyDomain.Empty();

      if (headerFromDomain.IsEmpty())
         return Result::NoRecord;

      String domain = headerFromDomain;
      domain.ToLower();

      Result result = LocateAtDomain_(domain, record);

      if (result != Result::NoRecord)
      {
         if (result == Result::Found)
            policyDomain = domain;

         return result;
      }

      // No record at the domain itself. Try the organizational domain.
      String organizationalDomain = DMARCEvaluator::GetOrganizationalDomain(domain);

      if (organizationalDomain == domain)
         return Result::NoRecord;

      result = LocateAtDomain_(organizationalDomain, record);

      if (result == Result::Found)
         policyDomain = organizationalDomain;

      return result;
   }

   DMARCPolicyLocator::Result
   DMARCPolicyLocator::LocateAtDomain_(const String &domain, DMARCRecord &record)
   {
      std::vector<String> textRecords;

      if (!lookup_->GetTXTRecords("_dmarc." + domain, textRecords))
         return Result::TemporaryError;

      std::vector<AnsiString> dmarcRecords;

      for (String textRecord : textRecords)
      {
         AnsiString candidate = textRecord;

         if (DMARCRecord::HasVersionTag(candidate))
            dmarcRecords.push_back(candidate);
      }

      // More than one DMARC record means the domain has no usable policy.
      if (dmarcRecords.size() != 1)
         return Result::NoRecord;

      if (!DMARCRecord::Parse(dmarcRecords[0], record))
         return Result::NoRecord;

      return Result::Found;
   }
}
