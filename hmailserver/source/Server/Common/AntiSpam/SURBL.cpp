// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\surbl.h"

#include "../../Common/BO/MessageData.h"
#include "../../Common/BO/SURBLServer.h"
#include "../../Common/TCPIP/DNSResolver.h"

#include "../../Common/Util/TLD.h"
#include <boost/regex.hpp>

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SURBL::SURBL(void)
   {
      
   }

   SURBL::~SURBL(void)
   {
   }

   const int maxURLsToProcess = 15;

   bool
   SURBL::ExtractUrls(std::shared_ptr<MessageData> pMessageData, std::vector<String> &vecUrls)
   {
      LOG_DEBUG("SURBL: Execute");

      // Extract body
      String sBody = pMessageData->GetBody() + pMessageData->GetHTMLBody();

      // Extract URL's from the mail body:
      // Original: (?:(?>https?)?(?>:\/\/|\%3A\%2F\%2F))(?:www\.)?([a-z0-9\-\.\=\r\n]+)

      String sRegex = "(?:(?>https?)?(?>:\\/\\/|\\%3A\\%2F\\%2F))(?:www\\.)?([a-z0-9\\-\\.\\=\\r\\n]+)";

      std::set<String> addresses;

      try
      {
         boost::wregex expression(sRegex, boost::wregex::icase);
         boost::wsmatch matches;

         String sRemainingSearchSpace = sBody;

         while (boost::regex_search(sRemainingSearchSpace, matches, expression))
         {
            if (matches.size() < 2)
            {
               ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5702, "SURBL::Run", "A regex match was found, but there were <2 matches in the result");
               break;
            }

            String sURL = matches[1];

            // Clean the URL from linefeeds
            CleanURL_(sURL);

            // ignore some HTML doctype url's, which are default in for example Outlook composed mails
            // www.w3.org
            // www.w3c.org
            // schemas.microsoft.com
            // fonts.googleapis.com
            // fonts.gstatic.com
            sRegex = "(?=.*)(w3(?:c)?\\.org|(?:schemas\\.microsoft|fonts\\.(?:googleapis|gstatic))\\.com)";
            boost::wregex expr(sRegex, boost::wregex::icase);
            if (boost::regex_match(sURL, expr))
            {
               sRemainingSearchSpace = matches.suffix();
               continue;
            }

            if (addresses.find(sURL) == addresses.end())
            {
               String slogMessage;
               slogMessage.Format(_T("SURBL: Found URL: %s"), sURL.c_str());
               LOG_DEBUG(slogMessage);

               addresses.insert(sURL);

               if (addresses.size() > maxURLsToProcess)
               {
                  break;
               }
            }

            // Trim away top domain
            if (!CleanHost_(sURL))
            {
               sRemainingSearchSpace = matches.suffix();
               continue;
            }

            if (addresses.find(sURL) == addresses.end())
            {
               String logMessage;
               logMessage.Format(_T("SURBL: Found URL: %s"), sURL.c_str());
               LOG_DEBUG(logMessage);

               addresses.insert(sURL);

               if (addresses.size() > maxURLsToProcess)
               {
                  break;
               }
            }

            sRemainingSearchSpace = matches.suffix();
         }
      }
      catch (std::runtime_error &err) // regex_match will throw runtime_error if regexp is too complex.
      {
         ErrorManager::Instance()->ReportError(ErrorManager::Medium, 5701, "SURBL::Run", "Parsing HTML body with regular expression threw a runtime_error", err);
         return true;
      }

      if (addresses.size() > 0)
      {
         String logMessage = Formatter::Format("SURBL: {0} unique domain addresses found.", addresses.size());
         LOG_DEBUG(logMessage);
      }

      for (auto address : addresses)
      { 
         // store found domain addresses in std::vector<String>
         vecUrls.push_back(address);
      }

      if (vecUrls.empty())
      {
         LOG_DEBUG("SURBL: No domain addresses found.");
         return false;
      }

      return true;
   }

   bool 
   SURBL::Run(std::shared_ptr<SURBLServer> pSURBLServer, std::vector<String> &vecUrls)
   {
      boost::chrono::system_clock::time_point start_time = boost::chrono::system_clock::now();

      int processedAddresses = 0;
      for (auto sURL : vecUrls)
      {
         boost::chrono::duration<double> elapsed_seconds = boost::chrono::system_clock::now() - start_time;

         if (elapsed_seconds.count() > 10)
         {
            LOG_DEBUG("SURBL: Aborting. Too long time elapsed.");
            return true;
         }

         if (processedAddresses > maxURLsToProcess)
         {
            LOG_DEBUG("SURBL: Aborting. Too many urls.");
            return true;
         }

         String sHostToLookup = sURL + "." + pSURBLServer->GetDNSHost();

         LOG_DEBUG(Formatter::Format(_T("SURBL: Lookup: {0}"), sHostToLookup));

         std::vector<String> saFoundNames;
         DNSResolver resolver;
         if (!resolver.GetIpAddresses(sHostToLookup, saFoundNames, false))
         {
            LOG_DEBUG("SURBL: DNS query failed.");
            return true;
         }

         if (saFoundNames.size() > 0)
         {
            LOG_DEBUG("SURBL: Match found");
            return false;
         }

         processedAddresses++;
      }

      LOG_DEBUG("SURBL: Match not found");
      return true;
   }

   void
   SURBL::CleanURL_(String &url) const
   {
      if (url.empty())
         return;

      url.Replace(_T("=\r\n"), _T(""));
      url.Replace(_T("=\r"), _T(""));
      url.Replace(_T("=\n"), _T(""));
      if (url.EndsWith(_T("=0D=0A")))
         url = url.substr(0, url.length() - 6);
      url.MakeLower();

      int newLinePosition = url.FindOneOf(_T("\r\n"));

      if (newLinePosition >= 0)
         url = url.Left(newLinePosition);
   }

   bool
   SURBL::CleanHost_(String &sDomain) const
   {
      bool bIsIPAddress = false;
      return TLD::Instance()->GetDomainNameFromHost(sDomain, bIsIPAddress);
   }
}