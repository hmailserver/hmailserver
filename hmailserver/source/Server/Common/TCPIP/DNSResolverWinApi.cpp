// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "DNSResolverWinApi.h"
#include "../Util/Assert.h"
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
         fOptions |= DNS_QUERY_BYPASS_CACHE;
      }

      // Use a Custom DNS server IPv4 address if specified in the settings. 
      // If the address is invalid, we will fallback to the system DNS servers.
      AnsiString sCustomDNS;
      sCustomDNS = IniFileSettings::Instance()->GetDNSServer().Trim();
      if (!sCustomDNS.IsEmpty())
      {
         // Allocate and zero-initialize for safety.
         pSrvList = (PIP4_ARRAY)malloc(sizeof(IP4_ARRAY));
         if (!pSrvList) 
         {
            String sMessage;
            sMessage.Format(_T("Unable to allocate memory for DNS server list. Query: %s, Type: %d."), query, resourceType);
            ErrorManager::Instance()->ReportError(ErrorManager::Low, 4401, "DNSResolver::_Resolve", sMessage);

            return false;
         }

         // Parse IPv4 address
         pSrvList->AddrCount = 1;
         pSrvList->AddrArray[0] = inet_addr(sCustomDNS.c_str());
         if (pSrvList->AddrArray[0] == INADDR_NONE) 
         {
            String sMessage;
            sMessage.Format(_T("Invalid DNSServer IP address. DNSServer IP: %hs."), sCustomDNS.c_str());
            ErrorManager::Instance()->ReportError(ErrorManager::Low, 4401, "DNSResolver::_Resolve", sMessage);

            // free and fallback to the system dns servers
            free(pSrvList);
            pSrvList = NULL;
         }
         else
         {
            // If using a custom server, bypass local cache to ensure we query the specified server.
            fOptions |= DNS_QUERY_BYPASS_CACHE;
         }
      }

      DNS_STATUS nDnsStatus = DnsQuery(query, resourceType, fOptions, pSrvList, &pDnsRecord, NULL);

      // DnsQuery only reads the server list, so it can be released immediately.
      free(pSrvList);
      pSrvList = NULL;

      PDNS_RECORD pDnsRecordsToDelete = pDnsRecord;

      if (nDnsStatus != 0)
      {
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
             NameMatchesQuery(query, name))
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

      std::sort(foundRecords.begin(), foundRecords.end(), SortDnsRecordsByPreference);


      return true;
   }

   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Compares a record name with the queried name. A trailing dot is the root label,
   // so example.com. and example.com are the same name (RFC 1035, 5.1) and it is ignored.
   // Windows Server 2025 returns PTR names in that form.
   //---------------------------------------------------------------------------()
   bool
   DNSResolverWinApi::NameMatchesQuery(const String &query, const String &recordName)
   {
      String name = recordName;

      if (!name.empty() && name.back() == '.')
         name.pop_back();

      return query.Equals(name);
   }

   void
   DNSResolverWinApiTester::Test()
   {
      // Names are compared as-is when neither side is fully qualified.
      Assert::IsTrue(DNSResolverWinApi::NameMatchesQuery(_T("example.com"), _T("example.com")));
      Assert::IsFalse(DNSResolverWinApi::NameMatchesQuery(_T("example.com"), _T("other.com")));

      // A fully qualified name matches the same name without the root label.
      Assert::IsTrue(DNSResolverWinApi::NameMatchesQuery(_T("1.0.0.127.in-addr.arpa"), _T("1.0.0.127.in-addr.arpa.")));
      Assert::IsTrue(DNSResolverWinApi::NameMatchesQuery(
         _T("1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.ip6.arpa"),
         _T("1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.ip6.arpa.")));

      // The root label is stripped for every record type, not just PTR.
      Assert::IsTrue(DNSResolverWinApi::NameMatchesQuery(_T("example.com"), _T("example.com.")));

      // Stripping the root label must not turn a different name into a match.
      Assert::IsFalse(DNSResolverWinApi::NameMatchesQuery(_T("example.com"), _T("evil-example.com.")));
      Assert::IsFalse(DNSResolverWinApi::NameMatchesQuery(_T("1.0.0.127.in-addr.arpa"), _T("1.0.0.127.in-addr.arpa.evil.com")));

      // Empty names must not trip the trailing-dot handling.
      Assert::IsFalse(DNSResolverWinApi::NameMatchesQuery(_T("example.com"), _T("")));
      Assert::IsTrue(DNSResolverWinApi::NameMatchesQuery(_T(""), _T("")));
   }
}

