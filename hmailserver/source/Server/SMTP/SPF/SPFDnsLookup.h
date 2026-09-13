// Copyright (c) 2010 Martin Knafve / hMailServer.com.
// http://www.hmailserver.com

#pragma once

namespace HM
{
   // The DNS lookups an SPF evaluation makes, RFC 7208 section 5. Abstract so
   // that an evaluation can be run against a zone which is known in advance
   // rather than against DNS, which is what the conformance suite under
   // SPF/Conformance does with the openspf.org tests for RFC 7208.
   //
   // Names and records cross this interface as bytes rather than as wide
   // strings. RFC 7208 section 3.1 restricts a policy to 7-bit ASCII and
   // requires an implementation to reject anything else, so an evaluator has to
   // be able to see the bytes it was handed.
   //
   // Every lookup returns false if the query could not be answered - a server
   // failure, or a timeout - which RFC 7208 section 4.4 turns into temperror.
   // A name which does not exist is not a failure: the lookup returns true and
   // no records, and RFC 7208 section 5 leaves each mechanism to decide what
   // that means.
   class SPFDnsLookup
   {
   public:

      virtual ~SPFDnsLookup() {}

      // The TXT records of a domain. RFC 7208 section 3 has an SPF policy
      // published as a TXT record, and section 6.2 has the explanation of an
      // exp modifier read from one. The character-strings a single TXT record
      // is made of are already joined, as section 3.3 requires.
      virtual bool GetTXTRecords(const AnsiString &domain, std::vector<AnsiString> &records) = 0;

      // The addresses of a host, one method per family. Which of the two an a
      // or mx mechanism matches against is decided by the family of the client
      // address, RFC 7208 sections 5.3 and 5.4, so the two cannot be merged
      // into one list.
      virtual bool GetARecords(const AnsiString &host, std::vector<AnsiString> &addresses) = 0;
      virtual bool GetAAAARecords(const AnsiString &host, std::vector<AnsiString> &addresses) = 0;

      // The host names a domain's MX records name, in preference order.
      virtual bool GetMXRecords(const AnsiString &domain, std::vector<AnsiString> &hostNames) = 0;

      // The host names a reverse-mapping name points at, for the ptr mechanism
      // of RFC 7208 section 5.5 and the p macro of section 7.3. The name -
      // 40.218.168.192.in-addr.arpa, say - rather than the address is what
      // crosses this interface, because the evaluator has to be able to
      // construct it anyway: it is the name the ir and v macros expand to, and
      // an implementation which built it in two places could build it two ways.
      virtual bool GetPTRRecords(const AnsiString &reverseName, std::vector<AnsiString> &hostNames) = 0;
   };
}
