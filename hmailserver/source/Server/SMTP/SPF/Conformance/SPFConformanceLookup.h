// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SPFConformanceSuite.h"

#include "../SPFDnsLookup.h"

namespace HM
{
   namespace SPFConformance
   {
      // Answers the DNS queries of an SPF evaluation out of one section of the
      // conformance suite, so that the cases in that section can be run without
      // reaching DNS. Each section has a zone of its own: the same host name
      // means different things in different sections.
      class ConformanceLookup : public SPFDnsLookup
      {
      public:

         explicit ConformanceLookup(const Section &section);

         virtual bool GetTXTRecords(const AnsiString &domain, std::vector<AnsiString> &records);
         virtual bool GetARecords(const AnsiString &host, std::vector<AnsiString> &addresses);
         virtual bool GetAAAARecords(const AnsiString &host, std::vector<AnsiString> &addresses);
         virtual bool GetMXRecords(const AnsiString &domain, std::vector<AnsiString> &hostNames);
         virtual bool GetPTRRecords(const AnsiString &reverseName, std::vector<AnsiString> &hostNames);

         // How many queries have been answered. RFC 7208 section 4.6.4 caps
         // how many an evaluation may make, and a case which runs away is
         // easier to recognise from the count than from its result.
         int GetQueryCount() const { return queryCount_; }

      private:

         bool Lookup_(const AnsiString &name, RecordType type, std::vector<AnsiString> &values);

         // Collects the values of the zone's own records of one type. Returns
         // false if the zone holds none, which is what decides whether a zone
         // marked as timing out answers or fails.
         bool Collect_(const Zone &zone, RecordType type, std::vector<AnsiString> &values) const;

         const Zone *FindZone_(const AnsiString &name) const;

         const Section &section_;
         int queryCount_;
      };
   }
}
