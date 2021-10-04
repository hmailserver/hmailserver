// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "DNSResolverWinApi.h"
#include <iphlpapi.h>
#include <windns.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   bool SortDnsRecordsByPreference(DNSRecord first, DNSRecord second) { return (first.GetPreference() < second.GetPreference()); }

   DNSResolverWinApi::DNSResolverWinApi()
   {

   }

   DNSResolverWinApi::~DNSResolverWinApi()
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
   DNSResolverWinApi::IsDNSError_(int iErrorMessage)
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
   DNSResolverWinApi::Query(const String &query, int resourceType, std::vector<DNSRecord> &foundRecords)
   {
      PDNS_RECORD pDnsRecord = NULL;
      
      PIP4_ARRAY pSrvList = NULL;

      DWORD fOptions;
      fOptions = DNS_QUERY_STANDARD;

      if (!IniFileSettings::Instance()->GetUseDNSCache())
      {
         fOptions += DNS_QUERY_BYPASS_CACHE;
      }

      AnsiString sCustomDNS;
      sCustomDNS = IniFileSettings::Instance()->GetDNSServer().Trim();
      if (!sCustomDNS.IsEmpty())
      {
         pSrvList = (PIP4_ARRAY)malloc(sizeof(IP4_ARRAY));
         if (!pSrvList) {

            String sMessage;
            sMessage.Format(_T("Unable to allocate memory for DNS server list. Query: %s, Type: %d."), query, resourceType);
            ErrorManager::Instance()->ReportError(ErrorManager::Low, 4401, "DNSResolver::_Resolve", sMessage);

            return false;
         }

         // Custom DNSServer IPv4 address
         pSrvList->AddrCount = 1;
         pSrvList->AddrArray[0] = inet_addr(sCustomDNS.c_str()); //Custom DNS server IP address
         if (pSrvList->AddrArray[0] == INADDR_NONE) {

            String sMessage;
            sMessage.Format(_T("Invalid DNSServer IP address. DNSServer IP: %hs."), sCustomDNS.c_str());
            ErrorManager::Instance()->ReportError(ErrorManager::Low, 4401, "DNSResolver::_Resolve", sMessage);

            // fallback to the system dns servers
            pSrvList = NULL;
         }
         else
         {
            // We need this if not using system dns servers
            if (fOptions != DNS_QUERY_BYPASS_CACHE)
               fOptions += DNS_QUERY_BYPASS_CACHE;
         }
      }

      DNS_STATUS nDnsStatus = DnsQuery(query, resourceType, fOptions, pSrvList, &pDnsRecord, NULL);

      PDNS_RECORD pDnsRecordsToDelete = pDnsRecord;

      if (nDnsStatus != 0)
      {
         if (pDnsRecordsToDelete)
            _FreeDNSRecord(pDnsRecordsToDelete);

         bool bDNSError = IsDNSError_(nDnsStatus);

         if (bDNSError)
         {
            String sMessage;
            sMessage.Format(_T("DNS - Query failure. Query: %s, Type: %d, DnsQuery return value: %d."), query.c_str(), resourceType, nDnsStatus);
            LOG_TCPIP(sMessage);
            return false;
         }

         return true;
      }

      while (pDnsRecord != nullptr)
      {
         String name = pDnsRecord->pName;

         if (pDnsRecord->wType == resourceType && 
             query.Equals(name))
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
                     DNSRecord record(buf, pDnsRecord->wType, 0);
                     foundRecords.push_back(record);
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
                     DNSRecord record(buf, pDnsRecord->wType, 0);
                     foundRecords.push_back(record);
                  }

                  break;
               }
               case DNS_TYPE_CNAME:
               {
                  String sDomainName = pDnsRecord->Data.CNAME.pNameHost;

                  DNSRecord record(sDomainName, pDnsRecord->wType, 0);
                  foundRecords.push_back(record);
                  break;
               }
               case DNS_TYPE_MX:
               {
                  if (pDnsRecord->Flags.S.Section == DNSREC_ANSWER)
                  {
                     DNSRecord record(String(pDnsRecord->Data.MX.pNameExchange), pDnsRecord->wType, pDnsRecord->Data.MX.wPreference);
                     foundRecords.push_back(record);
                  }

                  break;
               }
               case DNS_TYPE_TEXT:
               {
                  AnsiString retVal;

                  for (u_int i = 0; i < pDnsRecord->Data.TXT.dwStringCount; i++)
                     retVal += pDnsRecord->Data.TXT.pStringArray[i];

                  DNSRecord record(retVal, pDnsRecord->wType, 0);
                  foundRecords.push_back(record);
                  break;
               }
               case DNS_TYPE_PTR:
               {
                  AnsiString retVal;
                  retVal = pDnsRecord->Data.PTR.pNameHost;

                  DNSRecord record(retVal, pDnsRecord->wType, 0);
                  foundRecords.push_back(record);
                  break;
               }
               default:
               {
                  ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5036, "DNSResolverWinApi::Resolve_", Formatter::Format("Queried for {0} but received type {1}", resourceType, pDnsRecord->wType));
                  break;
               }
            }
         }

         pDnsRecord = pDnsRecord->pNext;
      }
      
      _FreeDNSRecord(pDnsRecordsToDelete);
      pDnsRecordsToDelete = 0;

      if (pSrvList != NULL)
         free(pSrvList);

      std::sort(foundRecords.begin(), foundRecords.end(), SortDnsRecordsByPreference);


      return true;
   }
}

