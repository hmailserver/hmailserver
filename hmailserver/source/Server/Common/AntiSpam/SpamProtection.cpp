// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "StdAfx.h"

#include "SpamProtection.h"

#include "../AntiSpam/WhiteListCache.h"
#include "../AntiSpam/SpamTestRunner.h"
#include "../AntiSpam/SpamTestData.h"
#include "../AntiSpam/SpamTestResult.h"

#include "../AntiSpam/SpamTestSPF.h"

#include "../BO/Message.h"
#include "../BO/Domain.h"
#include "../BO/MessageData.h"

#include "../Cache/CacheContainer.h"

#include "../Persistence/PersistentMessage.h"

#include "../TCPIP/DNSResolver.h"
#include "../TCPIP/HostNameAndIpAddress.h"

#include "../Util/FileUtilities.h"

#include "../../SMTP/GreyListing.h"

#include "DKIM/DKIM.h"

#include "AntiSpamConfiguration.h"


#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{
   SpamProtection::SpamProtection(void)
   {

   }

   SpamProtection::~SpamProtection(void)
   {
   }

   void
   SpamProtection::Load()
   {
      spam_test_runner_ = std::shared_ptr<SpamTestRunner>(new SpamTestRunner);
      spam_test_runner_->LoadSpamTests();

      DKIM::Initialize();
   }

   std::set<std::shared_ptr<SpamTestResult> >
   SpamProtection::RunPreTransmissionTests(const String &sFromAddress, 
                                           const IPAddress &iOriginatingIP,
                                           const IPAddress &iConnectingIP,
                                           const String &sHeloHost)
   {

      std::shared_ptr<SpamTestData> pTestData = std::shared_ptr<SpamTestData>(new SpamTestData);
      
      pTestData->SetEnvelopeFrom(sFromAddress);
      pTestData->SetHeloHost(sHeloHost);
      pTestData->SetOriginatingIP(iOriginatingIP);
      pTestData->SetConnectingIP(iConnectingIP);

      AntiSpamConfiguration &config = Configuration::Instance()->GetAntiSpamConfiguration();
      int maxScore = std::max(config.GetSpamDeleteThreshold(), config.GetSpamMarkThreshold());

      std::set<std::shared_ptr<SpamTestResult> > setResult = spam_test_runner_->RunSpamTest(pTestData, SpamTest::PreTransmission, maxScore);

      return setResult;
   }

   std::set<std::shared_ptr<SpamTestResult> >
   SpamProtection::RunPostTransmissionTests(const String &sFromAddress, 
                                            const IPAddress & iOriginatingIP,
                                            const IPAddress & iConnectingIP,
                                            std::shared_ptr<Message> pMessage)
   {

      const String fileName = PersistentMessage::GetFileName(pMessage);

      AntiSpamConfiguration &config = Configuration::Instance()->GetAntiSpamConfiguration();

      // If the user has configured a maximum message size to scan, use that size if it is below 256 MB.
      // If not, limit scanning to messages smaller than 256 MB. Messages larger than
      // this is very unlikely to be spam.

      int maxSizeToScanKB = 1024 * 256;

      if (config.GetAntiSpamMaxSizeKB() > 0)
         maxSizeToScanKB = std::min(config.GetAntiSpamMaxSizeKB(), maxSizeToScanKB);

      int messageSizeKB = FileUtilities::FileSize(fileName) / 1024;
      if (messageSizeKB > maxSizeToScanKB)
      {
         // The message is larger than the max message size to scan, so we'll skip scanning it.
         std::set<std::shared_ptr<SpamTestResult> > emptySet;
         return emptySet;
      }

      std::shared_ptr<SpamTestData> pTestData = std::shared_ptr<SpamTestData>(new SpamTestData);      
      std::shared_ptr<MessageData> pMessageData = std::shared_ptr<MessageData>(new MessageData);
      
      pMessageData->LoadFromMessage(fileName, pMessage);

      pTestData->SetEnvelopeFrom(sFromAddress);
      pTestData->SetOriginatingIP(iOriginatingIP);
      pTestData->SetConnectingIP(iConnectingIP);
      pTestData->SetMessageData(pMessageData);

      int maxScore = std::max(config.GetSpamDeleteThreshold(), config.GetSpamMarkThreshold());

      std::set<std::shared_ptr<SpamTestResult> > setResult = spam_test_runner_->RunSpamTest(pTestData, SpamTest::PostTransmission, maxScore);

      return setResult;
   }

   bool
   SpamProtection::PerformGreyListing(std::shared_ptr<Message> message, const std::set<std::shared_ptr<SpamTestResult> > &spamTestResults, const String &toAddress, const IPAddress &ipaddress)
   {
      if (!Configuration::Instance()->GetAntiSpamConfiguration().GetUseGreyListing())
      {
         // The sender is either local, or grey listing is not enabled. We should
         // not do grey list checks in these cases.
         return true;
      }

      // Check if we should use grey listing for the recipient domain.
      String sRecipientDomain = StringParser::ExtractDomain(toAddress);
      std::shared_ptr<const Domain> pDomain = CacheContainer::Instance()->GetDomain(sRecipientDomain);         

      if (pDomain && !pDomain->GetASUseGreyListing())
      {
         // Grey listing not enabled in domain. Skip it.
         return true;
      }


      // Check if the SPF test has succeeded. If so, maybe we should not do
      if (Configuration::Instance()->GetAntiSpamConfiguration().GetBypassGreyListingOnSPFSuccess())
      {
         for(std::shared_ptr<SpamTestResult> testResult : spamTestResults)
         {
            if (testResult->GetTestName() == SpamTestSPF::GetTestName())
            {
               if (testResult->GetResult() == SpamTestResult::Pass)
               {
                  // We should not run grey listing since the SPF test has passed
                  LOG_DEBUG("SPF passed, skipping greylisting.");
                  return true;
               }
            }
         }
      }

      // Check if the mail is coming from the domain A record or MX record. If so, bypass grey listing.
      if (Configuration::Instance()->GetAntiSpamConfiguration().GetBypassGreyListingOnMailFromMX())
      {
         String senderDomain = StringParser::ExtractDomain(message->GetFromAddress());
         
         if (senderDomain.GetLength() > 0)
         {
            std::vector<String> found_ip_addresses;
               
            DNSResolver resolver;
            resolver.GetIpAddresses(senderDomain, found_ip_addresses, false);

            std::vector<HostNameAndIpAddress> host_name_with_addresses;
            resolver.GetEmailServers(senderDomain, host_name_with_addresses);

            for(HostNameAndIpAddress host_and_ip : host_name_with_addresses)
               found_ip_addresses.push_back(host_and_ip.GetIpAddress());

            String actualFromAddress = ipaddress.ToString();
            for(String found_ip_address : found_ip_addresses)
            {
               if (found_ip_address.CompareNoCase(actualFromAddress) == 0)
               {
                  // The message is coming from either an A record or a MX record. Skip greylisting.
                  LOG_DEBUG("Mail coming from A or MX record. Skipping grey listing.");
                  return true;
               }
            }
         }
      }

      // Grey listing is enabled, and there's no reason to skip it. Go ahead!
      if (!GreyListingAllowSend(message->GetFromAddress(), toAddress, ipaddress))
         return false;

      return true; 
   }

   std::shared_ptr<MessageData>
   SpamProtection::AddSpamScoreHeaders(std::shared_ptr<Message> pMessage, std::set<std::shared_ptr<SpamTestResult> > setResult, bool classifiedAsSpam)
   {
      std::shared_ptr<MessageData> pMessageData;

      AntiSpamConfiguration &config = Configuration::Instance()->GetAntiSpamConfiguration();

      if (!config.GetAddHeaderSpam() && 
          !config.GetAddHeaderReason() &&
           !config.GetPrependSubject())
      {
         // Nothing to do.
         return pMessageData;
      }

      if (!pMessage)
         return pMessageData;

      pMessageData = std::shared_ptr<MessageData>(new MessageData);
      if (!pMessageData->LoadFromMessage(PersistentMessage::GetFileName(pMessage), pMessage))
         return pMessageData;

      if (classifiedAsSpam)
      {
         if (config.GetAddHeaderSpam())
         {
            pMessageData->SetFieldValue("X-hMailServer-Spam", "YES");
         }

         if (config.GetPrependSubject())
         {
            // Add subject
            String sSubject = pMessageData->GetFieldValue("Subject");

            // Check if subject is already prepended. If so, don't do it again.
            if (sSubject.Find(config.GetPrependSubjectText()) != 0)
            {
               sSubject = config.GetPrependSubjectText() + " " + sSubject;
               pMessageData->SetFieldValue("Subject", sSubject);
            }
         }
      }

      if (config.GetAddHeaderReason())
      {
         auto iter = setResult.begin();
         auto iterEnd = setResult.end();

         int iFieldIdx = 1;
         int iTotalScore = 0;
         for (; iter != iterEnd; iter++, iFieldIdx++)
         {
            std::shared_ptr<SpamTestResult> pResult = (*iter);

            // Only if the test has failed should we add a header for it. If the test
            // is neutral or pass, we shouldn't mention it in the headers.
            if (pResult->GetResult() != SpamTestResult::Fail)
               continue;

            iTotalScore += pResult->GetSpamScore();

            String sHeaderValue;
            sHeaderValue.Format(_T("%s - (Score: %d)"), pResult->GetMessage().c_str(), pResult->GetSpamScore());

            pMessageData->SetFieldValue("X-hMailServer-Reason-" + StringParser::IntToString(iFieldIdx), sHeaderValue);
         }

         pMessageData->SetFieldValue("X-hMailServer-Reason-Score", StringParser::IntToString(iTotalScore));
      }

      return pMessageData;
      
   }



   bool 
   SpamProtection::GreyListingAllowSend(const String &sSenderAddress, const String &sRecipientAddress, const IPAddress & iRemoteIP)
   {
      if (IsWhiteListed(sSenderAddress, iRemoteIP))
         return true;

      GreyListing oGreyList;
      if (!oGreyList.GetAllowSend(sSenderAddress, sRecipientAddress, iRemoteIP))
         return false;

      return true;
   }

   bool 
   SpamProtection::IsWhiteListed(const String &sFromAddress, const IPAddress &iIPAddress)
   {
      WhiteListCache cache;
      return cache.IsWhitelisted(sFromAddress, iIPAddress);
   }

   int 
   SpamProtection::CalculateTotalSpamScore(std::set<std::shared_ptr<SpamTestResult> > result)
   {
      int iTotalSpamScore = 0;

      auto iter = result.begin();
      auto iterEnd = result.end();

      for (; iter != iterEnd; iter++)
      {
         std::shared_ptr<SpamTestResult> pResult = (*iter);
         iTotalSpamScore += pResult->GetSpamScore();

      }

      return iTotalSpamScore;
   }


}