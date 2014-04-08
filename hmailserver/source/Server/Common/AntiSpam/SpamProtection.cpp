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
#include "../BO/WhiteListAddresses.h"

#include "../Cache/CacheContainer.h"

#include "../Persistence/PersistentMessage.h"

#include "../Util/MessageUtilities.h"
#include "../TCPIP/DNSResolver.h"

#include "../../SMTP/BLCheck.h"
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
      m_pSpamTestRunner = shared_ptr<SpamTestRunner>(new SpamTestRunner);
      m_pSpamTestRunner->LoadSpamTests();

      DKIM::Initialize();
   }

   set<shared_ptr<SpamTestResult> >
   SpamProtection::RunPreTransmissionTests(const String &sFromAddress, 
                                           const IPAddress &iOriginatingIP,
                                           const IPAddress &iConnectingIP,
                                           const String &sHeloHost)
   {

      shared_ptr<SpamTestData> pTestData = shared_ptr<SpamTestData>(new SpamTestData);
      
      pTestData->SetEnvelopeFrom(sFromAddress);
      pTestData->SetHeloHost(sHeloHost);
      pTestData->SetOriginatingIP(iOriginatingIP);
      pTestData->SetConnectingIP(iConnectingIP);

      AntiSpamConfiguration &config = Configuration::Instance()->GetAntiSpamConfiguration();
      int maxScore = max(config.GetSpamDeleteThreshold(), config.GetSpamMarkThreshold());

      set<shared_ptr<SpamTestResult> > setResult = m_pSpamTestRunner->RunSpamTest(pTestData, SpamTest::PreTransmission, maxScore);

      return setResult;
   }

   set<shared_ptr<SpamTestResult> >
   SpamProtection::RunPostTransmissionTests(const String &sFromAddress, 
                                            const IPAddress & iOriginatingIP,
                                            const IPAddress & iConnectingIP,
                                            shared_ptr<Message> pMessage)
   {

      const String fileName = PersistentMessage::GetFileName(pMessage);

      AntiSpamConfiguration &config = Configuration::Instance()->GetAntiSpamConfiguration();

      // If the user has configured a maximum message size to scan, use that size.
      // If not, limit scanning to messages smaller than 5 MB. Messages larger than
      // this is very unlikely to be spam.

      int maxSizeToScanKB = 1024 * 5;

      if (config.GetAntiSpamMaxSizeKB() > 0)
         maxSizeToScanKB = min(config.GetAntiSpamMaxSizeKB(), maxSizeToScanKB);

      int messageSizeKB = FileUtilities::FileSize(fileName) / 1024;
      if (messageSizeKB > maxSizeToScanKB)
      {
         // The message is larger than the max message size to scan, so we'll skip scanning it.
         set<shared_ptr<SpamTestResult> > emptySet;
         return emptySet;
      }

      shared_ptr<SpamTestData> pTestData = shared_ptr<SpamTestData>(new SpamTestData);      
      shared_ptr<MessageData> pMessageData = shared_ptr<MessageData>(new MessageData);
      
      pMessageData->LoadFromMessage(fileName, pMessage);

      pTestData->SetEnvelopeFrom(sFromAddress);
      pTestData->SetOriginatingIP(iOriginatingIP);
      pTestData->SetConnectingIP(iConnectingIP);
      pTestData->SetMessageData(pMessageData);

      int maxScore = max(config.GetSpamDeleteThreshold(), config.GetSpamMarkThreshold());

      set<shared_ptr<SpamTestResult> > setResult = m_pSpamTestRunner->RunSpamTest(pTestData, SpamTest::PostTransmission, maxScore);

      return setResult;
   }

   bool
   SpamProtection::PerformGreyListing(shared_ptr<Message> message, const set<shared_ptr<SpamTestResult> > &spamTestResults, const String &toAddress, const IPAddress &ipaddress)
   {
      if (!Configuration::Instance()->GetAntiSpamConfiguration().GetUseGreyListing())
      {
         // The sender is either local, or grey listing is not enabled. We should
         // not do grey list checks in these cases.
         return true;
      }

      // Check if we should use grey listing for the recipient domain.
      String sRecipientDomain = StringParser::ExtractDomain(toAddress);
      shared_ptr<const Domain> pDomain = CacheContainer::Instance()->GetDomain(sRecipientDomain);         

      if (pDomain && !pDomain->GetASUseGreyListing())
      {
         // Grey listing not enabled in domain. Skip it.
         return true;
      }


      // Check if the SPF test has succeeded. If so, maybe we should not do
      if (Configuration::Instance()->GetAntiSpamConfiguration().GetBypassGreyListingOnSPFSuccess())
      {
         boost_foreach(shared_ptr<SpamTestResult> testResult, spamTestResults)
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
            std::vector<String> foundAddresses;
               
            DNSResolver resolver;
            resolver.GetARecords(senderDomain, foundAddresses);
            resolver.GetEmailServers(senderDomain, foundAddresses);

            String actualFromAddress = ipaddress.ToString();
            boost_foreach(String foundAddress, foundAddresses)
            {
               if (foundAddress.CompareNoCase(actualFromAddress) == 0)
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

   shared_ptr<MessageData>
   SpamProtection::TagMessageAsSpam(shared_ptr<Message> pMessage, set<shared_ptr<SpamTestResult> > setResult)
   {
      shared_ptr<MessageData> pMessageData;

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

      pMessageData = shared_ptr<MessageData>(new MessageData);
      if (!pMessageData->LoadFromMessage(PersistentMessage::GetFileName(pMessage), pMessage))
         return pMessageData;

      if (config.GetAddHeaderSpam())
         pMessageData->SetFieldValue("X-hMailServer-Spam", "YES");

      if (config.GetAddHeaderReason())
      {
         set<shared_ptr<SpamTestResult> >::iterator iter = setResult.begin();
         set<shared_ptr<SpamTestResult> >::iterator iterEnd = setResult.end();

         int iFieldIdx = 1;
         int iTotalScore = 0;
         for (; iter != iterEnd; iter++, iFieldIdx++)
         {
            shared_ptr<SpamTestResult> pResult = (*iter);

            // Only if the test has failed should we add a header for it. If the test
            // is neutral or pass, we shouldn't mention it in the headers.
            if (pResult->GetResult() != SpamTestResult::Fail)
               continue;

            iTotalScore += pResult->GetSpamScore();

            String sHeaderValue;
            sHeaderValue.Format(_T("%s - (Score: %d)"), pResult->GetMessage(), pResult->GetSpamScore());

            pMessageData->SetFieldValue("X-hMailServer-Reason-" + StringParser::IntToString(iFieldIdx), sHeaderValue);
         }

         pMessageData->SetFieldValue("X-hMailServer-Reason-Score", StringParser::IntToString(iTotalScore));
         
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
   SpamProtection::CalculateTotalSpamScore(set<shared_ptr<SpamTestResult> > result)
   {
      int iTotalSpamScore = 0;

      set<shared_ptr<SpamTestResult> >::iterator iter = result.begin();
      set<shared_ptr<SpamTestResult> >::iterator iterEnd = result.end();

      for (; iter != iterEnd; iter++)
      {
         shared_ptr<SpamTestResult> pResult = (*iter);
         iTotalSpamScore += pResult->GetSpamScore();

      }

      return iTotalSpamScore;
   }


}