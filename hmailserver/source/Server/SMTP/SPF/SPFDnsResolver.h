// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SPFDnsLookup.h"

namespace HM
{
   // The SPFDnsLookup an evaluation uses in the server: real DNS, through
   // DNSResolver.
   //
   // Every query is asked for by type through DNSResolver::GetRecordsOfType
   // rather than through the typed methods next to it, which add behaviour SPF
   // must not have - see the comment on that method for which and why.
   //
   // Names and records cross SPFDnsLookup as bytes rather than as the wide string
   // the rest of the server uses, because RFC 7208 section 3.1 restricts a policy
   // to 7-bit ASCII and requires anything else to be rejected, which an evaluator
   // can only do if it sees the bytes it was handed. Converting between the two is
   // most of what this does.
   class SPFDnsResolver : public SPFDnsLookup
   {
   public:

      virtual bool GetTXTRecords(const AnsiString &domain, std::vector<AnsiString> &records);
      virtual bool GetARecords(const AnsiString &host, std::vector<AnsiString> &addresses);
      virtual bool GetAAAARecords(const AnsiString &host, std::vector<AnsiString> &addresses);
      virtual bool GetMXRecords(const AnsiString &domain, std::vector<AnsiString> &hostNames);
      virtual bool GetPTRRecords(const AnsiString &reverseName, std::vector<AnsiString> &hostNames);

   private:

      static bool Query_(const AnsiString &query, int resourceType, std::vector<AnsiString> &values);
   };
}
