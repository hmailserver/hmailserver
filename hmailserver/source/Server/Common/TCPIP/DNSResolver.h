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
      bool GetIpAddresses(const String &sDomain, std::vector<String> &saFoundNames, bool followCnameRecords);
      bool GetTXTRecords(const String &sDomain, std::vector<String> &foundResult);
      bool GetPTRRecords(const String &sIP, std::vector<String> &vecFoundNames);
   private:

      bool GetIpAddressesRecursive_(const String &hostName, std::vector<String> &addresses, int recursionLevel, bool followCnameRecords);
      bool GetTXTRecordsRecursive_(const String &sDomain, std::vector<String> &foundResult, int recursionLevel);
      bool GetMXRecordsRecursive_(const String &sDomain, std::vector<String> &vecFoundNames, int recursionLevel);

      std::vector<String> GetDnsRecordsValues_(std::vector<DNSRecord> records);
   };


}
