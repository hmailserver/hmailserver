// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "DNSResolver.h"
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
   struct structMailServer
   {
      long lPreference;
      String sHostName;
   };

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
   DNSResolver::_IsDNSError(int iErrorMessage)
   {
      // Assume non-fatal if we aren't sure it's not.
      switch (iErrorMessage)
      {
      case DNS_ERROR_RCODE_NAME_ERROR: // Domain doesn't exist
         return false;
      case ERROR_INVALID_NAME:
         return false;
      case DNS_INFO_NO_RECORDS: // No records were found for the host. Not an error.
         return false;
      case DNS_ERROR_NO_DNS_SERVERS: // No DNS servers found.
         return true;
      }

      return true;
   }


   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Determines whether the result of a WSA-function is an error or not.
   //---------------------------------------------------------------------------()
   bool
   DNSResolver::_IsWSAError(int iErrorMessage)
   {
      // Assume non-fatal if we aren't sure it's not.
      switch (iErrorMessage)
      {
      case WSAHOST_NOT_FOUND: // Domain doesn't exist
         return false;
      }

      return true;
   }

   bool
   DNSResolver::_Resolve(const String &sSearchFor, vector<String> &vecFoundNames, WORD wType, int iRecursion)
   {
      USES_CONVERSION;

      if (iRecursion > 10)
      {
         String sMessage;
         sMessage.Format(_T("Too many recursions during query. Query: %s, Type: %d."), sSearchFor, wType);
         ErrorManager::Instance()->ReportError(ErrorManager::Low, 4401, "DNSResolver::_Resolve", sMessage);

         return false;
      }

      PDNS_RECORD pDnsRecords = NULL;
      PIP4_ARRAY pSrvList = NULL;

      DWORD fOptions;
      fOptions = DNS_QUERY_STANDARD;

      // We need this if not using system dns servers
      //      fOptions = DNS_QUERY_BYPASS_CACHE;
      
      DNS_STATUS nDnsStatus = DnsQuery(sSearchFor, wType, fOptions, NULL, &pDnsRecords,NULL);

      PDNS_RECORD pDnsRecordsToDelete = pDnsRecords;

      if (nDnsStatus != 0)
      {
         if (pDnsRecordsToDelete)
            _FreeDNSRecord(pDnsRecordsToDelete);

         bool bDNSError = _IsDNSError(nDnsStatus);

         if (bDNSError)
         {
            String sMessage;
            sMessage.Format(_T("DNS - Query failure. Treating as temporary failure. Query: %s, Type: %d, DnsQuery return value: %d."), sSearchFor, wType, nDnsStatus);
            LOG_SMTP(0,"TCP",sMessage);
            return false;
         }

         return true;
      }

      switch (wType)
      {
         case DNS_TYPE_CNAME:
         {
            String sDomainName = pDnsRecords->Data.CNAME.pNameHost;
            if (!_Resolve(sDomainName, vecFoundNames, wType, iRecursion+1))
               return false;

            break;
         }
         case DNS_TYPE_MX: 
            {
               if (pDnsRecords->wType == DNS_TYPE_CNAME)
               {
                  // we received a CNAME response so we need to recurse over that.
                  String sDomainName = pDnsRecords->Data.CNAME.pNameHost;
                  if (!_Resolve(sDomainName, vecFoundNames, DNS_TYPE_MX, iRecursion+1))
                     return false;
               }            
               else if (pDnsRecords->wType == DNS_TYPE_MX)
               {
                  std::vector<structMailServer> vecMailServersUnsorted;

                  do 
                  {
                     String sName = pDnsRecords->pName;
                     bool bNameMatches = (sName.CompareNoCase(sSearchFor) == 0);

                     if (pDnsRecords->Flags.S.Section == DNSREC_ANSWER && bNameMatches) 
                     {
                        structMailServer structServer; 
                        structServer.sHostName = pDnsRecords->Data.MX.pNameExchange;
                        structServer.lPreference = pDnsRecords->Data.MX.wPreference;

                        vecMailServersUnsorted.push_back(structServer);
                     }
                     pDnsRecords = pDnsRecords->pNext;
                  } while (pDnsRecords);

                  // Add the servers in sorted order.
                  while (vecMailServersUnsorted.size() > 0)
                  {
                     // Find lowest preference.
                     std::vector<structMailServer>::iterator iterServer = vecMailServersUnsorted.begin();
                     long iCurLowest = -1;
                     while (iterServer != vecMailServersUnsorted.end())
                     {
                        if (iCurLowest == -1 || (*iterServer).lPreference < iCurLowest)
                           iCurLowest = (*iterServer).lPreference;

                        iterServer++;
                     }

                     // Add lowest to sorted list and remove from unsorted.
                     iterServer = vecMailServersUnsorted.begin();
                     while (iterServer != vecMailServersUnsorted.end())
                     {
                        if ((*iterServer).lPreference == iCurLowest)
                        {
                           vecFoundNames.push_back((*iterServer).sHostName);
                           iterServer = vecMailServersUnsorted.erase(iterServer);
                        }
                        else
                           iterServer++;
                     }
                  }
               }
            }
         break;
      case DNS_TYPE_TEXT: 
         {
            if (pDnsRecords->wType == DNS_TYPE_CNAME)
            {
               // we received a CNAME response so we need to recurse over that.
               String sDomainName = pDnsRecords->Data.CNAME.pNameHost;
               if (!_Resolve(sDomainName, vecFoundNames, DNS_TYPE_TEXT, iRecursion+1))
                  return false;
            }   
            else if (pDnsRecords->wType == DNS_TYPE_TEXT)
            {
               AnsiString retVal;

               for (u_int i = 0; i < pDnsRecords->Data.TXT.dwStringCount; i++) 
               {
                  retVal += pDnsRecords->Data.TXT.pStringArray[i];
               }
               
               vecFoundNames.push_back(retVal);
            }
            break;
         }
      // JDR: Added to perform PTR lookups.
      case DNS_TYPE_PTR: 
         {
            if (pDnsRecords->wType == DNS_TYPE_CNAME)
            {
               // we received a CNAME response so we need to recurse over that.
               String sDomainName = pDnsRecords->Data.CNAME.pNameHost;
               if (!_Resolve(sDomainName, vecFoundNames, DNS_TYPE_PTR, iRecursion+1))
                  return false;
            }   
            else if (pDnsRecords->wType == DNS_TYPE_PTR)
            {
               AnsiString retVal;
               retVal = pDnsRecords->Data.PTR.pNameHost;
               vecFoundNames.push_back(retVal);
            }
            break;
         }
         default:
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5036, "DNSResolver::_Resolve", "Querying unknown wType.");
            }
            
            break;
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
   DNSResolver::GetARecords(const String &sDomain, std::vector<String> &saFoundNames)
   {
      // Do a DNS/A lookup. This may result in a AAAA result, if IPV6 is installed in the system.
      boost::asio::io_service io_service;

      // Get a list of endpoints corresponding to the server name.
      tcp::resolver resolver(io_service);
      tcp::resolver::query query(AnsiString(sDomain), "25", tcp::resolver::query::numeric_service);

      boost::system::error_code errorCode;
      tcp::resolver::iterator endpoint_iterator = resolver.resolve(query, errorCode);
      
      if (errorCode)
      {
         bool bDNSError = _IsWSAError(errorCode.value());

         if (bDNSError)
         {
            String sMessage;
            sMessage.Format(_T("DNS query failure. Treating as temporary failure. Query: %s, Type: A/AAAA, DnsQuery return value: %d. Message: %s"), sDomain, errorCode.value(), String(errorCode.message()));
            LOG_SMTP(0,"TCP",sMessage);
            return false;
         }

         return true;
      }


      while (endpoint_iterator != tcp::resolver::iterator())   
      {
         tcp::endpoint endpoint = *endpoint_iterator;
         boost::asio::ip::address adr = endpoint.address();

         std::string result = adr.to_string(errorCode);

         if (errorCode)
         {
            String sMessage;
            sMessage.Format(_T("DNS query failure. Treating as temporary failure. Conversion of DNS record to string failed. Domain: %s, Error code: %d, Message: %s"), sDomain, errorCode.value(), String(errorCode.message()));
            LOG_SMTP(0,"TCP",sMessage);
            return false;
         }

         saFoundNames.push_back(result);

         endpoint_iterator++;
      }
      
      return true;
   }

   bool 
   DNSResolver::GetTXTRecords(const String &sDomain, std::vector<String> &foundResult)
   {
      return _Resolve(sDomain, foundResult, DNS_TYPE_TEXT, 0);
   }

   bool
   DNSResolver::GetEmailServers(const String &sDomainName, std::vector<String> &saFoundNames )
   {
      LOG_SMTP(0,"TCP","DNS - MX Lookup: " + sDomainName);

      std::vector<String> vecFoundMXRecords;

      try
      {
         if (!_Resolve(sDomainName, vecFoundMXRecords, DNS_TYPE_MX, 0))
         {
            String logMessage;
            logMessage.Format(_T("Failed to resolve email servers (MX lookup). Domain name: %s."), sDomainName);
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
            if (!GetARecords(sDomainName, saFoundNames))
            {
               String logMessage;
               logMessage.Format(_T("Failed to resolve email servers (A lookup). Domain name: %s."), sDomainName);
               LOG_DEBUG(logMessage);

               return false;
            }
         }
         else
         {
            // We've been able to find host names in the MX records. We should
            // now translate them to IP addresses. Some host names may result
            // in several IP addreses.
            std::vector<String>::iterator iterDomain = vecFoundMXRecords.begin();

            bool dnsSuccess = false;
            boost_foreach(String domain, vecFoundMXRecords)
            {
               // Resolve to domain name to IP address and put it in the list.
               int iCountBefore = saFoundNames.size();

               if (!GetARecords(domain, saFoundNames))
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
                     saFoundNames.push_back(domain);
                  }
               }
            }

            if (!dnsSuccess)
            {
               // All dns queries failed.
               String logMessage;
               logMessage.Format(_T("Failed to resolve email servers (A lookup). Domain name: %s."), sDomainName);
               LOG_DEBUG(logMessage);

               return false;
            }

         }

         String sLogMsg;
         sLogMsg.Format(_T("DNS - MX Result: %d IP addresses were found."), saFoundNames.size());

         LOG_SMTP(0,"TCP",sLogMsg);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5038, "DNSResolver::GetEmailServers", "An exception was thrown.");
         throw;
      }

      StringParser::RemoveDuplicateItems(saFoundNames);


      return true;
   }

   bool 
   DNSResolver::GetMXRecords(const String &sDomain, std::vector<String> &vecFoundNames)
   {
      try
      {
         return _Resolve(sDomain, vecFoundNames, DNS_TYPE_MX, 0);
      }
      catch (...)
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5039, "DNSResolver::GetMXRecords", "An exception was thrown.");
         throw;
      }

      return false;
   }

   // JDR: added to do PTR lookups.
   bool 
   DNSResolver::GetPTRRecords(const String &sIP, std::vector<String> &vecFoundNames)
   {

      try
      {
         std::vector<String> vecItems = StringParser::SplitString(sIP, ".");
         reverse(vecItems.begin(), vecItems.end());
         String result = StringParser::JoinVector(vecItems, ".");
         return _Resolve(result + ".in-addr.arpa", vecFoundNames, DNS_TYPE_PTR, 0);
      }
      catch (std::exception& e)
      {
         string ErrorMessage = "An exception was thrown: ";
         ErrorMessage.append(e.what());
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5039, "DNSResolver::GetPTRRecords", ErrorMessage);
         throw;
      }

      return false;
   }


   #ifdef _DEBUG
   void DNSResolverTester::Test()
   {
      DNSResolver resolver;
      int iStart = GetTickCount();
      for (int i = 1; i < 25; i++)
      {
         std::vector<String> vecFoundNames;
         resolver.GetEmailServers("hmailserver.com", vecFoundNames);
      }
      int iStop = GetTickCount();
      int iDiff = iStop - iStart;
      String sResult;
      sResult.Format(_T("%d"), iDiff);

      OutputDebugString(sResult);
   }
   #endif
}
