/* Copyright (c) Martin Knafve / The hMailServer Community Developers (HCD) hMailServer.com */

#include <stdafx.h>
#include "DNSResolver.h"
#include <iphlpapi.h>
#include <windns.h>
#include <boost/asio.hpp>

#include "HostNameAndIpAddress.h"

using boost::asio::ip::tcp;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   struct DnsRecordWithPreference
   {
      DnsRecordWithPreference(long preference, String value)
      {
         Preference = preference;
         Value = value;
      }

      long Preference;
      String Value;
   };

   bool SortDnsRecordWithPreference(DnsRecordWithPreference first, DnsRecordWithPreference second) { return (first.Preference<second.Preference); }

   DNSResolver::DNSResolver()
   {

   }

   DNSResolver::~DNSResolver()
   {

   }
 
   void 
   _FreeDNSRecord(PDNS_RECORD pRecord)
   {
      if (!pRecord) 
         return;

      DNS_FREE_TYPE freetype = DnsFreeRecordListDeep;
      DnsRecordListFree(pRecord, freetype);
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Determines whether the result of a DnsQuery call is an error or not.
   //---------------------------------------------------------------------------()
   bool
   DNSResolver::IsDNSError_(int iErrorMessage)
   {
      switch (iErrorMessage)
      {
      case DNS_ERROR_RCODE_NAME_ERROR: // Domain doesn't exist
         return false;
      case ERROR_INVALID_NAME:
         return false;
      case DNS_INFO_NO_RECORDS:        // No records were found for the host. Not an error.
         return false;
      case DNS_ERROR_NO_DNS_SERVERS:   // No DNS servers found.
         return true;
      }

      return true;
   }

   bool
   DNSResolver::Resolve_(const String &sSearchFor, std::vector<String> &vecFoundNames, WORD wType, int iRecursion)
   {
      if (iRecursion > 10)
      {
         String sMessage = Formatter::Format("Too many recursions during query. Query: {0}, Type: {1}", sSearchFor, wType);
         ErrorManager::Instance()->ReportError(ErrorManager::Low, 4401, "DNSResolver::Resolve_", sMessage);

         return false;
      }

      PDNS_RECORD pDnsRecord = NULL;
      PIP4_ARRAY pSrvList = NULL;

      DWORD fOptions = DNS_QUERY_STANDARD;
      
      DNS_STATUS nDnsStatus = DnsQuery(sSearchFor, wType, fOptions, NULL, &pDnsRecord, NULL);

      PDNS_RECORD pDnsRecordsToDelete = pDnsRecord;

      if (nDnsStatus != 0)
      {
         if (pDnsRecordsToDelete)
            _FreeDNSRecord(pDnsRecordsToDelete);

         bool bDNSError = IsDNSError_(nDnsStatus);

         if (bDNSError)
         {
            String sMessage;
            sMessage.Format(_T("DNS - Query failure. Query: %s, Type: %d, DnsQuery return value: %d."), sSearchFor.c_str(), wType, nDnsStatus);
            LOG_TCPIP(sMessage);
            return false;
         }

         return true;
      }

      std::vector<DnsRecordWithPreference> foundDnsRecordsWithPreference;

      while (pDnsRecord != nullptr)
      {
         String name = pDnsRecord->pName;

         if (pDnsRecord->wType == wType && sSearchFor.Equals(name))
         {
            switch (pDnsRecord->wType)
            {
               case DNS_TYPE_A:
               {
                  SOCKADDR_IN addr;
                  memset(&addr, 0, sizeof addr);

                  addr.sin_family = AF_INET;
                  addr.sin_addr = *((in_addr*)&(pDnsRecord->Data.A.IpAddress));

                  char buf[128];
                  DWORD bufSize = sizeof(buf);

                  if (WSAAddressToStringA((sockaddr*)&addr, sizeof addr, NULL, buf, &bufSize) == 0)
                  {
                     DnsRecordWithPreference rec(0, buf);
                     foundDnsRecordsWithPreference.push_back(rec);
                  }

                  break;
               }
               case DNS_TYPE_AAAA:
               {
                  SOCKADDR_IN6 addr;
                  memset(&addr, 0, sizeof addr);

                  addr.sin6_family = AF_INET6;
                  addr.sin6_addr = *((in_addr6*)&(pDnsRecord->Data.AAAA.Ip6Address));

                  char buf[128];
                  DWORD bufSize = sizeof(buf);

                  if (WSAAddressToStringA((sockaddr*)&addr, sizeof addr, NULL, buf, &bufSize) == 0)
                  {
                     DnsRecordWithPreference rec(0, buf);
                     foundDnsRecordsWithPreference.push_back(rec);
                  }

                  break;
               }
               case DNS_TYPE_CNAME:
               {
                  String sDomainName = pDnsRecord->Data.CNAME.pNameHost;
                  if (!Resolve_(sDomainName, vecFoundNames, wType, iRecursion + 1))
                     return false;

                  break;
               }
               case DNS_TYPE_MX:
               {
                  String sName = pDnsRecord->pName;
                  bool bNameMatches = (sName.CompareNoCase(sSearchFor) == 0);

                  if (pDnsRecord->Flags.S.Section == DNSREC_ANSWER && bNameMatches)
                  {
                     DnsRecordWithPreference rec(pDnsRecord->Data.MX.wPreference, pDnsRecord->Data.MX.pNameExchange);
                     foundDnsRecordsWithPreference.push_back(rec);
                  }

                  break;
               }
               case DNS_TYPE_TEXT:
               {
                  AnsiString retVal;

                  for (u_int i = 0; i < pDnsRecord->Data.TXT.dwStringCount; i++)
                     retVal += pDnsRecord->Data.TXT.pStringArray[i];

                  DnsRecordWithPreference rec(0, retVal);
                  foundDnsRecordsWithPreference.push_back(rec);

                  break;
               }
               case DNS_TYPE_PTR:
               {
                  AnsiString retVal;
                  retVal = pDnsRecord->Data.PTR.pNameHost;

                  DnsRecordWithPreference rec(0, retVal);
                  foundDnsRecordsWithPreference.push_back(rec);
                  break;
               }
               default:
               {
                  ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5036, "DNSResolver::Resolve_", Formatter::Format("Queried for {0} but received type {1}", wType, pDnsRecord->wType));
                  break;
               }
            }
         }

         pDnsRecord = pDnsRecord->pNext;
      }
      

      std::sort(foundDnsRecordsWithPreference.begin(), foundDnsRecordsWithPreference.end(), SortDnsRecordWithPreference);

      for (DnsRecordWithPreference item : foundDnsRecordsWithPreference)
      {
         vecFoundNames.push_back(item.Value);
      }

      _FreeDNSRecord(pDnsRecordsToDelete);
      pDnsRecordsToDelete = 0;

      return true;
   }


   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Do a DNS A/AAAA lookup.
   // 
   // Code is platform independent.
   //---------------------------------------------------------------------------()
   bool 
   DNSResolver::GetIpAddresses(const String &sDomain, std::vector<String> &vecFoundNames)
   {
      if (sDomain.IsEmpty())
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5516, "DNSResolver::GetARecords", "Attempted DNS lookup for empty host name.");
         return false;
      }

      bool ipv4AddressResult = Resolve_(sDomain, vecFoundNames, DNS_TYPE_A, 0);

      bool ipv6AddressResult = Configuration::Instance()->IsIPv6Available() ?
                                 Resolve_(sDomain, vecFoundNames, DNS_TYPE_AAAA, 0) :
                                 false;

      return ipv4AddressResult || ipv6AddressResult;
   }

   bool 
   DNSResolver::GetTXTRecords(const String &sDomain, std::vector<String> &foundResult)
   {
      return Resolve_(sDomain, foundResult, DNS_TYPE_TEXT, 0);
   }

   bool
   DNSResolver::GetEmailServers(const String &sDomainName, std::vector<HostNameAndIpAddress> &saFoundNames )
   {
      String message = Formatter::Format("DNS MX lookup: {0}", sDomainName);
      LOG_TCPIP(message);

      std::vector<String> vecFoundMXRecords;

      if (!Resolve_(sDomainName, vecFoundMXRecords, DNS_TYPE_MX, 0))
      {
         String logMessage;
            logMessage.Format(_T("Failed to resolve email servers (MX lookup). Domain name: %s."), sDomainName.c_str());
         LOG_DEBUG(logMessage);

         return false;
      }

      if (vecFoundMXRecords.empty())
      {
         /* RFC 2821:
            If no MX records are found, but an A RR is found, the A RR is treated as
            if it was associated with an implicit MX RR, with a preference of 0,
            pointing to that host.  If one or more MX RRs are found for a given
            name, SMTP systems MUST NOT utilize any A RRs associated with that
            name unless they are located using the MX RRs;
            (implemented here)
         */

         std::vector<String> a_records;
         if (!GetIpAddresses(sDomainName, a_records))
         {
            String logMessage;
               logMessage.Format(_T("Failed to resolve email servers (A lookup). Domain name: %s."), sDomainName.c_str());
            LOG_DEBUG(logMessage);

            return false;
         }
		 
		    for(String record : a_records)         
		    {
		 	   HostNameAndIpAddress hostAndAddress;
            hostAndAddress.SetHostName(sDomainName);
            hostAndAddress.SetIpAddress(record);

            saFoundNames.push_back(hostAndAddress);
         }
      }
      else
      {
         // We've been able to find host names in the MX records. We should
         // now translate them to IP addresses. Some host names may result
         // in several IP addreses.
         auto iterDomain = vecFoundMXRecords.begin();

         bool dnsSuccess = false;
         for(String domain : vecFoundMXRecords)
         {
            // Resolve to domain name to IP address and put it in the list.
            size_t iCountBefore = saFoundNames.size();

            std::vector<String> a_records;
            if (!GetIpAddresses(domain, a_records))
               continue;

            dnsSuccess = true;
            
            if (saFoundNames.size() == iCountBefore)
            {
               // No mx records were found for this host name. Check if the host
               // name is actually an IP address? It shouldn't be but....

               if (StringParser::IsValidIPAddress(domain))
               {
                  // Okay, this is an invalid MX record. The MX record should always contain 
                  // a host name but in this case it appears an IP address. We'll be kind to
                  // the domain owner and still deliver the email to him.
                  a_records.push_back(domain);
               }
            }

            for(String record : a_records)
            {
               HostNameAndIpAddress hostAndAddress;
               hostAndAddress.SetHostName(domain);
               hostAndAddress.SetIpAddress(record);

               saFoundNames.push_back(hostAndAddress);
            }
         }

         if (!dnsSuccess)
         {
            // All dns queries failed.
            String logMessage;
               logMessage.Format(_T("Failed to resolve email servers (A lookup). Domain name: %s."), sDomainName.c_str());
            LOG_DEBUG(logMessage);

            return false;
         }

      }

      String sLogMsg;
      sLogMsg.Format(_T("DNS - MX Result: %d IP addresses were found."), saFoundNames.size());

      LOG_TCPIP(sLogMsg);

            
      // Remove duplicate names.
      auto iter = saFoundNames.begin();
      std::set<String> duplicateCheck;

      while (iter != saFoundNames.end())
      {
         String name = (*iter).GetIpAddress();
         if (duplicateCheck.find(name) != duplicateCheck.end())
         {
            // We found a duplicate. Remove it.
            iter = saFoundNames.erase(iter);
         }
         else
         {
            // This is not a duplicate. Move to next.
            iter++;

            duplicateCheck.insert(name);
         }
      }

      return true;
   }

   bool 
   DNSResolver::GetMXRecords(const String &sDomain, std::vector<String> &vecFoundNames)
   {
      return Resolve_(sDomain, vecFoundNames, DNS_TYPE_MX, 0);
   }

   bool 
   DNSResolver::GetPTRRecords(const String &sIP, std::vector<String> &vecFoundNames)
   {
      IPAddress address;
      if (!address.TryParse(AnsiString(sIP), true))
         return false;

      if (address.GetType() == IPAddress::IPV4)
      {
         std::vector<String> vecItems = StringParser::SplitString(sIP, ".");
         reverse(vecItems.begin(), vecItems.end());
         String result = StringParser::JoinVector(vecItems, ".");
         return Resolve_(result + ".in-addr.arpa", vecFoundNames, DNS_TYPE_PTR, 0);
      }
      else
      {
         AnsiString long_ipv6 = address.ToLongString();
         long_ipv6.MakeReverse();
         long_ipv6.Remove(':');

         for (int i = long_ipv6.GetLength() - 1; i > 0; i--)
         {
            long_ipv6.insert(i, 1, '.');
         }

         return Resolve_(long_ipv6 + ".ip6.arpa", vecFoundNames, DNS_TYPE_PTR, 0);
      }


   }

}

