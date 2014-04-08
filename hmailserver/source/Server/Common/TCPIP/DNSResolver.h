// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{

   class DNSResolver
   {
   private:

   public:
	   DNSResolver();
	   virtual ~DNSResolver();

      bool GetEmailServers(const String &sDomainName, std::vector<String> &saFoundNames);
      bool GetMXRecords(const String &sDomain, std::vector<String> &vecFoundNames);
      bool GetARecords(const String &sDomain, std::vector<String> &saFoundNames);
      bool GetTXTRecords(const String &sDomain, std::vector<String> &foundResult);
      bool GetPTRRecords(const String &sIP, std::vector<String> &vecFoundNames);
   private:

      bool _Resolve(const String &sSearchFor, vector<String> &vecFoundNames, WORD ResourceType, int iRecursion);
      bool _IsDNSError(int iErrorMessage);

      bool _IsWSAError(int iErrorMessage);

   };

   #ifdef _DEBUG
   class DNSResolverTester
   {
   private:

   public:

      DNSResolverTester() {};
      virtual ~DNSResolverTester() {} ;

      void Test();
   private:

      
   };
   #endif

}
