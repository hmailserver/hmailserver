// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#include "stdafx.h"

#include "SPFDnsResolver.h"

#include <windns.h>

#include "../../Common/TCPIP/DNSResolver.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool
   SPFDnsResolver::GetTXTRecords(const AnsiString &domain, std::vector<AnsiString> &records)
   {
      // The character-strings of one TXT record arrive already joined, which is
      // what RFC 7208 section 3.3 asks for.
      return Query_(domain, DNS_TYPE_TEXT, records);
   }

   bool
   SPFDnsResolver::GetARecords(const AnsiString &host, std::vector<AnsiString> &addresses)
   {
      return Query_(host, DNS_TYPE_A, addresses);
   }

   bool
   SPFDnsResolver::GetAAAARecords(const AnsiString &host, std::vector<AnsiString> &addresses)
   {
      return Query_(host, DNS_TYPE_AAAA, addresses);
   }

   bool
   SPFDnsResolver::GetMXRecords(const AnsiString &domain, std::vector<AnsiString> &hostNames)
   {
      if (!Query_(domain, DNS_TYPE_MX, hostNames))
         return false;

      // The null MX of RFC 7505: a domain saying it accepts no mail at all. To an
      // mx mechanism that is a domain with no exchangers to match, section 5.4,
      // and the root is not a host name to go looking for addresses of.
      for (int i = (int) hostNames.size(); i > 0; i--)
      {
         if (hostNames[i - 1] == "." || hostNames[i - 1].IsEmpty())
            hostNames.erase(hostNames.begin() + (i - 1));
      }

      return true;
   }

   bool
   SPFDnsResolver::GetPTRRecords(const AnsiString &reverseName, std::vector<AnsiString> &hostNames)
   {
      // The reverse-mapping name is built by the evaluation rather than here: it
      // is what %{ir}.%{v}.arpa expands to, and SPFAddress is the one place that
      // spells it.
      return Query_(reverseName, DNS_TYPE_PTR, hostNames);
   }

   bool
   SPFDnsResolver::Query_(const AnsiString &query, int resourceType, std::vector<AnsiString> &values)
   {
      values.clear();

      DNSResolver resolver;

      std::vector<String> found;

      if (!resolver.GetRecordsOfType(String(query), resourceType, found))
         return false;

      for (size_t i = 0; i < found.size(); i++)
         values.push_back(AnsiString(found[i]));

      return true;
   }
}
