// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class HostNameAndIpAddress;
   class DNSRecord;

   class DNSResolver
   {
   private:

   public:
	   DNSResolver();
	   virtual ~DNSResolver();

      bool GetEmailServers(const String &sDomainName, std::vector<HostNameAndIpAddress> &saFoundNames);
      bool GetMXRecords(const String &sDomain, std::vector<String> &vecFoundNames);
      bool GetIpAddresses(const String &sDomain, std::vector<String> &saFoundNames, bool followCnameRecords, bool bypassCache = false);
      bool GetIpAddressesWithRetry(const String &sDomain, std::vector<String> &saFoundNames, bool followCnameRecords);
      bool GetTXTRecords(const String &sDomain, std::vector<String> &foundResult);
      bool GetPTRRecords(const String &sIP, std::vector<String> &vecFoundNames);

      // The values of one record type, as they came back, following CNAMEs the
      // way the methods above do.
      //
      // Those methods each add something their own callers want and an SPF
      // evaluation does not. GetIpAddresses merges A and AAAA into one list and
      // leaves AAAA out unless IPv6 is available on this server; RFC 7208
      // sections 5.3 and 5.4 pick the type from the family the client connected
      // over, whatever this server is configured for. GetPTRRecords builds a
      // reverse-mapping name from an address; an SPF evaluation has already built
      // that name, because it is what the i and v macros of section 7.1 expand
      // to, and a name built in two places can be built two ways. GetMXRecords
      // reports the null MX of RFC 7505 the way it reports a failed query, which
      // would make it a temperror; to section 5.4 a domain with a null MX is a
      // domain with no exchangers to match.
      //
      // resourceType is a DNS_TYPE_ constant, as DNSResolverWinApi::Query takes.
      bool GetRecordsOfType(const String &query, int resourceType, std::vector<String> &values);

   private:

      bool GetEmailServersRecursive_(const String &sDomainName, std::vector<HostNameAndIpAddress> &saFoundNames, int recursionLevel);
      bool GetIpAddressesRecursive_(const String &hostName, std::vector<String> &addresses, int recursionLevel, bool followCnameRecords, bool bypassCache);
      bool GetTXTRecordsRecursive_(const String &sDomain, std::vector<String> &foundResult, int recursionLevel);
      bool GetMXRecordsRecursive_(const String &sDomain, std::vector<String> &vecFoundNames, int recursionLevel);
      bool GetRecordsOfTypeRecursive_(const String &query, int resourceType, std::vector<String> &values, int recursionLevel);

      std::vector<String> GetDnsRecordsValues_(std::vector<DNSRecord> records);
   };


}
