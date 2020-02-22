// Copyright (c) 2010 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#pragma once

namespace HM
{
   class Message;
   class MessageData;

   class SpamTestRunner;
   class SpamTestResult;

   class SpamProtection : public Singleton<SpamProtection>
   {
   public:
      SpamProtection(void);
      ~SpamProtection(void);

      void Load();

      std::set<std::shared_ptr<SpamTestResult> > RunPreTransmissionTests(const String &sFromAddress, const IPAddress & iOriginatingIP, const IPAddress &iConnectingIP, const String &sHeloHost);
      std::set<std::shared_ptr<SpamTestResult> > RunPostTransmissionTests(const String &sFromAddress, const IPAddress & iOriginatingIP, const IPAddress &iConnectingIP, std::shared_ptr<Message> pMessage);

      static std::shared_ptr<MessageData> AddSpamScoreHeaders(std::shared_ptr<Message> pMessage, std::set<std::shared_ptr<SpamTestResult> > setResult, bool classifiedAsSpam);
      static bool GreyListingAllowSend(const String &sSenderAddress, const String &sRecipientAddress, const IPAddress & iRemoteIP);

      static int CalculateTotalSpamScore(std::set<std::shared_ptr<SpamTestResult> > result);

      static bool IsWhiteListed(const String &sFromAddress, const IPAddress & iIPAddress);

      bool PerformGreyListing(std::shared_ptr<Message> message, const std::set<std::shared_ptr<SpamTestResult> > &spamTestResults, const String &toAddress, const IPAddress &ipaddress);
   private:
      
      
      std::shared_ptr<SpamTestRunner> spam_test_runner_;
      
   };

}