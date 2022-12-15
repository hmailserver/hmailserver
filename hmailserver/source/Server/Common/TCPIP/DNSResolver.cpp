// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "DNSResolver.h"
#include "DNSResolverWinApi.h"
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

   std::vector<String>
   DNSResolver::GetDnsRecordsValues_(std::vector<DNSRecord> records)
   {
      std::vector<String> values;

      for (DNSRecord record : records)
      {
         values.push_back(record.GetValue());
      }

      return values;
   }
   
   bool 
   DNSResolver::GetIpAddresses(const String &sDomain, std::vector<String> &vecFoundNames, bool followCnameRecords)
   {
      return GetIpAddressesRecursive_(sDomain, vecFoundNames, 0, followCnameRecords);
   }

   bool
   DNSResolver::GetIpAddressesRecursive_(const String &hostName, std::vector<String> &addresses, int recursionLevel, bool followCnameRecords)
   {
      if (hostName.IsEmpty())
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5516, "DNSResolver::GetIpAddressesRecursive_", "Attempted DNS lookup for empty host name.");
         return false;
      }

      if (recursionLevel > 10)
      {
         String sMessage = Formatter::Format("Too many recursions during IP address lookup. Query: {0}", hostName);
         ErrorManager::Instance()->ReportError(ErrorManager::Low, 4401, "DNSResolver::GetIpAddressesRecursive_", sMessage);

         return false;
      }

      DNSResolverWinApi resolver;

      std::vector<DNSRecord> foundRecords;
      bool ipv4QueryResult = resolver.Query(hostName, DNS_TYPE_A, foundRecords);
      bool ipv6QueryResult = Configuration::Instance()->IsIPv6Available() ? resolver.Query(hostName, DNS_TYPE_AAAA, foundRecords) : false;

      if (foundRecords.size() == 0 && followCnameRecords)
      {
         // The queries for A/AAAA didn't return any records. Attempt to look up via CNAME
         std::vector<DNSRecord> foundCNames;
         bool cnameQueryResult = resolver.Query(hostName, DNS_TYPE_CNAME, foundCNames);

         // A CNAME should only point at a single host name.
         if (cnameQueryResult && foundCNames.size() == 1)
         {
            auto cnameHostName = foundCNames[0].GetValue();
            return GetIpAddressesRecursive_(cnameHostName, addresses, recursionLevel + 1, followCnameRecords);
         }
      }

      std::vector<String> foundValues = GetDnsRecordsValues_(foundRecords);
      addresses.insert(addresses.end(), foundValues.begin(), foundValues.end());

      return ipv4QueryResult || ipv6QueryResult;
   }


   bool 
   DNSResolver::GetTXTRecords(const String &sDomain, std::vector<String> &foundResult)
   {
      return GetTXTRecordsRecursive_(sDomain, foundResult, 0);
   }

   bool
   DNSResolver::GetTXTRecordsRecursive_(const String &sDomain, std::vector<String> &foundResult, int recursionLevel)
   {
      if (sDomain.IsEmpty())
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5516, "DNSResolver::GetTXTRecordsRecursive_", "Attempted DNS lookup for empty host name.");
         return false;
      }

      if (recursionLevel > 10)
      {
         String sMessage = Formatter::Format("Too many recursions during TXT record lookup. Query: {0}", sDomain);
         ErrorManager::Instance()->ReportError(ErrorManager::Low, 4402, "DNSResolver::GetTXTRecordsRecursive_", sMessage);

         return false;
      }
      
      DNSResolverWinApi resolver;

      std::vector<DNSRecord> foundRecords;

      bool result = resolver.Query(sDomain, DNS_TYPE_TEXT, foundRecords);

      if (foundRecords.size() == 0)
      {
         // The queries for TXT didn't return any records. Attempt to look up via CNAME
         std::vector<DNSRecord> foundCNames;
         bool cnameQueryResult = resolver.Query(sDomain, DNS_TYPE_CNAME, foundCNames);

         // A CNAME should only point at a single host name.
         if (cnameQueryResult && foundCNames.size() == 1)
         {
            auto cnameHostName = foundCNames[0].GetValue();
            return GetTXTRecordsRecursive_(cnameHostName, foundResult, recursionLevel + 1);
         }
      }

      foundResult = GetDnsRecordsValues_(foundRecords);

      return result;
   }

   bool
   DNSResolver::GetEmailServers(const String &sDomainName, std::vector<HostNameAndIpAddress> &saFoundNames )
   {
      String message = Formatter::Format("DNS MX lookup: {0}", sDomainName);
      LOG_TCPIP(message);

      DNSResolverWinApi resolver;
      std::vector<DNSRecord> foundMxRecords;

      if (!resolver.Query(sDomainName, DNS_TYPE_MX, foundMxRecords))
      {
         String logMessage;
            logMessage.Format(_T("Failed to resolve email servers (MX lookup). Domain name: %s."), sDomainName.c_str());
         LOG_DEBUG(logMessage);

         return false;
      }

      if (foundMxRecords.empty())
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
         if (!GetIpAddresses(sDomainName, a_records, true))
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
         bool dnsQueryFailure = false;
         for (DNSRecord dnsRecord : foundMxRecords)
         {
            // Null MX, see: rfc7505 
            // https://tools.ietf.org/html/rfc7505
            if (dnsRecord.GetValue() == "." && dnsRecord.GetPreference() == 0)
            {
               continue;
            }

            // Resolve to domain name to IP address and put it in the list.
            std::vector<String> a_records;

            String hostName = dnsRecord.GetValue();
            if (StringParser::IsValidIPAddress(hostName))
            {
               // Okay, this is an invalid MX record. The MX record should always contain 
               // a host name but in this case it appears an IP address. We'll be kind to
               // the domain owner and still deliver the email to him.
               a_records.push_back(hostName);
            }
            else
            {
               if (!GetIpAddresses(hostName, a_records, true))
               {
                  dnsQueryFailure = true;
                  continue;
               }

               for (String record : a_records)
               {
                  HostNameAndIpAddress hostAndAddress;
                  hostAndAddress.SetHostName(hostName);
                  hostAndAddress.SetIpAddress(record);

                  saFoundNames.push_back(hostAndAddress);
               }
            }
         }

         if (dnsQueryFailure)
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
      return GetMXRecordsRecursive_(sDomain, vecFoundNames, 0);
   }

   bool
   DNSResolver::GetMXRecordsRecursive_(const String &sDomain, std::vector<String> &vecFoundNames, int recursionLevel)
   {
      if (sDomain.IsEmpty())
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5516, "DNSResolver::GetMXRecordsRecursive_", "Attempted DNS lookup for empty host name.");
         return false;
      }

      if (recursionLevel > 10)
      {
         String sMessage = Formatter::Format("Too many recursions during MX record lookup. Query: {0}", sDomain);
         ErrorManager::Instance()->ReportError(ErrorManager::Low, 4403, "DNSResolver::GetMXRecordsRecursive_", sMessage);

         return false;
      }
      
      DNSResolverWinApi resolver;
      std::vector<DNSRecord> foundMxRecords;

      bool result = resolver.Query(sDomain, DNS_TYPE_MX, foundMxRecords);

      if (foundMxRecords.size() == 0)
      {
         // The queries for MX didn't return any records. Attempt to look up via CNAME
         std::vector<DNSRecord> foundCNames;
         bool cnameQueryResult = resolver.Query(sDomain, DNS_TYPE_CNAME, foundCNames);

         // A CNAME should only point at a single host name.
         if (cnameQueryResult && foundCNames.size() == 1)
         {
            auto cnameHostName = foundCNames[0].GetValue();
            return GetMXRecordsRecursive_(cnameHostName, vecFoundNames, recursionLevel + 1);
         }
      }

      vecFoundNames = GetDnsRecordsValues_(foundMxRecords);

      return result;
   }

   bool 
   DNSResolver::GetPTRRecords(const String &sIP, std::vector<String> &vecFoundNames)
   {
      IPAddress address;
      if (!address.TryParse(AnsiString(sIP), true))
         return false;

      String fullQuery;

      if (address.GetType() == IPAddress::IPV4)
      {
         std::vector<String> vecItems = StringParser::SplitString(sIP, ".");
         reverse(vecItems.begin(), vecItems.end());
         String result = StringParser::JoinVector(vecItems, ".");
         fullQuery = result + ".in-addr.arpa";
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
         fullQuery = long_ipv6 + ".ip6.arpa";
      }


      DNSResolverWinApi resolver;
      std::vector<DNSRecord> foundRecords;

      bool result = resolver.Query(fullQuery, DNS_TYPE_PTR, foundRecords);

      vecFoundNames = GetDnsRecordsValues_(foundRecords);
      return result;

   }

}

