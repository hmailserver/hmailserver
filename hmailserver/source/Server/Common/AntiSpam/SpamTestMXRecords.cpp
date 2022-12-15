// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SpamTestMXRecords.h"

#include "SpamTestData.h"
#include "SpamTestResult.h"

#include "AntiSpamConfiguration.h"

#include "../TCPIP/DNSResolver.h"
#include "../Util/TLD.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   String 
   SpamTestMXRecords::GetName() const
   {
      return "SpamTestMXRecords";
   }

   bool 
   SpamTestMXRecords::GetIsEnabled()
   {
      if (Configuration::Instance()->GetAntiSpamConfiguration().GetUseMXChecks())
         return true;
      else
         return false;
   }


   std::set<std::shared_ptr<SpamTestResult> >
   SpamTestMXRecords::RunTest(std::shared_ptr<SpamTestData> pTestData)
   {
      std::set<std::shared_ptr<SpamTestResult> > setSpamTestResults;


      String sEnvelopeFrom = pTestData->GetEnvelopeFrom();

      if (!sEnvelopeFrom.IsEmpty() && !HasAnyMXRecords_(sEnvelopeFrom))
      {
         // Blocked by SPF.
         String sMessage = _T("Sender domain does not have any MX records.");
         int iScore = Configuration::Instance()->GetAntiSpamConfiguration().GetUseMXChecksScore();;

         std::shared_ptr<SpamTestResult> pResult = std::shared_ptr<SpamTestResult>(new SpamTestResult(GetName(), SpamTestResult::Fail, iScore, sMessage));
         setSpamTestResults.insert(pResult);   

      }

      return setSpamTestResults;
   }

   bool 
   SpamTestMXRecords::HasAnyMXRecords_(const String &sSenderEMail)
   {
      const String sHostName = StringParser::ExtractDomain(sSenderEMail);

      // Check if full host name has any MX records.
      std::vector<String> vecFoundNames;

      DNSResolver resolver;
      if (!resolver.GetMXRecords(sHostName, vecFoundNames))
      {
         // DNS query failed. Let's pretend he has MX records.
         return true;
      }

      // DNS query succeeded. Check if MX records exists.
      if (vecFoundNames.size() > 0)
         return true;

      // No MX records were found.

      // Extract the domain name from the host name. For example, replace
      // cgi.example.com with example.com.
      bool bIsIPAddress = false;
      String sDomainName = sHostName;
      if (!TLD::Instance()->GetDomainNameFromHost(sDomainName, bIsIPAddress))
         return true;

      if (bIsIPAddress)
      {
         // Sender address contains an IP address. Skip MX checks.
         return true;
      }

      if (sDomainName == sHostName)
      {
         // Host name is same as domain name. We have already queried the host name
         // so there's no need to re-run this query.
         return false;
      }

      if (!resolver.GetMXRecords(sDomainName, vecFoundNames))
      {
         // DNS query failed. Let's pretend he has MX records.
         return true;
      }

      return (vecFoundNames.size() > 0);
   }


}