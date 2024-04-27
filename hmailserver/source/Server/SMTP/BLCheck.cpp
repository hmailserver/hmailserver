// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"
#include "BLCheck.h"
#include "../common/tcpip/dnsresolver.h"
#include "../common/tcpip/IPAddress.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{

   BLCheck::BLCheck(void)
   {
   }

   BLCheck::~BLCheck(void)
   {
   }

   bool
   BLCheck::ClientExistsInDNSBL(const IPAddress &sClientIP, const String &sDNSBLHost, const String &sExpectedResult)
   {
      String sReverted = GetRevertedIP(sClientIP.ToString());
      if (sReverted.GetLength() == 0)
         return false;

      String sCheckHost = sReverted + "." + sDNSBLHost;

      std::vector<String> foundAddresses;
      DNSResolver resolver;
      resolver.GetIpAddresses(sCheckHost, foundAddresses, false);

      bool isBlocked = false;

      // It's possible to specify that one or many intervals are blocked,
      // using syntax such as 127.0.0.1-5|127.0.0.10-3
      const std::set<String> blockedAddresses = ExpandAddresses(sExpectedResult);

      //
      // The server may return a list of addresses.
      //
      for(const String foundAddress : foundAddresses)
      {
         // Go through all list of blocked addresses and see if host is in it.
         for(const String &blockedAddress : blockedAddresses)
         {
            // We not only support ranges, we support wildcards as well.
            isBlocked = StringParser::WildcardMatch(blockedAddress, foundAddress);

            if (isBlocked)
            {
               // We have a match. No need to continue now.
               break;
            }
         }
         
         if (isBlocked)
         {
            // We have a match. No need to continue.
            break;
         }
      }

      // If a message is rejected, it's good to mention in the log which DNSBL
      // caused the rejection...
      // http://www.hmailserver.com/forum/viewtopic.php?f=10&t=16221
      //
      String foundAddressesJoined = StringParser::JoinVector(foundAddresses, ", ");
      if (foundAddressesJoined.IsEmpty())
         foundAddressesJoined = "(none)";

      String logMessage = Formatter::Format("DNS lookup: {0}, {1} addresses found: {2}, Match: {3}", sCheckHost, foundAddresses.size(), foundAddressesJoined, isBlocked);
      LOG_TCPIP(logMessage);

      return isBlocked;
   }

   std::set<String> 
   BLCheck::ExpandAddresses(const String &input)
   {
      std::set<String> addresses;

      std::vector<String> inputAddresses = StringParser::SplitString(input, "|");

      for(String address : inputAddresses)
      {
         address.Trim();

         if (address.Contains(_T("-")))
         {
            // This is a range...
            std::vector<String> addressParts = StringParser::SplitString(address, ".");

            if (addressParts.size() != 4)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5342, "BLCheck::ExpandAddresses", 
                  Formatter::Format("Unable to parse address in black list: {0}. Expected 3 dots.", address));

               continue;
            }
            
            String lastPart = addressParts[3];

            if (lastPart.GetLength() < 3)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5342, "BLCheck::ExpandAddresses", 
                  Formatter::Format("Unable to parse address in black list: {0}. Expected at least 3 chars in last section.", address));

               continue;
            }

            int pos = lastPart.Find(_T("-"));
            if (pos <= 0)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5342, "BLCheck::ExpandAddresses", 
                  Formatter::Format("Unable to parse address in black list: {0}. Expected at - in the last section.", address));

               continue;
            }
            
            int valueBefore = _ttoi(lastPart.Mid(0, pos));
            int valueAfter = _ttoi(lastPart.Mid(pos+1));

            for (int i = valueBefore; i <= valueAfter; i++)
            {
               String value = StringParser::IntToString(i);

               String outputAddress = addressParts[0] + "." + addressParts[1] + "." +addressParts[2] + "." + value;

               addresses.insert(outputAddress);
            }
         }
         else
         {
            addresses.insert(address);
         }
      }

      return addresses;
   }

   String
   BLCheck::GetRevertedIP(const String &sIP)
   {
      String result;

      IPAddress address;
      if (!address.TryParse(AnsiString(sIP), true))
         return result;

      if (address.GetType() == IPAddress::IPV4)
      {
         std::vector<String> vecItems = StringParser::SplitString(sIP, ".");
         if (vecItems.size() != 4)
            return result;

         reverse(vecItems.begin(), vecItems.end());

         result = StringParser::JoinVector(vecItems, ".");
      }
      else if (address.GetType() == IPAddress::IPV6)
      {
         AnsiString long_ipv6 = address.ToLongString();
         long_ipv6.MakeReverse();
         long_ipv6.Remove(':');

         for (int i = long_ipv6.GetLength() - 1; i > 0; i--)
         {
            long_ipv6.insert(i, 1, '.');
         }

         result = long_ipv6;
      }

      return result;
   }


   void
   BLCheckTester::Test()
   {

      if (BLCheck::GetRevertedIP(_T("1.2.3.4")) != _T("4.3.2.1"))
         throw;

      if (BLCheck::GetRevertedIP(_T("111.222.333.444")) != _T("444.333.222.111"))
         throw;

      std::set<String> expandedaddresses = BLCheck::ExpandAddresses("127.0.0.*");

      if (expandedaddresses.find("127.0.0.*") == expandedaddresses.end())
         throw;
      if (expandedaddresses.size() != 1)
         throw;

      expandedaddresses = BLCheck::ExpandAddresses("127.0.0.1-3");

      if (expandedaddresses.find("127.0.0.1") == expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.2") == expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.3") == expandedaddresses.end())
         throw;

      if (expandedaddresses.find("127.0.0.4") != expandedaddresses.end())
         throw;

      expandedaddresses = BLCheck::ExpandAddresses("127.0.0.1-3|127.0.0.5");

      if (expandedaddresses.find("127.0.0.1") == expandedaddresses.end() ||
         expandedaddresses.find("127.0.0.2") == expandedaddresses.end() ||
         expandedaddresses.find("127.0.0.3") == expandedaddresses.end() ||
         expandedaddresses.find("127.0.0.5") == expandedaddresses.end())
         throw; 

      if (expandedaddresses.find("127.0.0.4") != expandedaddresses.end() || 
          expandedaddresses.find("127.0.0.6") != expandedaddresses.end())
         throw;

      expandedaddresses = BLCheck::ExpandAddresses("127.0.0.1|127.0.0.255");

      if (expandedaddresses.find("127.0.0.1") == expandedaddresses.end() ||
         expandedaddresses.find("127.0.0.255") == expandedaddresses.end())
         throw; 

      if (expandedaddresses.find("127.0.0.3") != expandedaddresses.end() || 
         expandedaddresses.find("127.0.0.4") != expandedaddresses.end())
         throw;

      expandedaddresses = BLCheck::ExpandAddresses("127.0.0.1 | 127.0.0.255");

      if (expandedaddresses.find("127.0.0.1") == expandedaddresses.end() ||
         expandedaddresses.find("127.0.0.255") == expandedaddresses.end())
         throw; 

      if (expandedaddresses.find("127.0.0.3") != expandedaddresses.end() || 
         expandedaddresses.find("127.0.0.4") != expandedaddresses.end())
         throw;

      expandedaddresses = BLCheck::ExpandAddresses("127.0.0.2-8|127.0.0.10-11");

      if (expandedaddresses.find("127.0.0.2") == expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.3") == expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.4") == expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.5") == expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.6") == expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.7") == expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.8") == expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.10") == expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.11") == expandedaddresses.end())
         throw; 

      if (expandedaddresses.find("127.0.0.1") != expandedaddresses.end() || 
          expandedaddresses.find("127.0.0.9") != expandedaddresses.end() ||
          expandedaddresses.find("127.0.0.12") != expandedaddresses.end())
         throw;

      expandedaddresses = BLCheck::ExpandAddresses("127.0.0.1-1");

      if (expandedaddresses.find("127.0.0.1") == expandedaddresses.end())
         throw;
      if (expandedaddresses.size() != 1)
         throw;

      expandedaddresses = BLCheck::ExpandAddresses("127.0.0.2");

      if (expandedaddresses.find("127.0.0.2") == expandedaddresses.end())
         throw;
      if (expandedaddresses.size() != 1)
         throw;
   }

}