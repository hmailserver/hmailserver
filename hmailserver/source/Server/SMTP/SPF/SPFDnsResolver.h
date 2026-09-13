// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

#include "SPFDnsLookup.h"

namespace HM
{
   // The SPFDnsLookup the server uses: real DNS, through
   // DNSResolver::GetRecordsOfType rather than the typed methods beside it - see
   // README.md. Names cross as bytes, because section 3.1 restricts a policy.
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
