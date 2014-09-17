// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class HostNameAndIpAddress;

   class DNSResolver
   {
   private:

   public:
	   DNSResolver();
	   virtual ~DNSResolver();

      bool GetEmailServers(const String &sDomainName, std::vector<HostNameAndIpAddress> &saFoundNames);
      bool GetMXRecords(const String &sDomain, std::vector<String> &vecFoundNames);
      bool GetARecords(const String &sDomain, std::vector<String> &saFoundNames);
      bool GetTXTRecords(const String &sDomain, std::vector<String> &foundResult);
      bool GetPTRRecords(const String &sIP, std::vector<String> &vecFoundNames);
   private:

      bool Resolve_(const String &sSearchFor, std::vector<String> &vecFoundNames, WORD ResourceType, int iRecursion);
      bool IsDNSError_(int iErrorMessage);

      bool IsWSAError_(int iErrorMessage);
   };


}
