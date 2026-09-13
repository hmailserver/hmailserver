// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The DNS lookups an SPF evaluation makes, RFC 7208 section 5. False means the
   // query could not be answered, which section 4.4 makes a temperror; true with no
   // records means the name does not exist. Names and records cross as bytes.
   class SPFDnsLookup
   {
   public:

      virtual ~SPFDnsLookup() {}

      // The TXT records of a domain: the policy of section 3, and the text an exp
      // modifier points at, section 6.2. The character-strings of one record are
      // already joined, as section 3.3 requires.
      virtual bool GetTXTRecords(const AnsiString &domain, std::vector<AnsiString> &records) = 0;

      // The addresses of a host, one method per family. Sections 5.3 and 5.4 select by
      // the family the client connected over, so the two cannot be merged into one
      // list.
      virtual bool GetARecords(const AnsiString &host, std::vector<AnsiString> &addresses) = 0;
      virtual bool GetAAAARecords(const AnsiString &host, std::vector<AnsiString> &addresses) = 0;

      // The host names a domain's MX records name, in preference order.
      virtual bool GetMXRecords(const AnsiString &domain, std::vector<AnsiString> &hostNames) = 0;

      // The host names a reverse-mapping name points at, for ptr (section 5.5) and the
      // p macro (section 7.3). The name - 40.218.168.192.in-addr.arpa - rather than the
      // address, because an evaluation has built it already for the ir and v macros.
      virtual bool GetPTRRecords(const AnsiString &reverseName, std::vector<AnsiString> &hostNames) = 0;
   };
}
