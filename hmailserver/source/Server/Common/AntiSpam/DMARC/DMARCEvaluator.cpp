// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "StdAfx.h"

#include "DMARCEvaluator.h"

#include "../../Util/TLD.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String
   DMARCEvaluator::GetOrganizationalDomain(const String &domain)
   {
      String organizationalDomain = domain;
      organizationalDomain.ToLower();

      bool isIPAddress = false;

      if (!TLD::Instance()->GetDomainNameFromHost(organizationalDomain, isIPAddress) || isIPAddress)
      {
         organizationalDomain = domain;
         organizationalDomain.ToLower();
      }

      return organizationalDomain;
   }

   bool
   DMARCEvaluator::IsAligned(const String &authenticatedDomain, const String &headerFromDomain,
                             DMARCRecord::Alignment alignment)
   {
      if (authenticatedDomain.IsEmpty() || headerFromDomain.IsEmpty())
         return false;

      if (authenticatedDomain.CompareNoCase(headerFromDomain) == 0)
         return true;

      if (alignment == DMARCRecord::Alignment::Strict)
         return false;

      return GetOrganizationalDomain(authenticatedDomain) == GetOrganizationalDomain(headerFromDomain);
   }

   DMARCRecord::Policy
   DMARCEvaluator::GetApplicablePolicy(const DMARCRecord &record, const String &headerFromDomain,
                                       const String &policyDomain)
   {
      if (record.GetHasSubdomainPolicy() && headerFromDomain.CompareNoCase(policyDomain) != 0)
         return record.GetSubdomainPolicy();

      return record.GetPolicy();
   }
}
