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

      set<shared_ptr<SpamTestResult> > RunPreTransmissionTests(const String &sFromAddress, const IPAddress & iOriginatingIP, const IPAddress &iConnectingIP, const String &sHeloHost);
      set<shared_ptr<SpamTestResult> > RunPostTransmissionTests(const String &sFromAddress, const IPAddress & iOriginatingIP, const IPAddress &iConnectingIP, shared_ptr<Message> pMessage);

      static shared_ptr<MessageData> TagMessageAsSpam(shared_ptr<Message> pMessage, set<shared_ptr<SpamTestResult> > setResult);
      static bool GreyListingAllowSend(const String &sSenderAddress, const String &sRecipientAddress, const IPAddress & iRemoteIP);

      static int CalculateTotalSpamScore(set<shared_ptr<SpamTestResult> > result);

      static bool IsWhiteListed(const String &sFromAddress, const IPAddress & iIPAddress);

      bool PerformGreyListing(shared_ptr<Message> message, const set<shared_ptr<SpamTestResult> > &spamTestResults, const String &toAddress, const IPAddress &ipaddress);
   private:
      
      
      shared_ptr<SpamTestRunner> m_pSpamTestRunner;
      
   };

}