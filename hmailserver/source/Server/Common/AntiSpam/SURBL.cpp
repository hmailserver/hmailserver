// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"
#include ".\surbl.h"

#include "../../Common/BO/MessageData.h"
#include "../../Common/BO/SURBLServer.h"
#include "../../Common/TCPIP/DNSResolver.h"
#include "../../Common/Util/FileUtilities.h"

#include "../../Common/Util/TLD.h"

#include <boost/timer/timer.hpp>

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

   

   bool 
   SURBL::Run(std::shared_ptr<SURBLServer> pSURBLServer, std::shared_ptr<MessageData> pMessageData)
   {
      LOG_DEBUG("SURBL: Execute");

      // Extract body
      String sBody = pMessageData->GetBody() + pMessageData->GetHTMLBody(); 

      int iCurPos = -1;
	  
      std::set<String> addresses;

      while (true)
      {
         iCurPos = GetURLStart_(sBody, iCurPos);

         if (iCurPos < 0 )
            break;

         // Start of URL
         int iURLEnd = GetURLEndPos_(sBody, iCurPos);
         int iURLLength = iURLEnd - iCurPos ;

         String sURL = sBody.Mid(iCurPos, iURLLength);

         // Clean the URL from linefeeds
         CleanURL_(sURL);

         // Trim away top domain
         if (!CleanHost_(sURL))
            continue;

			if (addresses.find(sURL) == addresses.end())
			{
				String logMessage;
            logMessage.Format(_T("SURBL: Found URL: %s"), sURL.c_str());
				LOG_DEBUG(logMessage);

				addresses.insert(sURL);
			}
      }

		if (addresses.size() > 0)
		{
			String logMessage = Formatter::Format("SURBL: {0} unique addresses found.", addresses.size());
			LOG_DEBUG(logMessage);
		}

      // We stop processing URL's if:
      // - 15 or more URLss have been processed.
      // - More than 10 seconds have passed.
      //
     	// NEED FOR IMPROVEMENT: max URL's & time should be user-adjustable even if just by INI

	   const int maxURLsToProcess = 15;

      boost::chrono::system_clock::time_point start_time = boost::chrono::system_clock::now();

      int processedAddresses = 0;
      for(String sURL : addresses)
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
         if (!resolver.GetARecords(sHostToLookup, saFoundNames))
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

   int 
   SURBL::GetURLEndPos_(const String &sBody, int iURLStart)
   {
      for (int i = iURLStart; i < sBody.GetLength(); i++)
      {
         // Space added as fix to no test on plain-text emails without end slash
         // Bill: cr and lf added as well for same reason
         // Martin: Removed newline characters again. A URL may span multiple lines
         //         If we stop searching for end of URL at end of newline, we'll
         //         miss certain tests. Please see test case TestSURBLWithWrappedURL.
         // Might be best to look for 1st non-allowed domain char instead..
         wchar_t c = sBody[i];
         if (c == '<' || c == '/' ||  c == '\\' ||  c == '>' ||c == ' ' || c == '"' || c == '\'')
            return i;

         if (c == '\r')
         {
            // if previous char was a =, which indicates that the url
            // continues on the next line, keep on looking for end of link
            if (i > 0)
            {
               if (sBody[i-1] != '=')
                  return i;
            }
         }
      }

      return -1;
   }

   int 
   SURBL::GetURLStart_(const String &sBody, int iCurrentPos)
   {
      int iHttpStart = sBody.Find(_T("http://"), iCurrentPos+1);
      int iHttpsStart = sBody.Find(_T("https://"), iCurrentPos+1);

		const int HttpPrefixLength = 7;
		const int HttpsPrefixLength = 8;

		// If neither http nor https is found, return
		if (iHttpStart == -1 && iHttpsStart == -1)
			return -1;

		// If only https is found, return thatt
      if (iHttpStart == -1)
         return iHttpsStart + HttpsPrefixLength;

		// If only http is found, return that
      if (iHttpsStart == -1)
         return iHttpStart + HttpPrefixLength;

		// If both are found, return closest.
      if (iHttpStart < iHttpsStart)
         return iHttpStart + HttpPrefixLength;
      else
         return iHttpsStart + HttpsPrefixLength;
   }

   void
   SURBL::CleanURL_(String &url) const
   {
      url.Replace(_T("=\r\n"), _T(""));
      // We need to replace them individually as well just in case..
      url.Replace(_T("=\r"), _T(""));
      url.Replace(_T("=\n"), _T(""));
      url.MakeLower();
   }

   bool
   SURBL::CleanHost_(String &sDomain) const
   {
      bool bIsIPAddress = false;
      return TLD::Instance()->GetDomainNameFromHost(sDomain, bIsIPAddress);

   }
}